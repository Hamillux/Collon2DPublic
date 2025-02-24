#pragma once

#include "Core.h"
#include "Utility/IJsonSerializable.h"
#include <type_traits>

template<typename GameConfigItemDerived>
struct GameConfigItem : public IJsonObject
{
	GameConfigItem() {}
	virtual ~GameConfigItem() {}

	using KeyType = const char*;
};