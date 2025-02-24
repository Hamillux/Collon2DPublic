#include "Vector2D.h"
#include <nlohmann/json.hpp>
#include <cmath>
#include <cfenv>
#include <DxLib.h>
#include "Utility/Core/Rendering/CameraParams.h"
#include "SystemTypes.h"

Vector2D& Vector2D::operator=(const Vector2D& in_vector)
{
	// 自己代入をチェック
	if (this == &in_vector)
	{
		return *this;
	}

	x = in_vector.x;
	y = in_vector.y;

	return *this;
}

Vector2D& Vector2D::operator=(const std::array<float, 2>& std_arr)
{
	x = std_arr.at(0);
	y = std_arr.at(1);
	return *this;
}

Vector2D::operator std::array<float, 2>() const
{
	return std::array<float,2>{ x, y };
}

void Vector2D::ToJsonValue(nlohmann::json& value_json) const
{
	value_json = {x, y};
}

void Vector2D::FromJsonValue(const nlohmann::json& value_json)
{
	const bool json_is_valid =
		value_json.is_array() && value_json.size() == 2;
	assert(json_is_valid);

	x = value_json.at(0).get<float>();
	y = value_json.at(1).get<float>();
}

const Vector2D Vector2D::operator +(const Vector2D& in_vector) const
{
	return Vector2D(x + in_vector.x, y + in_vector.y);
}

Vector2D& Vector2D::operator +=(const Vector2D& in_vector)
{
	x += in_vector.x;
	y += in_vector.y;
	return *this;
}

const Vector2D Vector2D::operator -(const Vector2D& in_vector) const
{
	return Vector2D(x - in_vector.x, y - in_vector.y);
}

Vector2D& Vector2D::operator -=(const Vector2D& in_vector)
{
	x -= in_vector.x;
	y -= in_vector.y;
	return *this;
}

const Vector2D Vector2D::operator *(float in_scalar) const
{
	return Vector2D(x * in_scalar, y * in_scalar);
}

Vector2D& Vector2D::operator *=(float in_scalar)
{
	x *= in_scalar;
	y *= in_scalar;
	return *this;
}

const Vector2D Vector2D::operator*(const Vector2D& in_vector) const
{
	return Vector2D(x * in_vector.x, y * in_vector.y);
}

Vector2D& Vector2D::operator *=(const Vector2D& in_vector)
{
	x *= in_vector.x;
	y *= in_vector.y;
	return *this;
}

const Vector2D Vector2D::operator /(float in_scalar) const
{
	if (in_scalar == 0.f)
	{
		// ゼロ除算は結果を0にする
		return Vector2D(0.f, 0.f);
	}

	return Vector2D(x / in_scalar, y / in_scalar);
}

Vector2D& Vector2D::operator /=(float in_scalar)
{
	if (in_scalar == 0.f)
	{
		// ゼロ除算は結果を0にする
		x = 0.f;
		y = 0.f;
	}
	else
	{
		x /= in_scalar;
		y /= in_scalar;
	}

	return *this;
}


bool Vector2D::operator==(const Vector2D& in_vector) const
{
	const bool equivalent =
		x == in_vector.x &&
		y == in_vector.y;
	return equivalent;
}

bool Vector2D::operator!=(const Vector2D& in_vector) const
{
	return !(*this == in_vector);
}

Vector2D Vector2D::ex()
{
	return Vector2D(1.f, 0.f);
}

Vector2D Vector2D::ey()
{
	return Vector2D(0.f, 1.f);
}

float Vector2D::Dot(const Vector2D& A, const Vector2D& B)
{
	return (A.x * B.x) + (A.y * B.y);
}

float Vector2D::Cross(const Vector2D& A, const Vector2D& B)
{
	return (A.x * B.y) - (A.y * B.x);
}

Vector2D Vector2D::Rotate(const Vector2D& A, float theta_rad)
{
	if (theta_rad == 0.f) return A;
	
	Vector2D rotated_vector
	(
		A.x * cos(theta_rad) - A.y * sin(theta_rad),
		A.x * sin(theta_rad) + A.y * cos(theta_rad)
	);
	return rotated_vector;
}

Vector2D Vector2D::GetRotated(const float theta_rad) const
{
	return Vector2D::Rotate(*this, theta_rad);
}

Vector2D Vector2D::WorldToViewport(const Vector2D& world_position, const CameraParams& camera_params)
{
	return camera_params.TransformPosition_WorldToViewport(world_position);
}

bool Vector2D::IsZeroVector() const
{
	return ( x == 0.f && y == 0.f);
}

float Vector2D::Length() const
{
	return sqrtf(powf(x, 2.0f) + powf(y, 2.0f));
}

float Vector2D::LengthSquared() const
{
	return x * x + y * y;
}

Vector2D Vector2D::Normalize() const
{
	return *this / Length();
}

Vector2D Vector2D::GetX() const
{
	return Vector2D(x, 0);
}

Vector2D Vector2D::GetY() const
{
	return Vector2D(0, y);
}

void Vector2D::ToIntRound(int& out_x, int& out_y) const
{
	out_x = (int)std::round(x);
	out_y = (int)std::round(y);
}

void Vector2D::ToIntFloor(int& out_x, int& out_y) const
{
	out_x = (int)std::floor(x);
	out_y = (int)std::floor(y);
}

void Vector2D::ToIntCeil(int& out_x, int& out_y) const
{
	out_x = (int)std::ceil(x);
	out_y = (int)std::ceil(y);
}

std::ostream& operator<<(std::ostream& os, const Vector2D& v)
{
	os << "(";
	if (v.x == FLT_MAX)
	{
		os << "MAX";
	}
	else
	{
		os << v.x;
	}
	os << ",";
	if (v.y == FLT_MAX)
	{
		os << "MAX";
	}
	else
	{
		os << v.y;
	}
	os << ")";

	return os;
}
