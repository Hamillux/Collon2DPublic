#include "StageJson.h"
#include "SystemTypes.h"
//#include "Actor/Actor.h"
//#include "Actor/Character/Player/PlayerInitialParams.h"
//#include "Actor/Mapchip/Block/RectangleBlock/RectangleBlockInitialParams.h"
#include "Scene/StageInteractiveScene/SpawnActorInfo.h"
#include <fstream>

namespace
{
	constexpr const char* JKEY_STAGE_ID = "stageId";
	constexpr const char* JKEY_STAGE_NAME = "stageName";
	constexpr const char* JKEY_DESCRIPTION = "description";
	constexpr const char* JKEY_TIME_LIMIT = "timeLimit";
	constexpr const char* JKEY_STAGE_LENGTH_TILES = "stageLengthTiles";
	constexpr const char* JKEY_STAGE_HEIGHT = "stageHeight";
	constexpr const char* JKEY_INITIAL_SCREEN_OFFSET = "initialScreenOffset";
	constexpr const char* JKEY_BG_LAYER_ID = "bgLayerId";
	constexpr const char* JKEY_BGM_ID = "bgmId";
	constexpr const char* JKEY_ACTORS = "actors";

	constexpr const char* TEMPLATE_FILE_PATH = "resources/stage_templates/stage_template_1.json";
}

Stage::Stage()
	: stage_id(StageId::NONE)
	, stage_name("NANASHI")
	, description("NANASHI NO STAGE")
	, time_limit(60)
	, _bg_layer_id(INVALID_MASTER_ID)
	, _stage_height(StageHeight_Short)
	, _stage_length_tile_count(MIN_STAGE_LENGTH)
{
}

Stage::~Stage()
{
}

void Stage::ToJsonObject(nlohmann::json& stage_json) const
{
	stage_id.ToJsonValue(stage_json[JKEY_STAGE_ID]);
	stage_json[JKEY_STAGE_NAME] = stage_name;
	stage_json[JKEY_DESCRIPTION] = description;
	stage_json[JKEY_TIME_LIMIT] = time_limit;
	stage_json[JKEY_STAGE_LENGTH_TILES] = _stage_length_tile_count;
	stage_json[JKEY_STAGE_HEIGHT] = _stage_height;
	stage_json[JKEY_BG_LAYER_ID] = _bg_layer_id;
	stage_json[JKEY_BGM_ID] = _bgm_id;
	stage_json[JKEY_ACTORS] = nlohmann::json::array();
	for (const auto& actor_info : spawn_actor_infos)
	{
		nlohmann::json actor_info_json;
		actor_info->ToJsonObject(actor_info_json);
		stage_json[JKEY_ACTORS].push_back(actor_info_json);
	}
}

void Stage::FromJsonObject(const nlohmann::json& stage_json)
{
	stage_id.FromJsonValue(stage_json.at(JKEY_STAGE_ID));
	stage_json.at(JKEY_STAGE_NAME).get_to(stage_name);
	stage_json.at(JKEY_DESCRIPTION).get_to(description);
	stage_json.at(JKEY_TIME_LIMIT).get_to(time_limit);
	stage_json.at(JKEY_STAGE_LENGTH_TILES).get_to(_stage_length_tile_count);
	stage_json.at(JKEY_STAGE_HEIGHT).get_to(_stage_height);
	stage_json.at(JKEY_BG_LAYER_ID).get_to(_bg_layer_id);
	stage_json.at(JKEY_BGM_ID).get_to(_bgm_id);

	for (const auto& actor_info_json : stage_json.at(JKEY_ACTORS))
	{
		SpawnActorInfo actor_info;
		actor_info.FromJsonObject(actor_info_json);
		AddSpawnActor(std::make_unique<SpawnActorInfo>(actor_info));
	}
}

bool Stage::SaveToFile(const std::string& save_destination_file_path) const
{
	std::ofstream destination(save_destination_file_path);
	if (destination.bad())
	{
		return false;
	}

	nlohmann::json j;
	ToJsonObject(j);

	destination << j.dump(4);
	destination.close();
	return true;
}

