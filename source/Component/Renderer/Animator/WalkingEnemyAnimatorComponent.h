#pragma once

#include "Component/Renderer/Animator/AnimatorComponent.h"

class WalkingEnemy;

class WalkingEnemyAnimatorComponent : public AnimatorComponent<WalkingEnemyAnimatorComponent>
{
public:
	WalkingEnemyAnimatorComponent()
		: _walking_enemy_ref(nullptr)
	{}
	virtual ~WalkingEnemyAnimatorComponent() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Finalize() override;
	virtual void Tick(const float delta_seconds) override;
	//~ End ComponentBase interface

public:
	Vector2D _velocity;

private:
	WalkingEnemy* _walking_enemy_ref;
};