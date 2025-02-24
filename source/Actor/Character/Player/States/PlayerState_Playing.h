#pragma once

#include "PlayerState.h"

class PlayerState_Playing : public PlayerState
{
public:
	PlayerState_Playing()
		: _stomping_jump_timer(0.f)
		, _is_jump_key_active(false)
		, _should_show_controls(true)
	{}
	virtual ~PlayerState_Playing() {}

	//~ Begin PlayerState interface
public:
	virtual void OnEnter(Player& player) override;
	virtual void OnLeave(Player& player) override;
	virtual void Tick(Player& player, float delta_seconds) override;
	// virtual void Draw(Player& player, const CameraParams& camera_params) override;
	virtual void DrawForeground(Player& player, const CanvasInfo& canvas_info) override;
	virtual void HandleInput(Player& player) override;
	virtual void StompCharacter(Player& player, Character * const stomped_character) override;
	//~ End PlayerState interface

private:

	static constexpr float STOMPING_JUMP_TIME = 0.3f;	// 踏みつけジャンプ受付時間
	float _stomping_jump_timer;	// (0, STOMPING_JUMP_TIME]の間踏みつけジャンプが受付可能
	bool _is_jump_key_active;
	bool CanStompingJump() const
	{
		return _stomping_jump_timer > 0.f && _stomping_jump_timer <= STOMPING_JUMP_TIME;
	}

	bool _should_show_controls;		// 操作説明を表示するか
	void ShowControls();
};