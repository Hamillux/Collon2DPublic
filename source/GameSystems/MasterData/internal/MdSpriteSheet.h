#pragma once
#include "MasterDataBase.h"

struct MdSpriteSheet : public MasterData<MdSpriteSheet>
{
	MasterDataID id;
	MasterDataID image_id;
	int num_rows;
	int num_columns;
	EBlendMode    blend_mode;

	auto GetMembers()
	{
		return std::tie(
			id,
			image_id,
			num_rows,
			num_columns,
			blend_mode
		);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	// コンストラクタ
	MdSpriteSheet()
		: id(0)
		, image_id(INVALID_MASTER_ID)
		, num_rows(1)
		, num_columns(1)
		, blend_mode(EBlendMode::Alpha)
	{}

};
