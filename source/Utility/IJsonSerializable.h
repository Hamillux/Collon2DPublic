#pragma once

#include <nlohmann/json.hpp>

struct IJsonObject
{
	virtual void ToJsonObject(nlohmann::json& j) const {}
	virtual void FromJsonObject(const nlohmann::json& j) = 0;
};

struct IJsonValue
{
	virtual ~IJsonValue() {}

	virtual void ToJsonValue(nlohmann::json& value_json) const = 0;
	virtual void FromJsonValue(const nlohmann::json& value_json) = 0;
};