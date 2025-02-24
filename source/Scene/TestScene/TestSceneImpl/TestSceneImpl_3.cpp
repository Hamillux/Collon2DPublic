#include "TestSceneImpl_3.h"
#include <DxLib.h>
#include "GameSystems/FontManager.h"

TestSceneImpl_3::TestSceneImpl_3()
{
}

TestSceneImpl_3::~TestSceneImpl_3()
{
}

void TestSceneImpl_3::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);
}

SceneType TestSceneImpl_3::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);

	ImGui::PushFont(FontManager::GetInstance().GetFont(1));

	if(ImGui::Begin("InputJapaneseText"))
	{
		static char buf[256] = "";
		static char ref[256] = "English";
		static char u8ref[256] = u8"English";
		ImGui::InputText("InputText", buf, sizeof(buf));
		ImGui::Text("InputText: %s", buf);
		ImGui::Text(ref);
		ImGui::Text(u8ref);
	}
	ImGui::End();

	ImGui::PopFont();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoInputs;
	window_flags |= ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowPos(ImVec2(5, 5));
	ImGui::SetNextWindowSize(ImVec2(400,400));
	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
	constexpr int NUM_WINDOW_STYLE_VAR = 2;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::Begin("TestSceneImpl_3", NULL, window_flags);
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 0, 1));
		constexpr int NUM_CHILD_WND_STYLE_VAR = 2;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4,4));

		ImVec2 MESSAGE_CARD_SIZE = ImVec2(300,100);
		ImVec2 MESSAGE_TYPE_ICON_SIZE = ImVec2(48, 48);

		ImGui::BeginChild("##1", MESSAGE_CARD_SIZE, ImGuiChildFlags_Border, window_flags);
		// 垂直方向中央揃え
		ImGui::SetCursorPosY((MESSAGE_CARD_SIZE.y - MESSAGE_TYPE_ICON_SIZE.y) / 2);

		ImGui::Button("MessageType", MESSAGE_TYPE_ICON_SIZE);
		ImGui::SameLine();
		ImGui::SetCursorPosY(0);
		ImGui::TextWrapped(
			"MESSAGE WILL BE SHOWWILL ");
		ImGui::EndChild();

		ImGui::PopStyleVar(NUM_CHILD_WND_STYLE_VAR);
		ImGui::PopStyleColor();
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(NUM_WINDOW_STYLE_VAR);

	ImGui::Begin("Debug");
	ImGui::Text("IsAnyWindowHovered?: %d", ImGui::IsAnyWindowHovered());
	ImGui::Text("IsAnyItemHovered?: %d", ImGui::IsAnyItemHovered());
	ImGui::End();

	

	return GetSceneType();
}

void TestSceneImpl_3::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);
}
