#pragma once

#include "MasterDataBase.h"

struct MdStageBGM : public MasterData<MdStageBGM>
{
	MasterDataID id;
	std::string title;
	std::string file_path;

	std::tuple<MasterDataID&, std::string&, std::string&> GetMembers()
	{
		return std::tie(id, title, file_path);
	};

	MasterDataID GetMapKey() const
	{
		return id;
	}
};