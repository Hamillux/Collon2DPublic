#pragma once
#include "Utility/Core/Math/Vector2D.h"
#include "SystemTypes.h"
#include "Utility/Core/Rendering/DrawBlendInfo.h"
#include <vector>
#include <stdint.h>

namespace UIElements
{
	constexpr uint8_t MAX_ALPHA = DrawBlendInfo::MAX_BLEND_VALUE;
	class RectLabel
	{
	public:
		enum class Alignment
		{
			LEFT = 0,
			CENTER = 1,
			RIGHT = 2,
			TOP = 0,
			BOTTOM = 2
		};

		/// <summary>
		/// 矩形に付けるラベル
		/// </summary>
		/// <param name="text_in">文字列</param>
		/// <param name="fill_color_in">塗りつぶし色</param>
		/// <param name="border_color_in">境界色</param>
		/// <param name="alpha_in">アルファ値</param>
		/// <param name="font_size_in">フォントサイズ</param>
		/// <param name="horizontal_alignment_in">水平位置</param>
		/// <param name="vertical_alignment_in">垂直位置</param>
		RectLabel(
			const std::string& text_in,
			const int fill_color_in = 0xFFFFFF,
			const int border_color_in = 0x0,
			const int alpha_in = DrawBlendInfo::MAX_BLEND_VALUE,
			const uint16_t font_size_in = 16,
			const Alignment horizontal_alignment_in = Alignment::CENTER,
			const Alignment vertical_alignment_in = Alignment::CENTER
		);

		void Draw(const Vector2D& rect_left_top, const Vector2D& rect_extent) const;

	private:
		int CalcTextLeft(const Vector2D& rect_left_top, const Vector2D& rect_extent) const;
		int CalcTextTop(const Vector2D& rect_left_top, const Vector2D& rect_extent) const;

		std::string text;
		int fill_color;
		int border_color;
		int alpha;
		uint16_t font_size;
		Vector2D text_extent;
		Alignment vertical_alignment;
		Alignment horizontal_alignment;
	};

	/// <summary>
	/// 境界線あり, 回転なしの矩形
	/// </summary>
	class Rectangle
	{
	public:
		/// <param name="color_in">矩形の色</param>
		/// <param name="alpha_in">矩形のアルファ値</param>
		/// <param name="border_thickness_in">境界線の太さ</param>
		/// <param name="border_color_in">境界線の色</param>
		/// <param name="border_alpha_in">境界線のアルファ値</param>
		Rectangle(
			const int color_in, const int alpha_in = DrawBlendInfo::MAX_BLEND_VALUE,
			const int border_thickness_in = 0, const int border_color_in = 0, const int border_alpha_in = DrawBlendInfo::MAX_BLEND_VALUE
		)
			: color(color_in)
			, alpha(alpha_in)
			, border_thickness(border_thickness_in)
			, border_color(border_color_in)
			, border_alpha(border_alpha_in)
		{}

		virtual ~Rectangle() {}

		//~ Begin Rectangle interface
		virtual void Draw(const Vector2D& left_top, const Vector2D& extent) const;
		//~ End Rectangle interface

		void Draw(const Vector2D& left_top, const Vector2D& extent, const RectLabel& label) const;
		Vector2D CalcRectCenter(const Vector2D& left_top, const Vector2D& extent) const;
	private:
		void DrawBorder(const int left, const int top, const int right, const int bottom) const;
		int color;
		int alpha;
		int border_thickness;
		int border_color;
		int border_alpha;
	};

	class ImageRectangle : public Rectangle
	{
	public:
		/// <summary>
		/// 画像付矩形. 矩形の中心と画像の中心は一致する.
		/// </summary>
		/// <param name="graphic_handle">画像ハンドル</param>
		/// <param name="image_ratio_to_rect">rect_extent_inを1としたときの画像の相似比.</param>
		/// <param name="s_lefttop_in">矩形左上のスクリーン座標</param>
		/// <param name="rect_extent_in">スクリーン上での矩形サイズ</param>
		/// <param name="color_in">矩形の色</param>
		/// <param name="alpha_in">矩形のアルファ値</param>
		/// <param name="border_thickness_in">境界線の太さ</param>
		/// <param name="border_color_in">境界線の色</param>
		/// <param name="border_alpha_in">境界線のアルファ値</param>
		ImageRectangle(
			const int graphic_handle, const float image_ratio_to_rect, const int color_in = 0xFFFFFF, const int alpha_in = 0,
			const int border_thickness_in = 0, const int border_color_in = 0, const int border_alpha_in = 0
		);
		virtual ~ImageRectangle() {}

		//~ Begin Rectangle interface
	public:
		virtual void Draw(const Vector2D& left_top, const Vector2D& extent) const override;
		//~ End Rectangle interface
		
		void SetImage(const int new_handle);

	private:
		int handle;
		float image_ratio_to_rect;

		// 画像左上を原点としたときの画像中心のスクリーン座標
		Vector2D image_extent;
	};
}
