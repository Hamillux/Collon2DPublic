#pragma once
#include "SceneObject/Actor/Mapchip/Block/BlockBase.h"
#include "SlopeBlockInitialParams.h"
#include <array>

class TriangleCollider;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// 外接グリッドの原点(左上)の座標は
/// <para>SlopeBlock::GetPosition() - Vector2D(-1,-1)*UNIT_TILE_SIZE</para>
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
	virtual void GetOccupyingTileCounts(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const override;
	//~ End Actor interface

	//~ Begin BlockBase interface
public:
	/// <summary>
	/// BlockBase::Init()内部で呼び出される.
	/// </summary>
	/// <param name="out_vertices">頂点リスト</param>
	/// <param name="out_indices">インデックスリスト</param>
	virtual void CreateVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices) override;
private:
	virtual void GetVerticesOffset(float& x_offset, float& y_offset) const override;
	//~ End BlockBase interface

private:
	// ブロックの外接矩形の各辺の座標を取得
	void GetExternalRectLocal(float& left, float& top, float& right, float& bottom) const;
	void GetSlopeVertexLocalPositions(std::array<Vector2D, 3>& out_vertex_positions) const;

	enum class LineOrientation
	{
		OBLIQUE, HORIZONTAL, VERTICAL
	};


	int _width_per_height;
	int32_t _scale;

	// スロープを形成する線形コライダー
	TriangleCollider* _slope_collider;
};

template<> struct initial_params_of_actor<SlopeBlock> { using type = SlopeBlockInitialParams; };