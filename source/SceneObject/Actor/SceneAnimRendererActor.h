#pragma once

#include "SceneObject/Actor/Actor.h"
#include <array>

class AnimRendererComponent;

CLN2D_GEN_DEFINE_ACTOR()
class SceneAnimRendererActor : public Actor
{
	static constexpr int NUM_ANIM_COMPONENTS = 100;
public:
	SceneAnimRendererActor();
	virtual ~SceneAnimRendererActor();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) override;
	virtual void RequestToSetActorHidden(const bool new_hidden) override;
	//~ End Actor interface


	size_t PlayAnimation(const AnimPlayInfo& anim_play_info, const Transform& transform, Actor* const attach_to = nullptr);
	void StopAnimation(const size_t scene_anim_index);

private:
	size_t GetAvailableAnimComponentIndex() const;
	std::array<AnimRendererComponent*, NUM_ANIM_COMPONENTS> _anim_components;
};

template<>
struct initial_params_of_actor<SceneAnimRendererActor> { using type = initial_params_of_actor_t<Actor>; };