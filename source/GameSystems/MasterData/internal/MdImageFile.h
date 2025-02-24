#pragma once
#include "MasterDataBase.h"

/// <summary>
/// 画像ファイル
/// </summary>
struct MdImageFile : public MasterData<MdImageFile>
{
	MasterDataID id;
	int width;
	int height;
	std::string path;

	std::tuple<MasterDataID&, int&, int&, std::string&> GetMembers()
	{
		return std::tie(id, width, height, path);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	static int GetHeight(const MasterDataID key) { return Get(key).height; }
	static int GetWidth(const MasterDataID key) { return Get(key).width; }
	static const std::string& GetPath(const MasterDataID key) { return Get(key).path; }

	MdImageFile()
		: id(0)
		, height(0)
		, width(0)
		, path("")
	{}
};
