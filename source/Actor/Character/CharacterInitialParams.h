#pragma once

#include "Actor/ActorInitialParams.h"

struct CharacterInitialParams : public ActorInitialParams
{
	CharacterInitialParams()
		: max_hp(100)
		, max_walk_speed(300)
		, max_fly_speed(300)
		, look_right(true)
	{
		// 親クラス由来のメンバのデフォルト値
		physics.gravity_scale = 1.0;
		physics.k_air_friction = 1.0;
	}
	virtual ~CharacterInitialParams() {}

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& initial_params_json) const override;
	virtual void FromJsonObject(const nlohmann::json& initial_params_json) override;
	//~ End IJsonObject interface

	//~ Begin IEditableParameter interface

	// max_hp, face_left
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	uint16_t max_hp;
	uint16_t max_walk_speed;
	uint16_t max_fly_speed;
	bool look_right;

protected:
	void AddChildParamEditNodeToGroup_MaxWalkSpeed(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	);

	void AddChildParamEditNodeToGroup_MaxFlySpeed(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	);
};