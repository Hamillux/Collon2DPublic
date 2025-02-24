#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorScene.h"

class StageEditorSceneState_Paused : public StageEditorSceneState
{
	static constexpr const char* STR_ID_PAUSE_MODAL = "stage_editor_scene_paused_modal";

public:
	StageEditorSceneState_Paused();
	virtual ~StageEditorSceneState_Paused();

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
};