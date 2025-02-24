#pragma once
#include "Actor/ActorInitialParams.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

struct BlockInitialParams : public ActorInitialParams
{
public:
	BlockInitialParams()
		: block_id(1)
		, _is_horizontal_flip_enabled(false)
	{
	}
	virtual ~BlockInitialParams() {}
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

	MasterDataID block_id;
	bool _is_horizontal_flip_enabled;
};