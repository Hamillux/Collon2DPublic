#pragma once

#include "Actor/Actor.h"

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// コイン
/// </summary>
class Coin : public Actor
{
public:
	Coin();
	virtual ~Coin();

	//~ Begin Actor interface
public:
	virtual void Initialize(const initial_params_of_actor_t<Actor>* const actor_params) override;
	virtual void OnHitCollision(const HitResult& hit_result) override;
	//~ End Actor interface
};

template<> struct initial_params_of_actor<Coin> { using type = initial_params_of_actor_t<Actor>; };