#pragma once

#include "States/PlayerState.h"
#include "States/PlayerState_Emerging.h"
#include "States/PlayerState_Playing.h"
#include "States/PlayerState_Dead.h"
#include "States/PlayerState_Goaled.h"

#include <stack>
#include <memory>

class PlayerStateStack 
{
public:
	PlayerStateStack() {}
	~PlayerStateStack() {}

	void ChangeState(Player& player, std::shared_ptr<PlayerState> state)
	{
		if (!_state_stack.empty())
		{
			_state_stack.top()->OnLeave(player);
			_state_stack.pop();
		}

		_state_stack.push(state);
		_state_stack.top()->OnEnter(player);
	}

	void Tick(Player& player, float delta_seconds)
	{
		auto current_state = GetCurrentState();
		current_state->HandleInput(player);
		current_state->Tick(player, delta_seconds);
		if (current_state->GetNextState() != nullptr)
		{
			ChangeState(player, GetCurrentState()->GetNextState());
		}
	}

	void Draw(Player& player, const CameraParams& camera_params)
	{
		GetCurrentState()->Draw(player, camera_params);
	}

	void DrawForeground(Player& player, const CanvasInfo& canvas_info) 
	{
		GetCurrentState()->DrawForeground(player, canvas_info);
	}

	void StompCharacter(Player& player, Character* const stomped_character)
	{
		GetCurrentState()->StompCharacter(player, stomped_character);
	}

private:
	std::shared_ptr<PlayerState> GetCurrentState() const
	{
		return _state_stack.top();
	}
	std::stack<std::shared_ptr<PlayerState>> _state_stack;
};