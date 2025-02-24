#include "BlockInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace {
	constexpr const char* JKEY_BLOCK_ID = "blockId";
	constexpr const char* JKEY_HORIZONAL_FLIP = "horizontalFlip";
	constexpr const char* LABEL_BLOCK_ID = "BlockID";
	constexpr const char* LABEL_FLIPPED = "Flipped";
}

void BlockInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JKEY_BLOCK_ID] = block_id;
	initial_params_json[JKEY_HORIZONAL_FLIP] = _is_horizontal_flip_enabled;
}

void BlockInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
	initial_params_json.at(JKEY_BLOCK_ID).get_to(block_id);
	initial_params_json.at(JKEY_HORIZONAL_FLIP).get_to(_is_horizontal_flip_enabled);
}

void BlockInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);


	AddChildParamEditNodeToGroup<EditParamType::BLOCK_SKIN>(
		parent,
		command_history,
		LABEL_BLOCK_ID,
		block_id);

	AddChildParamEditNodeToGroup<EditParamType::BOOL>(
		parent,
		command_history,
		LABEL_FLIPPED,
		_is_horizontal_flip_enabled);
}
