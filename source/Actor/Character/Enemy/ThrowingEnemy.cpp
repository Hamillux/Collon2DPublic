#include "ThrowingEnemy.h"
#include "Component/Renderer/Animator/ThrowingEnemyAnimatorComponent.h"

namespace
{
	constexpr AnimPlayInfo ThrowingEnemyAnimPlayInfo_Idle{ MasterDataID(52), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo ThrowingEnemyAnimPlayInfo_Walk{ MasterDataID(53), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo ThrowingEnemyAnimPlayInfo_Throw{ MasterDataID(54), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo ThrowingEnemyAnimPlayInfo_Damaged{ MasterDataID(55), 1.f, 1.5f, FALSE, FALSE };

	enum ThrowingEnemyAnimStateID : uint8_t
	{
		ThrowingEnemyAnimStateID_Idle,
		ThrowingEnemyAnimStateID_Walk,
		ThrowingEnemyAnimStateID_Throw,
		ThrowingEnemyAnimStateID_Damaged,
	};

	uint8_t ThrowingEnemyAnimStateUpdate_Idle(ThrowingEnemyAnimatorComponent* animator)
	{
		return ThrowingEnemyAnimStateID_Idle;
	}

	uint8_t ThrowingEnemyAnimStateUpdate_Walk(ThrowingEnemyAnimatorComponent* animator)
	{
		return ThrowingEnemyAnimStateID_Walk;
	}

	uint8_t ThrowingEnemyAnimStateUpdate_Throw(ThrowingEnemyAnimatorComponent* animator)
	{
		return ThrowingEnemyAnimStateID_Throw;
	}

	uint8_t ThrowingEnemyAnimStateUpdate_Damaged(ThrowingEnemyAnimatorComponent* animator)
	{
		return ThrowingEnemyAnimStateID_Damaged;
	}
}

ThrowingEnemy::ThrowingEnemy()
{
}

ThrowingEnemy::~ThrowingEnemy()
{
}

AnimRendererComponent* ThrowingEnemy::CreateAnimRenderer()
{
	ThrowingEnemyAnimatorComponent* animator = CreateComponent<ThrowingEnemyAnimatorComponent>(this);
	using ThrowingEnemyAnimState = AnimState<ThrowingEnemyAnimatorComponent>;
	const std::unordered_map<uint8_t, ThrowingEnemyAnimState> anim_state_mapping
	{
		{ThrowingEnemyAnimStateID_Idle, ThrowingEnemyAnimState(ThrowingEnemyAnimPlayInfo_Idle, ThrowingEnemyAnimStateUpdate_Idle)},
		{ThrowingEnemyAnimStateID_Walk, ThrowingEnemyAnimState(ThrowingEnemyAnimPlayInfo_Walk, ThrowingEnemyAnimStateUpdate_Walk)},
		{ThrowingEnemyAnimStateID_Throw, ThrowingEnemyAnimState(ThrowingEnemyAnimPlayInfo_Throw, ThrowingEnemyAnimStateUpdate_Throw)},
		{ThrowingEnemyAnimStateID_Damaged, ThrowingEnemyAnimState(ThrowingEnemyAnimPlayInfo_Damaged, ThrowingEnemyAnimStateUpdate_Damaged)},
	};

	animator->SetAnimStateMapping(anim_state_mapping, ThrowingEnemyAnimStateID_Idle);

	animator->SetLocalPosition(Vector2D{ 0.f, -10.f });

	return animator;
}

Vector2D ThrowingEnemy::GetBodySize() const
{
    return Vector2D{ UNIT_TILE_SIZE, 50 };
}
