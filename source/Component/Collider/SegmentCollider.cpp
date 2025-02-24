#include "SegmentCollider.h"
#include "Scene/SceneBase.h"
#include "Component/Collider/HitResult.h"
#include "Component/Collider/BoxCollider.h"


void SegmentCollider::Initialize()
{
	__super::Initialize();
}

void SegmentCollider::Finalize()
{
	__super::Finalize();
}

void SegmentCollider::RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params)
{
	query_result = QueryResult_SingleLineTrace{};

	if (!ShouldCheckQueryHit(query_params))
	{
		query_result.has_hit = false;
		return;
	}
	else
	{
		FSegment this_segment = FSegment
		{
			CalcSegmentEndPosition(SegmentEnd::LEFT),
			CalcSegmentEndPosition(SegmentEnd::RIGHT)
		};

		query_result.hit_collider = this;
		query_result.has_hit
			= GeometricUtility::DoesSegmentIntersectWithAnother(
				query_result.hit_location,
				query_params.segment,
				this_segment
			);

		if(query_result.has_hit)
		{
			// 法線
			Vector3D D = Vector3D::MakeFromXY(query_params.segment.GetDirectionUnnormalized());
			Vector3D L = Vector3D::MakeFromXY(this_segment.GetDirectionUnnormalized());
			query_result.hit_normal = (D.Cross(L).Cross(L)).XY().Normalize();
		}
	}
}

void SegmentCollider::RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params)
{
	if (!ShouldCheckQueryHit(query_params))
	{
		query_result.has_hit = false;
		return;
	}
	else
	{
		FSegment segment
		{
			CalcSegmentEndPosition(SegmentEnd::LEFT),
			CalcSegmentEndPosition(SegmentEnd::RIGHT)
		};

		query_result.has_hit = GeometricUtility::DoesRectOverlapWithSegment(query_params.rect.ToFRect(), segment);
		if (query_result.has_hit)
		{
			query_result.hit_colliders.push_back(this);
		}
	}
}

void SegmentCollider::DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc)
{
	__super::DrawDebugLines(camera_params, desc);

	const Vector2D left_end = CalcSegmentEndPosition(SegmentEnd::LEFT);
	const Vector2D right_end = CalcSegmentEndPosition(SegmentEnd::RIGHT);

	int x1, y1, x2, y2;
	Vector2D::WorldToViewport(left_end, camera_params).ToIntRound(x1, y1);
	Vector2D::WorldToViewport(right_end, camera_params).ToIntRound(x2, y2);

	BlendDrawHelper::DrawLine(
		DrawBlendInfo(DX_BLENDMODE_ALPHA, desc.segment.line_alpha),
		x1, y1, x2, y2,
		desc.segment.line_color,
		desc.segment.line_thickness);
}

Vector2D SegmentCollider::GetCenterWorldPosition() const
{
	return GetWorldPosition();
}

void SegmentCollider::GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const
{
	const Vector2D end_a = CalcSegmentEndPosition(SegmentEnd::LEFT);
	const Vector2D end_b = CalcSegmentEndPosition(SegmentEnd::RIGHT);
	out_left_top.x = std::min(end_a.x, end_b.x);
	out_left_top.y = std::min(end_a.y, end_b.y);
	out_right_bottom.x = std::max(end_a.x, end_b.x);
	out_right_bottom.y = std::max(end_a.y, end_b.y);
}

ColliderBase::ColliderShape SegmentCollider::GetColliderShape() const
{
	return ColliderShape::SEGMENT;
}

void SegmentCollider::GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const
{
	out_vertex_positions.push_back(CalcSegmentEndPosition(SegmentEnd::LEFT));
	out_vertex_positions.push_back(CalcSegmentEndPosition(SegmentEnd::RIGHT));
}

void SegmentCollider::SetSegmentColliderParams(const CollisionType new_collision_type, const CollisionObjectType new_collision_object_type, const std::vector<CollisionObjectType>& new_hit_object_types, const bool new_pushability, const float new_length)
{
	SetColliderCommonParams(
		new_collision_type,
		new_collision_object_type,
		new_hit_object_types,
		new_pushability
	);
	_length = new_length;
}

Vector2D SegmentCollider::CalcSegmentEndPosition(const SegmentEnd segment_end) const
{
	const Vector2D segment_end_local = GetSegmentEndLocalPosition(segment_end);
	return GetWorldTransform().TransformLocation(segment_end_local);
}

Vector2D SegmentCollider::CalcSegmentCenterToEnd(const SegmentEnd segment_end) const
{
	const Vector2D segment_end_local = GetSegmentEndLocalPosition(segment_end);
	return GetWorldTransform().TransformDirection(segment_end_local);
}

Vector2D SegmentCollider::GetSegmentEndLocalPosition(const SegmentEnd segment_end) const
{
	switch (segment_end)
	{
	case SegmentEnd::LEFT:
		return Vector2D::ex() * (-_length * 0.5f);
	case SegmentEnd::RIGHT:
		return Vector2D::ex() * _length * 0.5f;
	default:
		throw std::runtime_error("Invalid SegmentEnd");
	}
}

void SegmentCollider::SetLength(const float new_length)
{
	_length = new_length;
}

void SegmentCollider::DrawLine(const CameraParams& camera_params, const int color, const int thickness)
{
	const Vector2D from = CalcSegmentEndPosition(SegmentEnd::LEFT);
	const Vector2D to = CalcSegmentEndPosition(SegmentEnd::RIGHT);
	int x1, y1, x2, y2;
	Vector2D::WorldToViewport(from, camera_params).ToIntRound(x1, y1);
	Vector2D::WorldToViewport(to, camera_params).ToIntRound(x2, y2);

	DxLib::DrawLine(x1, y1, x2, y2, color, thickness);
}
