#include "MagicProjectile.h"

MagicProjectile::MagicProjectile()
{
}

void MagicProjectile::Draw(const ScreenParams& screen_params)
{
	__super::Draw(screen_params);

	Vector2D screen_position = Vector2D::WorldToViewport(GetActorWorldPosition(), screen_params);

	DxLib::DrawCircleAA(screen_position.x, screen_position.y, 8.f, 32, 0xFF0000, 1);
}
