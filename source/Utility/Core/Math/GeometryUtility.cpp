#include "Utility/Core/MathCore.h"
#include "SystemTypes.h"

const float GeometricUtility::PENETRATION_DEPTH_OFFSET = 0.f;

bool GeometricUtility::DoesSegmentIntersectWithAnother(Vector2D& out_intersection_position, const FSegment& seg_a, const FSegment& seg_b)
{
	const Vector2D& OA = seg_a.start;
	const Vector2D& OB = seg_a.end;
	const Vector2D& OC = seg_b.start;
	const Vector2D& OD = seg_b.end;

	// OA + sAB = OC + tCD, (0 <= s,t <= 1)
	const Vector2D AB = OB - OA;
	const Vector2D CD = OD - OC;
	const float ABxCD = Vector2D::Cross(AB, CD);
	if (fabsf(ABxCD) < EPSIRON)
	{
		return false;
	}

	const float s = Vector2D::Cross(OC - OA, CD) / ABxCD;
	const float t = Vector2D::Cross(OC - OA, AB) / ABxCD;

	if (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f)
	{
		out_intersection_position = OA + AB * s;
		return true;
	}

	return false;
}

bool GeometricUtility::DoesSegmentIntersectWithAnother(Vector2D& out_intersection_position, const Vector2D& seg_a_start, const Vector2D& seg_a_end, const Vector2D& seg_b_start, const Vector2D& seg_b_end)
{
	return DoesSegmentIntersectWithAnother(out_intersection_position, FSegment{ seg_a_start, seg_a_end }, FSegment{ seg_b_start, seg_b_end });
}

bool GeometricUtility::DoesSegmentIntersectWithAnother(const FSegment& seg_a, const FSegment& seg_b)
{
	Vector2D dummy{};
	return DoesSegmentIntersectWithAnother(dummy, seg_a, seg_b);
}

bool GeometricUtility::DoesTriangleOverlapWIthAnother(const FTriangle& triangle_a, const FTriangle& triangle_b, Vector2D* penetration_depth)
{
	return false;
}

int GeometricUtility::GetSegmentRectIntersections(
    std::array<Vector2D, 2>& out_intersection_posisions, const FSegment& segment, const FRect& rect, std::array<FSegment, 2>* intersected_rect_edges
)
{
	int intersection_count = 0;

    std::array<Vector2D, 4> rect_vertices;
    rect.GetVertices(rect_vertices);

    for (size_t i = 0; i < 4; i++)
	{
		const FSegment rect_edge{ rect_vertices.at(i), rect_vertices.at((i + 1) % 4) };

		if
		(
			DoesSegmentIntersectWithAnother
			(
				out_intersection_posisions.at(intersection_count), segment, rect_edge
			)
		)
		{
			if (intersected_rect_edges != nullptr)
			{
				intersected_rect_edges->at(intersection_count) = rect_edge;
			}
			intersection_count++;
		}

        if (intersection_count == 2)
        {
            break;
        }
    }

	return intersection_count;
}

int GeometricUtility::GetSegmentRectIntersections(const FSegment& segment, const FRect& rect, std::array<FSegment, 2>* intersected_rect_edges)
{
	std::array<Vector2D, 2> intersections;
	return GetSegmentRectIntersections(intersections, segment, rect, intersected_rect_edges);
}

bool GeometricUtility::DoesRectContainsSegment(const FRect& rect, const FSegment& segment)
{
	return DoesRectContainPoint(segment.start, rect) && DoesRectContainPoint(segment.end, rect);
}

