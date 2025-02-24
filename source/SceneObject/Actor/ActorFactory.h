#pragma once
#include "GameSystems/GameObjectManager.h"
#include <memory>
#include "EntityType.h"
#include "ActorInitialParams.h"
#include "ActorTraits.h"
#include <unordered_map>
#include <functional>
#include <type_traits>

class Actor;
class SceneBase;

class ActorFactory
{
public:
	template<class T>
	static T* CreateAndInitializeActor(const ActorInitialParams* const actor_params, SceneBase* const owner_scene);

	static Actor* CreateAndInitializeActorByEntityType(const EEntityType entity_type, const ActorInitialParams* const initial_params, SceneBase* owner_scene);

	/// <summary>
	/// entity_typeに対応するアクターの初期化パラメータをデフォルトコンストラクタで生成する
	/// </summary>
	/// <param name="entity_type"></param>
	/// <returns></returns>
	static std::shared_ptr<ActorInitialParams> CreateInitialParamsByEntityType(const EEntityType entity_type);

	static void DestroyActor(Actor*& actor);

private:
	static void InitializeActor(Actor* actor, const ActorInitialParams* const params, SceneBase* owner_scene);
};

template<class T>
inline T* ActorFactory::CreateAndInitializeActor(const ActorInitialParams* const actor_params, SceneBase* owner_scene)
{
	static_assert(std::is_base_of<Actor, T>::value, "T is not a derived class of Actor");

	T* new_actor = GameObjectManager::GetInstance().CreateObject<T>();
	InitializeActor(new_actor, actor_params, owner_scene);
	return new_actor;
}
