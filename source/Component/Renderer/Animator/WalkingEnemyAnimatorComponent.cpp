#include "WalkingEnemyAnimatorComponent.h"
#include "Actor/Character/Enemy/WalkingEnemy.h"

void WalkingEnemyAnimatorComponent::Initialize()
{
	__super::Initialize();

	_walking_enemy_ref = dynamic_cast<WalkingEnemy*>(GetOwnerActor());
}

void WalkingEnemyAnimatorComponent::Finalize()
{
	_walking_enemy_ref = nullptr;
	__super::Finalize();
}

void WalkingEnemyAnimatorComponent::Tick(const float delta_seconds)
{
	__super::Tick(delta_seconds);

	if (!_walking_enemy_ref)
	{
		return;
	}

	_velocity = _walking_enemy_ref->GetVelocity();
}
