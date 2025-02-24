#include "DrawHelper.h"
#include <DxLib.h>
#include "Utility/Core/MathCore.h"

template<typename DxDrawFunc, typename... Args>
int DrawHelperImpl(DxDrawFunc DrawFunc, const DrawBlendInfo& blend_info, Args... args)
{
	int last_blend_mode, last_blend_value;
	GetDrawBlendMode(&last_blend_mode, &last_blend_value);

	SetDrawBlendMode(blend_info.dx_blend_mode, blend_info.blend_value);
	int ret = DrawFunc(args...);

	SetDrawBlendMode(last_blend_mode, last_blend_value);
	return ret;
}

void BlendDrawHelper::ExecuteDrawProcess(const DrawBlendInfo& blend_info, std::function<void()>&& draw_process)
{
	int last_blend_mode, last_blend_value;
	GetDrawBlendMode(&last_blend_mode, &last_blend_value);

	SetDrawBlendMode(blend_info.dx_blend_mode, blend_info.blend_value);
	draw_process();

	SetDrawBlendMode(last_blend_mode, last_blend_value);
}

int BlendDrawHelper::DrawLine(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, unsigned int Color, int Thickness)
{
    return DrawHelperImpl(DxLib::DrawLine, blend_info, x1, y1, x2, y2, Color, Thickness);
}

int BlendDrawHelper::DrawBox(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, unsigned int Color, int FillFlag)
{
	return DrawHelperImpl(DxLib::DrawBox, blend_info, x1, y1, x2, y2, Color, FillFlag);
}

int BlendDrawHelper::DrawExtendGraph(const DrawBlendInfo& blend_info, int x1, int y1, int x2, int y2, int GrHandle, int TransFlag)
{
	return DrawHelperImpl(DxLib::DrawExtendGraph, blend_info, x1, y1, x2, y2, GrHandle, TransFlag);
}

int BlendDrawHelper::DrawRotaGraph(const DrawBlendInfo& blend_info, int x, int y, double ExtRate, double Angle, int GrHandle, int TransFlag, int ReverseXFlag, int ReverseYFlag)
{
	return DrawHelperImpl(DxLib::DrawRotaGraph, blend_info, x, y, ExtRate, Angle, GrHandle, TransFlag, ReverseXFlag, ReverseYFlag);
}

int DrawStringHelper::DrawString(uint16_t font_size, int x, int y, const TCHAR* String, unsigned int Color, unsigned int EdgeColor)
{
	const int last_font_size = GetFontSize();

	SetFontSize(font_size);
	const int ret = DxLib::DrawString(x, y, String, Color, EdgeColor);

	SetFontSize(last_font_size);

	return ret;
}

int DrawStringHelper::DrawStringC(const uint16_t font_size, const int cx, const int cy, const TCHAR* str, const unsigned int color, unsigned int EdgeColor)
{
	const int str_width = GetDrawStringWidth(font_size, str, strlenDx(str));
	return DrawString(font_size, cx - str_width / 2, cy - font_size / 2, str, color, EdgeColor);
}

int DrawStringHelper::GetDrawStringWidth(const uint16_t font_size, const TCHAR* String, int StrLen, int VerticalFlag)
{
	const int last_font_size = GetFontSize();

	SetFontSize(font_size);
	int string_width = DxLib::GetDrawStringWidth(String, StrLen, VerticalFlag);

	SetFontSize(last_font_size);
	return string_width;
}

int DrawStringHelper::DrawKeyInputString(const uint16_t font_size, int x, int y, int InputHandle, int DrawCandidateList)
{
	const int last_font_size = GetFontSize();

	SetFontSize(font_size);
	const int ret = DxLib::DrawKeyInputString(x, y, InputHandle, DrawCandidateList);

	SetFontSize(last_font_size);

	return ret;
}

int DrawStringHelper::DrawKeyInputStringC(const uint16_t font_size, int cx, int cy, int InputHandle, int DrawCandidateList)
{
	TCHAR input_string[256];
	GetKeyInputString(input_string, InputHandle);
	const int str_width = GetDrawStringWidth(font_size, input_string, strlenDx(input_string));
	const int left = cx - str_width / 2;
	const int top = cy - font_size / 2;
	return DrawKeyInputString(font_size, left, top, InputHandle, DrawCandidateList);
}

void GraphDrawHelper::DrawBoxWithThickness(int x1, int y1, int x2, int y2, unsigned int Color, int Thickness)
{
	DxLib::DrawLine(x1, y1, x1, y2, Color, Thickness);
	DxLib::DrawLine(x1, y2, x2, y2, Color, Thickness);
	DxLib::DrawLine(x2, y2, x2, y1, Color, Thickness);
	DxLib::DrawLine(x2, y1, x1, y1, Color, Thickness);
}

void GraphDrawHelper::DrawConvexPolygon(const std::vector<Vector2D>& vertexes, const int fill_color, const int border_color, const int border_thickness)
{
	if (vertexes.size() < 3)
	{
		return;
	}

	// 塗りつぶし
	for (int i = 2; i < vertexes.size(); i++)
	{
		const Vector2D& a = vertexes.at(0);
		const Vector2D& b = vertexes.at(i - 1);
		const Vector2D& c = vertexes.at(i);

		DrawTriangle(a.x, a.y, b.x, b.y, c.x, c.y, fill_color, true);
	}


	if (border_thickness <= 0)
	{
		return;
	}
	// 辺
	for (int i = 0; i < vertexes.size(); i++)
	{
		const Vector2D& from = vertexes.at(i);
		const Vector2D& to = (i == vertexes.size() - 1) ? vertexes.at(0) : vertexes.at(i + 1);
		DrawLine(from.x, from.y, to.x, to.y, border_color, border_thickness);
	}
}
