#include "PlayerState_Emerging.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"

void PlayerState_Emerging::OnEnter(Player& player)
{
	__super::OnEnter(player);
	if (player.GetScene()->GetSceneType() != SceneType::INGAME_SCENE)
	{
		player._ingame_scene_ref = nullptr;
		return;
	}

	_timer = 0.f;
	player._player_animator->SetVisibility(false);
	player._ingame_scene_ref = static_cast<InGameScene*>(player.GetScene());
	player._ingame_scene_ref->SetTimerStopped(true);

	Transform anim_transform(player.GetActorWorldPosition() + Vector2D{0.f, -32.f}, 0.f);
	player._ingame_scene_ref->PlayAnimation(AnimPlayInfo{ 22, 1.f, 1.f, FALSE, FALSE }, anim_transform	);
}

void PlayerState_Emerging::OnLeave(Player& player)
{
	player.player_events.OnPlayerEmergenceSequenceFinished.Dispatch();

	__super::OnLeave(player);
}

void PlayerState_Emerging::Tick(Player& player, float delta_seconds)
{
	if (player._ingame_scene_ref == nullptr)
	{
		return;
	}

	__super::Tick(player, delta_seconds);
	
	_timer += delta_seconds;
	if (_timer > 0.5f)
	{
		player._player_animator->SetVisibility(true);
	}
	if (_timer > 1.f)
	{
		player._ingame_scene_ref->SetTimerStopped(false);
		SetNextState(player._player_states.playing);
	}
}
