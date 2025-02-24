#pragma once
#include "Actor/Mapchip/Block/BlockInitialParams.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

struct SlopeBlockInitialParams : public BlockInitialParams
{
public:
	SlopeBlockInitialParams()
		: scale(1)
		, width_per_height(1)
	{}
	virtual ~SlopeBlockInitialParams() {}

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

	int scale;
	int width_per_height;
};
