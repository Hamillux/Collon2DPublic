#pragma once
#include <string>
#include <codecvt>
#include <fstream>
#include "Utility/Core/EnumInfo.h"
#include "Utility/Core/StringUtils.h"

/// <summary>
/// テキストファイルの行数をカウント
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
inline size_t CountLines(const std::string& filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file for counting lines");
	}

	size_t lines = 0;
	std::string line;
	while (std::getline(file, line))
	{
		++lines;
	}

	return lines;
}


////////////////////////////////////////
// CSVファイル読み込み
////////////////////////////////////////

// std::string->T
namespace
{
/// <summary>
/// stringstreamを使用して文字列から値に変換
/// </summary>
template<typename T>
void FromString_Impl(bool, const std::string& str, T& x)
{
	std::stringstream ss_buff(str);
	ss_buff >> x;
	if (ss_buff.fail())
	{
		throw std::runtime_error("Conversion failed");
	}
}

template<>
void FromString_Impl(bool, const std::string& str, std::wstring& wstr)
{
	wstr = StringToWString(str);
}

/// <summary>
/// EnumInfo::StringToEnumで文字列を列挙型に変換
/// </summary>
template<typename E>
auto FromString_Impl(int, const std::string& str, E& e)
-> typename std::enable_if_t<std::is_enum<E>::value>
{
	e = EnumInfo<E>::StringToEnum(str);
}

/// <summary>
/// std::stringからTに変換
/// </summary>
template<typename T>
void FromString(const std::string& str, T& x)
{
	// T func(int, U){...}  <-(A)
	// T func(bool,U){...}  <-(B)
	// func(0, u);		<- 暗黙の型変換が必要な(B)は(A)よりも優先度が低い
	FromString_Impl(0, str, x);
}

} // end anonymous-namespace

template<typename T>
inline void LoadFromCsvRow_Impl(std::stringstream& csv_row, T& x)
{
	std::string buff;
	std::getline(csv_row, buff, ',');

	FromString<T>(buff, x);
}

inline void LoadFromCsvRow(std::stringstream&) {}

template<typename Head, typename... Tails>
inline void LoadFromCsvRow(std::stringstream& csv_row, Head& head, Tails&... tails)
{
	LoadFromCsvRow_Impl(csv_row, head);
	LoadFromCsvRow(csv_row, tails...);
}

template<size_t I = 0, typename... Args>
inline typename std::enable_if<I == sizeof...(Args), void>::type
LoadFromCsvRow(std::stringstream& csv_row, std::tuple<Args...> tpl) {}

template<size_t I = 0, typename... Args>
inline typename std::enable_if < I < sizeof...(Args), void>::type
	LoadFromCsvRow(std::stringstream& csv_row, std::tuple<Args...> tpl)
{
	LoadFromCsvRow_Impl(csv_row, std::get<I>(tpl));
	LoadFromCsvRow<I + 1, Args...>(csv_row, tpl);
}
////////////////////////////////////////

////////////////////////////////////////
// ファイル拡張子
////////////////////////////////////////

/// <summary>
/// ファイルの拡張子
/// </summary>
enum class EFileExtension
{
	UNKNOWN,
	TXT,
	CSV,
	JSON,
	XML,
	BMP,
	PNG,
	DDS,
	MP3,
	WAV,
	OGG,
	// 必要に応じて他の拡張子を追加
};

/// <summary>
/// ファイルの拡張子を取得
/// </summary>
/// <param name="filePath">ファイルパス</param>
/// <returns>拡張子</returns>
inline EFileExtension GetFileExtension(const std::string& filePath)
{
	static std::unordered_map<std::string, EFileExtension> extensionMap =
	{
		{ "txt", EFileExtension::TXT },
		{ "csv", EFileExtension::CSV },
		{ "json", EFileExtension::JSON },
		{ "xml", EFileExtension::XML },
		{ "bmp", EFileExtension::BMP },
		{ "png", EFileExtension::PNG },
		{ "dds", EFileExtension::DDS },
		{ "mp3", EFileExtension::MP3 },
		{ "wav", EFileExtension::WAV },
		{ "ogg", EFileExtension::OGG },
		// 必要に応じて他の拡張子を追加
	};

	size_t dotPos = filePath.find_last_of('.');
	if (dotPos == std::string::npos)
	{
		return EFileExtension::UNKNOWN;
	}

	std::string extension = filePath.substr(dotPos + 1);
	auto it = extensionMap.find(extension);
	if (it == extensionMap.end())
	{
		return EFileExtension::UNKNOWN;
	}

	return it->second;
}

/// <summary>
/// ファイルの拡張子を取得
/// </summary>
/// <param name="filePath">ファイルパス</param>
/// <returns>拡張子</returns>
inline EFileExtension GetFileExtension(const std::wstring& filePath)
{
	static std::unordered_map<std::wstring, EFileExtension> extensionMap =
	{
		{ L"txt", EFileExtension::TXT },
		{ L"csv", EFileExtension::CSV },
		{ L"json", EFileExtension::JSON },
		{ L"xml", EFileExtension::XML },
		{ L"bmp", EFileExtension::BMP },
		{ L"png", EFileExtension::PNG },
		{ L"dds", EFileExtension::DDS },
		// 必要に応じて他の拡張子を追加
	};

	size_t dotPos = filePath.find_last_of(L'.');
	if (dotPos == std::wstring::npos)
	{
		return EFileExtension::UNKNOWN;
	}

	std::wstring extension = filePath.substr(dotPos + 1);
	auto it = extensionMap.find(extension);
	if (it == extensionMap.end())
	{
		return EFileExtension::UNKNOWN;
	}

	return it->second;
}
////////////////////////////////////////