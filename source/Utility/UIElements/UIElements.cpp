#include "UIElements.h"
#include "Utility/Core/Rendering/DrawHelper.h"
#include <DxLib.h>
#include "Utility/Core/StringUtils.h"

void UIElements::Rectangle::Draw(const Vector2D& left_top, const Vector2D& extent) const
{
	int left, top;
	left_top.ToIntRound(left, top);

	int extent_x, extent_y;
	extent.ToIntRound(extent_x, extent_y);

	//// 矩形本体の描画
	const int right = left + extent_x;
	const int bottom = top + extent_y;

	if (alpha == DrawBlendInfo::MAX_BLEND_VALUE)
	{
		DrawBox(left, top, right, bottom, color, true);
	}
	else
	{
		BlendDrawHelper::DrawBox(DrawBlendInfo(DX_BLENDMODE_ALPHA, alpha), left, top, right, bottom, color, true);
	}

	//// 境界線描画
	if (border_thickness > 0)
	{
		DrawBorder(left, top, right, bottom);
	}
}

void UIElements::Rectangle::Draw(const Vector2D& left_top, const Vector2D& extent, const RectLabel& label) const
{
	Draw(left_top, extent);
	label.Draw(left_top, extent);
}


void UIElements::Rectangle::DrawBorder(const int left, const int top, const int right, const int bottom) const
{
	//// 境界線の描画
	if (border_alpha == DrawBlendInfo::MAX_BLEND_VALUE)
	{
		// ブレンドなしで描画
		GraphDrawHelper::DrawBoxWithThickness(left, top, right, bottom, border_color, border_thickness);
	}
	else
	{
		BlendDrawHelper::ExecuteDrawProcess(DrawBlendInfo(DX_BLENDMODE_ALPHA, border_alpha), [this, left, top, right, bottom]()
		{
			GraphDrawHelper::DrawBoxWithThickness(left, top, right, bottom, border_color, border_thickness);
		});
	}
}

Vector2D UIElements::Rectangle::CalcRectCenter(const Vector2D& left_top, const Vector2D& extent) const
{
	return left_top + extent * 0.5f;
}

UIElements::ImageRectangle::ImageRectangle(const int graphic_handle, const float image_ratio_to_rect_in, const int color_in, const int alpha_in, const int border_thickness_in, const int border_color_in, const int border_alpha_in)
	: Rectangle(color_in, alpha_in, border_thickness_in, border_color_in, border_alpha_in)
	, image_ratio_to_rect(image_ratio_to_rect_in)
{
	SetImage(graphic_handle);
}

void UIElements::ImageRectangle::Draw(const Vector2D& left_top, const Vector2D& extent) const
{
	__super::Draw(left_top, extent);
	int draw_center_x, draw_center_y;
	CalcRectCenter(left_top, extent).ToIntRound(draw_center_x, draw_center_y);

	const Vector2D image_lefttop_to_center = image_extent * 0.5f;

	// extent * image_ratio_to_rect = image_extent * image_scale
	float image_scale_x = extent.x * image_ratio_to_rect / image_extent.x;
	float image_scale_y = extent.y * image_ratio_to_rect / image_extent.y;

	DrawRotaGraph3(draw_center_x, draw_center_y, image_lefttop_to_center.x, image_lefttop_to_center.y, image_scale_x, image_scale_y, 0, handle, true);
}

void UIElements::ImageRectangle::SetImage(const int new_handle)
{
	handle = new_handle;

	int sx, sy;
	GetGraphSize(new_handle, &sx, &sy);
	image_extent.x = sx;
	image_extent.y = sy;
}

UIElements::RectLabel::RectLabel(
	const std::string& text_in, 
	const int fill_color_in, 
	const int border_color_in, 
	const int alpha_in,
	const uint16_t font_size_in, 
	const Alignment horizontal_alignment_in, 
	const Alignment vertical_alignment_in
)

	: text(text_in)
	, fill_color(fill_color_in)
	, border_color(border_color_in)
	, alpha(alpha_in)
	, font_size(font_size_in)
	, vertical_alignment(vertical_alignment_in)
	, horizontal_alignment(horizontal_alignment_in)
{
	const int last_font_size = GetFontSize();

	const tstring text_tstring = to_tstring(text);
	SetFontSize(font_size);
	text_extent.x = GetDrawStringWidth(text_tstring.c_str(), strlenDx(text_tstring.c_str()));
	text_extent.y = font_size;

	SetFontSize(last_font_size);
}

void UIElements::RectLabel::Draw(const Vector2D& rect_left_top, const Vector2D& rect_extent) const
{
	const tstring text_tstring = to_tstring(text);
	const int text_left = CalcTextLeft(rect_left_top, rect_extent);
	const int text_top = CalcTextTop(rect_left_top, rect_extent);
	
	if (alpha == DrawBlendInfo::MAX_BLEND_VALUE)
	{
		DrawStringHelper::DrawString(font_size, text_left, text_top, text_tstring.c_str(), fill_color, border_color);
		return;
	}

	int last_blend_mode, last_blend_value;
	GetDrawBlendMode(&last_blend_mode, &last_blend_value);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringHelper::DrawString(font_size, text_left, text_top, text_tstring.c_str(), fill_color, border_color);

	SetDrawBlendMode(last_blend_mode, last_blend_value);
}

int UIElements::RectLabel::CalcTextLeft(const Vector2D& rect_left_top, const Vector2D& rect_extent) const
{
	switch (horizontal_alignment)
	{
	case Alignment::LEFT:
		return rect_left_top.x;
	case Alignment::CENTER:
		return rect_left_top.x + (rect_extent.x - text_extent.x) * 0.5f;
	case Alignment::RIGHT:
		return rect_left_top.x + rect_extent.x - text_extent.x;
	}

	// デフォルトではAlignment::CENTERとする
	return rect_left_top.x + (rect_extent.x - text_extent.x) * 0.5f;
}

int UIElements::RectLabel::CalcTextTop(const Vector2D& rect_left_top, const Vector2D& rect_extent) const
{
	switch (vertical_alignment)
	{
	case Alignment::TOP:
		return rect_left_top.y;
	case Alignment::CENTER:
		return rect_left_top.y + (rect_extent.y - text_extent.y) * 0.5f;
	case Alignment::BOTTOM:
		return rect_left_top.y + rect_extent.y - text_extent.y;
	}

	// デフォルトではAlignment::CENTERとする
	return rect_left_top.y + (rect_extent.y - text_extent.y) * 0.5f;
}