#include "RenderingCore.h"

void FColor3::ToJsonValue(nlohmann::json& value_json) const
{
	value_json = nlohmann::json::array({ r, g, b });
}

void FColor3::FromJsonValue(const nlohmann::json& value_json)
{
	std::array<float, 3> color;
	value_json.get_to(color);
	
	r = color[0];
	g = color[1];
	b = color[2];
}

void FColor4::ToJsonValue(nlohmann::json& value_json) const
{
	value_json = nlohmann::json::array({ r, g, b, a });
}

void FColor4::FromJsonValue(const nlohmann::json& value_json)
{
	std::array<float, 4> color;
	value_json.get_to(color);

	r = color[0];
	g = color[1];
	b = color[2];
	a = color[3];
}
