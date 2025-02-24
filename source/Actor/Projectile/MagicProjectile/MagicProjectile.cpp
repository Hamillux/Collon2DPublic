#include "MagicProjectile.h"

MagicProjectile::MagicProjectile()
{
}

void MagicProjectile::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);

	Vector2D screen_position = Vector2D::WorldToViewport(GetActorWorldPosition(), camera_params);

	DxLib::DrawCircleAA(screen_position.x, screen_position.y, 8.f, 32, 0xFF0000, 1);
}
