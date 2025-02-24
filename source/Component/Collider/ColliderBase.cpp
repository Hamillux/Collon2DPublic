#include "ColliderBase.h"
#include "GameSystems/CollisionManager.h"
#include "Scene/SceneBase.h"
#include "Component/Collider/HitResult.h"
#include "Actor/Actor.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/Collider/SegmentCollider.h"
// #include "Component/Collider/CircleCollider.h"
#include "Component/Collider/TriangleCollider.h"
#include "Input/DeviceInput.h"

void ColliderBase::Initialize()
{
	__super::Initialize();
	should_call_component_tick = false;

	// CollisionManagerに新たなコリジョンの生成を通知する
	CollisionManager::GetInstance().OnNewColliderInitialized(this);
}

void ColliderBase::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);
}

void ColliderBase::Finalize()
{
	CollisionManager::GetInstance().OnColliderFinalize(this);
	__super::Finalize();
}

void ColliderBase::OnParentActorHiddenChanged(const bool new_hidden)
{
	__super::OnParentActorHiddenChanged(new_hidden);

	if (new_hidden)
	{
		Deactivate();
	}
	else
	{
		Activate();
	} 
}

void ColliderBase::OnThisWorldTransformChanged()
{
	__super::OnThisWorldTransformChanged();
	CollisionManager::GetInstance().OnColliderTransformed(this);
}

bool ColliderBase::IsOverlappingWith(const ColliderBase* other_collider) const
{
	if (!ShouldCheckHitWith(other_collider))
	{
		return false;
	}

	HitResult result;
	CheckHitWith(result, other_collider);
	return (
		result.has_hit && 
		result.collision_type == CollisionType::OVERLAP
	);
}

void ColliderBase::HandleCollisionWith(const ColliderBase* other_collider) const
{
	if (ShouldCheckHitWith(other_collider))
	{
		HitResult hit_result_for_self;
		CheckHitWith(hit_result_for_self, other_collider);

		if (hit_result_for_self.has_hit)
		{
			HandleHitResult(hit_result_for_self);
		}
	}
}

bool ColliderBase::IsHitCheckTarget(const CollisionObjectType target_object_type) const
{
	auto casted_target_object_type = static_cast<CollisionObjectType_UnderlyingType>(target_object_type);
	return ((hit_object_types & casted_target_object_type) != 0);
}

bool ColliderBase::IsHitCheckTarget(const CollisionObjectType target_object_type, const CollisionObjectType_UnderlyingType hit_targets)
{
	return ((hit_targets & static_cast<CollisionObjectType_UnderlyingType>(target_object_type)) != 0);
}

void ColliderBase::SetHitObjectTypes(const std::vector<CollisionObjectType>& new_hit_object_types)
{
	hit_object_types = 0;
	for (const auto& hit_object_type : new_hit_object_types)
	{
		hit_object_types |= static_cast<CollisionObjectType_UnderlyingType>(hit_object_type);
	}
}

CollisionType ColliderBase::GetCollisionTypeBetween(const ColliderBase* other) const
{
	if (!IsHitCheckTarget(other->GetCollisionObjectType()))
	{
		return CollisionType::NONE;
	}

	const CollisionType self_collision_type = GetCollisionType();
	const CollisionType other_collision_type = other->GetCollisionType();

	if (self_collision_type == CollisionType::BLOCK || other_collision_type == CollisionType::BLOCK)
	{
		return CollisionType::BLOCK;
	}

	return CollisionType::OVERLAP;
}

void ColliderBase::Deactivate()
{
	is_active = false;
}

void ColliderBase::Activate()
{
	is_active = true;
}

bool ColliderBase::IsActive() const
{
	return is_active;
}

void ColliderBase::SetShouldCheckWithHasCommonParent(const bool new_should_check_with_has_common_parent)
{
	should_check_with_has_common_parent = new_should_check_with_has_common_parent;
}

void ColliderBase::AddHitTarget(const CollisionObjectType new_target)
{
	hit_object_types |= static_cast<unsigned int>(new_target);
}

void ColliderBase::RemoveHitTarget(const CollisionObjectType target_to_remove)
{
	hit_object_types &= ~(static_cast<unsigned int>(target_to_remove));
}

void ColliderBase::AddToHitTargetsOtherThan(const CollisionObjectType target_exception)
{
	AddHitTarget(CollisionObjectType::WILDCARD);
	RemoveHitTarget(target_exception);
	return;
}

