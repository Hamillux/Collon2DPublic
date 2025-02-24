#pragma once

#include "Actor/Actor.h"
#include "GoalFlagInitialParams.h"

class InAnimateRenderer;
class BoxCollider;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// ステージのゴールフラッグ.
/// </summary>
class GoalFlag : public Actor
{
public:
	GoalFlag();
	virtual ~GoalFlag();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	//virtual void TickActor(float delta_seconds) override;
	//virtual void Draw(const CameraParams& camera_params) override;
	virtual void Finalize() override;
	//virtual void OnHitCollision(const HitResult& hit_result) override;
	virtual void GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes) override;
	//~ End Actor interface

private:

	InAnimateRenderer* _renderer;
	BoxCollider* _collider;
	int _ghandle;
};

template<> struct initial_params_of_actor<GoalFlag> { using type = GoalFlagInitialParams; };