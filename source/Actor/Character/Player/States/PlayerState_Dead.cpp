#include "PlayerState_Dead.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "Component/CharacterMovementComponent.h"
#include "Scene/SceneBase.h"

namespace 
{
	AnimPlayInfo anim_play_info_vanishment{ MasterDataID(21), 1.f, 2.f, FALSE, FALSE };
	Vector2D anim_play_position_offset{0.f, -16.f};	// プレイヤーの位置からのオフセット
	constexpr float VANISHMENT_START_TIME = 0.5f;
	constexpr float HIDDEN_TIME = 1.f;
	constexpr float DEATH_SEQUENCE_END_TIME = 2.f;
}

void PlayerState_Dead::OnEnter(Player& player)
{
	__super::OnEnter(player);

	player.SetDeadAnimation();
	player.GetCharacterMovementComponent()->_gravity_scale = 0.f;
	player.SetVelocity(Vector2D{});
}

void PlayerState_Dead::OnLeave(Player& player)
{
	__super::OnLeave(player);
}

void PlayerState_Dead::Tick(Player& player, float delta_seconds)
{
	__super::Tick(player, delta_seconds);

	if (!_vanishment_time_passed && _timer >= VANISHMENT_START_TIME)
	{
		const Transform transform{ player.GetActorWorldPosition() + anim_play_position_offset, 0.f };
		player.GetScene()->PlayAnimation(anim_play_info_vanishment, transform);
		_vanishment_time_passed = true;
	}

	if (!_hidden_time_passed && _timer >= HIDDEN_TIME)
	{
		player._player_animator->SetVisibility(false);
		_hidden_time_passed = true;
	}

	if (_timer >= DEATH_SEQUENCE_END_TIME)
	{
		player.player_events.OnPlayerDeathSequenceFinished.Dispatch();
		player.MarkAsShouldDestroy();
	}
	
	_timer += delta_seconds;
}

void PlayerState_Dead::DrawForeground(Player& player, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(player, canvas_info);
	player.DrawPlayerUI(canvas_info);
}
