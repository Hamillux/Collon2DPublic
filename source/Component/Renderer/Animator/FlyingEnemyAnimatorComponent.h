#pragma once

#include "Component/Renderer/Animator/AnimatorComponent.h"

class FlyingEnemyAnimatorComponent : public AnimatorComponent<FlyingEnemyAnimatorComponent>
{
public:
	FlyingEnemyAnimatorComponent() {}
	virtual ~FlyingEnemyAnimatorComponent() {}
};