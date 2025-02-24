#include "CameraParams.h"
#include "SystemTypes.h"

namespace
{
    constexpr int HALF_SCREEN_WIDTH = UNIT_TILE_SIZE * 16;
    constexpr int HALF_SCREEN_HEIGHT = UNIT_TILE_SIZE * 9;

    constexpr int VIEWPORT_WIDTH = WINDOW_SIZE_X;
    constexpr int VIEWPORT_HEIGHT = WINDOW_SIZE_Y;

    static_assert(HALF_SCREEN_WIDTH* VIEWPORT_HEIGHT == HALF_SCREEN_HEIGHT * VIEWPORT_WIDTH,
        "Screen size and viewport size must be same aspect ratio");
}

// 変換行列のヘルパー関数
namespace
{
    Matrix3x3 GetViewMatrix(const Vector2D& view_position)
    {
        return Matrix3x3(
            1.f, 0.f, -view_position.x,
            0.f, 1.f, -view_position.y,
            0.f, 0.f, 1.f
        );
    }

    Matrix3x3 GetProjectionMatrix(const float distance_from_camera, const Vector2D& screen_half_size)
    {
        return Matrix3x3(
            1.f / (distance_from_camera * screen_half_size.x), 0.f, 0.f,
            0.f, 1.f / (distance_from_camera * screen_half_size.y), 0.f,
            0.f, 0.f, 1.f
        );
    }

    Matrix3x3 GetInverseProjectionMatrix(const float distance_from_camera, const Vector2D& screen_half_size)
    {
        return Matrix3x3(
            distance_from_camera * screen_half_size.x, 0.f, 0.f,
            0.f, distance_from_camera * screen_half_size.y, 0.f,
            0.f, 0.f, 1.f
        );
    }

    Matrix3x3 GetInverseViewMatrix(const Vector2D& offset)
    {
        return Matrix3x3(
            1.f, 0.f, offset.x,
            0.f, 1.f, offset.y,
            0.f, 0.f, 1.f
        );
    }

	Matrix3x3 GetViewportTransformMatrix(const float viewport_width, const float viewport_height)
	{
		return Matrix3x3(
			viewport_width / 2.f, 0.f, viewport_width/ 2.f,
			0.f, viewport_height/ 2.f, viewport_height / 2.f,
			0.f, 0.f, 1.f
		);
	}

	Matrix3x3 GetInverseViewportTransformMatrix(const float viewport_width, const float viewport_height)
	{
		return Matrix3x3(
			2.f / viewport_width, 0.f, -1.f,
			0.f, 2.f / viewport_height, -1.f,
			0.f, 0.f, 1.f
		);
	}

