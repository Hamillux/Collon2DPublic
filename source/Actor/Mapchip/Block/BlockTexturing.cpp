#include "BlockTexturing.h"
#include <stdexcept>

using TT = TileType;

constexpr double div16(const int x)
{
	return x / 16.f;
}

const std::unordered_map<TileType, BlockTextureMapping::TextureRegion> BlockTextureMapping::_tile_texture_region_map =
{
	{TT::M2,	{div16(0),	div16(0),	div16(4),	div16(3)}},
	{TT::J2,	{div16(5),	div16(0),	div16(8),	div16(3)}},
	{TT::M1,	{div16(9),	div16(0),	div16(12),	div16(3)}},
	{TT::J1,	{div16(14),	div16(0),	div16(16),	div16(3)}},
	{TT::I2,	{div16(0),	div16(4),	div16(2),	div16(6)}},
	{TT::L1,	{div16(3),	div16(4),	div16(5),	div16(6)}},
	{TT::K2,	{div16(6),	div16(4),	div16(8),	div16(6)}},
	{TT::A,		{div16(9),	div16(4),	div16(11),	div16(6)}},
	{TT::B,		{div16(12),	div16(4),	div16(13),	div16(6)}},
	{TT::C,		{div16(14),	div16(4),	div16(16),	div16(6)}},
	{TT::L2,	{div16(0),	div16(7),	div16(3),	div16(9)}},
	{TT::K1,	{div16(4),	div16(7),	div16(5),	div16(9)}},
	{TT::H1_1,	{div16(6),	div16(7),	div16(7),	div16(9)}},
	{TT::D,		{div16(9),	div16(7),	div16(11),	div16(8)}},
	{TT::E,		{div16(12),	div16(7),	div16(13),	div16(8)}},
	{TT::F,		{div16(14),	div16(7),	div16(16),	div16(8)}},
	{TT::F1,	{div16(0),	div16(10),	div16(2),	div16(11)}},
	{TT::H1_2,	{div16(3),	div16(10),	div16(4),	div16(12)}},
	{TT::H2_2,	{div16(5),	div16(10),	div16(6),	div16(12)}},
	{TT::E1_1,	{div16(7),	div16(10),	div16(8),	div16(11)}},
	{TT::G,		{div16(9),	div16(9),	div16(11),	div16(11)}},
	{TT::H,		{div16(12),	div16(9),	div16(13),	div16(11)}},
	{TT::I,		{div16(14),	div16(9),	div16(16),	div16(11)}},
	{TT::F2,	{div16(0),	div16(12),	div16(2),	div16(13)}},
	{TT::E1_2,	{div16(3),	div16(13),	div16(4),	div16(14)}},
	{TT::E2_2,	{div16(5),	div16(13),	div16(6),	div16(14)}},
	{TT::I1,	{div16(7),	div16(12),	div16(9),	div16(14)}},
};



void BlockTextureMapping::GetTextureRegion(
	float& out_u0,
	float& out_v0,
	float& out_u1,
	float& out_v1,
	const TileShape target_shape,
	const TileShape upper_shape,
	const TileShape left_shape,
	const TileShape lower_shape,
	const TileShape right_shape
)
{
	const TileType tile_type = GetTileType(target_shape, upper_shape, left_shape, lower_shape, right_shape);
	GetTextureRegion(out_u0, out_v0, out_u1, out_v1, tile_type);
}

void BlockTextureMapping::GetTextureRegion(float& out_u0, float& out_v0, float& out_u1, float& out_v1, const TileType tile_type)
{
	const TextureRegion& region = _tile_texture_region_map.at(tile_type);
	out_u0 = region.u0;
	out_v0 = region.v0;
	out_u1 = region.u1;
	out_v1 = region.v1;
}

void BlockTextureMapping::GetVertexPositionOffsetsFromTile(int& out_tiles_to_left, int& out_tiles_to_top, int& out_tiles_to_right, int& out_tiles_to_bottom, const TileShape target_shape, const TileShape upper_shape, const TileShape left_shape, const TileShape lower_shape, const TileShape right_shape)
{
	const TileType tile_type = GetTileType(target_shape, upper_shape, left_shape, lower_shape, right_shape);
	GetVertexPositionOffsetsFromTile(out_tiles_to_left, out_tiles_to_top, out_tiles_to_right, out_tiles_to_bottom, tile_type);
}

