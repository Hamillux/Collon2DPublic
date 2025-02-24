#include "RectangleBlockInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace {
	constexpr const char* JKEY_TILE_COUNT = "tileCount";
	constexpr const char* JKEY_RECTANGLE_BLOCK_ID = "rectangleBlockId";

	constexpr const char* LABEL_TILE_COUNT = "TileCount";
	constexpr const char* LABEL_BLOCK_ID = "BlockID";

	constexpr int MIN_TILE_COUNT = 1;
	constexpr int SHORT_STAGE_HEIGHT_TILES = 18;	// 小ステージの高さよりも大きくならないようにする
	const std::shared_ptr<EditParamValidator<EditParamType::INT2>> tileCountValidator
		= std::make_shared<EditParamValidator_ClampArray<EditParamType::INT2>>(MIN_TILE_COUNT, SHORT_STAGE_HEIGHT_TILES);
}


void RectangleBlockInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JKEY_TILE_COUNT] = static_cast<std::array<int, 2>>(tile_count);
}

void RectangleBlockInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
	initial_params_json.at(JKEY_TILE_COUNT).at(0).get_to(tile_count.x);
	initial_params_json.at(JKEY_TILE_COUNT).at(1).get_to(tile_count.y);
}

void RectangleBlockInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup<EditParamType::INT2>(
		parent,
		command_history,
		LABEL_TILE_COUNT,
		tile_count,
		tileCountValidator);
}