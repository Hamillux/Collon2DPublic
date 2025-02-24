#pragma once
#include <string>
#include <tchar.h>
#include <Windows.h>	// MultiByteToWideChar, WideCharToMultiByte

using tstring = std::basic_string<TCHAR>;

constexpr size_t TO_TSTRING_BUFFER_SIZE = 256;
tstring to_tstring(const int val, const TCHAR* format = _T("%d"));
tstring to_tstring(const unsigned int val, const TCHAR* format = _T("%u"));
tstring to_tstring(const long val, const TCHAR* format = _T("%ld"));
tstring to_tstring(const unsigned long val, const TCHAR* format = _T("%lu"));
tstring to_tstring(const long long val, const TCHAR* format = _T("%lld"));
tstring to_tstring(const unsigned long long val, const TCHAR* format = _T("%llu"));
tstring to_tstring(const float val, const TCHAR* format = _T("%f"));
tstring to_tstring(const double val, const TCHAR* format = _T("%f"));
tstring to_tstring(const long double val, const TCHAR* format = _T("%Lf"));
tstring to_tstring(const std::string& str);
tstring to_tstring(const char* str);

// https://www.northtorch.co.jp/archives/2943
inline std::wstring StringToWString(const std::string& str)
{
	// 変換に必要なサイズを取得
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
	// 変換先のwchar_t配列を作成
	std::wstring wstr(size_needed, 0);
	// 変換を実行
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
	return wstr;
}

inline std::string WStringToString(const std::wstring& wstr)
{
	// 変換に必要なサイズを取得
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
	// 変換先のchar配列を作成
	std::string str(size_needed, 0);
	// 変換を実行
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, NULL, NULL);
	return str;
}

size_t CountUtf8Characters(const std::string& utf8_str);

/// <summary>
/// utf8文字列を先頭から指定文字数分取り出す. 指定文字数が文字列の長さを超える場合は文字列全体が返る.
/// </summary>
void GetUtf8Substring(const std::string& utf8_str, const size_t num_chars, std::string& out_substr);