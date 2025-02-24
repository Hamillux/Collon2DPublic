#pragma once

#include "Scene/SceneState/SceneState.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"

class InGameSceneState : public StackableSceneState<InGameScene>
{
public:
	InGameSceneState() {}
	virtual ~InGameSceneState() {};

	//~ Begin SceneState interface
public:
	//virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	//virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	//virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	//virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	//virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	// virtual bool ShouldDestroyPreviousState() const override;
	//~ End StackableSceneState interface
};