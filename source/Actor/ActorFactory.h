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
	/// <summary>
	/// アクターの生成と初期化を行う
	/// </summary>
	template<class ActorDerived>
	static ActorDerived* CreateAndInitializeActor(const ActorInitialParams* const initial_params, SceneBase* const owner_scene);

	/// <summary>
	/// entity_typeに対応するアクターを生成し初期化する
	/// </summary>
	static Actor* CreateAndInitializeActorByEntityType(const EEntityType entity_type, const ActorInitialParams* const initial_params, SceneBase* owner_scene);

	/// <summary>
	/// entity_typeに対応するアクターの初期化パラメータをデフォルトコンストラクタで生成する
	/// </summary>
	/// <param name="entity_type"></param>
	/// <returns></returns>
	static std::shared_ptr<ActorInitialParams> CreateInitialParamsByEntityType(const EEntityType entity_type);

	/// <summary>
	/// アクターを破棄する
	/// </summary>
	static void DestroyActor(Actor*& actor);

private:
	static void InitializeActor(Actor* actor, const ActorInitialParams* const params, SceneBase* owner_scene);
};

template<class ActorDerived>
inline ActorDerived* ActorFactory::CreateAndInitializeActor(const ActorInitialParams* const actor_params, SceneBase* owner_scene)
{
	static_assert(std::is_base_of<Actor, ActorDerived>::value, "ActorDerived is not a derived class of Actor");

	ActorDerived* new_actor = GameObjectManager::GetInstance().CreateObject<ActorDerived>();
	InitializeActor(new_actor, actor_params, owner_scene);
	return new_actor;
}
