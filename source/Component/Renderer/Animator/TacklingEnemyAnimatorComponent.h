#pragma once

#include "Component/Renderer/Animator/AnimatorComponent.h"

class TacklingEnemy;
enum class TacklingEnemyMoveState;

class TacklingEnemyAnimatorComponent : public AnimatorComponent<TacklingEnemyAnimatorComponent>
{
public:
	TacklingEnemyAnimatorComponent();
	virtual ~TacklingEnemyAnimatorComponent() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;

	TacklingEnemy* _tackling_enemy_ref;
};
