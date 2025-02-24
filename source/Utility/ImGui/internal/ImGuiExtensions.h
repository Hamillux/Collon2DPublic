#pragma once
#include <imgui.h>
#include <stack>
#include <stdexcept>
#include <stdint.h>
#include "Utility/Core/Math/Vector2D.h"

inline constexpr ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline constexpr ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline constexpr ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline constexpr ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

template<typename T>
inline constexpr ImVec2 operator*(const ImVec2& imv, const T& a)
{
	return ImVec2(imv.x * a, imv.y * a);
}

template<typename T>
inline constexpr ImVec2 operator*(const T& a, const ImVec2& imv)
{
	return imv * a;
}

template<typename T>
inline constexpr ImVec2 operator/(const ImVec2& imv, const T& a)
{
	return ImVec2(imv.x / a, imv.y / a);
}

inline Vector2D operator+(const Vector2D& vector2d, const ImVec2& imvec2)
{
	return Vector2D(vector2d.x + imvec2.x, vector2d.y + imvec2.y);
}
inline Vector2D operator-(const Vector2D& vector2d, const ImVec2& imvec2)
{
	return Vector2D(vector2d.x - imvec2.x, vector2d.y - imvec2.y);
}

inline constexpr ImVec4 operator* (const ImVec4& lhs, const ImVec4& rhs)
{
	return ImVec4(
		lhs.x * rhs.x,
		lhs.y * rhs.y,
		lhs.z * rhs.z,
		lhs.w * rhs.w
		);
}

