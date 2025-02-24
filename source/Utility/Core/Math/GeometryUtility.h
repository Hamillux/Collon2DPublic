#pragma once
#include "Vector2D.h"
#include <array>
#include <vector>

// 矩形
struct FRect
{
	Vector2D center;
	float width;
	float height;
	float rotation;

	void GetVertices(std::vector<Vector2D>& out_vertices) const
	{
		std::array<Vector2D, 4> vertices;
		GetVertices(vertices);

		out_vertices = std::vector<Vector2D>(4);
		for (size_t i = 0; i < 4; i++)
		{
			out_vertices[i] = vertices[i];
		}
	}

	void GetVertices(std::array<Vector2D, 4>& out_vertices) const
	{
		const float half_width = width * 0.5f;
		const float half_height = height * 0.5f;

		const Vector2D to_v0 = Vector2D::Rotate(Vector2D{ -half_width, -half_height }, rotation);
		const Vector2D to_v3 = Vector2D::Rotate(Vector2D{ half_width, -half_height }, rotation);

		out_vertices[0] = center + to_v0;
		out_vertices[1] = center - to_v3;
		out_vertices[2] = center - to_v0;
		out_vertices[3] = center + to_v3;
	}

	/// <summary>
	/// 矩形の幅ベクトルを取得
	/// </summary>
	Vector2D GetWidthVector() const
	{
		return Vector2D::Rotate(Vector2D{ width, 0.f }, rotation);
	}

	/// <summary>
	/// 矩形の高さベクトルを取得
	/// </summary>
	Vector2D GetHeightVector() const
	{
		return Vector2D::Rotate(Vector2D{ 0.f, height }, rotation);
	}

	FRect()
		: center(Vector2D{})
		, width(0.f)
		, height(0.f)
		, rotation(0.f)
	{}

	/// <param name="center_">中心座標</param>
	/// <param name="width_">幅 (回転ゼロ時のX軸方向の長さ)</param>
	/// <param name="height_">高さ (回転ゼロ時のY軸方向の長さ)</param>
	/// <param name="rotation_">回転 [rad]</param>
	FRect(const Vector2D& center_, float width_, float height_, float rotation_)
		: center(center_)
		, width(width_)
		, height(height_)
		, rotation(rotation_)
	{}
};

// 軸に平行な矩形
struct FSegment;
struct FRectAA
{
	Vector2D left_top;
	Vector2D right_bottom;

	FRect ToFRect() const;

	void GetEdges(std::vector<FSegment>& out_edges) const;
	void GetEdges(std::array<FSegment, 4>& out_edges) const;

	FRectAA()
		: left_top(Vector2D{})
		, right_bottom(Vector2D{})
	{}

	/// <param name="left_top_">矩形左上の座標</param>
	/// <param name="right_bottom_">矩形右下の座標</param>
	FRectAA(const Vector2D& left_top_, const Vector2D& right_bottom_)
		: left_top(left_top_)
		, right_bottom(right_bottom_)
	{}

	FRectAA(const Vector2D& center_, float width_, float height_)
		: left_top(center_ - Vector2D{ width_ * 0.5f, height_ * 0.5f })
		, right_bottom(center_ + Vector2D{ width_ * 0.5f, height_ * 0.5f })
	{}
};

/// <summary>
/// 三角形
/// </summary>
struct FTriangle 
{
	std::array<Vector2D, 3> vertices;

	FTriangle()
	{}

	FTriangle(const Vector2D& vertex_a, const Vector2D& vertex_b, const Vector2D& vertex_c)
		: vertices({ vertex_a, vertex_b, vertex_c })
	{}

	FTriangle(const std::vector<Vector2D>& vertices_) 
	{
		for (size_t i = 0; i < 3; i++)
		{
			vertices[i] = vertices_[i];
		}
	}

	FTriangle(const std::array<Vector2D, 3>& vertices_)
		: vertices(vertices_)
	{}

