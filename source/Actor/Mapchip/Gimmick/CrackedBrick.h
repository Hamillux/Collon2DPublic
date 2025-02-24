#pragma once

#include "Actor/Actor.h"

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// ひび割れたレンガ. Crushダメージで破壊可能.
/// </summary>
class CrackedBrick : public Actor
{
public:
	CrackedBrick();
	virtual ~CrackedBrick();
	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void Draw(const CameraParams& camera_params) override;

protected:
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	//~ End Actor interface
	
private:
	std::shared_ptr<SoundInstance> _sound_instance_destroyed;
};

template<> struct initial_params_of_actor<CrackedBrick> { using type = initial_params_of_actor_t<Actor>; };