bool GeometricUtility::DoesRectOverlapWithSegment(const FRect& rect, const FSegment& segment, Vector2D& out_penetration_depth)
{
    constexpr float PENETRATION_DEPTH_OFFSET_RECT_SEGMENT = 0.f;

    const Vector2D segment_mid = segment.GetMidPoint();
	const Vector2D d = rect.center - segment_mid;
	const Vector2D rect_wvec = rect.GetWidthVector();
	const Vector2D rect_hvec = rect.GetHeightVector();
	const Vector2D segment_vec = segment.end - segment.start;
    const Vector2D segment_vertical_vec = Vector2D::Rotate(segment_vec, CLN2D_PI * 0.5f);

    auto get_penetration_depth = [&](const Vector2D& v)
        {
            const float v_length = v.Length();
            const float d0 = (fabsf(Vector2D::Dot(v, rect_wvec)) + fabsf(Vector2D::Dot(v, rect_hvec))) * 0.5f;
            const float d1 = fabsf(Vector2D::Dot(v, segment_vec)) * 0.5f;
            const float d2 = fabsf(Vector2D::Dot(v, d));

			const float depth = ((d0 + d1) - d2) / v_length + PENETRATION_DEPTH_OFFSET + PENETRATION_DEPTH_OFFSET_RECT_SEGMENT;
            return depth;
        };

	// 各軸に沿っためり込み深度. どれか1つでも0未満なら重なっていないと判定
	const float depth_rect_w = get_penetration_depth(rect_wvec);
	if (depth_rect_w < 0) { return false; }

	const float depth_rect_h = get_penetration_depth(rect_hvec);
	if (depth_rect_h < 0) { return false; }

	const float depth_segment = get_penetration_depth(segment_vec);
	if (depth_segment < 0) { return false; }

	const float depth_segment_vertical = get_penetration_depth(segment_vertical_vec);
	if (depth_segment_vertical < 0) { return false; }

	// めり込み深度が全て0以上なので重なっていると判定
	const float min_depth = std::min({ depth_rect_w, depth_rect_h, depth_segment, depth_segment_vertical });
	if (min_depth == depth_rect_w)
	{
		out_penetration_depth = rect_wvec * depth_rect_w / rect_wvec.Length();
	}
	else if (min_depth == depth_rect_h)
	{
		out_penetration_depth = rect_hvec * depth_rect_h / rect_hvec.Length();
	}
	else if (min_depth == depth_segment)
	{
		out_penetration_depth = segment_vec * depth_segment / segment_vec.Length();
	}
	else if (min_depth == depth_segment_vertical)
	{
		out_penetration_depth = segment_vertical_vec * depth_segment_vertical / segment_vertical_vec.Length();
	}
}

bool GeometricUtility::DoesRectOverlapWithSegment(const FRect& rect, const FSegment& segment)
{
	if (DoesRectContainsSegment(rect, segment))
	{
		return true;
	}

	const bool ret = GetSegmentRectIntersections(segment, rect) > 0;
	return ret;
}

