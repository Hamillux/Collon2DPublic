#include "MdItem.h"
#include "Actor/Character/Player/Player.h"

namespace
{
	void OnUsed_HealingDrink(const ItemCallbackArgs& args)
	{
		args.player->SetHP(args.player->GetMaxHP());
		// TODO: 効果音, ビジュアルエフェクト
	}

	void OnUsed_PicoHammer(const ItemCallbackArgs& args)
	{
		args.player->EquipHammer();
	}

	void OnUsed_EnergyDrink(const ItemCallbackArgs& args)
	{
		args.player->SetSP(args.player->GetMaxSP());
	}
}


std::unordered_map<std::string, std::function<void(const ItemCallbackArgs&)>> MdItem::item_use_actions =
{
	// TODO: アイテムの効果をスクリプト化する
	{"OnUsed_HealingDrink", OnUsed_HealingDrink},
	{"OnUsed_PicoHammer", OnUsed_PicoHammer},
	{"OnUsed_EnergyDrink", OnUsed_EnergyDrink}
};