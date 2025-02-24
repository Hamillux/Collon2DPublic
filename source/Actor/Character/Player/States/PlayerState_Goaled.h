#pragma once

#include "PlayerState.h"

class PlayerState_Goaled : public PlayerState
{
public:
	PlayerState_Goaled();
	virtual ~PlayerState_Goaled() {}

	//~ Begin PlayerState interface
public:
	virtual void OnEnter(Player& player) override;
	virtual void OnLeave(Player& player) override;
	virtual void Tick(Player& player, float delta_seconds) override;
	// virtual void Draw(Player& player, const CameraParams& camera_params) override;
	// virtual void DrawForeground(Player& player, const CanvasInfo& canvas_info) override;
	virtual void HandleInput(Player& player) override;
	//~ End PlayerState interface

private:
	void Vanish(Player& player);
	float _timer;
	float _player_alpha;
};