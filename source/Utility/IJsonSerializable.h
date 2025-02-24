#pragma once

#include <nlohmann/json.hpp>

/// <summary>
/// Jsonオブジェクトと相互変換するインターフェース
/// </summary>
struct IJsonObject
{
	virtual void ToJsonObject(nlohmann::json& j) const {}
	virtual void FromJsonObject(const nlohmann::json& j) = 0;
};

/// <summary>
/// Json値と相互変換するインターフェース
/// </summary>
struct IJsonValue
{
	virtual ~IJsonValue() {}

	virtual void ToJsonValue(nlohmann::json& value_json) const = 0;
	virtual void FromJsonValue(const nlohmann::json& value_json) = 0;
};