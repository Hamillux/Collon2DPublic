#pragma once

#include "Actor/Character/Enemy/EnemyBase.h"

CLN2D_GEN_DEFINE_ACTOR()
class ThrowingEnemy : public EnemyBase
{
public:
	ThrowingEnemy();
	virtual ~ThrowingEnemy();

	//~ Begin EnemyBase interface
protected:
	//virtual void TickEnemy(const float delta_seconds) override;
	virtual AnimRendererComponent* CreateAnimRenderer() override;
	virtual Vector2D GetBodySize() const override;
	//~ End EnemyBase interface
};

template<> struct initial_params_of_actor<ThrowingEnemy> { using type = initial_params_of_actor_t<EnemyBase>; };