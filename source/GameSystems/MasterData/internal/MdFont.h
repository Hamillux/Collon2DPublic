#pragma once
#include "MasterDataBase.h"

/// <summary>
/// フォントのマスターデータ
/// </summary>
struct MdFont : public MasterData<MdFont>
{
	MasterDataID id;
	int font_size;
	std::string ttf_path;

	// メンバ変数への参照をタプルで返す関数
	auto GetMembers()
	{
		return std::tie(
			id,
			font_size,
			ttf_path
		);
	};

	// マップキーを返す関数
	MasterDataID GetMapKey() const
	{
		return id;
	}

	static const char* GetTTFPath(const MasterDataID font_id)
	{
		return Get(font_id).ttf_path.c_str();
	}

	// コンストラクタ
	MdFont()
		: id(0)
		, font_size(0)
		, ttf_path("")
	{}
};