void BlockTextureMapping::GetVertexPositionOffsetsFromTile(int& out_tiles_to_left, int& out_tiles_to_top, int& out_tiles_to_right, int& out_tiles_to_bottom, const TileType tile_type)
{
	using T = TileType;

	if (tile_type == T::None)
	{
		throw std::invalid_argument("tile_type is None");
	}

	// left
	switch (tile_type)
	{
	case T::A:
	case T::D:
	case T::G:
	case T::M1:
	case T::J1:
	case T::K2:
	case T::L2:
		out_tiles_to_left = -1;
		break;

	case T::M2:
	case T::J2:
		out_tiles_to_left = -2;
		break;

	default:
		out_tiles_to_left = 0;
		break;
	}

	// top
	switch (tile_type)
	{
	case T::A:
	case T::B:
	case T::C:
	case T::M1:
	case T::J1:
	case T::L1:
	case T::K1:
	case T::M2:
	case T::J2:
	case T::L2:
	case T::K2:
		out_tiles_to_top = -1;
		break;
	default:
		out_tiles_to_top = 0;
		break;
	}

	// right
	switch (tile_type)
	{
	case T::C:
	case T::F:
	case T::I:
	case T::M1:
	case T::F1:
	case T::L1:
	case T::I1:
	case T::M2:
	case T::F2:
	case T::L2:
	case T::I2:
		out_tiles_to_right = 2;
		break;

	default:
		out_tiles_to_right = 1;
		break;
	}

	// bottom
	switch (tile_type)
	{
	case T::G:
	case T::H:
	case T::I:
	case T::M1:
	case T::H1_1:
	case T::I1:
	case T::M2:
	case T::I2:
	case T::H1_2:
	case T::H2_2:
	case T::J1:
	case T::J2:
		out_tiles_to_bottom = 2;
		break;
	default:
		out_tiles_to_bottom = 1;
		break;
	}
}

