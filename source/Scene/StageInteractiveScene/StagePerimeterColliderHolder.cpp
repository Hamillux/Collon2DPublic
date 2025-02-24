#include "StagePerimeterColliderHolder.h"
#include "Component/Collider/SegmentCollider.h"

StagePerimeterColliderHolder::StagePerimeterColliderHolder()
{
}

StagePerimeterColliderHolder::~StagePerimeterColliderHolder()
{
}

void StagePerimeterColliderHolder::RequestToSetActorHidden(const bool new_hidden)
{
	// 常に表示
}

void StagePerimeterColliderHolder::CreateColliders(const Vector2D& left_top, const Vector2D& right_bottom)
{
	// コライダー生成に使用するパラメータ
	const CollisionObjectType collision_object_type = CollisionObjectType::BARRIER;
	const float stage_height = right_bottom.y - left_top.y;
	const float stage_width = right_bottom.x - left_top.x;
	const Vector2D stage_center = left_top + Vector2D(stage_width / 2.f, stage_height / 2.f);

	const std::vector<CollisionObjectType> hit_targets =
	{
		CollisionObjectType::WILDCARD
	};

	//// コライダーの長さはステージの幅・高さよりも少し大きくする
	SegmentCollider* left_collider = CreateComponent<SegmentCollider>(this);
	left_collider->SetSegmentColliderParams(
		CollisionType::BLOCK,
		collision_object_type,
		hit_targets,
		false,
		stage_height + UNIT_TILE_SIZE
	);
	left_collider->SetLocalTransform(Vector2D(0.f, stage_height / 2.f), DX_PI_F / 2.f);

	SegmentCollider* right_collider = CreateComponent<SegmentCollider>(this);
	right_collider->SetSegmentColliderParams(
		CollisionType::BLOCK,
		collision_object_type,
		hit_targets,
		false,
		stage_height + UNIT_TILE_SIZE
	);
	right_collider->SetLocalTransform(Vector2D(stage_width, stage_height / 2.f), DX_PI_F / 2.f);

	SegmentCollider* top_collider = CreateComponent<SegmentCollider>(this);
	top_collider->SetSegmentColliderParams(
		CollisionType::BLOCK,
		collision_object_type,
		hit_targets,
		false,
		stage_width + UNIT_TILE_SIZE
	);
	top_collider->SetLocalTransform(Vector2D(stage_width / 2.f, 0.f), 0.f);

}
