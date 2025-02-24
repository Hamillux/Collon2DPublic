#pragma once

#include <DxLib.h>
#include "Utility/Core/StringUtils.h"

/// <summary>
/// DxLibのグラフィックハンドルを管理する構造体
/// <para>デストラクタでDeleteGraphを呼び出す</para>
/// </summary>
class DxLibScreenCapture
{
public:
	DxLibScreenCapture(const int screen_x, const int screen_y, const int use_alpha);
	~DxLibScreenCapture();
	int GetHandle() const { return _handle; }
	float GetWidthPerHeight() const { return static_cast<float>(_screen_x) / static_cast<float>(_screen_y); }
	void SaveToPNG(const tstring& file_name) const;

	// コピー禁止
	DxLibScreenCapture(const DxLibScreenCapture&) = delete;
	DxLibScreenCapture& operator=(const DxLibScreenCapture&) = delete;

private:
	int _screen_x;
	int _screen_y;
	int _handle;
};