bool GeometricUtility::DoesRectOverlapWithTriangle(const FRect& rect, const FTriangle& triangle, Vector2D* penetration_depth)
{
	std::array<Vector2D,4> rect_vertices;
	rect.GetVertices(rect_vertices);
	auto get_range_rect = [&rect_vertices](float& out_min, float& out_max, const Vector2D& axis)
		{
			out_min = FLT_MAX;
			out_max = -FLT_MAX;
			for (const auto& vertex : rect_vertices)
			{
				const float projection = Vector2D::Dot(vertex, axis);
				out_min = std::min(out_min, projection);
				out_max = std::max(out_max, projection);
			}
		};

	std::array<Vector2D,3> triangle_vertices;
	triangle.GetVertices(triangle_vertices);
	auto get_range_triangle = [&triangle_vertices](float& out_min, float& out_max, const Vector2D& axis)
		{
			out_min = FLT_MAX;
			out_max = -FLT_MAX;
			for (const auto& vertex : triangle_vertices)
			{
				const float projection = Vector2D::Dot(vertex, axis);
				out_min = std::min(out_min, projection);
				out_max = std::max(out_max, projection);
			}
		};

	// 分離軸の候補は矩形の幅ベクトルと高さベクトル、三角形の各辺の法線
	std::array<Vector2D, 5> separating_axis_candidates;
	separating_axis_candidates[0] = rect.GetWidthVector().Normalize();
	separating_axis_candidates[1] = rect.GetHeightVector().Normalize();
	separating_axis_candidates[2] = (triangle_vertices.at(1) - triangle_vertices.at(0)).GetRotated(CLN2D_HALF_PI).Normalize();
	separating_axis_candidates[3] = (triangle_vertices.at(2) - triangle_vertices.at(1)).GetRotated(CLN2D_HALF_PI).Normalize();
	separating_axis_candidates[4] = (triangle_vertices.at(0) - triangle_vertices.at(2)).GetRotated(CLN2D_HALF_PI).Normalize();

	size_t min_penetration_axis_index = -1;
	float min_penetration_depth = FLT_MAX;
	for (size_t i = 0; i < separating_axis_candidates.size(); i++)
	{
		const Vector2D& axis = separating_axis_candidates.at(i);

		float rect_min = 0, rect_max = 0, triangle_min = 0, triangle_max = 0;
		get_range_rect(rect_min, rect_max, axis);
		get_range_triangle(triangle_min, triangle_max, axis);

		// 射影の重複区間の長さを求める
		const float overlap_length = std::min(rect_max, triangle_max) - std::max(rect_min, triangle_min);

		if (overlap_length < 0)
		{
			return false;
		}
		else
		{
			if (overlap_length < min_penetration_depth)
			{
				min_penetration_depth = overlap_length;
				min_penetration_axis_index = i;
			}
		}
	}

	if (penetration_depth != nullptr)
	{
		*penetration_depth = separating_axis_candidates.at(min_penetration_axis_index) * min_penetration_depth;
	}

	return true;
}

bool GeometricUtility::DoesRectOverlapWithAnother(const FRect& rect_a, const FRect& rect_b, Vector2D* penetration_depth)
{
	const Vector2D d = rect_a.center - rect_b.center;
    const Vector2D wvec_a = rect_a.GetWidthVector();
	const Vector2D hvec_a = rect_a.GetHeightVector();
	const Vector2D wvec_b = rect_b.GetWidthVector();
	const Vector2D hvec_b = rect_b.GetHeightVector();

	// vはrect_aもしくはrect_bの幅ベクトル、高さベクトルのいずれか
    auto get_penetration_depth = [&d](const Vector2D& v, const Vector2D& other_wvec, const Vector2D& other_hvec)
        {
			const float v_length = v.Length();
            const float d0 = v_length * 0.5f;
            const float d1 = (fabsf(Vector2D::Dot(v, other_wvec)) + fabsf(Vector2D::Dot(v, other_hvec))) / (2.f * v_length);
			const float d2 = fabsf(Vector2D::Dot(v, d)) / v_length;

			// vに沿っためり込みの長さ. 0未満なら絶対値がvに沿った矩形間の隙間の長さを表す
            const float depth = (d0 + d1) - d2 + PENETRATION_DEPTH_OFFSET;
            return depth;
        };

	// 各軸に沿っためり込み深度. どれか1つでも0未満なら重なっていないと判定
	const float depth_a_w = get_penetration_depth(wvec_a, wvec_b, hvec_b);
	if (depth_a_w < 0){	return false;}

	const float depth_a_h = get_penetration_depth(hvec_a, wvec_b, hvec_b);
	if (depth_a_h < 0) { return false; }

	const float depth_b_w = get_penetration_depth(wvec_b, wvec_a, hvec_a);
	if (depth_b_w < 0) { return false; }

	const float depth_b_h = get_penetration_depth(hvec_b, wvec_a, hvec_a);
	if (depth_b_h < 0) { return false; }

	// めり込み深度が全て0以上なので重なっていると判定
	if (penetration_depth != nullptr)
	{
		const float min_depth = std::min({ depth_a_w, depth_a_h, depth_b_w, depth_b_h });
        if (min_depth == depth_a_w)
        {
			*penetration_depth = wvec_a * depth_a_w / wvec_a.Length();
        }
		else if (min_depth == depth_a_h)
		{
			*penetration_depth = hvec_a * depth_a_h / hvec_a.Length();
		}
		else if (min_depth == depth_b_w)
		{
			*penetration_depth = wvec_b * depth_b_w / wvec_b.Length();
		}
		else if (min_depth == depth_b_h)
		{
			*penetration_depth = hvec_b * depth_b_h / hvec_b.Length();
		}
	}
	return true;
}

