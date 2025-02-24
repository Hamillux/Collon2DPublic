#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
#endif

struct SpriteTextureInfo
{
#ifdef __cplusplus
	typedef unsigned int uint;
#endif
	uint sprite_rows;
	uint sprite_columns;

#ifdef __cplusplus
	SpriteTextureInfo(const unsigned int in_sprite_rows, const unsigned int in_sprite_columns);
	SpriteTextureInfo();
#endif
};