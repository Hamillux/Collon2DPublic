#pragma once

#include "EnemyBaseInitialParams.h"

struct FlyingEnemyInitialParams : public EnemyBaseInitialParams
{
	FlyingEnemyInitialParams() {}
	virtual ~FlyingEnemyInitialParams() {}

	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface
};