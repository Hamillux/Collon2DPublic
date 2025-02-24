#pragma once

#include "Utility/Core/Math/Vector2D.h"
#include "CollisionType.h"
#include "Utility/Core/Math/GeometryUtility.h"

class ColliderBase;
class Actor;

struct HitResult
{
	/**
	*	@param has_hit_in	衝突があるか
	*	@param collision_type_in	衝突の種類(NONE/OVERLAP/BLOCK)
	*	@param hit_object_in	衝突相手
	*	@param push_back_distance_in	BLOCKの場合に押し戻される距離
	*	@param normal_in	BLOCKの場合に押し戻される方向の単位ベクトル
	*/
	HitResult(
		const bool has_hit_in = false,
		const CollisionType collision_type_in = CollisionType::OVERLAP,
		const ColliderBase* self_collider_in = nullptr,
		const ColliderBase* other_collider_in = nullptr,
		const float push_back_distance_in = 0.f,
		const Vector2D& normal_in = Vector2D(0, 0)
	) 
		: has_hit(has_hit_in)
		, collision_type(collision_type_in)
		, self_collider(self_collider_in)
		, other_collider(other_collider_in)
		, total_push_back_distance(push_back_distance_in)
		, normal_from_other(normal_in)
	{}
	~HitResult() {}

	HitResult GetInverted() const;

	// 衝突の有無
	bool has_hit;

	// コリジョンタイプ Overlap or Block
	CollisionType collision_type;

	// 衝突したコライダー
	const ColliderBase* self_collider;
	const ColliderBase* other_collider;

	// 押し出し距離
	float total_push_back_distance;

	// 押し出し単位ベクトル
	Vector2D normal_from_other;

};

struct CollisionQueryParams_SingleLineTrace
{
	FSegment segment;
	CollisionObjectType trace_object_type{CollisionObjectType::WILDCARD};
	std::underlying_type_t<CollisionObjectType> hit_object_types{ 0 };
	std::vector<const Actor*> ignore_actors{};
};

struct CollisionQueryParams_Rect
{
	FRect rect;
	CollisionObjectType trace_object_type{ CollisionObjectType::WILDCARD };
	std::underlying_type_t<CollisionObjectType> hit_object_types{ 0 };
	std::vector<const Actor*> ignore_actors{};
};

struct CollisionQueryParams_RectAA
{
	FRectAA rect;
	CollisionObjectType trace_object_type{ CollisionObjectType::WILDCARD };
	std::underlying_type_t<CollisionObjectType> hit_object_types{ 0 };
	std::vector<const Actor*> ignore_actors{};
};

struct QueryResult_SingleLineTrace
{
	bool has_hit;
	Vector2D hit_location;
	const ColliderBase* hit_collider;
	Vector2D hit_normal;
};

struct QueryResult_MultiAARectTrace
{
	bool has_hit;
	std::vector<ColliderBase*> hit_colliders;
};