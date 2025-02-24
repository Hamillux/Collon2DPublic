#pragma once

#include "ColliderBase.h"

/// <summary>
/// 三角形コライダー
/// </summary>
class TriangleCollider : public ColliderBase
{
public:
	TriangleCollider();
	virtual ~TriangleCollider();

	//~ Begin ColliderBase interface
public:
	virtual ColliderShape GetColliderShape() const override;
	virtual void GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const override;
	virtual void RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params) override;
	virtual void RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params) override;
	virtual void DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc = ColliderDebugDrawDesc{}) override;

	/// <summary>
	/// 三角形の重心を返す
	/// </summary>
	/// <returns></returns>
	virtual Vector2D GetCenterWorldPosition() const override;
	virtual void GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const override;
	//~ End ColliderBase interface

public:
	FTriangle GetTriangle() const;

	/// <summary>
	/// 三角形コライダーのパラメータをまとめてセット
	/// </summary>
	/// <param name="new_collision_type">コリジョンタイプ</param>
	/// <param name="new_collision_object_type">オブジェクトタイプ</param>
	/// <param name="new_hit_object_types">衝突対象のオブジェクトタイプリスト</param>
	/// <param name="new_pushability">押し戻しの可否</param>
	/// <param name="vertex_local_positions">このコライダーを原点とする空間での頂点座標</param>
	void SetTriangleColliderParams(
		const CollisionType new_collision_type,
		const CollisionObjectType new_collision_object_type,
		const std::vector<CollisionObjectType>& new_hit_object_types,
		const bool new_pushability,
		const std::array<Vector2D, 3>& vertex_local_positions
	);

private:
	// このコライダーを原点とする空間での頂点座標
	std::array<Vector2D, 3> _vertex_local_positions;
};