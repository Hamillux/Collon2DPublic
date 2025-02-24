#pragma once
#include "EntityType.h"
#include <type_traits>

class Actor;

/// <summary>
/// Actor派生クラスの初期化パラメータ型を関連付けるためのトレイト構造体
/// <para>各Actor派生クラスに対応する初期化パラメータ型を定義するために使用される</para>
/// <para>Actor派生クラスごとに特殊化して、typeメンバーとして初期化パラメータ型を指定する必要がある</para>
/// <para>特殊化はActor派生クラスの.hファイルに記述すること</para>
/// </summary>
template<class ActorDerived>
struct initial_params_of_actor
{
	static_assert(std::is_base_of<Actor, ActorDerived>::value, "T is not a derived class of Actor");
	static_assert(!std::is_base_of<Actor, ActorDerived>::value, "Not found 'initial_params_of_actor' specialization for T");
};
template<class ActorDerived> using initial_params_of_actor_t = typename initial_params_of_actor<ActorDerived>::type;