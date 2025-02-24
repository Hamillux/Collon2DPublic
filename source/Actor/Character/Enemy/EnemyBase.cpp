#include "EnemyBase.h"
#include "GameSystems/Sound/SoundManager.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include "Component/Collider/BoxCollider.h"
#include "Actor/Character/Player/Player.h"
#include "Component/CharacterMovementComponent.h"
#include "Component/Renderer/Animator/AnimatorComponent.h"

namespace
{
	Character::Direction UNREVERSED_DIRECTION = Character::Direction::LEFT;
	constexpr int DEFAULT_BEATEN_SCORE = 500;
	constexpr int DEFAULT_VANISHED_SCORE = 500;
}

EnemyBase::EnemyBase()
	: _anim_renderer(nullptr)
	, _player_hit_damage(1)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	const EnemyBaseInitialParams* enemy_params = dynamic_cast<const EnemyBaseInitialParams*>(actor_params);
	_player_hit_damage = enemy_params->player_hit_damage;

	_anim_renderer = CreateAnimRenderer();
	_anim_renderer->SetReverseX(GetFacingDirection() != UNREVERSED_DIRECTION);

	LoadEnemySoundEffects();

	// プレイヤーの出現完了するまで動かないようにする
	SetShouldCallTickActor(false);

	BindEvents();
}

void EnemyBase::TickActor(float delta_seconds)
{
	__super::TickActor(delta_seconds);

	if(!IsDead())
	{
		TickEnemy(delta_seconds);
	}

	_anim_renderer->SetReverseX(GetFacingDirection() != UNREVERSED_DIRECTION);
}

void EnemyBase::Finalize()
{
	UnbindEvents();
	__super::Finalize();
}

void EnemyBase::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	if (GetScene()->GetSceneType() != SceneType::INGAME_SCENE)
	{
		return;
	}

	Player* player_ref = _ingame_scene_ref->GetPlayerRef();

	if (hit_result.other_collider == player_ref->GetBodyCollider())
	{
		const bool should_apply_damage = hit_result.normal_from_other.y <= 0.f;
		if(should_apply_damage)
		{
			DamageInfo damage_info{};
			damage_info.damage_value = _player_hit_damage;
			damage_info.damage_type = DamageType::SideHit;
			damage_info.damage_causer = this;
			player_ref->ApplyDamage(damage_info);
		}
	}

}

void EnemyBase::TakeDamage(const DamageInfo& damage_info)
{
	__super::TakeDamage(damage_info);

	SetMovementInputEnabled(false);

	if (damage_info.damage_type == DamageType::Punch)
	{
		KnockBack(Vector2D{ damage_info.punch_damage_info.impulse[0], damage_info.punch_damage_info.impulse[1] });
	}
	else if (damage_info.damage_type == DamageType::Crush)
	{
		KnockBack(Vector2D{ damage_info.crush_damage_info.impulse[0], damage_info.crush_damage_info.impulse[1] });
	}

	if (damage_info.damage_causer == _ingame_scene_ref->GetPlayerRef())
	{
		_ingame_scene_ref->GetPlayerRef()->AddScore(GetBeatenScore());
	}

	_sound_effects->damaged->Play();
}

void EnemyBase::OnDead(const CharacterDeathInfo* death_info)
{
	__super::OnDead(death_info);

	SetMovementInputEnabled(false);
	GetCharacterMovementComponent()->SetMovementMode(CharacterMovementMode::Falling);
	GetBodyCollider()->Deactivate();
}

void EnemyBase::SetupBodyCollider(BoxCollider* const body_collider)
{
	const Vector2D body_size = GetBodySize();

	auto hit_targets = GetHitTargetTypes();

	body_collider->SetBoxColliderParams(
		CollisionType::BLOCK,
		CollisionObjectType::ENEMY,
		GetHitTargetTypes(),
		true,
		body_size
	);

	const float body_local_y = UNIT_TILE_SIZE * 0.5f - body_size.y * 0.5f;
	body_collider->SetLocalPosition(Vector2D{ 0.f, body_local_y });
}

