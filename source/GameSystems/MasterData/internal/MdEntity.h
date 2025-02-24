#pragma once
#include "MasterDataBase.h"
#include "Actor/EntityType.h"

/// <summary>
/// エンティティ
/// </summary>
struct MdEntity: public MasterData<MdEntity>
{
	MasterDataID id;						// エンティティID
	std::string entity_type_str;			// 内部名
	std::string display_name;				// 表示名
	std::string category_str;					// エンティティカテゴリ
	MasterDataID icon_id;					// アイコンID. StageEditorSceneで使用
	std::string initial_params_json_name;	// エンティティ別に定義された初期化パラメータのJSONファイル名

	auto GetMembers()
	{
		return std::tie(
			id,
			entity_type_str,
			display_name,
			category_str,
			icon_id,
			initial_params_json_name
		);
	}

	MasterDataID GetMapKey() const
	{
		return id;
	}

	static void CountEntitiesInCategory(std::unordered_map<EEntityCategory, size_t>& out_counts);

	// コンストラクタ
	MdEntity()
		: id(0)
		, icon_id(0)
	{}

};