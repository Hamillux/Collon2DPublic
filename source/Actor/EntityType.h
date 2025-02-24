#pragma once
#include "Utility/Core/EnumInfo.h"

DEFINE_ENUM()
/// <summary>
/// ステージに配置可能なブロック, Mob等の総称をエンティティとする. Actorクラス->エンティティは1対多の関係.
/// </summary>
enum class EEntityType
{
	None,
	Actor,
	WalkingEnemy,
	FlyingEnemy,
	TacklingEnemy,
	ThrowingEnemy,
	Player,
	RectangleBlock,
	SlopeBlock,
	SlopeBlock2,	// 縦横比1:2のスロープブロック
	GoalFlag,
	Coin,
	ItemActor,
	CrackedBrick,
};

// TODO: EntityType列挙子を追加したら以下のファイルを更新する.
// - ActorFactory.cpp
// - EntityTraits.h

DEFINE_ENUM()
enum class EEntityCategory
{
	None,
	Character,
	Block,
	Gimmick,
	Item
};