inline constexpr ImVec4 operator* (const ImVec4& lhs, const float rhs)
{
	return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

inline constexpr ImVec4 operator* (const float lhs, const ImVec4& rhs)
{
	return rhs * lhs;
}

struct ImVec3
{
	float                                                     x, y, z;
	constexpr ImVec3() : x(0.0f), y(0.0f), z(0.0f) { }
	constexpr ImVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
};

inline constexpr ImVec4 operator* (const ImVec4& v4, const ImVec3& v3)
{
	return ImVec4(v4.x * v3.x, v4.y * v3.y, v4.z * v3.z, v4.w);
}

inline constexpr ImVec4 operator* (const ImVec3& v3, const ImVec4& v4)
{
	return v4 * v3;
}

namespace ImGui
{

void SetNextAlignedChildWindowPos(const ImVec2& pos, const ImVec2& pivot, const ImVec2& size, const ImVec2& offset);
/// <summary>
/// <para>
/// &lt;parent pos&gt; + &lt;parent size&gt; * pos + offset == &lt;child pos&gt; + &lt;child size&gt; * pivotとなる位置に子ウィンドウを表示.
/// </para>
/// </summary>
/// <param name="str_id"></param>
/// <param name="pos">(0,0)~(1,1)</param>
/// <param name="pivot">(0,0)~(1,1)</param>
/// <param name="child_size">子ウィンドウサイズ(px)</param>
/// <param name="offset">子ウィンドウ位置のオフセット(px)</param>
/// <param name="child_flags"></param>
/// <param name="window_flags"></param>
/// <returns></returns>
bool BeginAlignedChild(const char* str_id, const ImVec2& pos, const ImVec2& pivot, const ImVec2& child_size, const ImVec2& offset = ImVec2(), const ImGuiChildFlags child_flags = 0, const ImGuiWindowFlags window_flags = 0);
bool BeginAlignedChild(ImGuiID id, const ImVec2& pos, const ImVec2& pivot, const ImVec2& child_size, const ImVec2& offset = ImVec2(), const ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0);
void EndAlignedChild();

////////////////
// Sidebar
////////////////
enum class SidebarType { LEFT, RIGHT };

inline constexpr const char* GetSidebarToggleButtonWindowName(const SidebarType type)
{
	switch (type)
	{
	case SidebarType::LEFT:		return "__toggle_sidebar_left";
	case SidebarType::RIGHT:	return "__toggle_sidebar_right";
	}

	throw std::runtime_error("Unknown sidebar type");
}

// Call 'EndSideBarLeft()' after 'BeginSideBar()', regardless of its return value
void PushSidebarDefaultStyle();
void PopSidebarDefaultStyle();
void PushSidebarDefaultStyleColor();
void PopSidebarDefaultStyleColor();

inline constexpr ImGuiWindowFlags GetSidebarDefaultFlags()
{
	return
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoNav;
}

struct SideBarDesc
{
	SideBarDesc()
		: type(SidebarType::LEFT)
		, width(100)
		, height(100)
		, screen_width(800)
		, screen_height(600)
		, delta_width(100 / 60)
		, upper_padding(0)
		, button_size(32)
		, texture_id_open(nullptr)
		, texture_id_close(nullptr)
		, flags_sidebar(GetSidebarDefaultFlags())
		, flags_button(GetSidebarDefaultFlags())
	{}
	SidebarType type;
	int width;
	int height;
	int screen_width;
	int screen_height;
	unsigned int delta_width;	// 0の場合は即open, 即close
	int upper_padding;
	int button_size;
	ImTextureID texture_id_open;
	ImTextureID	texture_id_close;
	ImVec2 uv0_open;
	ImVec2 uv1_open;
	ImVec2 uv0_close;
	ImVec2 uv1_close;
	ImGuiWindowFlags flags_sidebar;
	ImGuiWindowFlags flags_button;
};

/// <summary>
/// 
/// </summary>
/// <param name="name"></param>
/// <param name="desc"></param>
/// <param name="open">Sidebar is opening or fully open</param>
/// <param name="current_width">Distance between screen edge and sidebar's inner edge</param>
/// <returns>Whether or not sidebar is completely open</returns>
bool BeginSideBar(
	const char* name,
	const SideBarDesc& desc,
	bool& open,
	int& current_width
);
void EndSideBar();
// End Sidebar

constexpr inline ImVec4 GetColorImVec4(const ImU32 argb)
{
	const float r = ((argb >> 16) & 0xFF) / 255.0f;
	const float g = ((argb >> 8) & 0xFF) / 255.0f;
	const float b = (argb & 0xFF) / 255.0f;
	const float a = ((argb >> 24) & 0xFF) / 255.0f;
	return ImVec4(r, g, b, a);
}

bool ImageRadioButton(
	const char* str,
	int& v,
	const int v_button,
	ImTextureID user_texture_id,
	const ImVec2& image_size,
	const ImVec2& uv0 = ImVec2(0, 0),
	const ImVec2& uv1 = ImVec2(1, 1),
	const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
	const ImVec4& tint_pressed = ImVec4(1, 1, 1, 1),
	const ImVec4& tint_released = ImVec4(0.5, 0.5, 0.5, 1)
);

bool ImageRadioButton(
	const char* str,
	int& v,
	const int v_button,
	ImTextureID user_texture_id,
	const ImVec2& image_size,
	const float padding,
	const ImVec4 padding_color_pressed,
	const ImVec4 padding_color_released,
	const ImVec2& uv0 = ImVec2(0, 0),
	const ImVec2& uv1 = ImVec2(1, 1),
	const ImVec4& bg_col = ImVec4(0, 0, 0, 1),
	const ImVec4& tint_pressed = ImVec4(1, 1, 1, 1),
	const ImVec4& tint_released = ImVec4(0.5, 0.5, 0.5, 1)
);

constexpr inline ImU32 SwizzleImU32_Implement(
	const ImU32 source,
	const uint8_t byte_shift_before_r, const uint8_t byte_shift_after_r,
	const uint8_t byte_shift_before_g, const uint8_t byte_shift_after_g,
	const uint8_t byte_shift_before_b, const uint8_t byte_shift_after_b,
	const uint8_t byte_shift_before_a, const uint8_t byte_shift_after_a)
{
	// 各チャンネルを抽出
	ImU32 r = (source >> (8 * byte_shift_before_r)) & 0xFF;  // 赤成分を抽出
	ImU32 g = (source >> (8 * byte_shift_before_g)) & 0xFF;  // 緑成分を抽出
	ImU32 b = (source >> (8 * byte_shift_before_b)) & 0xFF;  // 青成分を抽出
	ImU32 a = (source >> (8 * byte_shift_before_a)) & 0xFF;  // アルファ成分を抽出

	// 各チャンネルを新しい位置にスワップして再構築
	return (r << (8 * byte_shift_after_r)) |
		(g << (8 * byte_shift_after_g)) |
		(b << (8 * byte_shift_after_b)) |
		(a << (8 * byte_shift_after_a));
}


// RGBA -> ABGR
constexpr inline ImU32 SwizzleImU32_rgba_abgr(const ImU32 rgba)
{
	return SwizzleImU32_Implement(rgba, 3, 0, 2, 1, 1, 2, 0, 3);
}

// RGBA -> ARGB
constexpr inline ImU32 SwizzleImU32_rgba_argb(const ImU32 rgba)
{
	return SwizzleImU32_Implement(rgba, 3, 2, 2, 1, 1, 0, 0, 3);
}

// ARGB -> ABGR
constexpr inline ImU32 SwizzleImU32_argb_abgr(const ImU32 argb)
{
	return SwizzleImU32_Implement(argb, 2, 0, 1, 1, 0, 2, 3, 3);
}

void PushStyleColorU32_rgba(ImGuiCol idx, const ImU32 rgba);

// ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActiveに対してPushStyleColor実行
void PushStyleColors_Button(const ImVec4& base, const float brightness_hovered = 0.75f, const float brightness_active = 0.5f);
void PopStyleColors_Button();

struct Texture
{
	Texture()
		: im_tex_id(nullptr)
	{}

	operator bool() const
	{
		return im_tex_id != nullptr;
	}

	ImTextureID im_tex_id;
	ImVec2 uv0;
	ImVec2 uv1;

	float& u0() { return uv0.x; }
	float& v0() { return uv0.y; }
	float& u1() { return uv1.x; }
	float& v1() { return uv1.y; }
};

void Image(
	const Texture& texture,
	const ImVec2& image_size,
	const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
	const ImVec4& border_col = ImVec4(0, 0, 0, 0)
);

bool ImageButton(const char* str_id, const Texture& texture, const ImVec2& button_size);

bool IsAnyWindowHovered();

bool IsAnyPopupOpen();

ImVec2 GetScreenSize();

} // end namespace ImGui