bool GeometricUtility::DoesRectOverlapWithAnother(const FRect& rect, const FRectAA& aa_rect, Vector2D* penetration_depth)
{
	const Vector2D center = aa_rect.left_top + (aa_rect.right_bottom - aa_rect.left_top) / 2.f;
	const FRect aa_rect_as_rect = FRect(center, (aa_rect.right_bottom.x - aa_rect.left_top.x), (aa_rect.right_bottom.y - aa_rect.left_top.y), 0.f);
	return DoesRectOverlapWithAnother(rect, aa_rect_as_rect, penetration_depth);
}

bool GeometricUtility::IsPointOnSegment(const Vector2D& query_point, const Vector2D& start_point, const Vector2D& end_point, float tolerance)
{
    return Vector2D::Dot(query_point - start_point, query_point - end_point) < tolerance;
}

bool GeometricUtility::DoesRectContainAnother(const Vector2D& outer_left_top, const Vector2D& outer_right_bottom, const Vector2D& inner_left_top, const Vector2D& inner_right_bottom)
{
    bool outer_contains_inner =
        outer_left_top.x < inner_left_top.x &&
        outer_left_top.y < inner_left_top.y &&
        outer_right_bottom.x > inner_right_bottom.x &&
        outer_right_bottom.y > inner_right_bottom.y;

    return outer_contains_inner;
}

bool GeometricUtility::DoesRectContainAnother(const FRectAA& outer, const FRectAA& inner)
{
    return DoesRectContainAnother(outer.left_top, outer.right_bottom, inner.left_top, inner.right_bottom);
}

bool GeometricUtility::DoesRectContainPoint(const Vector2D& query_point, const Vector2D& A, const Vector2D& B, const Vector2D& D)
{
    const Vector2D& P = query_point;
    const Vector2D vecAP = P - A;
    const Vector2D vecAB = B - A;
    const Vector2D vecAD = D - A;


    // vecAPを辺AB, ADに射影し、各射影の長さがvecAB, vecADの長さよりも短ければ点Pは矩形内部に存在
    float AP_dot_AD = Vector2D::Dot(vecAP, vecAD);
    float AP_dot_AB = Vector2D::Dot(vecAP, vecAB);

    bool located_between_AB_and_CD =
        AP_dot_AD > 0 &&
        AP_dot_AD < Vector2D::Dot(vecAD, vecAD);
    bool located_between_AD_and_BC =
        AP_dot_AB > 0 &&
        AP_dot_AB < Vector2D::Dot(vecAB, vecAB);

    return located_between_AB_and_CD && located_between_AD_and_BC;
}

bool GeometricUtility::DoesRectContainPoint(const Vector2D& query_point, const Vector2D& rect_center, const Vector2D& base_rect_diag, const float rect_rotation)
{
    const Vector2D half_base_diag = base_rect_diag / 2.f;
    const Vector2D A = rect_center - Vector2D::Rotate(half_base_diag, rect_rotation);
    const Vector2D B = rect_center + Vector2D::Rotate(half_base_diag * Vector2D(-1.f, 1.f), rect_rotation);
    const Vector2D D = rect_center + Vector2D::Rotate(half_base_diag * Vector2D(1.f, -1.f), rect_rotation);
    
    return DoesRectContainPoint(query_point, A, B, D);
}

bool GeometricUtility::DoesRectContainPoint(const Vector2D& query_point, const FRect& rect)
{
	std::array<Vector2D, 4> vertices;
    rect.GetVertices(vertices);
	return DoesRectContainPoint(query_point, vertices.at(0), vertices.at(1), vertices.at(3));
}

