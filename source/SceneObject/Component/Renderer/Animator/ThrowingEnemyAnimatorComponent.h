#pragma once

#include "SceneObject/Component/Renderer/Animator/AnimatorComponent.h"

class ThrowingEnemyAnimatorComponent : public AnimatorComponent<ThrowingEnemyAnimatorComponent>
{
public:
	ThrowingEnemyAnimatorComponent() {}
	virtual ~ThrowingEnemyAnimatorComponent() {}
};