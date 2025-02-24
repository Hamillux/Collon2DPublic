#pragma once
#include "EntityType.h"
#include "ActorTraits.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include <memory>

///////////////////////
// プロトタイプ宣言
///////////////////////
class Actor;
class WalkingEnemy;
class FlyingEnemy;
class TacklingEnemy;
class ThrowingEnemy;
class Player;
class RectangleBlock;
class SlopeBlock;
class GoalFlag;
class Coin;
class ItemActor;
class InvincibleDrink;
class PicoHammer;
class RecoveryDrink;
class CrackedBrick;
// TODO: 全Actor派生クラスのプロトタイプ宣言

struct ActorInitialParams;

/// <summary>
/// EntityTypeの列挙子に対応するActor派生クラス
/// 特殊化はActorTraitsSpecializations.h
/// </summary>
/// <typeparam name="ET"></typeparam>
template<EEntityType ET>
struct matching_actor_of_entitytype
{};

template<EEntityType ET> using matching_actor_of_entitytype_t = typename matching_actor_of_entitytype<ET>::type;
template<EEntityType ET> using initial_params_of_entitytype_t = initial_params_of_actor_t<matching_actor_of_entitytype_t<ET>>;

template<> struct matching_actor_of_entitytype<EEntityType::Actor> { using type = Actor; };
template<> struct matching_actor_of_entitytype<EEntityType::WalkingEnemy> { using type = WalkingEnemy; };
template<> struct matching_actor_of_entitytype<EEntityType::FlyingEnemy> { using type = FlyingEnemy; };
template<> struct matching_actor_of_entitytype<EEntityType::TacklingEnemy> { using type = TacklingEnemy; };
template<> struct matching_actor_of_entitytype<EEntityType::ThrowingEnemy> { using type = ThrowingEnemy; };
template<> struct matching_actor_of_entitytype<EEntityType::Player> { using type = Player; };
template<> struct matching_actor_of_entitytype<EEntityType::RectangleBlock> { using type = RectangleBlock; };
template<> struct matching_actor_of_entitytype<EEntityType::SlopeBlock> { using type = SlopeBlock; };
template<> struct matching_actor_of_entitytype<EEntityType::SlopeBlock2> { using type = SlopeBlock; };
template<> struct matching_actor_of_entitytype<EEntityType::GoalFlag> { using type = GoalFlag; };
template<> struct matching_actor_of_entitytype<EEntityType::Coin> { using type = Coin; };
template<> struct matching_actor_of_entitytype<EEntityType::ItemActor> { using type = ItemActor; };
template<> struct matching_actor_of_entitytype<EEntityType::CrackedBrick> { using type = CrackedBrick; };
// TODO: 有効な列挙子全てをここに