#include "PlayerState_Goaled.h"
#include "Actor/Character/Player/Player.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include "Component/CharacterMovementComponent.h"

PlayerState_Goaled::PlayerState_Goaled()
	: _timer(0.f)
	, _player_alpha(1.f)
{
}

void PlayerState_Goaled::OnEnter(Player& player)
{
	__super::OnEnter(player);

	player.GetBodyCollider()->SetHitObjectTypes({ CollisionObjectType::GROUND });
	player.StopJumping();
	player.SetVelocity(player.GetVelocity() * Vector2D { 0, 0 });

	player._ingame_scene_ref = static_cast<InGameScene*>(player.GetScene());
	player._ingame_scene_ref->SetTimerStopped(true);

	constexpr float VANISHMENT_START_TIME = 1.f;
	player._ingame_scene_ref->MakeDelayedEventWorld(&player, VANISHMENT_START_TIME, [this, &player]() 
		{
			Vanish(player);
		}
	);
}

void PlayerState_Goaled::OnLeave(Player& player)
{

	__super::OnLeave(player);
}

void PlayerState_Goaled::Tick(Player& player, float delta_seconds)
{
	__super::Tick(player, delta_seconds);

	_timer += delta_seconds;
}

void PlayerState_Goaled::HandleInput(Player& player)
{
}

void PlayerState_Goaled::Vanish(Player& player)
{
	Transform anim_transform(player.GetActorWorldPosition() + Vector2D{ 0.f, -32.f }, 0.f);
	player._ingame_scene_ref->PlayAnimation(AnimPlayInfo{ 22, 1.f, 1.f, FALSE, FALSE }, anim_transform);
	player._ingame_scene_ref->MakeDelayedEventWorld(&player, 0.5f, [&player]()
		{
			player._player_animator->SetVisibility(false);
		}
	);
	player._ingame_scene_ref->MakeDelayedEventWorld(&player, 1.f, [&player]()
		{
			player.player_events.OnPlayerGoalSequenceFinished.Dispatch();
		}
	);
}
