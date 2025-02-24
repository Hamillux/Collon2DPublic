#pragma once

#include "Utility/ImGui/ImGuiInclude.h"

class StageEditorColor
{
public:
	//#define COL_THEME_0
#define COL_THEME_1

	static constexpr int BACK_COLORS[] =
	{
	#ifdef COL_THEME_0
		0xF38181, 0xFCE38A, 0xEAFFD0, 0x95E1D3
	#elif defined(COL_THEME_1)
		0xFFCFDF, 0xFEFDCA, 0xE0F9B5, 0xA5DEE5
	#endif
	};
	static constexpr int NUM_COLORS_IN_THEME = _countof(BACK_COLORS);

	static constexpr int FORE_COLORS[NUM_COLORS_IN_THEME] =
	{
	#ifdef COL_THEME_0
		0x212121, 0x757575, 0xBDBDBD, 0x000000
	#elif defined(COL_THEME_1)
		0x212121, 0x757575, 0xBDBDBD, 0x000000
	#endif
	};

	

	enum BYTE_OFFSET_ : int { BYTE_OFFSET_R = 2, BYTE_OFFSET_G = 1, BYTE_OFFSET_B = 0 };

	static constexpr float BackR(const int color_index) { return GetColor_Impl(color_index, BACK_COLORS, BYTE_OFFSET_R); }
	static constexpr float BackG(const int color_index) { return GetColor_Impl(color_index, BACK_COLORS, BYTE_OFFSET_G); }
	static constexpr float BackB(const int color_index) { return GetColor_Impl(color_index, BACK_COLORS, BYTE_OFFSET_B); }

	static constexpr float ForeR(const int color_index) { return GetColor_Impl(color_index, FORE_COLORS, BYTE_OFFSET_R); }
	static constexpr float ForeG(const int color_index) { return GetColor_Impl(color_index, FORE_COLORS, BYTE_OFFSET_G); }
	static constexpr float ForeB(const int color_index) { return GetColor_Impl(color_index, FORE_COLORS, BYTE_OFFSET_B); }

	static constexpr ImVec4 BackColor(const int color_index, const float alpha = 1.f) { return ImVec4(BackR(color_index), BackG(color_index), BackB(color_index), alpha); }
	static constexpr ImVec4 ForeColor(const int color_index, const float alpha = 1.f) { return ImVec4(ForeR(color_index), ForeG(color_index), ForeB(color_index), alpha); }

private:
	static constexpr float GetColor_Impl(const int idx, const int* arr, const int byte_offset)
	{
		const int shift = byte_offset * 8;
		const int mask = 0xFF << shift;
		return static_cast<float>((arr[idx] & mask) >> shift) / 255.f;
	}
};