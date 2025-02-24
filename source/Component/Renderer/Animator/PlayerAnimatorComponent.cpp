#include "PlayerAnimatorComponent.h"

void PlayerAnimatorComponent::Initialize()
{
	__super::Initialize();
	_player_ref = dynamic_cast<Player*>(GetOwnerActor());
}

void PlayerAnimatorComponent::Tick(const float delta_seconds)
{
	__super::Tick(delta_seconds);

	_movement_mode = _player_ref->GetMovementMode();
	_velocity = _player_ref->GetVelocity();
}
