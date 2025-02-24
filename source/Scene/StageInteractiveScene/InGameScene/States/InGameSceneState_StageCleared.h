#pragma once

#include "InGameSceneState.h"

class InGameSceneState_StageCleared : public InGameSceneState
{
public:
	InGameSceneState_StageCleared();
	virtual ~InGameSceneState_StageCleared() {}

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	//virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	//virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

private:
	void OnPlayerGoalSequenceFinished(ParentSceneClass& parent_scene);

	int CalculateAdditionalScore(ParentSceneClass& parent_scene) const;

	bool _is_player_goal_sequence_finished;
	size_t _selected_button;
	int _font_handle_stage_clear;
	int _font_handle_score;
	int _font_handle_buttons;
	std::shared_ptr<SoundInstance> _sound_instance_jingle;
};