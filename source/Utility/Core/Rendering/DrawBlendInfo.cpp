#include "DrawBlendInfo.h"
#include <DxLib.h>

const int DrawBlendInfo::BLENDMODE_NOBLEND = DX_BLENDMODE_NOBLEND;

int GetDxBlendModeValueByString(const std::string dx_blend_mode_str)
{
	if (dx_blend_mode_str == "NOBLEND")
	{
		return DX_BLENDMODE_NOBLEND;
	}
	else if (dx_blend_mode_str == "ALPHA")
	{
		return DX_BLENDMODE_ALPHA;
	}
	else if (dx_blend_mode_str == "ADD")
	{
		return DX_BLENDMODE_ADD;
	}
	else if (dx_blend_mode_str == "SUB")
	{
		return DX_BLENDMODE_SUB;
	}
	else if (dx_blend_mode_str == "MULA")
	{
		return DX_BLENDMODE_MULA;
	}
	else if (dx_blend_mode_str == "INVSRC")
	{
		return DX_BLENDMODE_INVSRC;
	}

	return DX_BLENDMODE_NOBLEND;
}

void DrawBlendInfo::FromJsonObject(const nlohmann::json& blend_info_json)
{
	const std::string blend_mode_str = blend_info_json.at("blendMode").get<std::string>();
	dx_blend_mode = GetDxBlendModeValueByString(blend_mode_str);
	blend_info_json.at("blendValue").get_to(blend_value);
}

bool DrawBlendInfo::operator==(const DrawBlendInfo& other) const
{
	return (
		dx_blend_mode == other.dx_blend_mode &&
		blend_value == other.blend_value
		);
}

bool DrawBlendInfo::IsNoBlend() const
{
	return dx_blend_mode == DX_BLENDMODE_NOBLEND;
}
