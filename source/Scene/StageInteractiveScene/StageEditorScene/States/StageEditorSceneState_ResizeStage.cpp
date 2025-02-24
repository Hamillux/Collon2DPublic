#include "StageEditorSceneState_ResizeStage.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"

StageEditorSceneState_ResizeStage::StageEditorSceneState_ResizeStage()
	: _from_length(1)
	, _last_camera_params()
{
}

StageEditorSceneState_ResizeStage::~StageEditorSceneState_ResizeStage()
{
}

void StageEditorSceneState_ResizeStage::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	parent_scene._should_show_controls = false;
	_from_length = parent_scene.GetStageRef().GetStageLength();
	_last_camera_params = parent_scene._camera_params;
}

void StageEditorSceneState_ResizeStage::OnLeaveState(ParentSceneClass& parent_scene)
{
	const int to_length = parent_scene.GetStageRef().GetStageLength();
	if (_from_length != to_length)
	{
		// stage_editor_scene::CmdChangeStageLength
		parent_scene._command_history->Push(std::make_shared<stage_editor_scene::CmdChangeStageLength>(&parent_scene, _from_length, to_length));
	}

	parent_scene._camera_params = _last_camera_params;

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorScene>> StageEditorSceneState_ResizeStage::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	constexpr int NUM_POP_STYLE_COLOR = 2;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, StageEditorColor::BackColor(0));
	ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));

	ImGui::PushFont(FontManager::GetInstance().GetFont(3));

	ImGui::SetNextWindowPos(ImVec2{ WINDOW_SIZE_X / 2.f, 0.f }, ImGuiCond_Always, ImVec2{ 0.5f, 0.f });
	ImGui::SetNextWindowSize(ImVec2{ WINDOW_SIZE_X / 2.f, 120.f });
	ImGui::SetNextWindowBgAlpha(0.75f);

	ImGuiWindowFlags window_flags = 
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoFocusOnAppearing | 
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	if (ImGui::Begin("str_id_stage_resize_config", NULL, window_flags))
	{
		ImGui::SetCursorPosX(40);

		ImGui::Text(u8"ステージ長", parent_scene.GetStageRef().GetStageLength());
		ImGui::SameLine();
		{
			int new_length = parent_scene.GetStageRef().GetStageLength();
			ImGui::InputInt("##", &new_length);

			int min_length, max_length;
			parent_scene.GetStageRef().GetStageLengthRange(min_length, max_length);
			new_length = clamp(new_length, min_length, max_length);

			parent_scene.GetStageRef().SetStageLength(new_length);
		}

		// 「確定」「キャンセル」ボタン
		constexpr ImVec2 BUTTON_SIZE = { 120, 50 };
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
		ImGui::SetCursorPosX(200);
		if (ImGui::Button(u8"確定", BUTTON_SIZE))
		{
			ResumeLastState(parent_scene);
		}
		ImGui::SameLine();

		ImGui::SetCursorPosX(350);
		if (ImGui::Button(u8"キャンセル", BUTTON_SIZE))
		{
			CancelResize(parent_scene);
		}
	}
	ImGui::End();

	ImGui::PopFont();

	ImGui::PopStyleColor(NUM_POP_STYLE_COLOR);

	return ret;
}

void StageEditorSceneState_ResizeStage::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	parent_scene.DrawWorldGrid(0xCCCCCC, 128);
}

void StageEditorSceneState_ResizeStage::UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
{
	parent_scene._camera_params.screen_scale = parent_scene.GetMaxScreenScale();
	parent_scene._camera_params.world_offset.x = FLT_MAX;
}

void StageEditorSceneState_ResizeStage::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		CancelResize(parent_scene);
		return;
	}
}

void StageEditorSceneState_ResizeStage::ShowControls(StageEditorScene& parent_editor_scene)
{
}

void StageEditorSceneState_ResizeStage::CancelResize(StageEditorScene& parent_scene)
{
	parent_scene.GetStageRef().SetStageLength(_from_length);
	ResumeLastState(parent_scene);
}
