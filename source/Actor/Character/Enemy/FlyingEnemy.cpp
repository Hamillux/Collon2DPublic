#include "FlyingEnemy.h"
#include "Component/Renderer/Animator/FlyingEnemyAnimatorComponent.h"
#include "Component/CharacterMovementComponent.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include "Actor/Character/Player/Player.h"

namespace
{
	constexpr AnimPlayInfo FlyingEnemyAnimPlayInfo_Idle{ MasterDataID(50), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo FlyingEnemyAnimPlayInfo_Move{ MasterDataID(51), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo FlyingEnemyAnimPlayInfo_Damaged{ MasterDataID(49), 1.f, 1.5f, FALSE, FALSE };

	enum FlyingEnemyAnimStateID : uint8_t
	{
		FlyingEnemyAnimStateID_Idle,
		FlyingEnemyAnimStateID_Move,
	};

	uint8_t FlyingEnemyAnimStateUpdate_Idle(FlyingEnemyAnimatorComponent* animator)
	{
		return FlyingEnemyAnimStateID_Move;
	}

	uint8_t FlyingEnemyAnimStateUpdate_Move(FlyingEnemyAnimatorComponent* animator)
	{
		return FlyingEnemyAnimStateID_Move;
	}
}

namespace
{
	constexpr float DETINATION_UPDATE_INTERVAL = 0.5f;	// 移動目標の更新間隔
}

FlyingEnemy::FlyingEnemy()
	: _destination(nullptr)
	, _destination_update_time(0.f)
{
}

FlyingEnemy::~FlyingEnemy()
{
}

void FlyingEnemy::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	_destination_update_time = GetScene()->GetWorldTime() + DETINATION_UPDATE_INTERVAL;

	CharacterMovementComponent* movement = GetCharacterMovementComponent();
	movement->SetMovementMode(CharacterMovementMode::Flying);
	movement->_max_fly_speed = 100.f;
	
}

void FlyingEnemy::Finalize()
{
	_destination.reset();

	__super::Finalize();
}

void FlyingEnemy::TakeDamage(const DamageInfo& damage_info)
{
	__super::TakeDamage(damage_info);

	if (damage_info.damage_type == DamageType::Punch)
	{
		constexpr float DAMAGED_TIME = 1.f;
		GetAnimRenderer()->SetOverrideAnimation(FlyingEnemyAnimPlayInfo_Damaged, DAMAGED_TIME);
		SetMovementInputEnabled(false);
		GetScene()->MakeDelayedEventWorld(this, 1.f, [this]()
			{
				if (!IsDead())
				{
					SetMovementInputEnabled(true);
				}
			}
		);
	}
}

void FlyingEnemy::OnDead(const CharacterDeathInfo* death_info)
{
	__super::OnDead(death_info);

	GetAnimRenderer()->SetOverrideAnimation(FlyingEnemyAnimPlayInfo_Damaged, 0.f);
}

std::vector<CollisionObjectType> FlyingEnemy::GetHitTargetTypes() const
{
	return { CollisionObjectType::PLAYER, CollisionObjectType::DAMAGE, CollisionObjectType::BARRIER };
}

void FlyingEnemy::TickEnemy(const float delta_seconds)
{
	__super::TickEnemy(delta_seconds);

	if (!_ingame_scene_ref)
	{
		return;
	}

	if (_destination == nullptr)
	{
		_destination = std::make_unique<Vector2D>(_ingame_scene_ref->GetPlayerRef()->GetBodyCollider()->GetWorldPosition());
	}
	else
	{
		const Vector2D move_dir = (*_destination - GetActorWorldPosition()).Normalize();
		AddMovementInput(move_dir);

		const float _current_time = GetScene()->GetWorldTime();
		Player* player_ref = _ingame_scene_ref->GetPlayerRef();
		if (IsValid(player_ref) && GetScene()->GetWorldTime() >= _destination_update_time)
		{
			_destination_update_time += DETINATION_UPDATE_INTERVAL;
			*_destination = player_ref->GetBodyCollider()->GetWorldPosition();
		}
	}
	
}

AnimRendererComponent* FlyingEnemy::CreateAnimRenderer()
{
	FlyingEnemyAnimatorComponent* animator = CreateComponent<FlyingEnemyAnimatorComponent>(this);
	using FlyingEnemyAnimState = AnimState<FlyingEnemyAnimatorComponent>;
	const std::unordered_map<uint8_t, FlyingEnemyAnimState> anim_state_mapping
	{
		{FlyingEnemyAnimStateID_Idle, FlyingEnemyAnimState(FlyingEnemyAnimPlayInfo_Idle, FlyingEnemyAnimStateUpdate_Idle)},
		{FlyingEnemyAnimStateID_Move, FlyingEnemyAnimState(FlyingEnemyAnimPlayInfo_Move, FlyingEnemyAnimStateUpdate_Move)},
	};

	animator->SetAnimStateMapping(anim_state_mapping, FlyingEnemyAnimStateID_Idle);

	animator->SetLocalPosition(Vector2D{ 0.f, -10.f });

	return animator;
}

Vector2D FlyingEnemy::GetBodySize() const
{
    return Vector2D{ UNIT_TILE_SIZE, 50 };
}

