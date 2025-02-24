#include "TacklingEnemyAnimatorComponent.h"
#include "Actor/Character/Enemy/TacklingEnemy.h"

TacklingEnemyAnimatorComponent::TacklingEnemyAnimatorComponent()
	: _tackling_enemy_ref(nullptr)
{
}

void TacklingEnemyAnimatorComponent::Initialize()
{
	__super::Initialize();
	_tackling_enemy_ref = dynamic_cast<TacklingEnemy*>(GetOwnerActor());
	assert(_tackling_enemy_ref);
}