int EnemyBase::GetBeatenScore() const
{
	return DEFAULT_BEATEN_SCORE;
}

std::vector<CollisionObjectType> EnemyBase::GetHitTargetTypes() const
{
	return { CollisionObjectType::PLAYER, CollisionObjectType::DAMAGE, CollisionObjectType::GROUND, CollisionObjectType::BARRIER };
}

void EnemyBase::TickEnemy(const float delta_seconds)
{
}

void EnemyBase::VanishEnemy()
{
	MarkAsShouldDestroy();

	if(!IsHidden())
	{
		_ingame_scene_ref->GetPlayerRef()->AddScore(DEFAULT_VANISHED_SCORE);

		AnimPlayInfo vanishment_effect{ MasterDataID(24), 2.f, 1.f, FALSE, FALSE };
		GetScene()->PlayAnimation(vanishment_effect, GetActorWorldTransform());
	}
}

bool EnemyBase::IsFacingAbyss()
{
	const Vector2D facing_dir = GetFacingDirectionAsVector2D();

	// 足下からbodyの高さと同じ長さのレイを下に伸ばす
	const Vector2D body_center = GetBodyCollider()->GetCenterWorldPosition();
	const Vector2D body_extent = GetBodyCollider()->GetBoxExtent();
	const float query_line_length = body_extent.y;
	const float query_line_x = body_center.x + body_extent.x * facing_dir.x;
	const float query_line_from_y = body_center.y + body_extent.y * 0.5f;
	const float query_line_to_y = query_line_from_y + query_line_length;
	const FSegment query_line{ Vector2D{query_line_x, query_line_from_y}, Vector2D{query_line_x, query_line_to_y} };

	CollisionQueryParams_SingleLineTrace query_params{};
	query_params.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
	query_params.ignore_actors.push_back(this);
	query_params.segment = query_line;
	query_params.trace_object_type = GetBodyCollider()->GetCollisionObjectType();

	QueryResult_SingleLineTrace query_result{};
	GetScene()->SingleLineTrace(query_result, query_params);
	return !query_result.has_hit;
}

void EnemyBase::BindEvents()
{
	if (_ingame_scene_ref && IsValid(_ingame_scene_ref->GetPlayerRef()))
	{
		// プレイヤーの出現完了時に動くようにする
		_ingame_scene_ref->GetPlayerRef()->player_events.OnPlayerEmergenceSequenceFinished.Bind(
			[this]()
			{
				SetShouldCallTickActor(true);
			},
			this
		);

		// プレイヤーゴール時に敵キャラをキル
		_ingame_scene_ref->GetPlayerRef()->player_events.OnPlayerReachedGoal.Bind(
			[this]()
			{
				VanishEnemy();
			},
			this
		);

		// プレイヤーが死んだら動かないようにする
		_ingame_scene_ref->GetPlayerRef()->character_events.OnDead.Bind(
			[this](const CharacterDeathInfo* death_info)
			{
				SetShouldCallTickActor(false);
			},
			this
		);
	}
}

void EnemyBase::UnbindEvents()
{
	if (_ingame_scene_ref && IsValid(_ingame_scene_ref->GetPlayerRef()))
	{
		_ingame_scene_ref->GetPlayerRef()->player_events.OnPlayerEmergenceSequenceFinished.UnBind(this);
		_ingame_scene_ref->GetPlayerRef()->player_events.OnPlayerReachedGoal.UnBind(this);
		_ingame_scene_ref->GetPlayerRef()->character_events.OnDead.UnBind(this);
	}
}

void EnemyBase::LoadEnemySoundEffects()
{
	_sound_effects = std::make_unique<EnemySoundEffects>();

	_sound_effects->damaged = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/se/ingame_scene/se_enemy_damaged.ogg");
	_sound_effects->damaged->SetVolume(50);
}