void ColliderBase::SetColliderCommonParams(const CollisionType new_collision_type, const CollisionObjectType new_collision_object_type, const std::vector<CollisionObjectType>& new_hit_object_types, const bool new_pushability)
{
	collision_type = new_collision_type;
	collision_object_type = new_collision_object_type;
	hit_object_types = 0;
	for (auto& hit_object_type : new_hit_object_types)
	{
		AddHitTarget(hit_object_type);
	}
	is_pushable = new_pushability;
}

bool ColliderBase::ShouldCheckHitWith(const ColliderBase* other_collider) const
{
	// どちらかのコライダーが無効なら衝突判定は不要
	if (!other_collider || !IsActive() || !other_collider->IsActive())
	{
		return false;
	}
	// 互いに衝突対象に指定されていなければ判定は不要
	if (
		!IsHitCheckTarget(other_collider->GetCollisionObjectType()) ||
		!other_collider->IsHitCheckTarget(GetCollisionObjectType())
		)
	{
		return false;
	}
	// 親が共通の場合
	if (!should_check_with_has_common_parent && GetOwnerActor() == other_collider->GetOwnerActor())
	{
		return false;
	}

	return true;
}

bool ColliderBase::HandlePushback(const HitResult& hit_result) const
{
	// 押し戻し処理が不要なら即return
	if (
		!hit_result.has_hit ||
		hit_result.collision_type != CollisionType::BLOCK ||
		hit_result.total_push_back_distance <= EPSIRON
		)
	{
 		return false;
	}

	Actor* self_owner = hit_result.self_collider->GetOwnerActor();
	Actor* other_owner = hit_result.other_collider->GetOwnerActor();

	// コライダー所有アクターの相対速度
	const Vector2D rvel_from_other =
		self_owner->GetVelocity() -
		other_owner->GetVelocity();

	// 押し戻し方向の相対速度の大きさ
	const float relative_speed_n = fabsf(Vector2D::Dot(hit_result.normal_from_other, rvel_from_other));

	// 押し戻し方向と直交する相対速度の大きさ
	const float relative_speed_t = fabsf(Vector2D::Cross(hit_result.normal_from_other, rvel_from_other));

	// 押し戻し方向の速度成分が, それと垂直な速度成分に比べて小さすぎると,
	// 例えば壁に沿って落下するときにガタガタと上下に振動する問題が発生するので,
	// 押し戻し方向の速度成分の大きさだけでなく、垂直成分との比率も調べる.
	constexpr float N_SPEED_THRESHOLD = 1.f;	// 実験的に求めた値.
	constexpr float T_PER_N_THRESHOLD = 2.0f;	// 勾配が T_PER_N_THRESHOLD x100% 未満の坂なら真上から衝突した際に巻き戻し処理が行われる.

	// めり込んでいて, 法線方向の速度がゼロでない
	const bool should_rewind_time =
		relative_speed_n > N_SPEED_THRESHOLD &&
		(relative_speed_t / relative_speed_n) < T_PER_N_THRESHOLD;

	if(should_rewind_time)
	{
		// 時間の巻き戻しによる押し戻し

		// "巻き戻し時間" = (めり込み深度) / (押し戻し方向スピード)
		float rewind_time = hit_result.total_push_back_distance / relative_speed_n;
		Vector2D delta_self_position = self_owner->GetVelocity() * (-rewind_time);
		Vector2D delta_other_position = other_owner->GetVelocity() * (-rewind_time);
		float rewind_distance_sq = 
			(
				hit_result.normal_from_other * Vector2D::Dot(hit_result.normal_from_other, (delta_self_position - delta_other_position))
			).LengthSquared();

		while (rewind_distance_sq > hit_result.total_push_back_distance*hit_result.total_push_back_distance)
		{
			rewind_time -= 1.f / 1024.f;
			if(rewind_time < 0.f)
			{
				break;
			}
			delta_self_position = self_owner->GetVelocity() * (-rewind_time);
			delta_other_position = other_owner->GetVelocity() * (-rewind_time);
			rewind_distance_sq =
				(
					hit_result.normal_from_other * Vector2D::Dot(hit_result.normal_from_other, (delta_self_position - delta_other_position))
					).LengthSquared();
		}

		// 巻き戻し時間が1フレームより十分に大きい場合は押し戻しを実行せず終了
		if (rewind_time > 1.f / 10.f) 
		{
			return false;
		}
		
		self_owner->AddWorldPosition(delta_self_position);
		other_owner->AddWorldPosition(delta_other_position);
	}	
	else
	{
		// 質量と押し戻し可否に応じた押し戻し距離の分配

		const bool self_is_pushable = IsPushable();
		const bool other_is_pushable = hit_result.other_collider->IsPushable();

		// コライダーのオーナーの押し戻し可否で分岐
		if (self_is_pushable && other_is_pushable)
		{
			// 質量に基づいて押し戻し距離を分配 D_a = D_total * (M_a / (Ma + Mb))
			const float self_mass = GetMass();
			const float other_mass = hit_result.other_collider->GetMass();
			const float total_mass = self_mass + other_mass;
			const float self_push_back_distance = hit_result.total_push_back_distance * self_mass / total_mass;
			const float other_push_back_distance = hit_result.total_push_back_distance * other_mass / total_mass;
			self_owner->AddWorldPosition(hit_result.normal_from_other * self_push_back_distance);
			self_owner->AddWorldPosition(hit_result.normal_from_other * (-other_push_back_distance));
		}
		else if (self_is_pushable)
		{
			// self_ownerだけ押し戻し可能
			self_owner->AddWorldPosition(hit_result.normal_from_other * hit_result.total_push_back_distance);
		}
		else if (other_is_pushable)
		{
			// other_ownerだけ押し戻し可能
			other_owner->AddWorldPosition(hit_result.normal_from_other * (-hit_result.total_push_back_distance));
		}
	}

	// 衝突法線方向の速度をゼロにする

	const Vector2D& normal_from_other = hit_result.normal_from_other;

	if (Vector2D::Dot(rvel_from_other.Normalize(), normal_from_other) < -EPSIRON)
	{
		// めり込む方向に動いているので速度の法線成分を打ち消す
		Vector2D self_velocity_n = normal_from_other * Vector2D::Dot(self_owner->GetVelocity(), normal_from_other);
		Vector2D other_velocity_n = normal_from_other * Vector2D::Dot(other_owner->GetVelocity(), normal_from_other);

		self_owner->AddVelocity(self_velocity_n * (-1.f));
		other_owner->AddVelocity(other_velocity_n * (-1.f));
	}

	return true;
}

