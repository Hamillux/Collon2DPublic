#include "FlyingEnemyInitialParams.h"

void FlyingEnemyInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup_MaxFlySpeed(parent, command_history);
}
