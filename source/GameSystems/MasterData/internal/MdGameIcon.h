#pragma once
#include "MasterDataBase.h"

/// <summary>
/// 
/// </summary>
struct MdGameIcon : public MasterData<MdGameIcon>
{
	MasterDataID id;
	MasterDataID image_id;
	int left;
	int top;
	uint32_t width;
	uint32_t height;

	// メンバ変数への参照をタプルで返す関数
	std::tuple<
		MasterDataID&,
		MasterDataID&,
		int&,
		int&,
		uint32_t&,
		uint32_t&
	> GetMembers()
	{
		return std::tie(
			id,
			image_id,
			left,
			top,
			width,
			height
		);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	// コンストラクタ
	MdGameIcon()
		: id(0)
		, image_id(0)
		, left(0)
		, top(0)
		, width(0)
		, height(0)
	{}

	void CalcUV(float& u_0, float& v_0, float& u_1, float& v_1) const;
};