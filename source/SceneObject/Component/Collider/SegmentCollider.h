#pragma once

#include <vector>
#include "SceneObject/Component/Collider/ColliderBase.h"

enum class SegmentEnd
{
	LEFT,RIGHT
};

class SegmentCollider : public ColliderBase
{
public:
	SegmentCollider() {}
	virtual ~SegmentCollider() {};

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Finalize() override;
	//~ End ComponentBase interface

	//~ Begin ColliderBase interface
public:
	virtual ColliderShape GetColliderShape() const override;
	virtual void GetVertexPositions(std::vector<Vector2D>& out_vertex_positions) const override;
	virtual void RespondToSingleLineTrace(QueryResult_SingleLineTrace& query_result, const CollisionQueryParams_SingleLineTrace& query_params) override;
	virtual void RespondToMultiAARectTrace(QueryResult_MultiAARectTrace& query_result, const CollisionQueryParams_RectAA& query_params) override;
	virtual void DrawDebugLines(const ScreenParams& screen_params, const ColliderDebugDrawDesc& desc = ColliderDebugDrawDesc{}) override;
	virtual Vector2D GetCenterWorldPosition() const override;
	virtual void GetAABB(Vector2D& out_left_top, Vector2D& out_right_bottom) const override;
	//~ End ColliderBase interface

public:
	/// <summary>
	/// 矩形コライダーのパラメータをまとめてセット
	/// </summary>
	/// <param name="new_collision_type">Block/Overlap</param>
	/// <param name="new_collision_object_type">衝突グループ</param>
	/// <param name="new_hit_object_types">衝突対象</param>
	/// <param name="new_pushability">押し戻し可能か</param>
	/// <param name="new_offset_to_center">親アクターのActor::positionからコライダー中心へのオフセット</param>
	/// <param name="new_length">線分の長さ</param>
	void SetSegmentColliderParams(
		const CollisionType new_collision_type,
		const CollisionObjectType new_collision_object_type,
		const std::vector<CollisionObjectType>& new_hit_object_types,
		const bool new_pushability,
		const float new_length
	);
	
	// 中心から端点へのベクトル
	Vector2D CalcSegmentCenterToEnd(const SegmentEnd segment_end) const;

	// 端点のワールド座標を計算
	Vector2D CalcSegmentEndPosition(const SegmentEnd segment_end) const;

	/// <summary>
	/// 端点のローカル座標を取得
	/// </summary>
	/// <param name="new_length"></param>
	Vector2D GetSegmentEndLocalPosition(const SegmentEnd segment_end) const;

	void SetLength(const float new_length);

	void DrawLine(const ScreenParams& screen_params, const int color, const int thickness);

private:
	// 線分の長さ
	float _length;

};