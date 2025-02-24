#pragma once

#include <stdint.h>
#include <type_traits>

/// <summary>
/// 衝突の種類
/// </summary>
enum class CollisionType : uint8_t
{
	NONE,
	OVERLAP,
	BLOCK,
};

/// <summary>
/// コライダーの分類. コライダーに自身のCollisionObjectTypeと, 衝突判定を行う相手のCollisionObjectTypeを設定することで衝突対象を指定する.
/// <para>ある1組の2コライダー間の衝突判定を行う条件は, 両コライダーが互いを衝突対象に設定している事</para>
/// <para>WILDCARDは他のどの分類としても解釈可能.</para>
/// <para>*自身と衝突対象を両方WILDCARDに設定すれば, 自身以外のすべてのコライダーとの衝突がチェックされる.</para>
/// <para>*自身のみWILDCARDにすれば, 自身"が"衝突対象にしている全てのコライダーとの衝突がチェックされる</para>
/// <para>*衝突相手のみWILDCARDにすれば, 自身"を"衝突対象にしている全てのコライダーとの衝突がチェックされる</para>
/// </summary>
enum class CollisionObjectType : uint32_t
{
	NONE = 0,
	GROUND = 1,
	PLAYER = 2,
	ENEMY = 4,
	ITEM = 8,
	DAMAGE = 16,
	GIMMICK = 32,
	BARRIER = 64,
	GOAL_FLAG = 128,
	WILDCARD = 0xFFFFFFFF,
};

using CollisionObjectType_UnderlyingType = std::underlying_type_t<CollisionObjectType>;