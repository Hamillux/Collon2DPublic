#pragma once

#include "Actor/Mapchip/Block/BlockInitialParams.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

struct RectangleBlockInitialParams : public BlockInitialParams
{
	RectangleBlockInitialParams()
		: tile_count(std::array<int, 2>{1, 1})
	{}
	virtual ~RectangleBlockInitialParams() {}

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
	
	struct TileCount 
	{
		int x; int y;
		TileCount() : x(1), y(1) {}
		TileCount(const std::array<int, 2>& tile_count) : x(tile_count[0]), y(tile_count[1]) {}
		operator std::array<int, 2>() const { return { x, y }; }
	};
	TileCount tile_count;
};