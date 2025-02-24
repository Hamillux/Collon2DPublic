#pragma once
#include "Actor/Mapchip/Block/BlockBase.h"
#include "SlopeBlockInitialParams.h"
#include <array>

class TriangleCollider;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// スロープブロック. ルート位置は三角形の外接矩形の中心
/// </summary>
class SlopeBlock : public BlockBase
{
public:
	SlopeBlock()
		: _slope_collider(nullptr)
		, _width_per_height(1)
		, _scale(1)
	{}
	virtual ~SlopeBlock(){}

	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes) override;
	virtual void Finalize() override;
	virtual void GetOccupyingTiles(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const override;
	//~ End Actor interface

	//~ Begin BlockBase interface
private:
	/// <summary>
	/// BlockBase::Init()内部で呼び出される.
	/// </summary>
	/// <param name="out_vertices">頂点リスト</param>
	/// <param name="out_indices">インデックスリスト</param>
	virtual void CreateDrawVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices) override;
	virtual void GetDrawVerticesOffset(float& x_offset, float& y_offset) const override;
	//~ End BlockBase interface

private:
	// ブロックの外接矩形の各辺の座標を取得
	void GetBoundingRectLocal(float& left, float& top, float& right, float& bottom) const;

	// ブロックの外接矩形の半対角線の長さを取得
	Vector2D GetBoundingRectHalfDiagonalLocal() const;

	void GetBoundingRectSize(int& out_width, int& out_height) const;
	Vector2D GetBoundingRectSize() const;

	void GetSlopeVertexLocalPositions(std::array<Vector2D, 3>& out_vertex_positions) const;

	int _width_per_height;
	int32_t _scale;
	TriangleCollider* _slope_collider;
};

template<> struct initial_params_of_actor<SlopeBlock> { using type = SlopeBlockInitialParams; };