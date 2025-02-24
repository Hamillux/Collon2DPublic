#include "TestSceneImpl_3.h"
#include <DxLib.h>
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

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

	// ParamEditComponentのデモ
	{
		int x = 1;
		auto validator_positive = std::make_shared<EditParamValidator_ClampMin<EditParamType::INT>>(1);	// コンストラクタの引数は最小値
		auto child_0 =
			std::make_shared<ParamEditNode<EditParamType::INT>>("Scalar", x, [&x](const int& value) {x = value; }, validator_positive);

		MasterDataID item_id = 1;
		auto child_1 =
			std::make_shared<ParamEditNode<EditParamType::ITEM_ID>>("Item", item_id, [&item_id](const MasterDataID& value) {item_id = value; });

		MasterDataID block_skin_id = 1;
		auto child_2 =
			std::make_shared <ParamEditNode<EditParamType::BLOCK_SKIN>>("BlockSkin", block_skin_id, [&block_skin_id](const MasterDataID& value) {block_skin_id = value; });

		std::array<float, 3> arr = { 1.0f, 2.0f, 3.0f };
		auto child_3 =
			std::make_shared<ParamEditNode<EditParamType::FLOAT3>>("Array", arr, [&arr](const std::array<float, 3>& value) {arr = value; });

		std::string button_text = "ButtonText";
		auto validator_str_length_10 = std::make_shared<EditParamValidator_MaxStringLength>(10);	// コンストラクタの引数は最大文字数
		auto grand_child_4_0 = 
			std::make_shared<ParamEditNode<EditParamType::STRING>>("Text", button_text, [&button_text](const std::string& value) {button_text = value; }, validator_str_length_10);

		bool button_active = true;
		auto grand_child_4_1 =
			std::make_shared<ParamEditNode<EditParamType::BOOL>>("Active", button_active, [&button_active](const bool& value) {button_active = value; });

		std::array<float, 4> button_color = { 1.0f, 0.f, 0.f, 1.0f };
		auto grand_child_4_2 =
			std::make_shared<ParamEditNode<EditParamType::COLOR4>>("Color", button_color, [&button_color](const std::array<float, 4>& value) {button_color = value; });

		auto child_4 = std::make_shared<ParamEditGroup>(
			"Button",
			std::vector<std::shared_ptr<ParamEditComponent>>{ grand_child_4_0, grand_child_4_1, grand_child_4_2 }
		);

		auto root_group = std::make_shared<ParamEditGroup>(
			"Root",
			std::vector<std::shared_ptr<ParamEditComponent>>{ child_0, child_1, child_2, child_3, child_4 }
		);

		ImGui::PushFont(FontManager::GetInstance().GetFont(2));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(.5, .5, .5, 1));

		ImGui::Begin("ParamEditComponent Demo");
		root_group->ShowAsImguiTreeElement();
		ImGui::End();

		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		
	}

	

	return GetSceneType();
}

void TestSceneImpl_3::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);
}
