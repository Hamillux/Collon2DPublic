#include "HitResult.h"

#include "Component/Collider/ColliderBase.h"

HitResult HitResult::GetInverted() const
{
    HitResult inverted_result;

	// 元の結果と反転した結果で共通の項目
    inverted_result.has_hit = has_hit;
	inverted_result.collision_type = collision_type;
	inverted_result.total_push_back_distance = total_push_back_distance;

	// 反転で入れ替える項目
	inverted_result.self_collider = other_collider;
	inverted_result.other_collider = self_collider;
	inverted_result.normal_from_other = normal_from_other * (-1.f);

	return inverted_result;
}

HitResult MakeHitResult(const ColliderBase* self_collider, const ColliderBase* other_collider, const Vector2D& penetration_depth)
{
	// OVERLAP
	if (
		self_collider->GetCollisionType() == CollisionType::OVERLAP ||
		other_collider->GetCollisionType() == CollisionType::OVERLAP
		)
	{
		HitResult overlap_result = {};
		overlap_result.has_hit = true;
		overlap_result.collision_type = CollisionType::OVERLAP;
		overlap_result.self_collider = self_collider;
		overlap_result.other_collider = other_collider;
		return overlap_result;
	}

	// BLOCK
	{
		const float penetration_depth_length = penetration_depth.Length();
		Vector2D normal = penetration_depth / penetration_depth_length;

		// selfをotherから押し戻すベクトルは
		// other->selfのベクトルとの内積がプラス
		const Vector2D other_to_self = self_collider->GetCenterWorldPosition() - other_collider->GetCenterWorldPosition();
		float dot = Vector2D::Dot(other_to_self, normal);
		if (dot < 0)
		{
			normal *= -1.f;
		}

		HitResult block_result{};
		block_result.has_hit = true;
		block_result.collision_type = CollisionType::BLOCK;
		block_result.self_collider = self_collider;
		block_result.other_collider = other_collider;
		block_result.total_push_back_distance = penetration_depth.Length();
		block_result.normal_from_other = normal;
		return block_result;
	}
}