#include "StringUtils.h"
#include <stdexcept>

size_t CountUtf8Characters(const std::string& utf8_str)
{
	size_t count = 0;
	size_t i = 0;
	while (i < utf8_str.size()) {
		unsigned char c = utf8_str[i];
		if ((c & 0x80) == 0) {
			// 1バイト文字 (ASCII)
			i += 1;
		}
		else if ((c & 0xE0) == 0xC0) {
			// 2バイト文字
			i += 2;
		}
		else if ((c & 0xF0) == 0xE0) {
			// 3バイト文字
			i += 3;
		}
		else if ((c & 0xF8) == 0xF0) {
			// 4バイト文字
			i += 4;
		}
		else {
			// 不正なUTF-8シーケンス
			throw std::runtime_error("Invalid UTF-8 sequence");
		}
		count++;
	}
	return count;
}


void GetUtf8Substring(const std::string& utf8_str, const size_t num_chars, std::string& out_substr)
{
	size_t count = 0;
	size_t i = 0;
	while (i < utf8_str.size() && count < num_chars) {
		unsigned char c = utf8_str[i];
		if ((c & 0x80) == 0) {
			// 1バイト文字 (ASCII)
			i += 1;
		}
		else if ((c & 0xE0) == 0xC0) {
			// 2バイト文字
			i += 2;
		}
		else if ((c & 0xF0) == 0xE0) {
			// 3バイト文字
			i += 3;
		}
		else if ((c & 0xF8) == 0xF0) {
			// 4バイト文字
			i += 4;
		}
		else {
			// 不正なUTF-8シーケンス
			throw std::runtime_error("Invalid UTF-8 sequence");
		}
		count++;
	}
	out_substr = utf8_str.substr(0, i);
}