#include "WalkingEnemy.h"
#include "Component/Renderer/Animator/WalkingEnemyAnimatorComponent.h"
#include "Scene/SceneBase.h"

namespace
{

	constexpr AnimPlayInfo WalkingEnemyAnimPlayInfo_Idle{ MasterDataID(47), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo WalkingEnemyAnimPlayInfo_Walk{ MasterDataID(48), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo WalkingEnemyAnimPlayInfo_Damaged{ MasterDataID(49), 1.f, 1.5f, FALSE, FALSE };

	enum WalkingEnemyAnimStateID : uint8_t
	{
		WalkingEnemyAnimStateID_Idle,
		WalkingEnemyAnimStateID_Walk,
	};

	uint8_t WalkingEnemyAnimStateUpdate_Idle(WalkingEnemyAnimatorComponent* animator)
	{
		if (animator->_velocity.x != 0.f)
		{
			return WalkingEnemyAnimStateID_Walk;
		}

		return WalkingEnemyAnimStateID_Idle;
	}

	uint8_t WalkingEnemyAnimStateUpdate_Walk(WalkingEnemyAnimatorComponent* animator)
	{
		constexpr float threshold = 10.f;
		if (fabsf(animator->_velocity.x) < threshold)
		{
			return WalkingEnemyAnimStateID_Idle;
		}

		return WalkingEnemyAnimStateID_Walk;
	}
}

WalkingEnemy::WalkingEnemy()
{}

WalkingEnemy::~WalkingEnemy()
{
}

void WalkingEnemy::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);
}

void WalkingEnemy::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);
}

void WalkingEnemy::Finalize()
{
	__super::Finalize();
}

void WalkingEnemy::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	const CollisionObjectType hit_obj_type = hit_result.other_collider->GetCollisionObjectType();
	
	const float abs_normal_x = fabsf(hit_result.normal_from_other.x);
	if
	(
		hit_result.self_collider == GetBodyCollider() &&
		hit_result.collision_type == CollisionType::BLOCK &&
		(hit_obj_type == CollisionObjectType::GROUND || hit_obj_type == CollisionObjectType::BARRIER) &&
		abs_normal_x > 1.f - EPSIRON
	)
	{
		const Direction new_facing_dir = hit_result.normal_from_other.x > 0.f ? Direction::RIGHT : Direction::LEFT;
		SetFacingDirection(new_facing_dir);
	}
}

void WalkingEnemy::TakeDamage(const DamageInfo& damage_info)
{
	__super::TakeDamage(damage_info);

	if(damage_info.damage_type == DamageType::Punch)
	{
		constexpr float DAMAGED_TIME = 1.f;
		GetAnimRenderer()->SetOverrideAnimation(WalkingEnemyAnimPlayInfo_Damaged, DAMAGED_TIME);
		SetMovementInputEnabled(false);
		GetScene()->MakeDelayedEventWorld(this, 1.f, [this]()
			{
				if(!IsDead())
				{
					SetMovementInputEnabled(true);
				}
			}
		);
	}
}

void WalkingEnemy::OnDead(const CharacterDeathInfo* death_info)
{
	__super::OnDead(death_info);

	GetAnimRenderer()->SetOverrideAnimation(WalkingEnemyAnimPlayInfo_Damaged, 0.f);
}

void WalkingEnemy::TickEnemy(const float delta_seconds)
{
	__super::TickEnemy(delta_seconds);

	if (!IsFacingAbyss())
	{
		AddMovementInput(GetFacingDirectionAsVector2D());
	}
	else if(!IsFalling())
	{
		SetFacingDirection(GetFacingDirection() == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
	}
}

AnimRendererComponent* WalkingEnemy::CreateAnimRenderer()
{
	WalkingEnemyAnimatorComponent* animator = CreateComponent<WalkingEnemyAnimatorComponent>(this);
	using WalkingEnemyAnimState = AnimState<WalkingEnemyAnimatorComponent>;
	const std::unordered_map<uint8_t, WalkingEnemyAnimState> anim_state_mapping
	{
		{WalkingEnemyAnimStateID_Idle, WalkingEnemyAnimState(WalkingEnemyAnimPlayInfo_Idle, WalkingEnemyAnimStateUpdate_Idle)},
		{WalkingEnemyAnimStateID_Walk, WalkingEnemyAnimState(WalkingEnemyAnimPlayInfo_Walk, WalkingEnemyAnimStateUpdate_Walk)},
	};
	animator->SetAnimStateMapping(anim_state_mapping, WalkingEnemyAnimStateID_Idle);

	animator->SetLocalPosition(Vector2D{ 0.f, -10.f });

	return animator;
}

Vector2D WalkingEnemy::GetBodySize() const
{
	return Vector2D{ UNIT_TILE_SIZE, 50 };
}