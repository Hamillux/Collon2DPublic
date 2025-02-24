#pragma once

#include "InGameSceneState.h"
#include <string>

struct CharacterDeathInfo;

class InGameSceneState_GameOver : public InGameSceneState
{
public:
	InGameSceneState_GameOver();
	virtual ~InGameSceneState_GameOver() {}

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	//virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	//virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	// virtual bool ShouldDestroyPreviousState() const override;
	//~ End StackableSceneState interface

public:
	void SetCauseOfDeath(const CharacterDeathInfo* death_info);

private:
	float _timer;
	float _fade_start_time;
	
	enum BUTTON : int
	{
		BUTTON_RETRY = 0,
		BUTTON_RETURN_TO_STAGE_SELECT = 1,
		BUTTON_RETURN_TO_TITLE = 2,
	};
	BUTTON _selected_button;

	std::unique_ptr<const CharacterDeathInfo> _player_death_info;

	std::shared_ptr<SoundInstance> _sound_instance_jingle;
};