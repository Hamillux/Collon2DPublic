#pragma once

#include "Actor/Actor.h"
#include "ItemInitialParams.h"

class BoxCollider;
class InAnimateRenderer;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// シーンに配置されるアイテム. 接触したプレイヤーはアイテムを取得する.
/// </summary>
class ItemActor : public Actor
{
public:
	ItemActor();
	virtual ~ItemActor();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void OnHitCollision(const HitResult& hit_result) override;
	//~ End Actor interface

private:
	BoxCollider* body_collider;
	InAnimateRenderer* renderer;
	MasterDataID _item_id;
};

template<> struct initial_params_of_actor<ItemActor> { using type = ItemActorInitialParams; };