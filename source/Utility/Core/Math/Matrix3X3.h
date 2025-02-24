#pragma once
#include "Utility/Core/Math/Vector2D.h"
#include "DirectXMath.h"

struct Matrix3x3
{
	static const Matrix3x3 Zero;
	static const Matrix3x3 Identity;

	Matrix3x3();

	Matrix3x3(
		const float in_00, const float in_01, const float in_02,
		const float in_10, const float in_11, const float in_12,
		const float in_20, const float in_21, const float in_22
	);

	/// <summary>
	/// DirectX::XMMATRIXに変換
	/// </summary>
	DirectX::XMMATRIX ToXMMATRIX() const;

	////////////
	/// 算術演算子
	////////////
	Matrix3x3 operator*(const Matrix3x3& rhs) const;

	// (in_v.x, in_v.y, 1.f)の3次元ベクトルを変換
	Vector2D TransformVector(const Vector2D& in_v) const;
	void TransformVector(const Vector2D& in_v, Vector2D& out_v) const;

	float _00;
	float _01;
	float _02;
	float _10;
	float _11;
	float _12;
	float _20;
	float _21;
	float _22;
};