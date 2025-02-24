#pragma once

#include "GameConfigItem.h"

struct StageEditorConfig : public GameConfigItem<StageEditorConfig>
{
	StageEditorConfig() {}
	virtual ~StageEditorConfig() {}

	static constexpr KeyType config_key = "StageEditorConfig";

	//~ Begin IJsonObject interface
	virtual void FromJsonObject(const nlohmann::json& j) override;
	virtual void ToJsonObject(nlohmann::json& j) const override;
	//~ End IJsonObject interface

	struct SoundVolume
	{
		int bgm{ 50 };
		int se{ 50 };
	};
	SoundVolume sound_volume;

	FColor4 grid_color{ 0.5,0.5,0.5,0.5 };	
};