#include "StageEditorSceneState_EditorSettings.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"
#include "GameSystems/GameConfig/GameConfig.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace
{
	static constexpr const char* STR_ID_EDITOR_SETTINGS_MODAL = "stage_editor_scene_editor_settings_modal";
}

StageEditorSceneState_EditorSettings::StageEditorSceneState_EditorSettings()
	: _should_reload_config(false)
{
}

StageEditorSceneState_EditorSettings::~StageEditorSceneState_EditorSettings()
{
}

void StageEditorSceneState_EditorSettings::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	const std::shared_ptr<StageEditorConfig> config = GameConfig::GetInstance().GetConfigItem<StageEditorConfig>();

	{
		std::vector<std::shared_ptr<ParamEditComponent>> child_list(1);

		const auto node_grid_color = std::make_shared<ParamEditNode<EditParamType::COLOR4>>(
			u8"GridColor",
			config->grid_color,
			[config](const std::array<float, 4>& validate_value) 
			{
				config->grid_color = validate_value;
			}
		);
		child_list.at(0) = node_grid_color;

		_param_edit_group_general = std::make_unique<ParamEditGroup>("root", child_list);
	}

	ImGui::OpenPopup(STR_ID_EDITOR_SETTINGS_MODAL);
}

void StageEditorSceneState_EditorSettings::OnLeaveState(ParentSceneClass& parent_scene)
{
	if (ImGui::IsPopupOpen(STR_ID_EDITOR_SETTINGS_MODAL))
	{
		ImGui::BeginPopupModal(STR_ID_EDITOR_SETTINGS_MODAL);
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorSceneState_EditorSettings::ParentSceneClass>> StageEditorSceneState_EditorSettings::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	_should_reload_config = false;

	ShowEditorSettingsPopup(parent_scene);

	if (_should_reload_config)
	{
		parent_scene.LoadStageEditorConfig(*GameConfig::GetInstance().GetConfigItem<StageEditorConfig>());
	}

	return ret;
}

void StageEditorSceneState_EditorSettings::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		ResumeLastState(parent_scene);
	}
}

void StageEditorSceneState_EditorSettings::ShowEditorSettingsPopup(ParentSceneClass& parent_scene)
{
	ImGui::PushFont(FontManager::GetInstance().GetFont(2));

	constexpr int NUM_POP_STYLE_COLOR = 8;
	{
		constexpr ImVec4 TAB_COLOR_BASE = StageEditorColor::BackColor(3);
		constexpr ImVec4 TAB_COLOR_DARK_1 = TAB_COLOR_BASE * ImVec4{ 0.75, 0.75, 0.75, 1.0 };
		constexpr ImVec4 TAB_COLOR_DARK_2 = TAB_COLOR_BASE * ImVec4{ 0.5, 0.5, 0.5, 1.0 };

		ImGui::PushStyleColor(ImGuiCol_PopupBg, StageEditorColor::BackColor(0));
		ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.5));
		ImGui::PushStyleColor(ImGuiCol_Tab, TAB_COLOR_DARK_1);
		ImGui::PushStyleColor(ImGuiCol_TabDimmed, TAB_COLOR_DARK_1);
		ImGui::PushStyleColor(ImGuiCol_TabActive, TAB_COLOR_BASE);
		ImGui::PushStyleColor(ImGuiCol_TabDimmedSelected, TAB_COLOR_BASE);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, TAB_COLOR_DARK_2);
	}

	ImGui::PushStyleColors_Button(StageEditorColor::BackColor(1));
	
	constexpr int NUM_POP_STYLE_VAR = 1;
	{
		ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 3.f);
	}

	constexpr ImVec2 modal_size = ImVec2(WINDOW_SIZE_X * 0.75, WINDOW_SIZE_Y * 0.75);
	constexpr ImVec2 modal_pos = ImVec2(WINDOW_SIZE_X / 2 - modal_size.x / 2, WINDOW_SIZE_Y / 2 - modal_size.y / 2);

	ImGui::SetNextWindowSize(modal_size);
	ImGui::SetNextWindowPos(modal_pos);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	if (ImGui::BeginPopupModal(STR_ID_EDITOR_SETTINGS_MODAL, NULL, flags))
	{
		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			ResumeLastState(parent_scene);
		}

		ImGui::Text(u8"エディタ設定");

		if (ImGui::BeginTabBar("EditorSettingsTabBar"))
		{
			// 一般設定タブ
			if (ImGui::BeginTabItem(u8"一般"))
			{
				ShowSettings_General(parent_scene);
				ImGui::EndTabItem();
			}

			// オーディオ設定タブ
			if (ImGui::BeginTabItem(u8"オーディオ"))
			{
				ShowSettings_Audio(parent_scene);
				ImGui::EndTabItem();
			}

			// 表示設定タブ
			/*if (ImGui::BeginTabItem(u8"キーコンフィグ"))
			{
				ShowSettings_KeyConfig(parent_scene);
				ImGui::EndTabItem();
			}*/

			ImGui::EndTabBar();
		}

		ImGui::Separator();
		
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 50);
		if (ImGui::Button(u8"閉じる", ImVec2(100, 30)))
		{
			ResumeLastState(parent_scene);
		}


		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(NUM_POP_STYLE_COLOR);
	ImGui::PopStyleColors_Button();
	ImGui::PopStyleVar(NUM_POP_STYLE_VAR);
	ImGui::PopFont();
}

void StageEditorSceneState_EditorSettings::ShowSettings_General(ParentSceneClass& parent_scene)
{
	_param_edit_group_general->ShowAsImguiTreeElement();
}

void StageEditorSceneState_EditorSettings::ShowSettings_Audio(ParentSceneClass& parent_scene)
{
	std::shared_ptr<StageEditorConfig> config = GameConfig::GetInstance().GetConfigItem<StageEditorConfig>();

	_should_reload_config |= ImGui::SliderInt("BGM", &config->sound_volume.bgm, 0, 100);

	if (ImGui::SliderInt("SE", &config->sound_volume.se, 0, 100))
	{
		_should_reload_config = true;
		parent_scene._editor_scene_sounds->se_place_actor->Play();
	}
}

void StageEditorSceneState_EditorSettings::ShowSettings_KeyConfig(ParentSceneClass& parent_scene)
{
	// 未実装
}
