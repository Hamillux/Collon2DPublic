#pragma once
#include "MasterDataBase.h"
#include <string>

struct MdParticle : public MasterData<MdParticle>
{
	MasterDataID id;
	std::string json_name;

	auto GetMembers()
	{
		return std::tie(
			id,
			json_name
		);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	// コンストラクタ
	MdParticle()
		: id(0)
	{}

};
