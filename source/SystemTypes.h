#pragma once
#include <string>
#include <stdint.h>
#include "Utility/Core/Math/Vector2D.h"

#undef max
#undef min

namespace Collon2D
{
	constexpr const uint16_t WINDOW_SIZE_X = 1280;
	constexpr const uint16_t WINDOW_SIZE_Y = 720;

	inline Vector2D GetWindowSize() { return Vector2D(WINDOW_SIZE_X, WINDOW_SIZE_Y); }
	inline Vector2D GetWindowHalfSize() { return Vector2D(WINDOW_SIZE_X, WINDOW_SIZE_Y) * 0.5f; }
	const uint8_t SCREEN_COLOR_BIT_NUM = 32;

	// TransColor
	const uint8_t TRANS_COLOR_R = 0;
	const uint8_t TRANS_COLOR_G = 100;
	const uint8_t TRANS_COLOR_B = 0;

	// フレームレート
	constexpr const uint8_t FRAME_RATE = 60;

	namespace ResourcePaths
	{
		namespace Dir
		{
			extern const char* ROOT;
			extern const char* MASTER_DATA;
			extern const char* IMAGES;
			extern const char* SOUNDS;
			extern const char* STAGES;
			extern const char* STAGE_TEMPLATES;
			extern const char* PARAMS;
			extern const char* PARTICLES;
		}
	}

	// タイルの基本サイズ. 偶数
	const int UNIT_TILE_SIZE = 32;
	static_assert(UNIT_TILE_SIZE % 2 == 0, "UNIT_TILE_SIZE must be even number");

	// ステージ(小)の高さタイル数. 9の倍数
	constexpr int SHORT_STAGE_HEIGHT_TILES = 18;
	constexpr int TALL_STAGE_HEIGHT_TILES = SHORT_STAGE_HEIGHT_TILES * 2;
	static_assert(SHORT_STAGE_HEIGHT_TILES % 9 == 0, "SHORT_STAGE_HEIGHT_TILES must be a multiple of 9");

	constexpr int MIN_STAGE_LENGTH = SHORT_STAGE_HEIGHT_TILES * 16 / 9;
	constexpr int MAX_STAGE_LENGTH = MIN_STAGE_LENGTH * 10;
}

using namespace Collon2D;