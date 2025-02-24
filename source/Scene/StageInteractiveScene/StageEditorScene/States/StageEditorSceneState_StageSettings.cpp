#include "StageEditorSceneState_StageSettings.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace {
	constexpr const char* id_editor_settings_modal = "editor_settings_modal";
}

StageEditorSceneState_StageSettings::StageEditorSceneState_StageSettings()
{
}

StageEditorSceneState_StageSettings::~StageEditorSceneState_StageSettings()
{
}

void StageEditorSceneState_StageSettings::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	parent_scene._should_show_controls = false;

	param_edit_group_stage_settings = std::make_shared<ParamEditGroup>("", 3);

	auto stage_name = AddChildParamEditNodeToGroup<EditParamType::STRING>(
		param_edit_group_stage_settings,
		parent_scene._command_history,
		u8"ステージ名",
		parent_scene.GetStageRef().GetStageNameRef(),
		std::make_shared<EditParamValidator_MaxStringLength>(10)
	);

	auto stage_description = AddChildParamEditNodeToGroup<EditParamType::STRING>(
		param_edit_group_stage_settings,
		parent_scene._command_history,
		u8"ステージの説明",
		parent_scene.GetStageRef().GetDescriptionRef(),
		std::make_shared<EditParamValidator_MaxStringLength>(70)
	);

	auto time_limit = AddChildParamEditNodeToGroup<EditParamType::INT>(
		param_edit_group_stage_settings,
		parent_scene._command_history,
		u8"制限時間",
		parent_scene.GetStageRef().GetTimeLimitRef(),
		std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(1, 9999)
	);

	ImGui::OpenPopup(id_editor_settings_modal);
}

void StageEditorSceneState_StageSettings::OnLeaveState(ParentSceneClass& parent_scene)
{
	ImGui::CloseCurrentPopup();

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorSceneState_StageSettings::ParentSceneClass>> 
	StageEditorSceneState_StageSettings::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	if (!ImGui::IsAnyItemActive() && DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		ResumeLastState(parent_scene);
	}

	ShowEditorSettingsModal(parent_scene);

	return ret;
}

void StageEditorSceneState_StageSettings::ShowEditorSettingsModal(StageEditorScene& parent_scene)
{
	ImGui::PushFont(FontManager::GetInstance().GetFont(2));
	constexpr int NUM_POP_STYLE_COLOR = 3;
	ImGui::PushStyleColor(ImGuiCol_PopupBg, StageEditorColor::BackColor(0));
	ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.5));
	ImGui::PushStyleColors_Button(StageEditorColor::BackColor(1));

	constexpr ImVec2 modal_size = ImVec2(WINDOW_SIZE_X * 0.75, WINDOW_SIZE_Y * 0.75);
	constexpr ImVec2 modal_pos = ImVec2(WINDOW_SIZE_X / 2 - modal_size.x / 2, WINDOW_SIZE_Y / 2 - modal_size.y / 2);
	
	ImGui::SetNextWindowSize(modal_size);
	ImGui::SetNextWindowPos(modal_pos);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	if (ImGui::BeginPopupModal(id_editor_settings_modal, NULL, flags))
	{
		ImGui::Text(u8"ステージ設定");

		param_edit_group_stage_settings->ShowAsImguiTreeElement();

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 50);
		if (ImGui::Button(u8"閉じる", ImVec2(100, 30)))
		{
			ResumeLastState(parent_scene);
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(NUM_POP_STYLE_COLOR);
	ImGui::PopStyleColors_Button();
	ImGui::PopFont();
}