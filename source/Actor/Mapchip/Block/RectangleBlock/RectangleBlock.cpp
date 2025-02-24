#include "RectangleBlock.h"
#include "Scene/SceneBase.h"
#include "Component/Collider/BoxCollider.h"

namespace
{
	enum class VerticalPosition : uint8_t
	{
		Top,
		Center,
		Bottom,
	};
	enum class HorizontalPosition : uint8_t
	{
		Left,
		Center,
		Right,
	};

	HorizontalPosition GetHorizontalPosition(const int x, const int width)
	{
		if (x == 0 && width >= 2)
		{
			return HorizontalPosition::Left;
		}
		else if (x == width - 1 && width >= 2)
		{
			return HorizontalPosition::Right;
		}
		else
		{
			return HorizontalPosition::Center;
		}
	}

	VerticalPosition GetVerticalPosition(const int y, const int height)
	{
		if (y == 0)
		{
			return VerticalPosition::Top;
		}
		else if (y == height - 1)
		{
			return VerticalPosition::Bottom;
		}
		else
		{
			return VerticalPosition::Center;
		}
	}

}

void RectangleBlock::Initialize(const ActorInitialParams* actor_params)
{
	typedef initial_params_of_actor_t<RectangleBlock> InitialParamsType;
	const InitialParamsType* rectangle_params = dynamic_cast<const InitialParamsType*>(actor_params);
	assert(rectangle_params);

	_tiles_x = rectangle_params->tile_count.x;
	_tiles_y = rectangle_params->tile_count.y;

	__super::Initialize(actor_params);

	collider = CreateComponent<BoxCollider>(this);
	collider->SetBoxColliderParams(
		CollisionType::BLOCK,
		CollisionObjectType::GROUND,
		{ CollisionObjectType::ENEMY, CollisionObjectType::PLAYER, CollisionObjectType::DAMAGE },
		false,
		Vector2D(_tiles_x, _tiles_y)* UNIT_TILE_SIZE
	);

	const float half_diagonal_length = Vector2D(UNIT_TILE_SIZE * _tiles_x, UNIT_TILE_SIZE * _tiles_y).Length() / 2.f;
	SetBoundingCircleRadius(half_diagonal_length);
}

void RectangleBlock::GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes)
{
	Vector2D left_top, right_bottom;
	collider->GetAABB(left_top, right_bottom);
	out_vertexes.push_back(left_top);
	out_vertexes.push_back(Vector2D(left_top.x, right_bottom.y));
	out_vertexes.push_back(right_bottom);
	out_vertexes.push_back(Vector2D(right_bottom.x, left_top.y));
}

void RectangleBlock::Finalize()
{

	__super::Finalize();
}

