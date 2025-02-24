#include "ItemInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace
{
	constexpr const char* JKEY_SCORE = "score";
	constexpr const char* JKEY_ITEM_ID = "itemId";
	constexpr const char* LABEL_ITEM_ID = "ItemID";
}
void ItemActorInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);

	initial_params_json[JKEY_ITEM_ID] = item_id;
}

void ItemActorInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);

	initial_params_json.at(JKEY_ITEM_ID).get_to(item_id);
}

void ItemActorInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup<EditParamType::ITEM_ID>(
		parent,
		command_history,
		LABEL_ITEM_ID,
		item_id);
}
