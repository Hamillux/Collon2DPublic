#pragma once

#include "Actor/Character/CharacterInitialParams.h"

struct EnemyBaseInitialParams : public CharacterInitialParams
{
	EnemyBaseInitialParams();

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& initial_params_json) const override;
	virtual void FromJsonObject(const nlohmann::json& initial_params_json) override;
	//~ End IJsonObject interface

	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	int player_hit_damage;
};