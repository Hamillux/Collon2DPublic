#include "PlayerInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace
{
	constexpr const char* JKEY_MAX_SP = "maxSp";
	constexpr const char* LABEL_MAX_SP = "SP";

	const std::shared_ptr<EditParamValidator<EditParamType::INT>> maxSpValidator =
		std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(1, 999);
}

void PlayerInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);

	initial_params_json[JKEY_MAX_SP] = _max_sp;
}

void PlayerInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);

	initial_params_json.at(JKEY_MAX_SP).get_to(_max_sp);
}


void PlayerInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_MAX_SP,
		_max_sp,
		maxSpValidator);
}