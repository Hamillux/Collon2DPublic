#pragma once

#include "Core.h"
#include "Utility/IJsonSerializable.h"
#include <type_traits>

/// <summary>
/// ゲーム設定項目の基底クラス. CRTP.
/// </summary>
/// <typeparam name="GameConfigItemDerived"></typeparam>
template<typename GameConfigItemDerived>
struct GameConfigItem : public IJsonObject
{
	GameConfigItem() {}
	virtual ~GameConfigItem() {}

	using KeyType = const char*;
	static KeyType GetConfigKey() 
	{
		// GameConfigItemDerivedにconfig_keyの定義を要求する
		return GameConfigItemDerived::config_key;
	}
};