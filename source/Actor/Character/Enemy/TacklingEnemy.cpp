#include "TacklingEnemy.h"
#include "Component/Renderer/Animator/TacklingEnemyAnimatorComponent.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include "Actor/Character/Player/Player.h"
#include "Component/CharacterMovementComponent.h"

namespace 
{
	constexpr AnimPlayInfo TacklingEnemyAnimPlayInfo_Idle{ MasterDataID(56), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo TacklingEnemyAnimPlayInfo_Walk{ MasterDataID(57), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo TacklingEnemyAnimPlayInfo_Tackle{ MasterDataID(58), 1.f, 1.5f, FALSE, FALSE };
	constexpr AnimPlayInfo TacklingEnemyAnimPlayInfo_Damaged{ MasterDataID(59), 1.f, 1.5f, FALSE, FALSE };
	enum TacklingEnemyAnimStateID : uint8_t
	{
		TacklingEnemyAnimStateID_Idle,
		TacklingEnemyAnimStateID_Walk,
		TacklingEnemyAnimStateID_Tackle,
	};

	uint8_t TacklingEnemyAnimStateUpdate_Idle(TacklingEnemyAnimatorComponent* animator)
	{
		if (animator->_tackling_enemy_ref->GetTacklingEnemyMoveState() == TacklingEnemyMoveState::Tackling)
		{
			return TacklingEnemyAnimStateID_Tackle;
		}

		return TacklingEnemyAnimStateID_Idle;
	}

	uint8_t TacklingEnemyAnimStateUpdate_Walk(TacklingEnemyAnimatorComponent* animator)
	{
		return TacklingEnemyAnimStateID_Walk;
	}

	uint8_t TacklingEnemyAnimStateUpdate_Tackle(TacklingEnemyAnimatorComponent* animator)
	{
		if (animator->_tackling_enemy_ref->GetTacklingEnemyMoveState() == TacklingEnemyMoveState::Idle)
		{
			return TacklingEnemyAnimStateID_Idle;
		}
		return TacklingEnemyAnimStateID_Tackle;
	}
}

namespace
{
	constexpr float FLIP_INTERVAL = 2.f;
	constexpr float TACKLE_CANCEL_TIME = 1.f;	// プレイヤーを見失った後, この時間経過でタックルをキャンセル
	constexpr float TACKLE_BEGIN_TIME = 1.f;    // プレイヤー発見からタックルで動き始めるまでの時間
}

TacklingEnemy::TacklingEnemy()
	: _move_state(TacklingEnemyMoveState::Idle)
	, _player_lost_time(0.f)
{
}

TacklingEnemy::~TacklingEnemy()
{
}

void TacklingEnemy::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	_next_flip_time = GetScene()->GetWorldTime() + FLIP_INTERVAL;
	_default_walk_speed = GetCharacterMovementComponent()->_max_walk_speed;
	_tackling_speed = _default_walk_speed * 1.5f;
}

void TacklingEnemy::Finalize()
{
	_move_state = TacklingEnemyMoveState::Idle;
	_player_lost_time = 0.f;

	__super::Finalize();
}

void TacklingEnemy::OnHitCollision(const HitResult& hit_result)
{
	if (GetScene()->GetSceneType() != SceneType::INGAME_SCENE)
	{
		return;
	}

	if(_move_state == TacklingEnemyMoveState::Tackling)
	{
		DamageInfo tackle_damage_info{};
		tackle_damage_info.damage_causer = this;
		tackle_damage_info.damage_type = DamageType::Crush;
		tackle_damage_info.damage_value = GetPlayerHitDamage() * 2; // TODO: パラメータ化

		Player* player_ref = _ingame_scene_ref->GetPlayerRef();
		CharacterMovementComponent* player_movement = player_ref->GetCharacterMovementComponent();
		if (IsValid(player_ref) && hit_result.other_collider == player_ref->GetBodyCollider())
		{
			const float max_walk_speed = GetCharacterMovementComponent()->_max_walk_speed;

			// TODO: パラメータ化
			const float knock_back_velocity_x = GetFacingDirectionAsVector2D().x * max_walk_speed * 0.5f;
			const float knock_back_velocity_y = -400;

			player_ref->KnockBack(player_movement->GetImpulseToAddVelocity(Vector2D{knock_back_velocity_x, knock_back_velocity_y}));
			player_ref->ApplyDamage(tackle_damage_info);
		}
		else if (hit_result.other_collider->GetOwnerActor() != player_ref)
		{
			hit_result.other_collider->GetOwnerActor()->ApplyDamage(tackle_damage_info);
		}
	}
	else
	{
		__super::OnHitCollision(hit_result);
	}
}

void TacklingEnemy::TakeDamage(const DamageInfo& damage_info)
{
	if (_move_state == TacklingEnemyMoveState::Tackling)
	{
		return;
	}

	if (damage_info.damage_type == DamageType::Stomp)
	{
		return;
	}

	__super::TakeDamage(damage_info);

	GetAnimRenderer()->SetOverrideAnimation(TacklingEnemyAnimPlayInfo_Damaged, 1.f);
}

void TacklingEnemy::OnDead(const CharacterDeathInfo* death_info)
{
	__super::OnDead(death_info);

	GetAnimRenderer()->SetOverrideAnimation(TacklingEnemyAnimPlayInfo_Damaged, 0.f);
}

void TacklingEnemy::TickEnemy(const float delta_seconds)
{
	__super::TickEnemy(delta_seconds);

	switch (_move_state)
	{
	case TacklingEnemyMoveState::Idle:
	{
		if (GetScene()->GetWorldTime() >= _next_flip_time)
		{
			_next_flip_time += FLIP_INTERVAL;
			SetFacingDirection(GetFacingDirection() == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
		}

		if (IsPlayerInFrontOfMe())
		{
			ChangeMoveState(TacklingEnemyMoveState::Tackling);
		}
	}
		break;
	case TacklingEnemyMoveState::Tackling:
	{
		AddMovementInput(GetFacingDirectionAsVector2D());

		if (!IsPlayerInFrontOfMe())
		{
			_player_lost_time += delta_seconds;
			if (_player_lost_time >= TACKLE_CANCEL_TIME)
			{
				ChangeMoveState(TacklingEnemyMoveState::Idle);
				_player_lost_time = 0.f;
			}
		}
		else
		{
			_player_lost_time = 0.f;
		}
	}
		break;
	}
}

AnimRendererComponent* TacklingEnemy::CreateAnimRenderer()
{
	TacklingEnemyAnimatorComponent* animator = CreateComponent<TacklingEnemyAnimatorComponent>(this);
	using TacklingEnemyAnimState = AnimState<TacklingEnemyAnimatorComponent>;
	const std::unordered_map<uint8_t, TacklingEnemyAnimState> anim_state_mapping
	{
		{TacklingEnemyAnimStateID_Idle, TacklingEnemyAnimState(TacklingEnemyAnimPlayInfo_Idle, TacklingEnemyAnimStateUpdate_Idle)},
		{TacklingEnemyAnimStateID_Walk, TacklingEnemyAnimState(TacklingEnemyAnimPlayInfo_Walk, TacklingEnemyAnimStateUpdate_Walk)},
		{TacklingEnemyAnimStateID_Tackle, TacklingEnemyAnimState(TacklingEnemyAnimPlayInfo_Tackle, TacklingEnemyAnimStateUpdate_Tackle)},
	};

	animator->SetAnimStateMapping(anim_state_mapping, TacklingEnemyAnimStateID_Idle);

	animator->SetLocalPosition(Vector2D{ 0.f, -10.f });

	return animator;
}

Vector2D TacklingEnemy::GetBodySize() const
{
    return Vector2D{ UNIT_TILE_SIZE, 50 };
}

void TacklingEnemy::ChangeMoveState(TacklingEnemyMoveState new_state)
{
	switch (new_state)
	{
	case TacklingEnemyMoveState::Idle:
		GetCharacterMovementComponent()->_max_walk_speed = _default_walk_speed;
		_next_flip_time = GetScene()->GetWorldTime() + FLIP_INTERVAL;
		break;
	case TacklingEnemyMoveState::Tackling:
		GetCharacterMovementComponent()->_max_walk_speed = _tackling_speed;
		SetMovementInputEnabled(false);
		GetScene()->MakeDelayedEventWorld(
			this, 
			TACKLE_BEGIN_TIME,
			[this]()
			{
				SetMovementInputEnabled(true);
			}
		);
		break;
	}

	_move_state = new_state;
}

bool TacklingEnemy::IsPlayerInFrontOfMe()
{
	const Vector2D player_body_center = _ingame_scene_ref->GetPlayerRef()->GetBodyCollider()->GetCenterWorldPosition();
	const Vector2D my_body_center = GetBodyCollider()->GetCenterWorldPosition();
	
	// プレイヤーが自分の前にいるか
	const float my_body_top = my_body_center.y - GetBodySize().y * 0.5f;
	const float my_body_bottom = my_body_center.y + GetBodySize().y * 0.5f;
	if (
		player_body_center.y < my_body_top ||
		player_body_center.y > my_body_bottom ||
		Vector2D::Dot(GetFacingDirectionAsVector2D(), player_body_center - my_body_center) <= 0.f
	)
	{
		return false;
	}

	// プレイヤーとの間に障害物があるか
	CollisionQueryParams_SingleLineTrace query_params{};
	query_params.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
	query_params.segment = FSegment(my_body_center, player_body_center);

	QueryResult_SingleLineTrace query_result{};
	GetScene()->SingleLineTrace(query_result, query_params);

	if (query_result.has_hit)
	{
		return false;
	}

	return true;
}
