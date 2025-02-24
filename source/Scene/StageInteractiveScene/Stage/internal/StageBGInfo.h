#pragma once

#include "Utility/IJsonSerializable.h"
#include <string>
#include "GameSystems/MasterData/MasterDataInclude.h"

struct StageBGInfo : public IJsonObject
{
	StageBGInfo()
		: distance(1.f)
		, image_id(INVALID_MASTER_ID)
	{}
	virtual ~StageBGInfo(){}

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& stage_bg_info_json) const override;
	virtual void FromJsonObject(const nlohmann::json& stage_bg_info_json) override;
	//~ End IJsonObject interface

	MasterDataID image_id;
	float distance;
};

struct StageBGLayer : public IJsonObject
{
	StageBGLayer() 
		: bg_layer_id(INVALID_MASTER_ID)
	{}
	virtual ~StageBGLayer() {}

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& stage_bg_layer_json) const override;
	virtual void FromJsonObject(const nlohmann::json& stage_bg_layer_json) override;
	//~ End IJsonObject interface

	static void LoadStageBGLayers(std::vector<StageBGLayer>& out_bg_layers);

	MasterDataID bg_layer_id;
	std::vector<StageBGInfo> bg_infos;
	std::string name;
};