bool GeometricUtility::DoesAARectContainPoint(const Vector2D& query_point, const Vector2D& A, const Vector2D& A_opposite)
{
	const float qx = query_point.x;
	const float qy = query_point.y;

	const bool is_in_range_x = (qx - A.x) * (qx - A_opposite.x) <= 0;
	const bool is_in_range_y = (qy - A.y) * (qy - A_opposite.y) <= 0;

	return is_in_range_x && is_in_range_y;
}

bool GeometricUtility::DoesAARectContainPoint(const Vector2D& query_point, const FRectAA& rect)
{
	return DoesAARectContainPoint(query_point, rect.left_top, rect.right_bottom);
}

bool GeometricUtility::DoesAARectContainSegment(const FRectAA& rect, const FSegment& segment)
{
	return DoesAARectContainPoint(segment.start, rect) && DoesAARectContainPoint(segment.end, rect);
}

bool GeometricUtility::DoesAARectContainsCircle(const FRectAA& rect, const FCircle& circle)
{
	const float rect_left = rect.left_top.x;
	const float rect_top = rect.left_top.y;
	const float rect_right = rect.right_bottom.x;
	const float rect_bottom = rect.right_bottom.y;

	const float circle_left = circle.center.x - circle.radius;
	const float circle_top = circle.center.y - circle.radius;
	const float circle_right = circle.center.x + circle.radius;
	const float circle_bottom = circle.center.y + circle.radius;

	return
		rect_left <= circle_left &&
		rect_top <= circle_top &&
		rect_right >= circle_right &&
		rect_bottom >= circle_bottom;
}

bool GeometricUtility::DoesAARectOverlapWithAnother(const FRectAA& rect_a, const FRectAA& rect_b)
{
	const bool is_overlap_x = (rect_a.left_top.x <= rect_b.right_bottom.x) && (rect_a.right_bottom.x >= rect_b.left_top.x);
	const bool is_overlap_y = (rect_a.left_top.y <= rect_b.right_bottom.y) && (rect_a.right_bottom.y >= rect_b.left_top.y);

	return is_overlap_x && is_overlap_y;
}

bool GeometricUtility::DoesAARectOverlapWithTriangle(const FRectAA& rect, const FTriangle& triangle)
{
	// 三角形の頂点が矩形内に存在するか
	for (auto& vertex : triangle.vertices)
	{
		if (DoesAARectContainPoint(vertex, rect))
		{
			return true;
		}
	}

	// 矩形の辺が三角形の辺と交差するか

	std::array<FSegment, 4> rect_edges;
	rect.GetEdges(rect_edges);

	std::array<FSegment, 3> triangle_edges;
	triangle.GetEdges(triangle_edges);

	for (auto& rect_edge : rect_edges)
	{
		for (auto& triangle_edge : triangle_edges)
		{
			if (DoesSegmentIntersectWithAnother(rect_edge, triangle_edge))
			{
				return true;
			}
		}
	}

	return false;
}

int GeometricUtility::GetSegmentAARectIntersections(std::array<Vector2D, 2>& out_intersection_positions, const FSegment& segment, const FRectAA& rect)
{
	int intersection_count = 0;

	const Vector2D& A = rect.left_top;
	const Vector2D& B = Vector2D(rect.right_bottom.x, rect.left_top.y);
	const Vector2D& C = rect.right_bottom;
	const Vector2D& D = Vector2D(rect.left_top.x, rect.right_bottom.y);

	auto should_test_intersection = [&intersection_count]() { return intersection_count < 2; };

	if (should_test_intersection() && DoesSegmentIntersectWithAnother(out_intersection_positions.at(intersection_count), segment, FSegment{ A, B }))
	{
		intersection_count++;
	}

	if (should_test_intersection() && DoesSegmentIntersectWithAnother(out_intersection_positions.at(intersection_count), segment, FSegment{ B, C }))
	{
		intersection_count++;
	}

	if (should_test_intersection() && DoesSegmentIntersectWithAnother(out_intersection_positions.at(intersection_count), segment, FSegment{ C, D }))
	{
		intersection_count++;
	}

	if (should_test_intersection() && DoesSegmentIntersectWithAnother(out_intersection_positions.at(intersection_count), segment, FSegment{ D, A }))
	{
		intersection_count++;
	}

	return intersection_count;
}