void ColliderBase::DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc)
{
	if(desc.draw_aabb)
	{
		Vector2D screen_world_offset = camera_params.world_offset;
		// 外接AABBの描画
		int x1, x2, y1, y2;
		Vector2D _aabb_left_top, _aabb_right_bottom;
		GetAABB(_aabb_left_top, _aabb_right_bottom);
		Vector2D bounding_aabb_left_top_screen = Vector2D::WorldToViewport(_aabb_left_top, camera_params);
		Vector2D bounding_aabb_right_bottom_screen = Vector2D::WorldToViewport(_aabb_right_bottom, camera_params);
		bounding_aabb_left_top_screen.ToIntRound(x1, y1);
		bounding_aabb_right_bottom_screen.ToIntRound(x2, y2);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, debug_line_params.aabb_alpha);
		DrawBox(x1, y1, x2, y2, debug_line_params.aabb_color, false);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void ColliderBase::CheckHitWith(HitResult& out_result_for_self, const ColliderBase* other_collider) const
{
	ColliderShape this_shape = GetColliderShape();
	ColliderShape other_shape = other_collider->GetColliderShape();
	if (this_shape == ColliderShape::BOX)
	{
		const BoxCollider* this_as_box = dynamic_cast<const BoxCollider*>(const_cast<ColliderBase*>(this));
		switch (other_shape)
		{
		case ColliderShape::BOX:
			CheckCollidersHitImpl(out_result_for_self, this_as_box, dynamic_cast<const BoxCollider*>(other_collider));
			return;
		case ColliderShape::SEGMENT:
			CheckCollidersHitImpl(out_result_for_self, this_as_box, dynamic_cast<const SegmentCollider*>(other_collider));
			return;
		case ColliderShape::CIRCLE:
			// CheckCollidersHitImpl(out_result_for_self, this_as_box, dynamic_cast<const CircleCollider*>(other_collider));
			throw std::runtime_error("Not implemented yet");
			return;
		case ColliderShape::TRIANGLE:
			CheckCollidersHitImpl(out_result_for_self, this_as_box, dynamic_cast<const TriangleCollider*>(other_collider));
			return;
		}
	}
	else if (this_shape == ColliderShape::SEGMENT)
	{
		const SegmentCollider* this_as_segment = dynamic_cast<const SegmentCollider*>(const_cast<ColliderBase*>(this));
		switch (other_shape)
		{
		case ColliderShape::BOX:
			CheckCollidersHitImpl(out_result_for_self, this_as_segment, dynamic_cast<const BoxCollider*>(other_collider));
			return;
		case ColliderShape::SEGMENT:
			CheckCollidersHitImpl(out_result_for_self, this_as_segment, dynamic_cast<const SegmentCollider*>(other_collider));
			return;
		case ColliderShape::CIRCLE:
			// CheckCollidersHitImpl(out_result_for_self, this_as_segment, dynamic_cast<const CircleCollider*>(other_collider));
			throw std::runtime_error("Not implemented yet");
			return;
		case ColliderShape::TRIANGLE:
			CheckCollidersHitImpl(out_result_for_self, this_as_segment, dynamic_cast<const TriangleCollider*>(other_collider));
			return;
		}
	}
	else if (this_shape == ColliderShape::CIRCLE)
	{
		throw std::runtime_error("Not implemented yet");
		// const CircleCollider* this_as_circle = dynamic_cast<const CircleCollider*>(const_cast<ColliderBase*>(this));
		//switch (other_shape)
		//{
		//case ColliderShape::BOX:
		//	CheckCollidersHitImpl(out_result_for_self, this_as_circle, dynamic_cast<const BoxCollider*>(other_collider));
		//	return;
		//case ColliderShape::SEGMENT:
		//	CheckCollidersHitImpl(out_result_for_self, this_as_circle, dynamic_cast<const SegmentCollider*>(other_collider));
		//	return;
		//case ColliderShape::CIRCLE:
		//	CheckCollidersHitImpl(out_result_for_self, this_as_circle, dynamic_cast<const CircleCollider*>(other_collider));
		//	throw std::runtime_error("Not implemented yet");
		//	return;
		//case ColliderShape::TRIANGLE:
		//	CheckCollidersHitImpl(out_result_for_self, this_as_circle, dynamic_cast<const TriangleCollider*>(other_collider));
		//	return;
		//}
	}
	else if (this_shape == ColliderShape::TRIANGLE)
	{
		const TriangleCollider* this_as_triangle = dynamic_cast<const TriangleCollider*>(const_cast<ColliderBase*>(this));
		switch (other_shape)
		{
		case ColliderShape::BOX:
			CheckCollidersHitImpl(out_result_for_self, this_as_triangle, dynamic_cast<const BoxCollider*>(other_collider));
			return;
		case ColliderShape::SEGMENT:
			CheckCollidersHitImpl(out_result_for_self, this_as_triangle, dynamic_cast<const SegmentCollider*>(other_collider));
			return;
		case ColliderShape::CIRCLE:
			// CheckCollidersHitImpl(out_result_for_self, this_as_triangle, dynamic_cast<const CircleCollider*>(other_collider));
			throw std::runtime_error("Not implemented yet");
			return;
		case ColliderShape::TRIANGLE:
			CheckCollidersHitImpl(out_result_for_self, this_as_triangle, dynamic_cast<const TriangleCollider*>(other_collider));
			return;
		}
	}
}

void ColliderBase::HandleHitResult(const HitResult& hit_result_for_self) const
{
	bool should_call_on_hit_collision = true;

	if (hit_result_for_self.collision_type == CollisionType::BLOCK)
	{
		const bool has_executed_pushback = HandlePushback(hit_result_for_self);
		should_call_on_hit_collision = has_executed_pushback;
	}

	if(should_call_on_hit_collision && _generate_hit_event && hit_result_for_self.other_collider->_generate_hit_event)
	{
		GetOwnerActor()->OnHitCollision(hit_result_for_self);

		const ColliderBase* other_collider = hit_result_for_self.other_collider;
		// 衝突相手のHitResultを用意して、相手の親Actorに渡す
		HitResult hit_result_for_other = hit_result_for_self.GetInverted();
		other_collider->GetOwnerActor()->OnHitCollision(hit_result_for_other);
	}
}