	void GetEdges(std::vector<FSegment>& out_edges) const;
	void GetEdges(std::array<FSegment, 3>& out_edges) const;
	void GetVertices(std::vector<Vector2D>& out_vertices) const
	{
		out_vertices = std::vector<Vector2D>(3);
		for (size_t i = 0; i < 3; i++)
		{
			out_vertices[i] = vertices[i];
		}
	}
	void GetVertices(std::array<Vector2D, 3>& out_vertices) const
	{
		for (size_t i = 0; i < 3; i++)
		{
			out_vertices[i] = vertices[i];
		}
	}
};

/// <summary>
/// 有向線分
/// </summary>
struct FSegment
{
	Vector2D start;
	Vector2D end;

	Vector2D GetMidPoint() const
	{
		return (start + end) * 0.5f;
	}

	Vector2D GetDirection() const
	{
		return (end - start).Normalize();
	}

	Vector2D GetDirectionUnnormalized() const
	{
		return end - start;
	}

	FSegment()
		: start(Vector2D{})
		, end(Vector2D{})
	{}

	/// <param name="start_">始点</param>
	/// <param name="end_">終点</param>
	FSegment(const Vector2D& start_, const Vector2D& end_)
		: start(start_)
		, end(end_)
	{}
};

/// <summary>
/// 円
/// </summary>
struct FCircle
{
	Vector2D center;
	float radius;

	FCircle()
		: center(Vector2D{})
		, radius(0.f)
	{
	}

	/// <param name="center_">中心座標</param>
	/// <param name="radius_">半径</param>
	FCircle(const Vector2D& center_, float radius_)
		: center(center_)
		, radius(radius_)
	{
	}
};


/// <summary>
/// 幾何的な計算や判定を行うユーティリティクラス
/// </summary>
class GeometricUtility
{
public:

	////////~ Begin DoesSegmentIntersectWithAnother overloads
	//
	/// <summary>
	/// 2つの線分が交差するかを判定
	/// </summary>
	/// <param name="out_intersection_position">交点</param>
	/// <param name="seg_a">線分1</param>
	/// <param name="seg_b">線分2</param>
	/// <returns></returns>
	static bool DoesSegmentIntersectWithAnother(Vector2D& out_intersection_position, const FSegment& seg_a, const FSegment& seg_b);

	/// <summary>
	/// 2つの線分が交差するかを判定
	/// </summary>
	/// <param name="out_intersection_position">交点</param>
	/// <param name="seg_a_start">線分1の始点</param>
	/// <param name="seg_a_end">線分1の終点</param>
	/// <param name="seg_b_start">線分2の始点</param>
	/// <param name="seg_b_end">線分2の終点</param>
	/// <returns></returns>
	static bool DoesSegmentIntersectWithAnother(Vector2D& out_intersection_position, const Vector2D& seg_a_start, const Vector2D& seg_a_end, const Vector2D& seg_b_start, const Vector2D& seg_b_end);

	/// <summary>
	/// 2つの線分が交差するかを判定. 交点座標が不要な場合はこちら
	/// </summary>
	/// <param name="seg_a">線分1</param>
	/// <param name="seg_b">線分2</param>
	/// <returns></returns>
	static bool DoesSegmentIntersectWithAnother(const FSegment& seg_a, const FSegment& seg_b);

	//
	//////~ End DoesSegmentIntersectWithAnother overloads

	static bool DoesTriangleOverlapWIthAnother(const FTriangle& triangle_a, const FTriangle& triangle_b, Vector2D* penetration_depth = nullptr);

	//////~ Begin GetSegmentRectIntersections overloads
	//
	/// <summary>
	/// 線分と矩形の交点を取得
	/// </summary>
	/// <param name="out_intersection_position">交点の格納先</param>
	/// <param name="rect">矩形</param>
	/// <param name="segment">線分</param>
	/// <returns>交点の数</returns>
	static int GetSegmentRectIntersections(
		std::array<Vector2D, 2>& out_intersection_posisions,
		const FSegment& segment,
		const FRect& rect,
		std::array<FSegment, 2>* intersected_rect_edges = nullptr
	);

