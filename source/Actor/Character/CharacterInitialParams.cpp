#include "CharacterInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace
{
	constexpr const char* JKEY_MAX_HP = "maxHp";
	constexpr const char* JKEY_LOOK_RIGHT = "lookRight";
	constexpr const char* JKEY_MAX_WALK_SPEED = "maxWalkSpeed";
	constexpr const char* JKEY_MAX_FLY_SPEED = "maxFlySpeed";

	constexpr const char* LABEL_HP = "HP";
	constexpr const char* LABEL_LOOK_RIGHT = "LookRight";
	constexpr const char* LABEL_WALK_SPEED = "WalkSpeed";
	constexpr const char* LABEL_FLY_SPEED = "FlySpeed";

	// maxHp, maxWalkSpeed, maxFlySpeedは1以上の整数
	const std::shared_ptr<EditParamValidator<EditParamType::INT>> maxHpValidator =
		std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(1, 999);

	const std::shared_ptr<EditParamValidator<EditParamType::INT>> maxWalkSpeedValidator =
		std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(50, 999);

	const std::shared_ptr<EditParamValidator<EditParamType::INT>> maxFlySpeedValidator =
		std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(50, 999);
}

void CharacterInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JKEY_MAX_HP] = max_hp;
	initial_params_json[JKEY_LOOK_RIGHT] = look_right;
	initial_params_json[JKEY_MAX_WALK_SPEED] = max_walk_speed;
	initial_params_json[JKEY_MAX_FLY_SPEED] = max_fly_speed;
}

void CharacterInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);

	initial_params_json.at(JKEY_MAX_HP).get_to(max_hp);
	initial_params_json.at(JKEY_LOOK_RIGHT).get_to(look_right);
	initial_params_json.at(JKEY_MAX_WALK_SPEED).get_to(max_walk_speed);
	initial_params_json.at(JKEY_MAX_FLY_SPEED).get_to(max_fly_speed);
}

void CharacterInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_HP,
		max_hp,
		maxHpValidator);

	AddChildParamEditNodeToGroup<EditParamType::BOOL>(
		parent,
		command_history,
		LABEL_LOOK_RIGHT,
		look_right,
		nullptr
	);
}

void CharacterInitialParams::AddChildParamEditNodeToGroup_MaxWalkSpeed(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_WALK_SPEED,
		max_walk_speed,
		maxWalkSpeedValidator);
}

void CharacterInitialParams::AddChildParamEditNodeToGroup_MaxFlySpeed(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_FLY_SPEED,
		max_fly_speed,
		maxFlySpeedValidator);
}

