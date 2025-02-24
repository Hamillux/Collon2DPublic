#include "Matrix3X3.h"

const Matrix3x3 Matrix3x3::Zero = Matrix3x3();
const Matrix3x3 Matrix3x3::Identity =
Matrix3x3(
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 0.f, 1.f
);

Matrix3x3::Matrix3x3()
	: _00(0.f)
	, _01(0.f)
	, _02(0.f)
	, _10(0.f)
	, _11(0.f)
	, _12(0.f)
	, _20(0.f)
	, _21(0.f)
	, _22(0.f)
{}

Matrix3x3::Matrix3x3(const float in_00, const float in_01, const float in_02, const float in_10, const float in_11, const float in_12, const float in_20, const float in_21, const float in_22)
	: _00(in_00), _01(in_01), _02(in_02)
	, _10(in_10), _11(in_11), _12(in_12)
	, _20(in_20), _21(in_21), _22(in_22)
{
}

DirectX::XMMATRIX Matrix3x3::ToXMMATRIX() const
{
	return DirectX::XMMATRIX(
		_00, _01, _02, 0.f,
		_10, _11, _12, 0.f,
		_20, _21, _22, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& rhs) const
{
	const float r_00 = _00 * rhs._00 + _01 * rhs._10 + _02 * rhs._20;
	const float r_01 = _00 * rhs._01 + _01 * rhs._11 + _02 * rhs._21;
	const float r_02 = _00 * rhs._02 + _01 * rhs._12 + _02 * rhs._22;

	const float r_10 = _10 * rhs._00 + _11 * rhs._10 + _12 * rhs._20;
	const float r_11 = _10 * rhs._01 + _11 * rhs._11 + _12 * rhs._21;
	const float r_12 = _10 * rhs._02 + _11 * rhs._12 + _12 * rhs._22;

	const float r_20 = _20 * rhs._00 + _21 * rhs._10 + _22 * rhs._20;
	const float r_21 = _20 * rhs._01 + _21 * rhs._11 + _22 * rhs._21;
	const float r_22 = _20 * rhs._02 + _21 * rhs._12 + _22 * rhs._22;

	return Matrix3x3(
		r_00, r_01, r_02,
		r_10, r_11, r_12,
		r_20, r_21, r_22);
}

Vector2D Matrix3x3::TransformVector(const Vector2D& in_v) const
{
	const float out_x = _00 * in_v.x + _01 * in_v.y + _02;
	const float out_y = _10 * in_v.x + _11 * in_v.y + _12;
	return Vector2D(out_x, out_y);
}

void Matrix3x3::TransformVector(const Vector2D& in_v, Vector2D& out_v) const
{
	out_v.x = _00 * in_v.x + _01 * in_v.y + _02;
	out_v.y = _10 * in_v.x + _11 * in_v.y + _12;
}
