#pragma once
#include "Actor/Actor.h"

class SegmentCollider;

class StagePerimeterColliderHolder : public Actor
{
public:
	StagePerimeterColliderHolder();
	virtual ~StagePerimeterColliderHolder();

	//~ Begin Actor interface
	virtual void RequestToSetActorHidden(const bool new_hidden) override;

public:
	void CreateColliders(const Vector2D& stage_left_top, const Vector2D& stage_right_bottom);

	SegmentCollider* left_collider;
	SegmentCollider* top_collider;
	SegmentCollider* right_collider;
	SegmentCollider* bottom_collider;
};

template<> struct initial_params_of_actor<StagePerimeterColliderHolder> { using type = ActorInitialParams; };