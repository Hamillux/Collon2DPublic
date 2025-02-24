#include "EnemyBaseInitialParams.h"

namespace
{
	constexpr int DEFAULT_PLAYER_HIT_DAMAGE = 50;

	constexpr const char* JSON_KEY_PLAYER_HIT_DAMAGE = "playerHitDamage";
}

EnemyBaseInitialParams::EnemyBaseInitialParams()
	: player_hit_damage(DEFAULT_PLAYER_HIT_DAMAGE)
{
}

void EnemyBaseInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JSON_KEY_PLAYER_HIT_DAMAGE] = player_hit_damage;
}

void EnemyBaseInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
	initial_params_json.at(JSON_KEY_PLAYER_HIT_DAMAGE).get_to(player_hit_damage);
}

void EnemyBaseInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);
}
