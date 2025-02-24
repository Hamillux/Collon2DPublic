#pragma once
#include "EntityType.h"
#include <type_traits>

class Actor;

/// <summary>
/// Actor派生クラスと1対1対応する初期化パラメータクラス.
/// 特殊化はActor派生クラスの.hに書く
/// </summary>
template<class ActorDerived>
struct initial_params_of_actor
{
	static_assert(std::is_base_of<Actor, ActorDerived>::value, "T is not a derived class of Actor");
	static_assert(!std::is_base_of<Actor, ActorDerived>::value, "Not found 'initial_params_of_actor' specialization for T");
};
template<class ActorDerived> using initial_params_of_actor_t = typename initial_params_of_actor<ActorDerived>::type;