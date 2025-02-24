#pragma once

#include "Actor/ActorInitialParams.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

struct ItemActorInitialParams : public ActorInitialParams
{
	ItemActorInitialParams()
		: item_id(1)
	{}
	virtual ~ItemActorInitialParams() {}

    //~ Begin interface of AbstractJsonSerializable
    virtual void ToJsonObject(nlohmann::json& initial_params_json) const override;
    virtual void FromJsonObject(const nlohmann::json& initial_params_json) override;
    //~ End interface of AbstractJsonSerializable

	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	MasterDataID item_id;
};
