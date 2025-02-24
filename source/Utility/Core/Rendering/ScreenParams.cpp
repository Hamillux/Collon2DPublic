#include "ScreenParams.h"
#include "SystemTypes.h"

namespace
{
	constexpr int HALF_SCREEN_WIDTH = UNIT_TILE_SIZE * 16;
	constexpr int HALF_SCREEN_HEIGHT = UNIT_TILE_SIZE * 9;

	constexpr int VIEWPORT_WIDTH = WINDOW_SIZE_X;
	constexpr int VIEWPORT_HEIGHT = WINDOW_SIZE_Y;
}

Matrix3x3 ScreenParams::GetMatrixWorldToScreen() const
{
	const Vector2D actual_screen_size = Vector2D{ HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT } *screen_scale;
	return Matrix3x3(
		1.f / actual_screen_size.x, 0.f, -world_offset.x / actual_screen_size.x,
		0.f, 1.f / actual_screen_size.y, -world_offset.y / actual_screen_size.y,
		0.f, 0.f, 1.f
	);
}

Matrix3x3 ScreenParams::GetMatrixScreenToViewport() const
{
	return Matrix3x3(
		VIEWPORT_WIDTH / 2.f, 0.f, VIEWPORT_WIDTH / 2.f,
		0.f, VIEWPORT_HEIGHT / 2.f, VIEWPORT_HEIGHT / 2.f,
		0.f, 0.f, 1.f
	);
}

Matrix3x3 ScreenParams::GetMatrixViewportToScreen() const
{
	return Matrix3x3(
		2.f / VIEWPORT_WIDTH, 0.f, -1.f,
		0.f, 2.f / VIEWPORT_HEIGHT, -1.f,
		0.f, 0.f, 1.f
	);
}

Matrix3x3 ScreenParams::GetMatrixScreenToWorld() const
{
	const Vector2D actual_screen_size = Vector2D{ HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT } *screen_scale;
	return Matrix3x3(
		actual_screen_size.x, 0.f, world_offset.x,
		0.f, actual_screen_size.y, world_offset.y,
		0.f, 0.f, 1.f
	);
}

Matrix3x3 ScreenParams::GetMatrixWorldToViewport() const
{
	return GetMatrixScreenToViewport() * GetMatrixWorldToScreen();
}

Matrix3x3 ScreenParams::GetMatrixViewportToWorld() const
{
	return GetMatrixScreenToWorld() * GetMatrixViewportToScreen();
}

Matrix3x3 ScreenParams::GetMatrixViewportToNormalizedDevice() const
{
	Matrix3x3 screen_to_normalized_device(
		2.f / VIEWPORT_WIDTH, 0.f, -1.f,
		0.f, -2.f / VIEWPORT_HEIGHT, 1.f,
		0.f, 0.f, 1.f
	);

	return screen_to_normalized_device;
}

Matrix3x3 ScreenParams::GetMatrixWorldToNormalizedDevice() const
{
	return GetMatrixViewportToNormalizedDevice() * GetMatrixWorldToViewport();
}

void ScreenParams::GetMatrixWorldToNormalizedDevice(DirectX::XMMATRIX& out_xmmat) const
{
	const Matrix3x3 mat = GetMatrixWorldToNormalizedDevice();
	out_xmmat = DirectX::XMMATRIX(
		mat._00, mat._01, mat._02, 0.0f,
		mat._10, mat._11, mat._12, 0.0f,
		mat._20, mat._21, mat._22, 0.0f,
		0.0f,    0.0f,    0.0f,    1.0f   
	);
}

void ScreenParams::ChangeScale(const float new_scale, const Vector2D& pivot_position_viewport)
{
	const float last_scale = screen_scale;
	const float delta_scale = new_scale - last_scale;
	screen_scale = new_scale;
	world_offset.x += delta_scale * HALF_SCREEN_WIDTH - 2.f * HALF_SCREEN_WIDTH * delta_scale * pivot_position_viewport.x / VIEWPORT_WIDTH;
	world_offset.y += delta_scale * HALF_SCREEN_HEIGHT - 2.f * HALF_SCREEN_HEIGHT * delta_scale * pivot_position_viewport.y / VIEWPORT_HEIGHT;
}

Vector2D ScreenParams::TransformPosition_WorldToScreen(const Vector2D& world_position) const
{
	return GetMatrixWorldToScreen().TransformVector(world_position);
}

Vector2D ScreenParams::TransformPosition_ScreenToViewport(const Vector2D& screen_position) const
{
	return GetMatrixScreenToViewport().TransformVector(screen_position);
}

Vector2D ScreenParams::TransformPosition_WorldToViewport(const Vector2D& world_position) const
{
	return GetMatrixWorldToViewport().TransformVector(world_position);
}

Vector2D ScreenParams::TransformPosition_ViewportToScreen(const Vector2D& viewport_position) const
{
	return GetMatrixViewportToScreen().TransformVector(viewport_position);
}

Vector2D ScreenParams::TransformPosition_ScreenToWorld(const Vector2D& screen_position) const
{
	return GetMatrixScreenToWorld().TransformVector(screen_position);
}

Vector2D ScreenParams::TransformPosition_ViewportToWorld(const Vector2D& viewport_position) const
{
	return GetMatrixViewportToWorld().TransformVector(viewport_position);
}

void ScreenParams::ChangeWorldOffset_World_Screen(const Vector2D world, const Vector2D screen)
{
	world_offset = world - GetScreenHalfExtentScaled() * screen;
}

Vector2D ScreenParams::GetScreenHalfExtentScaled() const
{
	return Vector2D{ HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT } * screen_scale;
}

Vector2D ScreenParams::GetScreenExtentUnscaled() const
{
	return Vector2D{ 2.f * HALF_SCREEN_WIDTH, 2.f * HALF_SCREEN_HEIGHT };
}

float ScreenParams::GetViewportPerScreen() const
{
	return static_cast<float>(VIEWPORT_WIDTH) / static_cast<float>(WINDOW_SIZE_X);
}

float ScreenParams::GetCameraZ() const
{
	// ÉJÉÅÉâÇÕz=1ÇÃÉèÅ[ÉãÉhïΩñ Çz<1ë§Ç©ÇÁå©ÇƒÇ¢ÇÈ
	return 1.f - screen_scale;
}

float ScreenParams::GetMaxScale(const Vector2D world_size) const
{
	const float max_scale_x = world_size.x / static_cast<float>(2 * HALF_SCREEN_WIDTH);
	const float max_scale_y = world_size.y / static_cast<float>(2 * HALF_SCREEN_HEIGHT);

	return std::min(max_scale_x, max_scale_y);
}
