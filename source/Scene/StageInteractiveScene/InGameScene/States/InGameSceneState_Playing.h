#pragma once

#include "InGameSceneState.h"

struct CharacterDeathInfo;

class InGameSceneState_Playing : public InGameSceneState
{ 
public:
	InGameSceneState_Playing();
	virtual ~InGameSceneState_Playing() {}

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;

	// プレイヤーが画面内に収まるようにスクリーンパラメータを調整する
	virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	// virtual bool ShouldDestroyPreviousState() const override;
	//~ End StackableSceneState interface

	void OnPlayerDead(ParentSceneClass& parent_scene, const CharacterDeathInfo* death_info);
	void OnPlayerReachedGoal(ParentSceneClass& parent_scene);
	void OnTimeUp(ParentSceneClass& parent_scene);

	float _reset_timer;
	std::vector<int> _spinner_icons;
};