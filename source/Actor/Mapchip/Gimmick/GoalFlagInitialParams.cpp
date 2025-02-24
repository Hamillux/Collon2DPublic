#include "GoalFlagInitialParams.h"

void GoalFlagInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
}

void GoalFlagInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
}

void GoalFlagInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	// TODO: ゴール条件を編集できるようにする
}
