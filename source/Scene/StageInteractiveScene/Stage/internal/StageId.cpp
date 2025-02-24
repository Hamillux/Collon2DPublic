#include "StageId.h"
#include "SystemTypes.h"
#include <nlohmann/json.hpp>
#include "Utility/Core/Math/RandomNumberGenerator.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

const StageId StageId::NONE(StageId::NONE_VALUE, StageId::NONE_VALUE);

StageId::StageId(const std::string uuid_format_string)
	: high_bits(NONE_VALUE)
	, low_bits(NONE_VALUE)
{
	LoadIdFromUUIDFormatString(uuid_format_string);
}

void StageId::ToJsonValue(nlohmann::json& value_json) const
{
	value_json = ToUUIDFormatString();
}

void StageId::FromJsonValue(const nlohmann::json& value_json)
{
	LoadIdFromUUIDFormatString(value_json.get<std::string>());
}

bool StageId::operator==(const StageId& other_id) const
{
	return (high_bits == other_id.high_bits) && (low_bits == other_id.low_bits);
}

bool StageId::operator!=(const StageId& other_id) const
{
	return !((*this) == other_id);
}

bool StageId::operator<(const StageId& other_id) const
{
	//// 上位ビット列同士の比較
	if (high_bits < other_id.high_bits)
	{
		return true;
	}
	else if(high_bits > other_id.high_bits)
	{
		return false;
	}

	//// 上位ビットが等しいので下位ビットを比較する
	return low_bits < other_id.low_bits;
}

bool StageId::IsValid() const
{
	return IsVersion4(high_bits) && HasVariantOfRFC4122(low_bits);
}

void StageId::LoadIdFromUUIDFormatString(std::string str_uuid)
{
	if (CheckUUIDFormat(str_uuid) != UUIDFormatCheckMsg::IS_UUID_FORMAT)
	{
		// str_uuidの形式がUUIDフォーマットではない
		return;
	}

	// UUID文字列からハイフンを削除
	size_t pos_hyphen = str_uuid.find('-');
	while (pos_hyphen != std::string::npos)
	{
		str_uuid.erase(pos_hyphen, 1);
		pos_hyphen = str_uuid.find('-');
	}

	// ハイフンをすると、32文字の16進文字列になる
	const std::string& hex_str_32 = str_uuid;

	std::stringstream ss_high(str_uuid.substr(0, 16));
	ss_high >> std::hex >> high_bits;

	std::stringstream ss_low(str_uuid.substr(16, 16));
	ss_low >> std::hex >> low_bits;
}

std::string StageId::ToHexString(const bool b_uppercase) const
{
	std::stringstream ss;
	if (b_uppercase)
	{
		ss << std::uppercase;
	}
	ss << std::hex << std::setfill('0') << std::setw(16) << high_bits << std::setw(16) << low_bits;

	return ss.str();
}

std::string StageId::ToUUIDFormatString(const bool b_uppercase) const
{
	const std::string hex_str = ToHexString(b_uppercase);
	// 8-4-4-4-12
	constexpr size_t num_hex_char = 32;
	constexpr size_t num_hyphen = 4;
	constexpr size_t uuid_str_length = num_hex_char + num_hyphen;
	std::string uuid_str(uuid_str_length, '-');

	uuid_str.replace(0, 8, hex_str, 0, 8);
	uuid_str.replace(9, 4, hex_str, 8, 4);
	uuid_str.replace(14, 4, hex_str, 12, 4);
	uuid_str.replace(19, 4, hex_str, 16, 4);
	uuid_str.replace(24, 12, hex_str, 20, 12);

	return uuid_str;
}

std::string StageId::GetJsonFileName() const
{
	// stage_[ステージID].json
	const std::string uuid_str = ToUUIDFormatString(false);
	return ("stage_" + uuid_str + ".json");
}

std::string StageId::GetJsonFilePath() const
{
	return ResourcePaths::Dir::STAGES + GetJsonFileName();
}

std::string StageId::GetThumbNailFileName() const
{
	// stage_[ステージID].png
	const std::string uuid_str = ToUUIDFormatString(false);
	return ("stage_" + uuid_str + ".png");
}

std::string StageId::GetThumbNailFilePath() const
{
	return ResourcePaths::Dir::STAGES + GetThumbNailFileName();
}

void StageId::Test()
{
	uint64_t high, low;
	RandomNumberGenerator::GenerateUUID(high, low);

	StageId id(high, low);

	std::string uuid_str = id.ToUUIDFormatString();
	
	StageId id2(id.ToUUIDFormatString());

	if (IsVersion4(high) && HasVariantOfRFC4122(low) && id == id2)
	{
		// succeeded
		for (;;)break;
	}
	else
	{
		// failed
		for (;;)break;
	}
}

StageId::UUIDFormatCheckMsg StageId::CheckUUIDFormat(const std::string& str)
{
	// 8-4-4-4-12

	// 文字列長チェック
	if (str.length() != UUID_FORMAT_STRING_LENGTH)
	{
		return UUIDFormatCheckMsg::INVALID_STRING_LENGTH;
	}

	// 文字チェック
	for (int i = 0; i < str.size(); i ++)
	{
		const char c = str.at(i);
		const bool in_0_to_9 = '0' <= c && c <= '9';
		const bool in_a_to_f = 'a' <= c && c <= 'f';
		const bool in_A_to_F = 'A' <= c && c <= 'F';
		const bool is_hyphen = c == '-';

		const bool is_valid_character = (in_0_to_9 || in_a_to_f || in_A_to_F || is_hyphen);
		if (!is_valid_character)
		{
			// 無効な文字を発見
			return UUIDFormatCheckMsg::CONTAINS_INVALID_CHARACTER;
		}

		// ハイフンの位置をチェック
		const bool i_is_hyphen_pos = (i == 8 || i == 13 || i == 18 || i == 23);
		if (i_is_hyphen_pos && !is_hyphen)
		{
			// ハイフンがあるべき位置の文字がハイフンではない
			return UUIDFormatCheckMsg::INVALID_HYPHEN_POSITION;
		}
		if (!i_is_hyphen_pos && is_hyphen)
		{
			// ハイフン以外であるべき位置にハイフンがある
			return UUIDFormatCheckMsg::INVALID_HYPHEN_POSITION;
		}

	}
	
	// RFC4122の要件を満たすかチェック
	// XXXXXXXX-XXXX-4XXX-NXXX-XXXXXXXXXXXX (Nは2進表記で10XX)
	const char N = str.at(19);
	const bool N_begins_with_10 = N == '8' || N == '9' || N == 'A' || N == 'B' || N == 'a' || N == 'b';
	if(str.at(14) != '4' || !N_begins_with_10)
	{
		return UUIDFormatCheckMsg::NOT_MEET_RFC4122;
	}

	return UUIDFormatCheckMsg::IS_UUID_FORMAT;
}

bool StageId::IsVersion4(const uint64_t high_bits)
{
	// 上位64bit部分はXXXXXXXX-XXXX-4XXX
	return ((high_bits ^ 0xFFFF'FFFF'FFFF'4FFF) & 0x0000'0000'0000'F000) == 0;
}

bool StageId::HasVariantOfRFC4122(const uint64_t low_bits)
{
	// 下位64bit部分はNXXX-XXXXXXXXXXXX (Nは2進表記で10XX)
	return ((low_bits ^ 0x8FFF'FFFF'FFFF'FFFF) & 0xC000'0000'0000'0000) == 0;
}
