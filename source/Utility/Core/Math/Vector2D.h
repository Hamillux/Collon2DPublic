#pragma once

#include <iosfwd>
#include <vector>
#include <array>
#include "Utility/IJsonSerializable.h"

struct CameraParams;

/**
 * 2Dベクトルクラス
 */
struct Vector2D : public IJsonValue
{
public:
	Vector2D()
		: x(0.0f), y(0.0f)
	{}

	Vector2D(float in_x, float in_y)
		: x(in_x), y(in_y)
	{}

	template <class T>
	explicit Vector2D(const std::vector<T>& xy_vector)
	{
		x = static_cast<float>(xy_vector.at(0));
		y = static_cast<float>(xy_vector.at(1));
	}

	explicit Vector2D(const std::array<float, 2>& std_arr)
		: x(std_arr.at(0)), y(std_arr.at(1))
	{}

	explicit operator std::array<float,2>()const;

	//~ Begin IJsonValue interface
	virtual void ToJsonValue(nlohmann::json& value_json) const override;
	virtual void FromJsonValue(const nlohmann::json& value_json) override;
	//~ End IJsonValue interface

	Vector2D& operator=(const Vector2D& in_vector);
	Vector2D& operator=(const std::array<float, 2>& std_arr);

	const Vector2D operator +(const Vector2D& in_vector) const;
	Vector2D& operator +=(const Vector2D& in_vector);
	const Vector2D operator -(const Vector2D& in_vector) const;
	Vector2D& operator -=(const Vector2D& in_vector);
	const Vector2D operator *(float in_scalar) const;
	Vector2D& operator *=(float in_scalar);
	const Vector2D operator *(const Vector2D& in_vector) const;
	Vector2D& operator *=(const Vector2D& in_vector);
	const Vector2D operator /(float in_scalar) const;
	Vector2D& operator /=(float in_scalars);

	bool operator==(const Vector2D& in_vector) const;
	bool operator!=(const Vector2D& in_vector) const;

	/// <summary>
	/// X方向単位ベクトル
	/// </summary>
	/// <returns></returns>
	static Vector2D ex();

	/// <summary>
	/// Y方向単位ベクトル
	/// </summary>
	/// <returns></returns>
	static Vector2D ey();

	static float Dot(const Vector2D& a, const Vector2D& b);
	static float Cross(const Vector2D& a, const Vector2D& b);
	static Vector2D Rotate(const Vector2D& a, float theta_rad);
	Vector2D GetRotated(const float theta_rad) const;

	// ワールド座標をビューポート座標に変換
	// TODO: CameraParams::TransformPosition_WorldToViewportで置換
	static Vector2D WorldToViewport(const Vector2D& world_position,const CameraParams& viewport_params);
	
	bool IsZeroVector() const;
	float Length() const;
	float LengthSquared() const;
	Vector2D Normalize() const;

	Vector2D GetX() const;
	Vector2D GetY() const;

	// 四捨五入によるintへの変換
	void ToIntRound(int& out_x, int& out_y) const;
	// 切り捨てによるintへの変換
	void ToIntFloor(int& out_x, int& out_y) const;
	// 切り上げによるintへの変換
	void ToIntCeil(int& out_x, int& out_y) const;

public:
	float x, y;
};

std::ostream& operator<<(std::ostream& os, const Vector2D& v);