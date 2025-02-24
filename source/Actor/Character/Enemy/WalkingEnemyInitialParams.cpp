#include "WalkingEnemyInitialParams.h"

void WalkingEnemyInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup_MaxWalkSpeed(parent, command_history);
}
