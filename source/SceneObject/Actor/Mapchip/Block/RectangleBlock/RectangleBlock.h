#pragma once

#include "SceneObject/Actor/Mapchip/Block/BlockBase.h"
#include "RectangleBlockInitialParams.h"
#include <array>

class BoxCollider;

CLN2D_GEN_DEFINE_ACTOR()
class RectangleBlock : public BlockBase
{
public:
	RectangleBlock()
		: collider(nullptr)
	{}
	virtual ~RectangleBlock() {}

	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes) override;
	virtual void Finalize() override;
	virtual void GetOccupyingTileCounts(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const override;
	//~ End Actor interface
	

	//~ Begin BlockBase interface
public:
	virtual void CreateVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices) override;
private:
	virtual void GetVerticesOffset(float& x_offset, float& y_offset) const override;
	//~ End BlockBase interface

private:
	int _tiles_x;
	int _tiles_y;
	BoxCollider* collider;
};

template<> struct initial_params_of_actor<RectangleBlock> { using type = RectangleBlockInitialParams; };