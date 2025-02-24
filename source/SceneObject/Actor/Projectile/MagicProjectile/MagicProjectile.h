#pragma once
#include "SceneObject/Actor/Projectile/ProjectileBase.h"

class MagicProjectile : public ProjectileBase
{
public:
	MagicProjectile();
	virtual ~MagicProjectile() {}
	
	//~ Begin Actor interface
	virtual void Draw(const ScreenParams& screen_params) override;
	//~ End Actor interface
};

template<> struct initial_params_of_actor<MagicProjectile> { using type = ProjectileInitialParams; };