	/// <summary>
	/// 線分と矩形の交点を取得. 交点の数のみ取得したい場合はこちら
	/// </summary>
	/// <param name="segment">線分</param>
	/// <param name="rect">矩形</param>
	/// <returns>交点数</returns>
	static int GetSegmentRectIntersections(const FSegment& segment, const FRect& rect, std::array<FSegment, 2>* intersected_rect_edges = nullptr);
	//
	//////~ End GetSegmentRectIntersections overloads

	/// <summary>
	/// 矩形内部に線分が内包されるかを判定
	/// </summary>
	/// <param name="rect">矩形</param>
	/// <param name="segment">線分</param>
	static bool DoesRectContainsSegment(const FRect& rect, const FSegment& segment);

	static bool DoesRectOverlapWithSegment(const FRect& rect, const FSegment& segment, Vector2D& penetration_depth);
	static bool DoesRectOverlapWithSegment(const FRect& rect, const FSegment& segment);

	static bool DoesRectOverlapWithTriangle(const FRect& rect, const FTriangle& triangle, Vector2D* penetration_depth = nullptr);

	static bool DoesRectOverlapWithAnother(const FRect& rect_a, const FRect& rect_b, Vector2D* penetration_depth = nullptr);
	static bool DoesRectOverlapWithAnother(const FRect& rect, const FRectAA& aa_rect, Vector2D* penetration_depth = nullptr);

	/**
	*	点が線分上に存在するかを判定
	*	@param query_point 判定する点の位置ベクトル
	*   @param start_point, end_point 線分の両端の位置ベクトル
	*/
	static bool IsPointOnSegment(const Vector2D& query_point, const Vector2D& start_point, const Vector2D& end_point, float tolerance = 1e-5f);


	////////~ Begin DoesRectContainPoint overloads
	//
	/// <summary>
	/// 矩形が点を内包するか
	/// </summary>
	/// <param name="outer_left_top"> 外側の矩形の左上座標 </param>
	/// <param name="outer_right_bottom"> 外側の矩形の右下座標 </param>
	/// <param name="inner_left_top"> 内側の矩形の左上座標 </param>
	/// <param name="inner_right_bottom"> 内側の矩形の右下座標 </param>
	/// <returns></returns>
	static bool DoesRectContainAnother(const Vector2D& outer_left_top, const Vector2D& outer_right_bottom, const Vector2D& inner_left_top, const Vector2D& inner_right_bottom);

	/// <summary>
	/// 矩形が別の矩形を内包するか
	/// </summary>
	/// <param name="outer"> 外側の矩形 </param>
	/// <param name="inner"> 内側の矩形 </param>
	static bool DoesRectContainAnother(const FRectAA& outer, const FRectAA& inner);
	//
	////////~ End DoesRectContainPoint overloads


	////////~ Begin DoesRectContainPoint overloads
	//
	/// <summary>
	/// 点が矩形内部に存在するか.
	/// <para>A,B,Dは, 矩形の任意の頂点Aと, Aと隣り合う2点B,D</para>
	/// </summary>
	/// <param name="query_point">チェック対象の点</param>
	/// <param name="A">矩形の任意の頂点</param>
	/// <param name="B">Aと隣接する頂点1</param>
	/// <param name="D">Aと隣接する頂点2</param>
	static bool DoesRectContainPoint(const Vector2D& query_point, const Vector2D& A, const Vector2D& B, const Vector2D& D);

	/// <summary>
	/// 点が矩形内部に存在するか.
	/// </summary>
	/// <param name="query_point">チェック対象の点</param>
	/// <param name="center">矩形の中心</param>
	/// <param name="base_diagonal">回転ゼロ時の対角線を表すベクトル</param>
	/// <param name="rotation">矩形の回転量</param>
	static bool DoesRectContainPoint(const Vector2D& query_point, const Vector2D& center, const Vector2D& base_diagonal, const float rotation);

