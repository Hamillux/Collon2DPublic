#pragma once
#include "Utility/SingletonBase.h"
#include <type_traits>
#include <unordered_set>
#include <cassert>

class GameObject;

/// <summary>
/// GameObjectの生成・破棄を行うクラス
/// </summary>
class GameObjectManager : public Singleton<GameObjectManager>
{
public:

	/// <summary>
	/// GameObjectインスタンスを生成する. 戻り値は常に有効
	/// </summary>
	/// <typeparam name="GameObjectDerived">GameObject派生クラス</typeparam>
	template<class GameObjectDerived>
	GameObjectDerived* CreateObject()
	{
		static_assert(std::is_base_of_v<GameObject, GameObjectDerived>, "GameObjectDerived must be derived from GameObject");
		GameObjectDerived* new_object = new GameObjectDerived();
		assert(new_object != nullptr);
		_objects.insert(new_object);
		return new_object;
	}

	template<class GameObjectDerived>
	void DestroyObject(GameObjectDerived*& object)
	{
		static_assert(std::is_base_of_v<GameObject, GameObjectDerived>, "GameObjectDerived must be derived from GameObject");
		_objects.erase(object);
		delete object;
		object = nullptr;
	}

	bool IsValid(const GameObject* const object) const
	{
		return _objects.find(object) != _objects.end();
	}

private:
	std::unordered_set<const GameObject*> _objects;
};
