#pragma once

#include "StageEditorSceneState.h"

class StageEditorSceneState_ResizeStage : public StageEditorSceneState
{
public:
	StageEditorSceneState_ResizeStage();
	virtual ~StageEditorSceneState_ResizeStage();

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StageEditorSceneState interface
public:
	virtual void ShowControls(StageEditorScene& parent_editor_scene) override;
	//~End StageEditorSceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface

private:
	void CancelResize(StageEditorScene& parent_scene);

	int _from_length;
	CameraParams _last_camera_params;
};