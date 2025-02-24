#pragma once

#include <stdint.h>
#include <vector>
#include <array>
#include "Component/Collider/ColliderBase.h"

enum class BoxColliderVertex : uint8_t;

/// <summary>
/// 矩形コライダー
/// </summary>
class BoxCollider : public ColliderBase
{
public:
	BoxCollider();
	virtual ~BoxCollider() {};

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
	virtual void DrawDebugLines(const CameraParams& camera_params, const ColliderDebugDrawDesc& desc = ColliderDebugDrawDesc{}) override;
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
	/// <param name="new_box_extent">矩形サイズ</param>
	void SetBoxColliderParams(
		const CollisionType new_collision_type,
		const CollisionObjectType new_collision_object_type,
		const std::vector<CollisionObjectType>& new_hit_object_types,
		const bool new_pushability,
		const Vector2D& new_box_extent
	);
	void SetBoxColliderExtent(const Vector2D& new_box_extent);

	/**
	* 中心から任意頂点へのベクトルを計算
	*/
	Vector2D CalcCenterToVertex(BoxColliderVertex vertex) const;

	Vector2D GetBoxExtent() const { return unrotaed_diagonal; }

	/// <summary>
	/// rotationがゼロであるときのコライダーの形状を軸平行な矩形として取得
	/// </summary>
	/// <returns></returns>
	FRectAA GetGeometricRectAA() const;

	FRect GetGeometricRect() const;

private:
	/**
	* 中心から各頂点へのベクトルを反時計回りに計算
	* 先頭要素は回転ゼロの状態で左上にあった頂点へのベクトル
	*/
	void CalcCenterToAllVertices(std::array<Vector2D,4>& out_center_to_vertexes) const;

	// rotationがゼロのときの対角線を表すベクトル
	// 向きは左上から右下
	Vector2D unrotaed_diagonal;

	// rotationがゼロのときの
	// 中心から各頂点へのベクトル
	// 順番は左上から反時計回り
	std::array<Vector2D, 4> center_to_vertices;

};

enum class BoxColliderVertex : uint8_t
{
	LEFT_TOP = 0,
	LEFT_BOTTOM = 1,
	RIGHT_BOTTOM = 2,
	RIGHT_TOP = 3
};