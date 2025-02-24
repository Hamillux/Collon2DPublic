#pragma once
#include "Actor/Actor.h"
#include "ProjectileInitialParams.h"

class ProjectileMovementComponent;

CLN2D_GEN_DEFINE_ACTOR()
class ProjectileBase : public Actor
{
public:
	ProjectileBase();
	virtual ~ProjectileBase() = 0;

public:
	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void Finalize();
	//~ End Actor interface

	ProjectileMovementComponent* GetProjectileMovement() const;
private:
	ProjectileMovementComponent* _projectile_movement;
};


template<> struct initial_params_of_actor<ProjectileBase>
{
	using type = ProjectileInitialParams;
};