	/// <summary>
	/// 点が矩形内部に存在するか.
	/// </summary>
	/// <param name="query_point">チェック対象の点</param>
	/// <param name="rect">矩形</param>
	static bool DoesRectContainPoint(const Vector2D& query_point, const FRect& rect);
	//
	////////~ End DoesRectContainPoint overloads


	////////~ Begin DoesAARectContainPoint overloads
	//
	/// <summary>
	/// 軸に平行な矩形が点を内包するか
	/// </summary>
	/// <param name="query_point">チェック対象点</param>
	/// <param name="A">軸に平行な矩形の任意頂点</param>
	/// <param name="A_opposite">Aの対頂点</param>
	static bool DoesAARectContainPoint(const Vector2D& query_point, const Vector2D& A, const Vector2D& A_opposite);

	/// <summary>
	/// 軸に平行な矩形が点を内包するか
	/// </summary>
	/// <param name="query_point">チェック対象点</param>
	/// <param name="rect">軸に平行な矩形</param>
	static bool DoesAARectContainPoint(const Vector2D& query_point, const FRectAA& rect);
	//
	////////~ End DoesAARectContainPoint overloads

	/// <summary>
	/// 軸に平行な矩形が線分を内包するか
	/// </summary>
	/// <param name="rect">軸に平行な矩形</param>
	/// <param name="segment">線分</param>
	static bool DoesAARectContainSegment(const FRectAA& rect, const FSegment& segment);

	/// <summary>
	/// 軸に平行な矩形が円を内包するか
	/// </summary>
	/// <param name="rect">軸に平行な矩形</param>
	/// <param name="circle">円</param>
	/// <returns></returns>
	static bool DoesAARectContainsCircle(const FRectAA& rect, const FCircle& circle);

	/// <summary>
	///	軸に平行な矩形2つが重なっているか
	/// </summary>
	/// <param name="rect_a">軸に平行な矩形1</param>
	/// <param name="rect_b">軸に平行な矩形2</param>
	static bool DoesAARectOverlapWithAnother(const FRectAA& rect_a, const FRectAA& rect_b);

	/// <summary>
	/// 軸平行な矩形が三角形と重なっているか
	/// </summary>
	/// <param name="rect">軸平行な矩形</param>
	/// <param name="triangle">三角形</param>
	/// <returns></returns>
	static bool DoesAARectOverlapWithTriangle(const FRectAA& rect, const FTriangle& triangle);

	/// <summary>
	/// 線分と軸に平行な矩形の交点を取得
	/// </summary>
	/// <param name="out_intersection_positions">交点</param>
	/// <param name="segment">線分</param>
	/// <param name="rect">軸に平行な矩形</param>
	/// <returns>交点の数</returns>
	static int GetSegmentAARectIntersections(std::array<Vector2D, 2>& out_intersection_positions, const FSegment& segment, const FRectAA& rect);

	/// <summary>
	/// 線分と軸に平行な矩形の交点を取得. 交点の数のみ取得したい場合はこちら
	/// </summary>
	/// <param name="segment"></param>
	/// <param name="rect"></param>
	/// <returns></returns>
	static int GetSegmentAARectIntersections(const FSegment& segment, const FRectAA& rect);

	/// <summary>
	/// 凸多角形が点を内包するか
	/// </summary>
	/// <param name="query_point">点</param>
	/// <param name="vertexes">凸多角形の頂点. 隣接要素は隣接する頂点</param>
	/// <returns></returns>
	static bool DoesConvexPolygonContainsPoint(const Vector2D& query_point, const std::vector<Vector2D>& vertices);

	static bool DoesCircleOverlapWithAnother(const FCircle& circle_a, const FCircle& circle_b, Vector2D* penetration_depth = nullptr);

private:
	// 計算されためり込み深度に加算するオフセット.
    // (計算されためり込み深度) + (オフセット) > 0 なら重なっていると判定され,
	// < 0 なら重なっていないと判定される
	static const float PENETRATION_DEPTH_OFFSET;
};