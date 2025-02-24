#pragma once

#include "Actor/Actor.h"
#include "ColliderHolderInitialParams.h"

class ColliderBase;

class ColliderHolder : public Actor
{
public:
	ColliderHolder();
	virtual~ColliderHolder();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void Draw(const CameraParams& camera_params) override;
	virtual void TickActor(float delta_seconds) override;
	//~ End Actor interface

private:
	ColliderBase* _collider;
};

template<>
struct initial_params_of_actor<ColliderHolder>
{
	using type = ColliderHolderInitialParams;
};