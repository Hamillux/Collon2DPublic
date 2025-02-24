#pragma once

#include "MasterDataBase.h"
#include <functional>

struct ItemCallbackArgs {
	class InGameScene* world;
	class Player* player;
};

struct MdItem : public MasterData<MdItem>
{
	MasterDataID item_id;
	std::string item_name;
	MasterDataID icon_id;
	int score;
	std::string on_used_action_name;	// 使用時のアクション名

	auto GetMembers()
	{
		return std::tie(item_id, item_name, icon_id, score, on_used_action_name);
	};

	MasterDataID GetMapKey() const
	{
		return item_id;
	};

	std::function<void(const ItemCallbackArgs&)> GetUseAction() const
	{
		return item_use_actions.at(on_used_action_name);
	}

	static std::unordered_map<std::string, std::function<void(const ItemCallbackArgs&)>> item_use_actions;
};