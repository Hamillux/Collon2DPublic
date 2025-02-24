#include "StageEditorSceneState.h"

std::shared_ptr<SceneState<StageEditorSceneState::ParentSceneClass>> StageEditorSceneState::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	if(parent_scene._should_show_controls)
	{
		if (parent_scene.BeginControlsImGuiWindow())
		{
			ShowControls(parent_scene);
		}
		parent_scene.EndControlsImGuiWindow();
	}

	return ret;
}

void StageEditorSceneState::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (DeviceInput::IsPressed(KEY_INPUT_F1))
	{
		const bool showing_controls = parent_scene._should_show_controls;
		const bool is_expanded = parent_scene._is_control_window_expanded;
		if (!showing_controls)
		{
			parent_scene._should_show_controls = true;
			parent_scene._is_control_window_expanded = false;
		}
		else if (!is_expanded)
		{
			parent_scene._is_control_window_expanded = true;
		}
		else
		{
			parent_scene._should_show_controls = false;
			parent_scene._is_control_window_expanded= false;
		}
	}
}

void StageEditorSceneState::ShowControls(StageEditorScene& parent_editor_scene)
{
	const char* text = parent_editor_scene._is_control_window_expanded ? u8"F1: 操作説明を閉じる" : u8"F1: 操作説明を拡大";
	ImGui::Text(text);
}
