#include "ImGuiExtensions.h"
#include <DxLib.h>
#include "Utility/Core/Math/MathUtils.h"

void ImGui::SetNextAlignedChildWindowPos(const ImVec2& pos, const ImVec2& pivot, const ImVec2& size, const ImVec2& offset)
{
	const ImVec2 parent_size = ImGui::GetContentRegionMax() - ImGui::GetWindowContentRegionMin() + ImVec2(1, 1);
	const ImVec2 parent_pos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	const ImVec2 next_window_pos = parent_pos + (parent_size * pos - size * pivot) + offset;
	ImGui::SetNextWindowPos(next_window_pos);
}

bool ImGui::BeginAlignedChild(const char* str_id, const ImVec2& pos, const ImVec2& pivot, const ImVec2& child_size, const ImVec2& offset, const ImGuiChildFlags child_flags, const ImGuiWindowFlags window_flags)
{
	SetNextAlignedChildWindowPos(pos, pivot, child_size, offset);
 	return ImGui::BeginChild(str_id, child_size, child_flags, window_flags);
}

bool ImGui::BeginAlignedChild(ImGuiID id, const ImVec2& pos, const ImVec2& pivot, const ImVec2& child_size, const ImVec2& offset, const ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags)
{
	SetNextAlignedChildWindowPos(pos, pivot, child_size, offset);
	return ImGui::BeginChild(id, child_size, child_flags, window_flags);
}

void ImGui::EndAlignedChild()
{
	ImGui::EndChild();
}

void ImGui::PushSidebarDefaultStyle()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2());
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
}

void ImGui::PopSidebarDefaultStyle()
{
	ImGui::PopStyleVar(4);
}

void ImGui::PushSidebarDefaultStyleColor()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 1, 1, 1));
}

void ImGui::PopSidebarDefaultStyleColor()
{
	ImGui::PopStyleColor();
}

bool ImGui::BeginSideBar(const char* name, const SideBarDesc& desc, bool& open, int& current_width)
{
	// get position of the button to open/close sidebar
	auto get_toggle_button_pos = [&desc, &current_width]()
	{
		switch (desc.type)
		{
		case SidebarType::LEFT:
		{
			return ImVec2(current_width, desc.upper_padding);
		}
		case SidebarType::RIGHT:
		{
			const int button_pos_x = desc.screen_width - current_width - desc.button_size;
			return ImVec2(button_pos_x, desc.upper_padding);
		}

		}

		throw std::runtime_error("Unknown sidebar type");
	};

	// get sidebar position
	auto get_sidebar_body_pos = [&desc, &current_width]()
	{
		switch (desc.type)
		{
		case SidebarType::LEFT:
		{
			const int x = current_width - desc.width;
			const int y = desc.upper_padding;
			return ImVec2(x, y);
		}
		case SidebarType::RIGHT:
		{
			const int x = desc.screen_width - current_width;
			const int y = desc.upper_padding;
			return ImVec2(x, y);
		}
		}

		throw std::runtime_error("Unknown sidebar type");
	};

	// Show button to open/close sidebar
	// return: Whether or not the button pressed
	auto show_open_close_button = [&get_toggle_button_pos, &desc, &open, &current_width]()
	{
		bool pressed = false;

		const bool should_show_open_button = !open;	// 開いていれば"閉じる"ボタンを表示
		const ImTextureID& texid = should_show_open_button ? desc.texture_id_open : desc.texture_id_close;
		const ImVec2& uv0 = should_show_open_button ? desc.uv0_open : desc.uv0_close;
		const ImVec2& uv1 = should_show_open_button ? desc.uv1_open : desc.uv1_close;
		const bool is_button_active = (!open && current_width == 0) || (open && current_width == desc.width);
		const ImGuiWindowFlags button_window_flags = desc.flags_button | (is_button_active ? 0 : ImGuiWindowFlags_NoMouseInputs);

		ImGui::SetNextWindowPos(get_toggle_button_pos());
		ImGui::SetNextWindowSize(ImVec2(desc.button_size, desc.button_size));
		if (ImGui::Begin(GetSidebarToggleButtonWindowName(desc.type), NULL, button_window_flags))
		{
			pressed = ImGui::ImageButton("button", texid, ImVec2(desc.button_size, desc.button_size), uv0, uv1);
		}
		ImGui::End();

		return pressed;
	};

	// return value
	bool is_sidebar_fully_open = false;

	// Tick window position
	if (open)
	{
		// opening
		if (current_width < desc.width)
		{
			current_width += desc.delta_width;
			current_width = clamp(current_width, 0, desc.width);
			is_sidebar_fully_open = false;
			show_open_close_button();
		}
		// fully-open
		else
		{
			is_sidebar_fully_open = true;
			if (show_open_close_button())
			{
				open = !open;
			}
		}

	}
	else
	{
		// closing
		if (current_width > 0)
		{
			current_width -= desc.delta_width;
			current_width = clamp(current_width, 0, desc.width);
			is_sidebar_fully_open = false;
			show_open_close_button();
		}
		// fully-closed
		else
		{
			is_sidebar_fully_open = false;
			if (show_open_close_button())
			{
				open = !open;
			}
		}
	}

	
	if (!is_sidebar_fully_open)
	{
		// IsWindowHoveredに応答するためのダミーウィンドウを表示

		ImGui::SetNextWindowPos(get_sidebar_body_pos());
		ImGui::SetNextWindowSize(ImVec2(desc.width, desc.height));
		ImGui::SetNextWindowBgAlpha(0);
		ImGuiWindowFlags dummy_window_flags = 0;
		dummy_window_flags |= ImGuiWindowFlags_NoTitleBar;
		dummy_window_flags |= ImGuiWindowFlags_NoMove;
		dummy_window_flags |= ImGuiWindowFlags_NoCollapse;
		dummy_window_flags |= ImGuiWindowFlags_NoScrollbar;
		dummy_window_flags |= ImGuiWindowFlags_NoResize;
		dummy_window_flags |= ImGuiWindowFlags_NoNav;
		ImGui::Begin("##", NULL, dummy_window_flags);
		ImGui::End();
	}

	ImGui::SetNextWindowPos(get_sidebar_body_pos());
	ImGui::SetNextWindowSize(ImVec2(desc.width, desc.height));
	const ImGuiWindowFlags sidebar_window_flags = desc.flags_sidebar | (is_sidebar_fully_open ? 0 : ImGuiWindowFlags_NoMouseInputs);
	ImGui::Begin(name, NULL, sidebar_window_flags);

	return is_sidebar_fully_open;
}

