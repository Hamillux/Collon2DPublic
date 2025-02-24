#include "BoxCollider.h"
#include "Component/Collider/HitResult.h"
#include "Actor/Actor.h"
#include "SegmentCollider.h"
#include "Scene/SceneBase.h"


BoxCollider::BoxCollider()
{
    center_to_vertices = std::array<Vector2D, 4>();
}

void BoxCollider::Initialize()
{
    __super::Initialize();
}

void BoxCollider::Finalize()
{
    __super::Finalize();
}

void BoxCollider::RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params)
{
	query_result = QueryResult_SingleLineTrace{};

	if(!ShouldCheckQueryHit(query_params))
	{
		query_result.has_hit = false;
		return;
	}

    std::array<Vector2D, 2> intersections;
    std::array<FSegment, 2> intersected_edges;
    const FRect rect = FRect(GetWorldPosition(), unrotaed_diagonal.x, unrotaed_diagonal.y, GetWorldRotation());
    const int intersection_count = GeometricUtility::GetSegmentRectIntersections(intersections, query_params.segment, rect, &intersected_edges);

    if (intersection_count == 0)
    {
		query_result.has_hit = false;
		return;
    }
    else
    {
        query_result.has_hit = true;
        query_result.hit_collider = this;

        size_t near_intersection_index = 0;
		if (intersection_count == 1)
		{
            near_intersection_index = 0;
		}
        else
        {
			// 2つの交点のうち、始点に近い方を選択
            const float sq_distance_to_start_0 = (query_params.segment.start - intersections.at(0)).LengthSquared();
			const float sq_distance_to_start_1 = (query_params.segment.start - intersections.at(1)).LengthSquared();
            near_intersection_index = sq_distance_to_start_0 < sq_distance_to_start_1 ? 0 : 1;
        }

		query_result.hit_location = intersections.at(near_intersection_index);

		// 法線ベクトルを求める
        Vector3D D = Vector3D::MakeFromXY(query_params.segment.GetDirectionUnnormalized());
        Vector3D L = Vector3D::MakeFromXY(intersected_edges.at(near_intersection_index).GetDirectionUnnormalized());
        query_result.hit_normal = (D.Cross(L).Cross(L)).XY().Normalize();
    }
}

void BoxCollider::RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params)
{
	if (!ShouldCheckQueryHit(query_params))
	{
		query_result.has_hit = false;
		return;
	}

	const FRect this_rect = FRect(GetWorldPosition(), unrotaed_diagonal.x, unrotaed_diagonal.y, GetWorldRotation());
	const bool is_hit = GeometricUtility::DoesRectOverlapWithAnother(this_rect, query_params.rect);
	query_result.has_hit = is_hit;
	if (is_hit)
	{
        query_result.hit_colliders.push_back(this);
	}
}

ColliderBase::ColliderShape BoxCollider::GetColliderShape() const
{
    return ColliderShape::BOX;
}

void BoxCollider::GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const
{
    const Vector2D center_position = GetWorldPosition();
    std::array<Vector2D,4> center_to_vertexes;
    CalcCenterToAllVertices(center_to_vertexes);

    out_vertex_positions = std::vector<Vector2D>(4);
	for (size_t i = 0; i < 4; i++)
	{
		out_vertex_positions.at(i) = center_position + center_to_vertexes.at(i);
	}
}


void BoxCollider::DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc)
{
    __super::DrawDebugLines(camera_params, desc);

    // コライダーの外周を描画
    const Vector2D center_position = GetWorldPosition();
    std::array<Vector2D,4> center_to_vertexes;
    CalcCenterToAllVertices(center_to_vertexes);

	std::vector<Vector2D> vertex_positions;
	GetVertexPositions(vertex_positions);

    DrawBlendInfo blend_info = DrawBlendInfo(DX_BLENDMODE_ALPHA, desc.box.line_alpha);
    for (size_t i = 0; i < 4; i++)
    {
		Vector2D& from = vertex_positions.at(i);
		Vector2D& to = vertex_positions.at((i + 1) % 4);
        GetScene()->DrawDebugLine(from, to, desc.box.line_color, desc.box.line_thickness, blend_info);
    }
}

Vector2D BoxCollider::GetCenterWorldPosition() const
{
    return GetWorldPosition();
}

void BoxCollider::GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const
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

void BoxCollider::SetBoxColliderParams(
    const CollisionType new_collision_type, const CollisionObjectType new_collision_object_type, const std::vector<CollisionObjectType>& new_hit_object_types,
    const bool new_pushability, const Vector2D& new_box_extent
)
{
    SetColliderCommonParams(
        new_collision_type,
        new_collision_object_type,
        new_hit_object_types,
        new_pushability
    );
    SetBoxColliderExtent(new_box_extent);
}

void BoxCollider::SetBoxColliderExtent(const Vector2D& new_box_extent)
{
    unrotaed_diagonal = new_box_extent;

    // 中心から各頂点へのベクトル
    Vector2D to_left_top = new_box_extent / (-2.0f);
    Vector2D to_left_bottom = to_left_top * Vector2D(1.0f, -1.0f);
    Vector2D to_right_bottom = to_left_top * Vector2D(-1.0f, -1.0f);
    Vector2D to_right_top = to_left_top * Vector2D(-1.0f, 1.0f);

    center_to_vertices.at(static_cast<uint8_t>(BoxColliderVertex::LEFT_TOP)) = to_left_top;
    center_to_vertices.at(static_cast<uint8_t>(BoxColliderVertex::LEFT_BOTTOM)) = to_left_bottom;
    center_to_vertices.at(static_cast<uint8_t>(BoxColliderVertex::RIGHT_BOTTOM)) = to_right_bottom;
    center_to_vertices.at(static_cast<uint8_t>(BoxColliderVertex::RIGHT_TOP)) = to_right_top;
}

Vector2D BoxCollider::CalcCenterToVertex(BoxColliderVertex vertex) const
{
    const uint8_t index = static_cast<uint8_t>(vertex);
    return Vector2D::Rotate(center_to_vertices.at(index), GetWorldRotation());
}

FRectAA BoxCollider::GetGeometricRectAA() const
{
	const Vector2D world_pos = GetWorldPosition();
    FRectAA ret{};
	ret.left_top = world_pos - unrotaed_diagonal / 2.0f;
	ret.right_bottom = world_pos + unrotaed_diagonal / 2.0f;
	return ret;
}

FRect BoxCollider::GetGeometricRect() const
{
	const Vector2D center = GetWorldPosition();
    const Vector2D extent = GetBoxExtent();
    const float rotation = GetWorldRotation();
    return FRect{ center, extent.x, extent.y, rotation };
}

void BoxCollider::CalcCenterToAllVertices(std::array<Vector2D,4>& out_center_to_vertexes) const
{
    for (uint8_t i = 0; i < center_to_vertices.size(); i++)
    {
        out_center_to_vertexes.at(i) = CalcCenterToVertex(static_cast<BoxColliderVertex>(i));
    }

}
