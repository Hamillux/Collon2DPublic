#include "EditorMessageManager.h"
#include "GameSystems/FontManager.h"
#include <DxLib.h>
#include <imgui.h>
#include <deque>

namespace
{
	MasterDataID MESSAGE_FONT_ID = 3;
}

EditorMessageManager::EditorMessageManager()
{
}

EditorMessageManager::~EditorMessageManager()
{
}

void EditorMessageManager::Initialize(const EditorMessageManagerDesc& desc)
{
    _desc = desc;
}

void EditorMessageManager::Tick(const float delta_seconds)
{
    for (auto& message : _messages)
    {
        message.remain_time -= delta_seconds;
    }

    while (!_messages.empty() && _messages.front().remain_time <= 0.0f)
    {
        _messages.pop_front();
    }

	ShowMessages();
}

void EditorMessageManager::Finalize()
{
    _messages.clear();
}

void EditorMessageManager::EnqueueMessage(const std::string& message)
{
    Message new_message;
    new_message.message = message;
    new_message.remain_time = _desc.display_time;
    _messages.push_back(new_message);

	while (_messages.size() > _desc.max_message_count)
	{
		_messages.pop_front();
	}
}

void EditorMessageManager::ShowMessages()
{
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    window_flags |= ImGuiWindowFlags_NoInputs;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    

    ImGui::SetNextWindowPos(_desc.window_pos, ImGuiCond_Always, _desc.window_pivot);
    ImGui::SetNextWindowSize(_desc.GetWindowSize());
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    constexpr int NUM_WINDOW_STYLE_VAR = 2;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("EditorMessageManager", NULL, window_flags);
    {
        constexpr int NUM_CHILD_WND_STYLE_COLOR = 3;
        ImGui::PushStyleColor(ImGuiCol_ChildBg, _desc.card_back_color);
        ImGui::PushStyleColor(ImGuiCol_Text, _desc.card_front_color);
        ImGui::PushStyleColor(ImGuiCol_Border, _desc.card_border_color);
        
        constexpr int NUM_CHILD_WND_STYLE_VAR = 2;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));

        ImVec2 MESSAGE_CARD_SIZE = _desc.message_card_size;
        ImVec2 MESSAGE_TYPE_ICON_SIZE = _desc.message_type_icon_size;

        for (int i = 0; i < _messages.size(); i++)
        {
            const ImVec2 child_pos = _desc.message_card_size * ImVec2(0, _desc.max_message_count - _messages.size() + i);
			ImGui::SetNextWindowPos(ImGui::GetWindowPos() + child_pos);
            ImGui::PushID(i);
			ImGui::BeginChild("##", MESSAGE_CARD_SIZE, ImGuiChildFlags_Border, window_flags);
			ImGui::PushFont(FontManager::GetInstance().GetFont(MESSAGE_FONT_ID));
			ImGui::TextWrapped(_messages[i].message.c_str());
			ImGui::PopFont();
			ImGui::EndChild();
            ImGui::PopID();
            if (_messages.size() > 3)
            {
                int x = 0;
            }
        }

        ImGui::PopStyleVar(NUM_CHILD_WND_STYLE_VAR);
        ImGui::PopStyleColor(NUM_CHILD_WND_STYLE_COLOR);
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(NUM_WINDOW_STYLE_VAR);
}
