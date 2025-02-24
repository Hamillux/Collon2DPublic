#pragma once
#include "MasterDataBase.h"


/// <summary>
/// RectangleBlock, SlopeBlockで使用するブロックスキン
/// </summary>
struct MdBlockSkin : public MasterData<MdBlockSkin>
{
	MasterDataID id;
	MasterDataID image_id;

	auto GetMembers()
	{
		return std::tie(
			id,
			image_id
		);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	// コンストラクタ
	MdBlockSkin()
		: id(0)
		, image_id(0)
	{}

};