Stage Stage::MakeFromTemplate(const MasterDataID stage_template_id)
{
	std::ifstream template_file(TEMPLATE_FILE_PATH);
	if (!template_file)
	{
		throw std::runtime_error("ifstream error");
	}

	nlohmann::json template_json = nlohmann::json::parse(template_file);
	Stage stage;
	stage.FromJsonObject(template_json);
	stage.stage_id = StageId::NONE;

	return stage;
}

StageId Stage::GetStageId() const
{
	return stage_id;
}

std::string Stage::GetStageName() const
{
	return stage_name;
}

std::string Stage::GetDescription() const
{
	return description;
}

std::string Stage::GetThumbnailFileName() const
{
	return stage_id.GetThumbNailFileName();
}

uint16_t Stage::GetTimeLimit() const
{
	return time_limit;
}

int Stage::GetStageLength() const
{
	return _stage_length_tile_count;
}

void Stage::GetStageLengthRange(int& out_min, int& out_max)
{
	out_min = MIN_STAGE_LENGTH;
	out_max = MAX_STAGE_LENGTH;
}

Vector2D Stage::GetStageLeftTop() const
{
	constexpr float left = 0.f;
	const float top = (_stage_height == StageHeight_Tall) ? (-SHORT_STAGE_HEIGHT_TILES* UNIT_TILE_SIZE) : 0.f;
	return Vector2D(left, top);
}

Vector2D Stage::GetStageRightBottom() const
{
	const float right = _stage_length_tile_count * UNIT_TILE_SIZE;
	constexpr float bottom = SHORT_STAGE_HEIGHT_TILES * UNIT_TILE_SIZE;
	return Vector2D(right, bottom);
}

Vector2D Stage::GetStageSize() const
{
	return GetStageRightBottom() - GetStageLeftTop();
}

MasterDataID Stage::GetBgLayerId() const
{
	return _bg_layer_id;
}

MasterDataID Stage::GetBgmId() const
{
	return _bgm_id;
}

void Stage::SetStageId(const StageId& stage_id_in)
{
	if (!stage_id_in.IsValid())
	{
		throw std::invalid_argument("Attempted to set an invalid StageId");
		return;
	}

	stage_id = stage_id_in;
}

void Stage::SetStageName(const std::string& stage_name_in)
{
	stage_name = stage_name_in;
}

void Stage::SetDescription(const std::string& description_in)
{
	description = description_in;
}

void Stage::SetTimeLimit(const uint16_t time_limit_in)
{
	time_limit = time_limit_in;
}

void Stage::SetStageLength(const int tiles_count)
{
	if (tiles_count < MIN_STAGE_LENGTH || MAX_STAGE_LENGTH < tiles_count)
	{
		return;
	}

	_stage_length_tile_count = tiles_count;
	stage_events.OnStageSizeChanged.Dispatch();
}

void Stage::SetBgLayerId(const MasterDataID bg_layer_id)
{
	_bg_layer_id = bg_layer_id;
}

void Stage::SetBgmId(const MasterDataID bgm_id)
{
	_bgm_id = bgm_id;
}

void Stage::SetSpawnActors(const std::vector<std::shared_ptr<SpawnActorInfo>>& spawn_actor_infos_in)
{
	spawn_actor_infos = spawn_actor_infos_in;
}

void Stage::AddSpawnActor(const std::shared_ptr<SpawnActorInfo> spawn_actor_info)
{
	auto it = std::find(spawn_actor_infos.begin(), spawn_actor_infos.end(), spawn_actor_info);
	if (it != spawn_actor_infos.end())
	{
		throw std::runtime_error("Attempted to add info which already exists in spawn_actor_infos");
	}
	spawn_actor_infos.push_back(spawn_actor_info);
}

void Stage::RemoveSpawnActorInfo(const std::shared_ptr<SpawnActorInfo> spawn_actor_info)
{
	auto it = std::find(spawn_actor_infos.begin(), spawn_actor_infos.end(), spawn_actor_info);
	if (it == spawn_actor_infos.end())
	{
		throw std::runtime_error("Attempted to remove info which does not exist in spawn_actor_infos");
	}

	spawn_actor_infos.erase(it);
}
