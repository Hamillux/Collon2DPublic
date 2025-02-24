#include "SlopeBlock.h"
#include "Component/Collider/TriangleCollider.h"
#include "Scene/SceneBase.h"

void SlopeBlock::Initialize(const ActorInitialParams* actor_params)
{
	typedef initial_params_of_actor_t<SlopeBlock> InitialParamsType;
	const InitialParamsType* slope_params = dynamic_cast<const InitialParamsType*>(actor_params);

	_scale = slope_params->scale;
	_width_per_height = slope_params->width_per_height;

	__super::Initialize(actor_params);

	// コライダーの初期化
	const CollisionType collision_type = CollisionType::BLOCK;
	const CollisionObjectType collision_object_type = CollisionObjectType::GROUND;
	const std::vector<CollisionObjectType> hit_targets =
	{
		CollisionObjectType::ENEMY, CollisionObjectType::PLAYER, CollisionObjectType::DAMAGE
	};
	const bool pushability = false;
	std::array<Vector2D, 3> vertex_local_positions;
	GetSlopeVertexLocalPositions(vertex_local_positions);

	_slope_collider = CreateComponent<TriangleCollider>(this);
	_slope_collider->SetLocalPosition(Vector2D{ 0, 0 });
	_slope_collider->SetTriangleColliderParams(
		collision_type,
		collision_object_type,
		hit_targets,
		pushability,
		vertex_local_positions
	);

	SetDrawAreaCheckIgnored(true);
}

void SlopeBlock::GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes)
{
	std::array<Vector2D, 3> vertex_local_positions;
	GetSlopeVertexLocalPositions(vertex_local_positions);
	out_vertexes = std::vector<Vector2D>(3);
	for (size_t i = 0; i < 3; i++)
	{
		out_vertexes.at(i) = GetActorWorldTransform().TransformLocation(vertex_local_positions.at(i));
	}
}

void SlopeBlock::Finalize()
{
	__super::Finalize();
}

void SlopeBlock::GetOccupyingTiles(int& out_tiles_x, int& out_tiles_y, Vector2D& out_snap_position_to_actor_position) const
{
	const int tiles_x = _width_per_height * _scale;
	const int tiles_y = _scale;

	out_tiles_x = tiles_x;
	out_tiles_y = tiles_y;

	const float sign_horizontal_flip = IsHorizontalFlipEnabled() ? -1.f : 1.f;
	const float offset_x = (tiles_x % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0) * sign_horizontal_flip;
	const float offset_y = tiles_y % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0;
	out_snap_position_to_actor_position = Vector2D{offset_x, offset_y};
}

