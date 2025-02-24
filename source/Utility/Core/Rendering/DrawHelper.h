#pragma once

#include "Utility/Core/Rendering/DrawBlendInfo.h"
#include "Utility/Core/Math/Vector2D.h"
#include <functional>
#include <stdint.h>
#include <tchar.h>

/// <summary>
/// ブレンドモードをセットして描画した後, 描画前のブレンドモードに戻す
/// </summary>
namespace BlendDrawHelper
{
	void ExecuteDrawProcess(const DrawBlendInfo& blend_info, std::function<void()>&& draw_process);

	int DrawLine(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, unsigned int Color, int Thickness = 1);
	int DrawBox(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, unsigned int Color, int FillFlag);
	int DrawCircle(const DrawBlendInfo& blend_info, int x, int y, int r, unsigned int Color, int FillFlag);
	int DrawTriangle(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, int x3, int y3, unsigned int Color, int FillFlag);

	int DrawGraph(const DrawBlendInfo& blend_info, int x, int y, int GrHandle, int TransFlag);
	int DrawExtendGraph(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, int GrHandle, int TransFlag);
	int DrawRotaGraph(const DrawBlendInfo& blend_info, int x, int y, double ExtRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag = 0, int ReverseYFlag = 0);
}

namespace GraphDrawHelper
{
	void DrawBoxWithThickness(int x1, int y1, int x2, int y2, unsigned int Color, int Thickness = 1);
	void DrawConvexPolygon(const std::vector<Vector2D>& vertexes, const int fill_color, const int border_color = 0xFFFFFF, const int border_thickness = 0);
}

// TODO: SetFontSizeは重いので予め使用するフォントのハンドルを作成しておくように変更
namespace DrawStringHelper
{
	// WARNING: パフォーマンス上の問題が大きいため、この関数は使用しないようにする
	int DrawString(uint16_t font_size, int x, int y, const TCHAR* String, unsigned int Color, unsigned int EdgeColor = 0);
	
	// WARNING: パフォーマンス上の問題が大きいため、この関数は使用しないようにする
	int DrawStringC(const uint16_t font_size, const int cx, const int cy, const TCHAR* str, const unsigned int color, unsigned int EdgeColor = 0);

	// WARNING: パフォーマンス上の問題が大きいため、この関数は使用しないようにする
	int GetDrawStringWidth(const uint16_t font_size, const TCHAR* String, int StrLen, int VerticalFlag = 0);

	// WARNING: パフォーマンス上の問題が大きいため、この関数は使用しないようにする
	int DrawKeyInputString(const uint16_t font_size, int x, int y, int InputHandle, int DrawCandidateList = 1);

	// WARNING: パフォーマンス上の問題が大きいため、この関数は使用しないようにする
	int DrawKeyInputStringC(const uint16_t font_size, int cx, int cy, int InputHandle, int DrawCandidateList = 1);
}

namespace DxLib
{
	constexpr int GetComplementaryColor(const int src_r, const int src_g, const int src_b)
	{
		const int min_value = std::min(src_r, std::min(src_g, src_b));
		const int max_value = std::max(src_r, std::max(src_g, src_b));
		const int sum = min_value + max_value;
		const int result_r = sum - src_r;
		const int result_g = sum - src_g;
		const int result_b = sum - src_b;

		// result == 0x00RRGGBB
		int result = 0;
		result |= result_r << 16;
		result |= result_g << 8;
		result |= result_b;

		return result;
	}

	constexpr int GetInvertedColor(const int src_r, const int src_g, const int src_b)
	{
		const int result_r = 255 - src_r;
		const int result_g = 255 - src_g;
		const int result_b = 255 - src_b;

		// result == 0x00RRGGBB
		int result = 0;
		result |= result_r << 16;
		result |= result_g << 8;
		result |= result_b;
		return result;
	}

	inline void GetRGBFromColor(const int color, int& r, int& g, int& b)
	{
		r = (color >> 16) & 0xFF;
		g = (color >> 8) & 0xFF;
		b = color & 0xFF;
	}

}