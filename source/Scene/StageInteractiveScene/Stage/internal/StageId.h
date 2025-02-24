#pragma once

#include "Utility/IJsonSerializable.h"
#include <stdint.h>
#include <string>

class StageId : public IJsonValue
{
public:
	static constexpr uint8_t NONE_VALUE = 0;
	static const StageId NONE;

	explicit StageId(const uint64_t high_bits_in, const uint64_t low_bits_in)
		: high_bits(high_bits_in)
		, low_bits(low_bits_in)
	{}
	explicit StageId(const std::string uuid_format_string);
	virtual ~StageId() {}

	//~ Begin IJsonValue interface
	virtual void ToJsonValue(nlohmann::json& value_json) const override;
	virtual void FromJsonValue(const nlohmann::json& value_json) override;
	//~ End IJsonValue interface

public:
	bool operator==(const StageId& other_id) const;
	bool operator!=(const StageId& other_id) const;
	bool operator<(const StageId& other_id) const;

	uint64_t GetHighBits() const
	{
		return high_bits;
	};
	uint64_t GetLowBits() const
	{
		return low_bits;
	}

	bool IsValid() const;

	void LoadIdFromUUIDFormatString(std::string str_uuid);

	/// <summary>
	/// 32文字の16進数表記に変換
	/// </summary>
	/// <param name="b_uppercase">アルファベットを大文字にするか</param>
	/// <returns></returns>
	std::string ToHexString(const bool b_uppercase = false) const;

	/// <summary>
	/// [8文字]-[4文字]-[4文字]-[4文字]-[12文字] 形式の文字列
	/// </summary>
	/// <param name="b_uppercase">アルファベットを大文字にするか</param>
	/// <returns></returns>
	std::string ToUUIDFormatString(const bool b_uppercase = false) const;

	std::string GetJsonFileName() const;
	std::string GetJsonFilePath() const;
	std::string GetThumbNailFileName() const;
	std::string GetThumbNailFilePath() const;

	static void Test();

private:
	//"XXXXXXXX-XXXX-4XXX-NXXX-XXXXXXXXXXXX (Nは2進表記で10XX)"なので, 16進文字数(=32) + ハイフン数(=4)
	static constexpr int UUID_FORMAT_STRING_LENGTH = 32 + 4;
	enum class UUIDFormatCheckMsg
	{
		IS_UUID_FORMAT,
		INVALID_STRING_LENGTH,
		CONTAINS_INVALID_CHARACTER,
		INVALID_HYPHEN_POSITION,
		NOT_MEET_RFC4122
	};
	static UUIDFormatCheckMsg CheckUUIDFormat(const std::string& str);
	static bool IsVersion4(const uint64_t high_bits);
	static bool HasVariantOfRFC4122(const uint64_t low_bits);
	uint64_t high_bits;
	uint64_t low_bits;
};

template<>
struct std::hash<StageId>
{
	size_t operator()(const StageId& id) const
	{
		uint64_t high = id.GetHighBits();
		high ^= high << 13;
		high ^= high >> 7;
		high ^= high << 17;

		uint64_t low = id.GetLowBits();
		low ^= low << 13;
		low ^= low >> 7;
		low ^= low << 17;
		
		return high ^ low;

	}
};