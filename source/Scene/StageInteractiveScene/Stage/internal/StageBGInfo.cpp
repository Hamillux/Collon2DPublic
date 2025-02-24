#include "StageBGInfo.h"
#include <fstream>

namespace
{
	constexpr const char* JKEY_DISTNACE = "distance";
	constexpr const char* JKEY_IMAGE_ID = "imageId";
}

void StageBGInfo::ToJsonObject(nlohmann::json& stage_bg_info_json) const
{
	stage_bg_info_json[JKEY_DISTNACE] = distance;
	stage_bg_info_json[JKEY_IMAGE_ID] = image_id;
}

void StageBGInfo::FromJsonObject(const nlohmann::json& stage_bg_info_json)
{
	stage_bg_info_json.at(JKEY_DISTNACE).get_to(distance);
	stage_bg_info_json.at(JKEY_IMAGE_ID).get_to(image_id);
}

namespace
{
	constexpr const char* JKEY_BG_LAYER_ID = "id";
	constexpr const char* JKEY_NAME = "name";
	constexpr const char* JKEY_BG_INFOS = "bgInfos";
	constexpr const char* STAGE_BG_JSON_PATH = "resources/stage_backgrounds.json";
}

void StageBGLayer::ToJsonObject(nlohmann::json& stage_bg_layer_json) const
{
	stage_bg_layer_json[JKEY_BG_LAYER_ID] = bg_layer_id;

	stage_bg_layer_json[JKEY_NAME] = name;

	nlohmann::json bg_infos_json = nlohmann::json::array();
	for (const auto& bg_info : bg_infos)
	{
		nlohmann::json bg_info_json;
		bg_info.ToJsonObject(bg_info_json);
		bg_infos_json.push_back(bg_info_json);
	}
}

void StageBGLayer::FromJsonObject(const nlohmann::json& stage_bg_layer_json)
{
	stage_bg_layer_json.at(JKEY_BG_LAYER_ID).get_to(bg_layer_id);

	stage_bg_layer_json.at(JKEY_NAME).get_to(name);

	const nlohmann::json& bg_infos_json = stage_bg_layer_json.at(JKEY_BG_INFOS);
	bg_infos.clear();
	for (const auto& bg_info_json : bg_infos_json)
	{
		StageBGInfo bg_info;
		bg_info.FromJsonObject(bg_info_json);
		bg_infos.push_back(bg_info);
	}
}

void StageBGLayer::LoadStageBGLayers(std::vector<StageBGLayer>& out_bg_layers)
{
	std::ifstream json_file(STAGE_BG_JSON_PATH);
	nlohmann::json stage_bg_layers_json = nlohmann::json::parse(json_file);
	out_bg_layers.clear();
	for (const auto& stage_bg_layer_json : stage_bg_layers_json)
	{
		StageBGLayer bg_layer;
		bg_layer.FromJsonObject(stage_bg_layer_json);
		out_bg_layers.push_back(bg_layer);
	}
}