void RectangleBlock::GetOccupyingTiles(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const
{
	out_tile_x = _tiles_x;
	out_tile_y = _tiles_y;

	const float sign_horizontal_flip = IsHorizontalFlipEnabled() ? -1.f : 1.f;
	const float offset_x = (_tiles_x % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0) * sign_horizontal_flip;
	const float offset_y = _tiles_y % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0;
	out_snap_position_to_actor_position = Vector2D{offset_x, offset_y};
}


void RectangleBlock::CreateDrawVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices)
{
	// 隣接タイルの形状を取得する関数(上下)
	auto get_upper_lower_shape = [](const VerticalPosition v_pos, TileShape& out_upper_shape, TileShape& out_lower_shape)
		{
			switch (v_pos)
			{
			case VerticalPosition::Top:
				out_upper_shape = TileShape::None;
				out_lower_shape = TileShape::Rectangle;
				break;
			case VerticalPosition::Center:
				out_upper_shape = TileShape::Rectangle;
				out_lower_shape = TileShape::Rectangle;
				break;
			case VerticalPosition::Bottom:
				out_upper_shape = TileShape::Rectangle;
				out_lower_shape = TileShape::None;
				break;
			}
		};

	// 隣接タイルの形状を取得する関数(左右)
	auto get_left_right_shape = [](const HorizontalPosition h_pos, TileShape& out_left_shape, TileShape& out_right_shape)
		{
			switch (h_pos)
			{
			case HorizontalPosition::Left:
				out_left_shape = TileShape::None;
				out_right_shape = TileShape::Rectangle;
				break;
			case HorizontalPosition::Center:
				out_left_shape = TileShape::Rectangle;
				out_right_shape = TileShape::Rectangle;
				break;
			case HorizontalPosition::Right:
				out_left_shape = TileShape::Rectangle;
				out_right_shape = TileShape::None;
				break;
			}
		};

	// 各タイルに4つずつの頂点を作成
	out_vertices = std::vector<Vertex>();
	out_vertices.reserve(4 * _tiles_x * _tiles_y);
	out_indices = std::vector<UINT>();
	out_indices.reserve(6 * _tiles_x * _tiles_y);
	for (int y = 0; y < _tiles_y; ++y)
	{
		for (int x = 0; x < _tiles_x; ++x)
		{
			// 頂点リストに追加
			{
				const VerticalPosition v_pos = GetVerticalPosition(y, _tiles_y);
				const HorizontalPosition h_pos = GetHorizontalPosition(x, _tiles_x);
				const int tile_x = x * UNIT_TILE_SIZE;
				const int tile_y = y * UNIT_TILE_SIZE;

				TileShape upper_shape, lower_shape, left_shape, right_shape;
				get_upper_lower_shape(v_pos, upper_shape, lower_shape);
				get_left_right_shape(h_pos, left_shape, right_shape);

				const TileType tile_type = BlockTextureMapping::GetTileType(TileShape::Rectangle, upper_shape, left_shape, lower_shape, right_shape);
				if (tile_type == TileType::None)
				{
					continue;
				}

				// タイルの描画矩形の頂点座標を計算
				int tiles_to_left, tiles_to_top, tiles_to_right, tiles_to_bottom;
				BlockTextureMapping::GetVertexPositionOffsetsFromTile(tiles_to_left, tiles_to_top, tiles_to_right, tiles_to_bottom, tile_type);

				int left, top, right, bottom;
				left = tile_x + tiles_to_left * UNIT_TILE_SIZE;
				top = tile_y + tiles_to_top * UNIT_TILE_SIZE;
				right = tile_x + tiles_to_right * UNIT_TILE_SIZE;
				bottom = tile_y + tiles_to_bottom * UNIT_TILE_SIZE;

				// タイルの描画矩形のテクスチャ座標を計算
				float u0, v0, u1, v1;
				BlockTextureMapping::GetTextureRegion(u0, v0, u1, v1, tile_type);

				// 頂点を左上から反時計回りに追加
				{
					out_vertices.push_back(Vertex(left, top, u0, v0));
					out_vertices.push_back(Vertex(right, top, u1, v0));
					out_vertices.push_back(Vertex(right, bottom, u1, v1));
					out_vertices.push_back(Vertex(left, bottom, u0, v1));
				}
			}

			// インデックスリストに追加
			{
				const int cell_idx = x + y * _tiles_x;

				// 頂点インデックス
				const int vidx_left_top = 4 * cell_idx;
				const int vidx_left_bottom = vidx_left_top + 1;
				const int vidx_right_bottom = vidx_left_top + 2;
				const int vidx_right_top = vidx_left_top + 3;

				// out_indicesに追加
				{
					// 左上が直角の三角形
					out_indices.push_back(vidx_left_top);
					out_indices.push_back(vidx_left_bottom);
					out_indices.push_back(vidx_right_top);

					// 右下が直角の三角形
					out_indices.push_back(vidx_left_bottom);
					out_indices.push_back(vidx_right_bottom);
					out_indices.push_back(vidx_right_top);
				}
			}
		}
	}
}

void RectangleBlock::GetDrawVerticesOffset(float& x_offset, float& y_offset) const
{
	x_offset = -_tiles_x * UNIT_TILE_SIZE / 2.f;
	y_offset = -_tiles_y * UNIT_TILE_SIZE / 2.f;
}