void SlopeBlock::CreateDrawVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices)
{
	const int grid_tiles_x = _width_per_height * _scale;
	const int grid_tiles_y = _scale;

	/// <summary>
	/// スロープタイルの種類を取得する.
	/// NOTE: x, yの位置のタイルがSlopeであることを前提とする.
	/// </summary>
	auto get_slope_tile_shape = [](const int width_per_height, const int x)
		{
			if (width_per_height == 1)
			{
				return TileShape::Slope_100;
			}
			else if (width_per_height == 2)
			{
				switch (x % width_per_height)
				{
				case 0:
					return TileShape::Slope_50_1;
				case 1:
					return TileShape::Slope_50_2;
				default:
					// ここには来ないはず.
					throw std::runtime_error("Unknown error");
				}
			}
			else
			{
				throw std::runtime_error("Invalid width_per_height.");
			}
		};

	// 各グリッド位置のタイルの形状を取得
	auto get_tile_shape = [this, get_slope_tile_shape](const int x, const int y)
		{
			if (x < 0 || y < 0 || x >= _scale * _width_per_height || y >= _scale)
			{
				return TileShape::None;
			}

			const bool is_slope = (x / _width_per_height + y) == _scale - 1;
			if (is_slope)
			{
				return get_slope_tile_shape(_width_per_height, x);
			}

			const bool is_rect = (x / _width_per_height + y) > _scale - 1;
			if (is_rect)
			{
				return TileShape::Rectangle;
			}

			return TileShape::None;
		};

	for (int y = 0; y < grid_tiles_y; y++)
	{
		for (int x = 0; x < grid_tiles_x; x++)
		{
			const float tile_pos_x = x * UNIT_TILE_SIZE;
			const float tile_pos_y = y * UNIT_TILE_SIZE;

			const TileShape shape = get_tile_shape(x, y);
			const TileShape upper_shape = get_tile_shape(x, y - 1);
			const TileShape left_shape = get_tile_shape(x - 1, y);
			const TileShape lower_shape = get_tile_shape(x, y + 1);
			const TileShape right_shape = get_tile_shape(x + 1, y);

			const TileType tile_type = BlockTextureMapping::GetTileType(shape, upper_shape, left_shape, lower_shape, right_shape);
			if (tile_type == TileType::None)
			{
				continue;
			}

			// 描画矩形の頂点座標
			int tiles_to_left, tiles_to_top, tiles_to_right, tiles_to_bottom;
			BlockTextureMapping::GetVertexPositionOffsetsFromTile(tiles_to_left, tiles_to_top, tiles_to_right, tiles_to_bottom, tile_type);

			// 描画矩形のテクスチャ座標
			float u0, v0, u1, v1;
			BlockTextureMapping::GetTextureRegion(u0, v0, u1, v1, tile_type);

			// 頂点リストに追加
			{
				float left, top, right, bottom;
				left = tile_pos_x + tiles_to_left * UNIT_TILE_SIZE;
				top = tile_pos_y + tiles_to_top * UNIT_TILE_SIZE;
				right = tile_pos_x + tiles_to_right * UNIT_TILE_SIZE;
				bottom = tile_pos_y + tiles_to_bottom * UNIT_TILE_SIZE;

				out_vertices.push_back(Vertex(left, top, u0, v0));
				out_vertices.push_back(Vertex(left, bottom, u0, v1));
				out_vertices.push_back(Vertex(right, bottom, u1, v1));
				out_vertices.push_back(Vertex(right, top, u1, v0));
			}

			// インデックスリストに追加
			{
				const int vidx_left_top = out_vertices.size() - 4;
				const int vidx_left_bottom = vidx_left_top + 1;
				const int vidx_right_bottom = vidx_left_top + 2;
				const int vidx_right_top = vidx_left_top + 3;

				out_indices.push_back(vidx_left_top);
				out_indices.push_back(vidx_left_bottom);
				out_indices.push_back(vidx_right_top);

				out_indices.push_back(vidx_left_bottom);
				out_indices.push_back(vidx_right_bottom);
				out_indices.push_back(vidx_right_top);
			}
		}
	}
}

void SlopeBlock::GetDrawVerticesOffset(float& x_offset, float& y_offset) const
{
	const Vector2D bounding_rect_half_diagonal = GetBoundingRectHalfDiagonalLocal();
	x_offset = bounding_rect_half_diagonal.x * -1.f;
	y_offset = bounding_rect_half_diagonal.y * -1.f;
}

void SlopeBlock::GetBoundingRectLocal(float& left, float& top, float& right, float& bottom) const
{
	// NOTE: スロープのルート位置は外接矩形の中心に一致する

	const Vector2D bounding_rect_half_diagonal = GetBoundingRectHalfDiagonalLocal();
	const Vector2D bounding_rect_left_top_local = Vector2D(-bounding_rect_half_diagonal.x, -bounding_rect_half_diagonal.y);
	const Vector2D bounding_rect_size = GetBoundingRectSize();

	left = bounding_rect_left_top_local.x;
	top = bounding_rect_left_top_local.y;
	right = bounding_rect_left_top_local.x + bounding_rect_size.x;
	bottom = bounding_rect_left_top_local.y + bounding_rect_size.y;
}

Vector2D SlopeBlock::GetBoundingRectHalfDiagonalLocal() const
{
	return Vector2D(
		static_cast<float>(UNIT_TILE_SIZE * _scale * _width_per_height) * 0.5f,
		static_cast<float>(UNIT_TILE_SIZE * _scale) * 0.5f
	);
}

void SlopeBlock::GetBoundingRectSize(int& out_width, int& out_height) const
{
	out_width = _width_per_height * _scale * UNIT_TILE_SIZE;
	out_height = _scale * UNIT_TILE_SIZE;
}

Vector2D SlopeBlock::GetBoundingRectSize() const
{
	int width, height;
	GetBoundingRectSize(width, height);
	return Vector2D(width, height);
}

void SlopeBlock::GetSlopeVertexLocalPositions(std::array<Vector2D, 3>& out_vertex_positions) const
{
	float left, top, right, bottom;
	GetBoundingRectLocal(left, top, right, bottom);

	out_vertex_positions.at(0) = Vector2D(left, bottom);
	out_vertex_positions.at(1) = Vector2D(right, bottom);

	// 右上がりをフリップ無しとして扱う
	if (IsHorizontalFlipEnabled())
	{
		out_vertex_positions.at(2) = Vector2D(left, top);
	}
	else
	{
		out_vertex_positions.at(2) = Vector2D(right, top);
	}
}
