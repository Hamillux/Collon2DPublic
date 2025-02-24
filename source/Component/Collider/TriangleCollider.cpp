#include "TriangleCollider.h"
#include "Scene/SceneBase.h"
#include "Actor/Actor.h"

TriangleCollider::TriangleCollider()
{
}

TriangleCollider::~TriangleCollider()
{
}

ColliderBase::ColliderShape TriangleCollider::GetColliderShape() const
{
	return ColliderShape::TRIANGLE;
}

void TriangleCollider::GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const
{
	out_vertex_positions = std::vector<Vector2D>();
	out_vertex_positions.reserve(3);
	for(auto& v_local : _vertex_local_positions)
	{
		out_vertex_positions.push_back(GetWorldTransform().TransformLocation(v_local));
	}
}

void TriangleCollider::RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params)
{
	query_result = QueryResult_SingleLineTrace{};
	query_result.has_hit = false;

	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);

	std::array<FSegment, 3> edge_segments;
	for (size_t i = 0; i < 3; i++)
	{
		edge_segments.at(i) = FSegment(vertex_positions.at(i), vertex_positions.at((i + 1) % 3));
	}

	// 三角形の各辺に対して線分との交差判定を行う
	Vector2D nearest_intersection_point{};
	float min_distance_sq = FLT_MAX;
	int nearest_edge_index = -1;
	for (size_t i = 0; i < 3; i++)
	{
		const FSegment& edge = edge_segments.at(i);
		Vector2D intersection_point{};
		if (GeometricUtility::DoesSegmentIntersectWithAnother(intersection_point, query_params.segment, edge))
		{
			query_result.has_hit = true;

			const float new_distance_sq = (intersection_point - query_params.segment.start).LengthSquared();
			if (new_distance_sq < min_distance_sq)
			{
				min_distance_sq = new_distance_sq;
				nearest_intersection_point = intersection_point;
				nearest_edge_index = i;
			}
		}
	}

	if (query_result.has_hit)
	{
		query_result.hit_collider = this;
		query_result.hit_location = nearest_intersection_point;
		const FSegment nearest_edge = edge_segments.at(nearest_edge_index);
		Vector3D v_edge = Vector3D::MakeFromXY(nearest_edge.end - nearest_edge.start);
		Vector3D v_query = Vector3D::MakeFromXY(query_params.segment.end - query_params.segment.start);
		query_result.hit_normal = (v_query.Cross(v_edge)).Cross(v_edge).XY().Normalize();
	}
}

void TriangleCollider::RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params)
{
	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);
	query_result.has_hit = GeometricUtility::DoesAARectOverlapWithTriangle(query_params.rect, FTriangle(vertex_positions));
	if (query_result.has_hit)
	{
		query_result.hit_colliders.push_back(this);
	}
}

void TriangleCollider::DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc)
{
	__super::DrawDebugLines(camera_params, desc);

	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);
	for (size_t i = 0; i < 3; i++)
	{
		const Vector2D& start = vertex_positions.at(i);
		const Vector2D& end = vertex_positions.at((i + 1) % 3);

		GetScene()->DrawDebugLine(start, end, desc.triangle.line_color, desc.triangle.line_thickness, DrawBlendInfo(DX_BLENDMODE_ALPHA, desc.triangle.line_alpha));
	}
}

Vector2D TriangleCollider::GetCenterWorldPosition() const
{
	Vector2D center{};
	for (auto& v : _vertex_local_positions)
	{
		center += GetWorldTransform().TransformLocation(v);
	}

	center /= 3.f;
	return center;
}

void TriangleCollider::GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const
{
	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);
	float left = FLT_MAX, top = FLT_MAX, right = -FLT_MAX, bottom = -FLT_MAX;
	for (const auto& vertex : vertex_positions)
	{
		left = std::min(left, vertex.x);
		top = std::min(top, vertex.y);
		right = std::max(right, vertex.x);
		bottom = std::max(bottom, vertex.y);
	}
	out_left_top = Vector2D(left, top);
	out_right_bottom = Vector2D(right, bottom);
}

FTriangle TriangleCollider::GetTriangle() const
{
	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);
	return FTriangle(vertex_positions);
}

void TriangleCollider::SetTriangleColliderParams(const CollisionType new_collision_type, const CollisionObjectType new_collision_object_type, const std::vector<CollisionObjectType>& new_hit_object_types, const bool new_pushability, const std::array<Vector2D, 3>& vertex_local_positions)
{
	SetColliderCommonParams(
		new_collision_type,
		new_collision_object_type,
		new_hit_object_types,
		new_pushability
	);

	_vertex_local_positions = vertex_local_positions;
}

