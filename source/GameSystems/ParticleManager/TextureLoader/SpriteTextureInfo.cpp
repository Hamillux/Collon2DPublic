#include "SpriteTextureInfo.h"

SpriteTextureInfo::SpriteTextureInfo(const unsigned int in_sprite_rows, const unsigned int in_sprite_columns)
	: sprite_rows(in_sprite_rows)
	, sprite_columns(in_sprite_columns)
{
}

SpriteTextureInfo::SpriteTextureInfo()
	: sprite_rows(1)
	, sprite_columns(1)
{
}
