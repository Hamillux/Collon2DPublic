#pragma once

#include "Rendering/BlendMode.h"
#include "Rendering/DrawBlendInfo.h"
#include "Rendering/DrawHelper.h"
#include "Rendering/CameraParams.h"
#include "Rendering/CanvasInfo.h"
#include "Rendering/AnimPlayInfo.h"

#include "Utility/IJsonSerializable.h"

struct FColor3 : public IJsonValue
{
	float r{ 0 };
	float g{ 0 };
	float b{ 0 };

	FColor3() {};
	FColor3(float r, float g, float b) : r(r), g(g), b(b) {};

	//~ Begin IJsonValue interface
	virtual void ToJsonValue(nlohmann::json& value_json) const override;
	virtual void FromJsonValue(const nlohmann::json& value_json) override;
	//~ End IJsonValue interface

	operator std::array<float, 3>() const
	{
		return { r, g, b };
	}

	FColor3& operator=(const std::array<float, 3>& arr) 
	{
		r = arr[0];
		g = arr[1];
		b = arr[2];
		return *this;
	}

	int Get00RRGGBB() const
	{
		return static_cast<int>(r * 255) << 16 | static_cast<int>(g * 255) << 8 | static_cast<int>(b * 255);
	}
};

struct FColor4 : public IJsonValue
{
	float r{ 0 };
	float g{ 0 };
	float b{ 0 };
	float a{ 0 };

	FColor4() {};
	FColor4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {};

	//~ Begin IJsonValue interface
	virtual void ToJsonValue(nlohmann::json& value_json) const override;
	virtual void FromJsonValue(const nlohmann::json& value_json) override;
	//~ End IJsonValue interface

	operator std::array<float, 4>() const
	{
		return { r, g, b, a };
	}

	FColor4& operator=(const std::array<float, 4>& arr)
	{
		r = arr[0];
		g = arr[1];
		b = arr[2];
		a = arr[3];
		return *this;
	}

	int Get00RRGGBB() const
	{
		return static_cast<int>(r * 255) << 16 | static_cast<int>(g * 255) << 8 | static_cast<int>(b * 255);
	}
};