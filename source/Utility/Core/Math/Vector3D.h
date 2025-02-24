#pragma once

#include "Vector2D.h"

struct Vector3D
{
	constexpr Vector3D()
		: x(0.0f), y(0.0f), z(0.0f)
	{
	}

	constexpr Vector3D(const float x_in, const float y_in, const float z_in)
		: x(x_in), y(y_in), z(z_in)
	{
	}

	static Vector3D MakeFromXY(const Vector2D& xy, const float z = 0.f)
	{
		return Vector3D{ xy.x, xy.y, z };
	}

	static Vector3D MakeFromXZ(const Vector2D& xz, const float y = 0.f)
	{
		return Vector3D{ xz.x, y, xz.y };
	}

	static Vector3D MakeFromYZ(const Vector2D& yz, const float x = 0.f)
	{
		return Vector3D{ x, yz.x, yz.y };
	}

	constexpr float Dot(const Vector3D& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	constexpr Vector3D Cross(const Vector3D& rhs) const
	{
		return Vector3D
		{
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		};
	}

	Vector2D XY() const
	{
		return Vector2D{ x, y };
	}

	Vector2D XZ() const
	{
		return Vector2D{ x, z };
	}

	Vector2D YZ() const
	{
		return Vector2D{ y, z };
	}
	
	float x;
	float y;
	float z;
};