void ImGui::EndSideBar()
{
	ImGui::End();
}

bool ImGui::ImageRadioButton(const char* str, int& v, const int v_button, ImTextureID user_texture_id, const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& bg_col, const ImVec4& tint_pressed, const ImVec4& tint_released)
{
	bool pressed = (v == v_button);

	ImVec4 tint = pressed ? tint_pressed : tint_released;
	pressed = ImGui::ImageButton(str, user_texture_id, image_size, uv0, uv1, bg_col*tint, tint);

	if (pressed)
	{
		v = v_button;
	}

	return pressed;
}

bool ImGui::ImageRadioButton(const char* str, int& v, const int v_button, ImTextureID user_texture_id, const ImVec2& image_size, const float padding, const ImVec4 padding_color_pressed, const ImVec4 padding_color_released, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& bg_col, const ImVec4& tint_pressed, const ImVec4& tint_released)
{
	bool pressed = (v == v_button);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));
	ImGui::PushStyleColor(ImGuiCol_Button, pressed ? padding_color_pressed : padding_color_released);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, pressed ? padding_color_pressed : padding_color_released);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, pressed ? padding_color_pressed : padding_color_released);

	pressed = ImageRadioButton(str, v, v_button, user_texture_id, image_size, uv0, uv1, bg_col, tint_pressed, tint_released);
	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
	return pressed;
}

void ImGui::PushStyleColorU32_rgba(ImGuiCol idx, const ImU32 rgba)
{
	ImGui::PushStyleColor(idx, ImGui::ColorConvertU32ToFloat4(ImGui::SwizzleImU32_rgba_abgr(rgba)));
}

void ImGui::PushStyleColors_Button(const ImVec4& base, const float brightness_hovered, const float brightness_active)
{
	ImGui::PushStyleColor(ImGuiCol_Button, base);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, base * ImVec4(brightness_hovered, brightness_hovered, brightness_hovered, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, base * ImVec4(brightness_active, brightness_active, brightness_active, 1.f));
}

void ImGui::PopStyleColors_Button()
{
	ImGui::PopStyleColor(3);
}

void ImGui::Image(const Texture& texture, const ImVec2& image_size, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGui::Image(texture.im_tex_id, image_size, texture.uv0, texture.uv1, tint_col, border_col);
}

bool ImGui::ImageButton(const char* str_id, const Texture& texture, const ImVec2& button_size)
{
	return ImGui::ImageButton(str_id, texture.im_tex_id, button_size, texture.uv0, texture.uv1);
}

bool ImGui::IsAnyWindowHovered()
{
	return ImGui::IsWindowHovered(
		ImGuiHoveredFlags_AnyWindow |
		ImGuiHoveredFlags_AllowWhenBlockedByActiveItem
	);
}

bool ImGui::IsAnyPopupOpen()
{
	return ImGui::IsPopupOpen("##", ImGuiPopupFlags_AnyPopup);
}

ImVec2 ImGui::GetScreenSize()
{
	int screen_width, screen_height;
	DxLib::GetWindowSize(&screen_width, &screen_height);
	return ImVec2(screen_width, screen_height);
}
