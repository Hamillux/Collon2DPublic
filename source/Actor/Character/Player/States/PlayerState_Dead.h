#pragma once
#include "PlayerState.h"

class PlayerState_Dead : public PlayerState
{
public:
	PlayerState_Dead()
		: _timer(0.f)
		, _vanishment_time_passed(false)
		, _hidden_time_passed(false)
	{}
	virtual ~PlayerState_Dead() {}

	//~ Begin PlayerState interface
public:
	virtual void OnEnter(Player& player) override;
	virtual void OnLeave(Player& player) override;
	virtual void Tick(Player& player, float delta_seconds) override;
	// virtual void Draw(Player& player, const CameraParams& camera_params) override;
	virtual void DrawForeground(Player& player, const CanvasInfo& canvas_info) override;
	// virtual void HandleInput(Player& player) override;
	//~ End PlayerState interface

private:
	float _timer;
	bool _vanishment_time_passed;
	bool _hidden_time_passed;
};