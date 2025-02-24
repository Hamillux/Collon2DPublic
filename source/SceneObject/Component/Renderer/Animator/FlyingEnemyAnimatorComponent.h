#pragma once

#include "SceneObject/Component/Renderer/Animator/AnimatorComponent.h"

class FlyingEnemyAnimatorComponent : public AnimatorComponent<FlyingEnemyAnimatorComponent>
{
public:
	FlyingEnemyAnimatorComponent() {}
	virtual ~FlyingEnemyAnimatorComponent() {}
};