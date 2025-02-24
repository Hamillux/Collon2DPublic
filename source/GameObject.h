#pragma once

#include "Core.h"
#include <type_traits>

/// <summary>
/// SceneBase, Actor, ComponentBaseの基底クラス
/// </summary>
class GameObject : public EventListener
{
public:
	GameObject();
	virtual ~GameObject();

	// コピー禁止
	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	template<typename GameObjectDerived>
	bool IsDerivedFrom()
	{
		static_assert(std::is_base_of_v<GameObject, GameObjectDerived>, "GameObjectDerived must be derived from GameObject");
		return dynamic_cast<GameObjectDerived*>(this) != nullptr;
	}

protected:
	static bool IsValid(const GameObject* const game_object);

};