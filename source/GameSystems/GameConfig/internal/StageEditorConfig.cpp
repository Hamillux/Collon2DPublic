#include "StageEditorConfig.h"

namespace
{
	constexpr const char* JKEY_SOUND_VOLUME = "soundVolume";
	constexpr const char* JKEY_BGM = "bgm";
	constexpr const char* JKEY_SE = "se";
	constexpr const char* JKEY_GRID_COLOR = "gridColor";
}

void StageEditorConfig::FromJsonObject(const nlohmann::json& j)
{
	j.at(JKEY_SOUND_VOLUME).at(JKEY_BGM).get_to(sound_volume.bgm);
	j.at(JKEY_SOUND_VOLUME).at(JKEY_SE).get_to(sound_volume.se);
	grid_color.FromJsonValue(j.at(JKEY_GRID_COLOR));
}

void StageEditorConfig::ToJsonObject(nlohmann::json& j) const
{
	j[JKEY_SOUND_VOLUME] = {};
	j[JKEY_SOUND_VOLUME][JKEY_BGM] = sound_volume.bgm;
	j[JKEY_SOUND_VOLUME][JKEY_SE] = sound_volume.se;
	grid_color.ToJsonValue(j[JKEY_GRID_COLOR]);
}