int GeometricUtility::GetSegmentAARectIntersections(const FSegment& segment, const FRectAA& rect)
{
	std::array<Vector2D, 2> intersections;
	return GetSegmentAARectIntersections(intersections, segment, rect);
}

bool GeometricUtility::DoesConvexPolygonContainsPoint(const Vector2D& query_point, const std::vector<Vector2D>& vertexes)
{
    if (vertexes.size() < 3)
    {
        return false;
    }

    float last_cross_value = 0;
    for (int i = 0; i < vertexes.size(); i++)
    {
        const Vector2D& vtx_from = vertexes.at(i);
        const Vector2D& vtx_to = (i == vertexes.size() - 1) ? vertexes.at(0) : vertexes.at(i + 1);

        const float cross_value = Vector2D::Cross(vtx_to - vtx_from, query_point - vtx_from);
        if (cross_value * last_cross_value < 0.f)
        {
            return false;
        }
        last_cross_value = cross_value;
    }

    return true;
}

bool GeometricUtility::DoesCircleOverlapWithAnother(const FCircle& circle_a, const FCircle& circle_b, Vector2D* penetration_depth)
{
	const Vector2D distance_vec = circle_b.center - circle_a.center;
	const float distance = distance_vec.Length();
	const float radius_sum = circle_a.radius + circle_b.radius;

	const bool is_overlapping = distance < radius_sum;

	if (!is_overlapping)
	{
		return false;
	}

	if (penetration_depth)
	{
		const float penetration_length = radius_sum - distance;
		*penetration_depth = distance_vec.Normalize() / distance * penetration_length;
	}

	return true;
}

FRect FRectAA::ToFRect() const
{
	const Vector2D center = (left_top + right_bottom) * 0.5f;
	const float width = right_bottom.x - left_top.x;
	const float height = right_bottom.y - left_top.y;
	return FRect(center, width, height, 0.f);
}

void FRectAA::GetEdges(std::vector<FSegment>& out_edges) const
{
	std::array<FSegment, 4> edges;
	GetEdges(edges);
	out_edges = std::vector<FSegment>(4);
	out_edges[0] = edges[0];
	out_edges[1] = edges[1];
	out_edges[2] = edges[2];
	out_edges[3] = edges[3];
}

void FRectAA::GetEdges(std::array<FSegment, 4>& out_edges) const
{
	out_edges[0] = FSegment(left_top, Vector2D{ right_bottom.x, left_top.y });
	out_edges[1] = FSegment(Vector2D{ right_bottom.x, left_top.y }, right_bottom);
	out_edges[2] = FSegment(right_bottom, Vector2D{ left_top.x, right_bottom.y });
	out_edges[3] = FSegment(Vector2D{ left_top.x, right_bottom.y }, left_top);
}

void FTriangle::GetEdges(std::vector<FSegment>& out_edges) const
{
	out_edges = std::vector<FSegment>(3);
	out_edges[0] = FSegment(vertices[0], vertices[1]);
	out_edges[1] = FSegment(vertices[1], vertices[2]);
	out_edges[2] = FSegment(vertices[2], vertices[0]);
}

void FTriangle::GetEdges(std::array<FSegment, 3>& out_edges) const
{
	out_edges[0] = FSegment(vertices[0], vertices[1]);
	out_edges[1] = FSegment(vertices[1], vertices[2]);
	out_edges[2] = FSegment(vertices[2], vertices[0]);
}