	void ConvertToXMMatrix(const Matrix3x3& mat, DirectX::XMMATRIX& out_xmmat)
	{
		out_xmmat = DirectX::XMMATRIX(
			mat._00, mat._01, mat._02, 0.0f,
			mat._10, mat._11, mat._12, 0.0f,
			mat._20, mat._21, mat._22, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
}

CameraParams::CameraParams()
    : screen_scale(1.f)
    , world_offset(0.f, 0.f)
{
}

Matrix3x3 CameraParams::GetMatrixWorldToScreen(const float world_z) const
{
    assert(world_z >= 1.f);

    const Vector2D screen_size = GetScreenHalfExtent();
    const float d = world_z - GetCameraZ();

    const Matrix3x3 view_matrix = GetViewMatrix(world_offset);
    const Matrix3x3 projection_matrix = GetProjectionMatrix(d, screen_size);

    return projection_matrix * view_matrix;
}

Matrix3x3 CameraParams::GetMatrixScreenToViewport() const
{
	return GetViewportTransformMatrix(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

Matrix3x3 CameraParams::GetMatrixViewportToScreen() const
{
	return GetInverseViewportTransformMatrix(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
}

Matrix3x3 CameraParams::GetMatrixScreenToWorld(const float world_z) const
{
    assert(world_z >= 1.f);

    const Vector2D screen_half_size = GetScreenHalfExtent();
    const float d = world_z - GetCameraZ();

	const Matrix3x3 projection_inv = GetInverseProjectionMatrix(d, screen_half_size);
	const Matrix3x3 view_inv = GetInverseViewMatrix(world_offset);

    return view_inv * projection_inv;
}

Matrix3x3 CameraParams::GetMatrixWorldToViewport(const float world_z) const
{
    return GetMatrixScreenToViewport() * GetMatrixWorldToScreen(world_z);
}

Matrix3x3 CameraParams::GetMatrixViewportToWorld(const float world_z) const
{
    return GetMatrixScreenToWorld(world_z) * GetMatrixViewportToScreen();
}

Matrix3x3 CameraParams::GetMatrixViewportToNormalizedDevice() const
{
    return GetMatrixScreenToNormalizedDevice() * GetMatrixViewportToScreen();
}

Matrix3x3 CameraParams::GetMatrixWorldToNormalizedDevice(const float world_z) const
{
    return GetMatrixViewportToNormalizedDevice() * GetMatrixWorldToViewport(world_z);
}

void CameraParams::GetMatrixWorldToNormalizedDevice(DirectX::XMMATRIX& out_xmmat, const float world_z) const
{
    ConvertToXMMatrix(GetMatrixWorldToNormalizedDevice(world_z), out_xmmat);
}

Matrix3x3 CameraParams::GetMatrixScreenToNormalizedDevice()
{
    return Matrix3x3(
        1.f, 0.f, 0.f,
        0.f, -1.f, 0.f,
        0.f, 0.f, 1.f
    );
}

void CameraParams::GetMatrixScreenToNormalizedDevice(DirectX::XMMATRIX& out_xmmat) const
{
    ConvertToXMMatrix(GetMatrixScreenToNormalizedDevice(), out_xmmat);
}

void CameraParams::ChangeScale(const float new_scale, const Vector2D& pivot_position_viewport)
{
    const Vector2D pivot_position_screen = TransformPosition_ViewportToScreen(pivot_position_viewport);
    const Vector2D pivot_position_world = TransformPosition_ScreenToWorld(pivot_position_screen);

    screen_scale = new_scale;

	ChangeWorldOffset_World_Screen(pivot_position_world, pivot_position_screen);
}

Vector2D CameraParams::TransformPosition_WorldToScreen(const Vector2D& world_position, const float world_z) const
{
    return GetMatrixWorldToScreen(world_z).TransformVector(world_position);
}

Vector2D CameraParams::TransformPosition_ScreenToViewport(const Vector2D& screen_position) const
{
    return GetMatrixScreenToViewport().TransformVector(screen_position);
}

Vector2D CameraParams::TransformPosition_WorldToViewport(const Vector2D& world_position, const float world_z) const
{
    return GetMatrixWorldToViewport(world_z).TransformVector(world_position);
}

Vector2D CameraParams::TransformPosition_ViewportToScreen(const Vector2D& viewport_position) const
{
    return GetMatrixViewportToScreen().TransformVector(viewport_position);
}

Vector2D CameraParams::TransformPosition_ScreenToWorld(const Vector2D& screen_position, const float world_z) const
{
    return GetMatrixScreenToWorld(world_z).TransformVector(screen_position);
}

Vector2D CameraParams::TransformPosition_ViewportToWorld(const Vector2D& viewport_position, const float world_z) const
{
    return GetMatrixViewportToWorld(world_z).TransformVector(viewport_position);
}

void CameraParams::ChangeWorldOffset_World_Screen(const Vector2D world, const Vector2D screen)
{
    world_offset = world - GetWorldViewHalfExtent() * screen;
}

Vector2D CameraParams::GetWorldViewHalfExtent() const
{
    return Vector2D{ HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT } * screen_scale;
}

Vector2D CameraParams::GetWorldViewExtent() const
{
    return Vector2D{ 2.f * HALF_SCREEN_WIDTH, 2.f * HALF_SCREEN_HEIGHT } * screen_scale;
}

Vector2D CameraParams::GetScreenHalfExtent() const
{
    return Vector2D{ HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT };
}

Vector2D CameraParams::GetScreenExtent() const
{
    return Vector2D{ 2.f * HALF_SCREEN_WIDTH, 2.f * HALF_SCREEN_HEIGHT };
}

float CameraParams::GetCameraZ() const
{
    return 1.f - screen_scale;
}

float CameraParams::GetMaxScale(const Vector2D world_size) const
{
    const float max_scale_x = world_size.x / static_cast<float>(2 * HALF_SCREEN_WIDTH);
    const float max_scale_y = world_size.y / static_cast<float>(2 * HALF_SCREEN_HEIGHT);

    return std::min(max_scale_x, max_scale_y);
}

void CameraParams::ClampWorldOffset(const Vector2D area_left_top, const Vector2D area_right_bottom, const float area_plane_z)
{
	const float d = area_plane_z - GetCameraZ();
	const Vector2D screen_half_size = GetScreenHalfExtent();

    const float cx_min = area_left_top.x + screen_half_size.x * d;
	const float cx_max = area_right_bottom.x - screen_half_size.x * d;
	const float cy_min = area_left_top.y + screen_half_size.y * d;
	const float cy_max = area_right_bottom.y - screen_half_size.y * d;

	world_offset.x = clamp(world_offset.x, cx_min, cx_max);
	world_offset.y = clamp(world_offset.y, cy_min, cy_max);
}
