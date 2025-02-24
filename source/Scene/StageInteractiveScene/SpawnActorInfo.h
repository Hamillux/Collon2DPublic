#pragma once

#include "Utility/IJsonSerializable.h"
#include "Actor/EntityType.h"
#include <stdint.h>
#include <memory>

struct ActorInitialParams;
struct SpawnActorInfo : public IJsonObject
{
	SpawnActorInfo();
	virtual ~SpawnActorInfo();

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& actor_info_json) const override;
	virtual void FromJsonObject(const nlohmann::json& actor_info_json) override;
	//~ End IJsonObject interface

	EEntityType entity_type;
	std::shared_ptr<ActorInitialParams> initial_params;
};