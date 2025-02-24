#include "ActorParamTypes.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"
#include <DxLib.h>
#include "SystemTypes.h"


namespace {
	constexpr const char* JKEY_GRAVITY_SCALE = "gravityScale";
	constexpr const char* JKEY_COEFF_AIR_FRICTION = "coeffAirFriction";
	constexpr const char* JKEY_MASS = "mass";

	constexpr const char* LABEL_GRAVITY_SCALE = "GravityScale";
	constexpr const char* LABEL_COEFF_AIR_FRICTION = "AirFriction";
	constexpr const char* LABEL_MASS = "Mass";
}

void Physics::ToJsonObject(nlohmann::json& physics_json) const
{
	physics_json[JKEY_GRAVITY_SCALE] = gravity_scale;
	physics_json[JKEY_COEFF_AIR_FRICTION] = k_air_friction;
	physics_json[JKEY_MASS] = mass;
}

void Physics::FromJsonObject(const nlohmann::json& physics_json)
{
	physics_json.at(JKEY_GRAVITY_SCALE).get_to(gravity_scale);
	physics_json.at(JKEY_COEFF_AIR_FRICTION).get_to(k_air_friction);
	physics_json.at(JKEY_MASS).get_to(mass);
}

void Physics::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	auto validator_clamp_min_epsiron = std::make_shared<EditParamValidator_ClampMin<EditParamType::FLOAT>>(EPSIRON);
	auto validator_clamp_min_zero = std::make_shared<EditParamValidator_ClampMin<EditParamType::FLOAT>>(0.f);

	auto physics_group = std::make_shared<ParamEditGroup>("Physics", 3);

	AddChildParamEditNodeToGroup<EditParamType::FLOAT>(
		physics_group,
		command_history,
		LABEL_GRAVITY_SCALE,
		gravity_scale,
		validator_clamp_min_zero);

	AddChildParamEditNodeToGroup<EditParamType::FLOAT>(
		physics_group,
		command_history,
		LABEL_MASS,
		mass,
		validator_clamp_min_epsiron);

	AddChildParamEditNodeToGroup<EditParamType::FLOAT>(
		physics_group,
		command_history,
		LABEL_COEFF_AIR_FRICTION,
		k_air_friction,
		validator_clamp_min_epsiron);

	parent->AddChild(physics_group);
}