#pragma once

#include "StageEditorSceneState.h"

class StageEditorSceneState_StageSettings : public StageEditorSceneState
{
public:
	StageEditorSceneState_StageSettings();
	virtual ~StageEditorSceneState_StageSettings();

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;

	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	// virtual void Draw(ParentSceneClass& parent_scene) override;
	// virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	// virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	// virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface

	void ShowEditorSettingsModal(StageEditorScene& parent_scene);

	std::shared_ptr<ParamEditGroup> param_edit_group_stage_settings;
};