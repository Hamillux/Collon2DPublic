#pragma once

#include "Actor/Character/Player/Player.h"
#include "Input//DeviceInput.h"

class PlayerState : public EventListener
{
public:
	PlayerState() {}
	virtual ~PlayerState() {}

	//~ Begin PlayerState interface
public:
	// _next_stateをリセット
	virtual void OnEnter(Player& player)
	{
		_next_state.reset();
	}
	// 何もしない
	virtual void OnLeave(Player& player) {}
	// 何もしない
	virtual void Tick(Player& player, float delta_seconds) {}
	// 何もしない
	virtual void Draw(Player& player, const CameraParams& camera_params) {}
	// 何もしない
	virtual void DrawForeground(Player& player, const CanvasInfo& canvas_info) {}
	// 何もしない
	virtual void HandleInput(Player& player) {}

	virtual void StompCharacter(Player& player, Character * const stomped_character) {}
	//~ End PlayerState interface

public:
	std::shared_ptr<PlayerState> GetNextState() const
	{
		return _next_state;
	}

protected:
	void SetNextState(std::shared_ptr<PlayerState> next_state)
	{
		_next_state = next_state;
	}

private:
	std::shared_ptr<PlayerState> _next_state;
};