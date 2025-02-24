#define DEFINE_ENUM()

// 基底型が指定されているシンプルなパターン
DEFINE_ENUM()
enum class MyEnumA : unsigned short
{
    A, B, C
};

// 基底型が指定されていないシンプルなパターン
DEFINE_ENUM()
enum class MyEnumB
{
    X, Y, Z
};

// 列挙子に値が指定されているパターン
DEFINE_ENUM()
enum class MyEnumC
{
    Alpha = 1,
    Beta = 2,
    Gamma = 3
};

// 列挙子が改行で区切られているパターン
DEFINE_ENUM()
enum class MyEnumD : int
{
    First,
    Second,
    Third
};

// 列挙子にコメントが入っているパターン
DEFINE_ENUM()
enum class MyEnumE
{
    Red,    // 赤
    Green,  // 緑
    Blue    // 青
};

// 改行や空白が多く含まれる複雑なパターン
DEFINE_ENUM()
enum class MyEnumF : long long
{
    Start   = 1000,   // 開始値
    Middle  = 5000,   // 中間値
    End     = 10000   // 終了値
};

// 基底型がunsigned intで、1つの列挙子に値が指定されているパターン
DEFINE_ENUM()
enum class MyEnumG : unsigned int
{
    X1 = 10,
    X2,     // X2 = 11
    X3 = 100,
    X4      // X4 = 101
};

// 別の型を使用するパターン
DEFINE_ENUM()
enum class MyEnumH : char
{
    A = 'A',
    B = 'B',
    C = 'C'
};

// 列挙子に大きな値が指定されているパターン
DEFINE_ENUM()
enum class MyEnumI : unsigned long long
{
    First = 1000000000000,
    Second,
    Third = 9999999999999,
};

DEFINE_ENUM()
enum class
    MyEnumJ : short
{
    A,
    B,
    C,
    D = 9,
}   
 ;

DEFINE_ENUM
     (
     )
     enum class MyEnumK : unsigned long long{
A, B, C, D, E
     };

DEFINE_ENUM()
     enum class MyEnumL : unsigned long long{
A, B, C, D, E
     };


// DEFINE_ENUM()とenum classの間にコメントがあるパターン
DEFINE_ENUM()
/// <summary>
/// 
/// </summary>
enum class EditParamType
{
	INT, INT2, INT3, INT4,				// ImGui::InputIntN()
	FLOAT, FLOAT2, FLOAT3, FLOAT4,		// ImGui::InputFloatN()
	BOOL,								// ImGui::Checkbox()
	STRING,								// ImGui::InputText()
	BLOCK_SKIN,							// BlockSkinModal
};

#pragma once
#include "Utility/Core/EnumInfo.h"

DEFINE_ENUM()
/// <summary>
/// ステージに配置可能なブロック, Mob等の総称をエンティティとする
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