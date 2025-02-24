#include "PlayerAnimState.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "Component/CharacterMovementComponent.h"

uint8_t PlayerAnimStateUpdate_Idle(PlayerAnimatorComponent* animator)
{
	if (animator->_movement_mode == CharacterMovementMode::Falling)
	{
		return PlayerAnimStateID_Fall;
	}

	if (animator->_movement_mode == CharacterMovementMode::Walking && animator->_velocity.Length() > 0.f)
	{
		return PlayerAnimStateID_Run;
	}

	return PlayerAnimStateID_Idle;
}

uint8_t PlayerAnimStateUpdate_Run(PlayerAnimatorComponent* animator)
{
	if (animator->_movement_mode == CharacterMovementMode::Falling)
	{
		return PlayerAnimStateID_Fall;
	}

	if (animator->_velocity.Length() < 10.f)
	{
		return PlayerAnimStateID_Idle;
	}

	return PlayerAnimStateID_Run;
}

uint8_t PlayerAnimStateUpdate_Jump(PlayerAnimatorComponent* animator)
{
	if (animator->_movement_mode == CharacterMovementMode::Falling && animator->_velocity.y >= 0.f)
	{
		return PlayerAnimStateID_Fall;
	}

	if (animator->_movement_mode == CharacterMovementMode::Walking)
	{
		return PlayerAnimStateID_Idle;
	}

	return PlayerAnimStateID_Jump;
}

uint8_t PlayerAnimStateUpdate_Fall(PlayerAnimatorComponent* animator)
{
	if (animator->_movement_mode == CharacterMovementMode::Walking)
	{
		return PlayerAnimStateID_Idle;
	}
	if (animator->_velocity.y < 0.f)
	{
		return PlayerAnimStateID_Jump;
	}

	return PlayerAnimStateID_Fall;
}

uint8_t PlayerAnimStateUpdate_Magic(PlayerAnimatorComponent* animator)
{
	return PlayerAnimStateID_Magic;
}
