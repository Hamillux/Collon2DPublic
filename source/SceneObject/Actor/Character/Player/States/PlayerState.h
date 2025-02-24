#pragma once

#include "SceneObject/Actor/Character/Player/Player.h"
#include "Input//DeviceInput.h"

class PlayerState : public EventListener
{
public:
	PlayerState() {}
	virtual ~PlayerState() {}

	//~ Begin PlayerState interface
public:
	// _next_state‚ðƒŠƒZƒbƒg
	virtual void OnEnter(Player& player)
	{
		_next_state.reset();
	}
	// ‰½‚à‚µ‚È‚¢
	virtual void OnLeave(Player& player) {}
	// ‰½‚à‚µ‚È‚¢
	virtual void Tick(Player& player, float delta_seconds) {}
	// ‰½‚à‚µ‚È‚¢
	virtual void Draw(Player& player, const ScreenParams& screen_params) {}
	// ‰½‚à‚µ‚È‚¢
	virtual void DrawForeground(Player& player, const CanvasInfo& canvas_info) {}
	// ‰½‚à‚µ‚È‚¢
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