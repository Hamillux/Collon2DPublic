#include "SpawnActorInfo.h"
#include "Actor/ActorInitialParams.h"
#include "Actor/ActorFactory.h"
#include "Actor/EntityTraits.h"

namespace
{
	constexpr const char* JKEY_ENTITY_TYPE = "entityType";
	constexpr const char* JKEY_INITIAL_PARAMS = "initialParams";
}

SpawnActorInfo::SpawnActorInfo()
	: entity_type(EEntityType::None)
{}

SpawnActorInfo::~SpawnActorInfo()
{}

void SpawnActorInfo::ToJsonObject(nlohmann::json & actor_info_json) const
{
	actor_info_json[JKEY_ENTITY_TYPE] = EnumInfo<EEntityType>::EnumToString(entity_type);
	initial_params->ToJsonObject(actor_info_json[JKEY_INITIAL_PARAMS]);
}

void SpawnActorInfo::FromJsonObject(const nlohmann::json& actor_info_json)
{
	entity_type = EnumInfo<EEntityType>::StringToEnum(actor_info_json.at(JKEY_ENTITY_TYPE).get<std::string>());
	initial_params = ActorFactory::CreateInitialParamsByEntityType(entity_type);
	initial_params->FromJsonObject(actor_info_json.at(JKEY_INITIAL_PARAMS));

}