constexpr TileType BlockTextureMapping::GetTileType(const TileShape target_shape, const TileShape upper_shape, const TileShape left_shape, const TileShape lower_shape, const TileShape right_shape)
{
	using TS = TileShape;
	constexpr int DONT_CARE_FLAGS_UPPER = 1 << 0;
	constexpr int DONT_CARE_FLAGS_LEFT = 1 << 1;
	constexpr int DONT_CARE_FLAGS_UPPER_LEFT = DONT_CARE_FLAGS_UPPER | DONT_CARE_FLAGS_LEFT;
	constexpr int DONT_CARE_FLAGS_LOWER = 1 << 2;
	constexpr int DONT_CARE_FLAGS_RIGHT = 1 << 3;
	struct TileShapes
	{
		constexpr TileShapes(const TileShape in_target_shape, const TileShape in_upper_shape, const TileShape in_left_shape, const TileShape in_lower_shape, const TileShape in_right_shape, const int in_dont_care_flags = 0)
			: target_shape(in_target_shape)
			, upper_shape(in_upper_shape)
			, left_shape(in_left_shape)
			, lower_shape(in_lower_shape)
			, right_shape(in_right_shape)
			, dont_care_flags(in_dont_care_flags)
		{
		}

		constexpr bool operator==(const TileShapes& other) const
		{
			const int flags = dont_care_flags | other.dont_care_flags;
			return
				other.target_shape == target_shape &&
				(other.upper_shape == upper_shape || flags & DONT_CARE_FLAGS_UPPER) &&
				(other.left_shape == left_shape || flags & DONT_CARE_FLAGS_LEFT) &&
				(other.lower_shape == lower_shape || flags & DONT_CARE_FLAGS_LOWER) &&
				(other.right_shape == right_shape || flags & DONT_CARE_FLAGS_RIGHT);
		}

		TileShape target_shape;
		TileShape upper_shape;
		TileShape left_shape;
		TileShape lower_shape;
		TileShape right_shape;
		int dont_care_flags;
	};

	const TileShapes shapes = { target_shape, upper_shape, left_shape, lower_shape, right_shape };

	// マッピング情報
	// none: TileShape::None
	// rect: TileShape::Rectangle
	// s100: TileShape::Slope_100
	// s50_1 : TileShape::Slope_50_1
	// s50_2 : TileShape::Slope_50_2
	// X : dont care
	// 
	// Type:	Target,	Upper,	Left,	Lower,	Right
	// A:       rect, none, none, X, rect
	// B:		rect, none, rect, X, rect
	// C:		rect, none, rect, X, none
	// D:		rect, rect, none, rect, rect
	// E:		rect, rect, rect, rect, rect
	// F:		rect, rect, rect, rect, none
	// G:		rect, rect, none, none, rect
	// H:		rect, rect, rect, none, rect
	// I:		rect, rect, rect, none, none
	// M1:		s100, X, X, none, none
	// J1:		s100, X, X, none, rect
	// K1:		s100, X, X, rect, rect
	// L1:      s100, X, X, rect, none
	// E1_1:    rect, s100, X, rect, rect
	// F1:      rect, s100, X, rect, none
	// H1_1:    rect, s100, X, none, rect
	// I1:      rect, s100, X, none, none
	// M2:      s50_2, X, X, none, none
	// J2:      s50_2, X, X, none, rect
	// K2:      s50_2, X, X, rect, rect
	// L2:      s50_2, X, X, rect, none
	// E1_2:    rect, s50_1, X, rect, rect
	// E2_2:    rect, s50_2, X, rect, rect
	// F2:      rect, s50_2, X, rect, none
	// H1_2:    rect, s50_1, X, none, rect
	// H2_2:    rect, s50_2, X, none, rect
	// I2:      rect, s50_2, X, none, none

	if (shapes == TileShapes{ TS::Rectangle, TS::None, TS::None, TS::Rectangle, TS::Rectangle , DONT_CARE_FLAGS_LOWER }) { return TileType::A; }
	if (shapes == TileShapes{ TS::Rectangle, TS::None, TS::Rectangle, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_LOWER }) { return TileType::B; }
	if (shapes == TileShapes{ TS::Rectangle, TS::None, TS::Rectangle, TS::Rectangle, TS::None, DONT_CARE_FLAGS_LOWER }) { return TileType::C; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::None, TS::Rectangle, TS::Rectangle }) { return TileType::D; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::Rectangle }) { return TileType::E; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::None }) { return TileType::F; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::None, TS::None, TS::Rectangle }) { return TileType::G; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::None, TS::Rectangle }) { return TileType::H; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Rectangle, TS::Rectangle, TS::None, TS::None }) { return TileType::I; }
	if (shapes == TileShapes{ TS::Slope_100, TS::None, TS::None, TS::None, TS::None, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::M1; }
	if (shapes == TileShapes{ TS::Slope_100, TS::None, TS::None, TS::None, TS::Rectangle, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::J1; }
	if (shapes == TileShapes{ TS::Slope_100, TS::None, TS::None, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::K1; }
	if (shapes == TileShapes{ TS::Slope_100, TS::None, TS::None, TS::Rectangle, TS::None, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::L1; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_100, TS::None, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::E1_1; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_100, TS::None, TS::Rectangle, TS::None, DONT_CARE_FLAGS_LEFT }) { return TileType::F1; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_100, TS::None, TS::None, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::H1_1; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_100, TS::None, TS::None, TS::None, DONT_CARE_FLAGS_LEFT }) { return TileType::I1; }
	if (shapes == TileShapes{ TS::Slope_50_2, TS::None, TS::None, TS::None, TS::None, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::M2; }
	if (shapes == TileShapes{ TS::Slope_50_2, TS::None, TS::None, TS::None, TS::Rectangle, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::J2; }
	if (shapes == TileShapes{ TS::Slope_50_2, TS::None, TS::None, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::K2; }
	if (shapes == TileShapes{ TS::Slope_50_2, TS::None, TS::None, TS::Rectangle, TS::None, DONT_CARE_FLAGS_UPPER_LEFT }) { return TileType::L2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_1, TS::None, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::E1_2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_2, TS::None, TS::Rectangle, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::E2_2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_2, TS::None, TS::Rectangle, TS::None, DONT_CARE_FLAGS_LEFT }) { return TileType::F2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_1, TS::None, TS::None, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::H1_2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_2, TS::None, TS::None, TS::Rectangle, DONT_CARE_FLAGS_LEFT }) { return TileType::H2_2; }
	if (shapes == TileShapes{ TS::Rectangle, TS::Slope_50_2, TS::None, TS::None, TS::None, DONT_CARE_FLAGS_LEFT }) { return TileType::I2; }

	return TileType::None;
}
