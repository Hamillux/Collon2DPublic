#pragma once

#include "Actor/Actor.h"
#include "Actor/Character/CharacterInitialParams.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/Collider/HitResult.h"
#include <vector>
#include <queue>
#include <map>

class InGameScene;
class BoxCollider;
struct CameraParams;
class CharacterMovementComponent;
enum class CharacterMovementMode;
class Character;

/// <summary>
/// キャラクターの死因
/// </summary>
enum class ECauseOfCharacterDeath
{
	ZERO_HP,
	FALL_FROM_STAGE,
	TIME_UP,
};

/// <summary>
/// キャラクターの死亡情報
/// </summary>
struct CharacterDeathInfo
{
	ECauseOfCharacterDeath cause_of_death;
};


CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// キャラクタークラス. CharacterMovementComponentによって移動処理を行い, HPが0になると死亡する
/// </summary>
class Character : public Actor 
{
public:
	struct CharacterEvents 
	{
		Event<> OnBeginFalling;
		Event<> OnLanded;
		Event<const CharacterDeathInfo*> OnDead;
	};
	CharacterEvents character_events;

	enum class Direction;

	Character();
	virtual ~Character() {};

	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) override;
	//virtual void GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes) override;

	/// <summary>
	/// body_colliderを囲む最小のタイル数
	/// </summary>
	virtual void GetOccupyingTiles(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const override;

	virtual void Finalize() override;
	virtual void OnHitCollision(const HitResult& hit_result) override;
	virtual void RequestToSetActorHidden(const bool new_hidden) override;

	/// <summary>
	/// IsDead(), IsUndamageable()がともにfalseの場合はTakeDamage()を呼ぶ
	/// </summary>
	/// <param name="damage_info"></param>
	void ApplyDamage(const DamageInfo& damage_info);

protected:
	/// <summary>
	/// ダメージを受ける. Character::TakeDamage()ではHPを減らす.
	/// NOTE: ダメージを与える側が受ける側のTakeDamageを呼ぶ.
	/// </summary>
	/// <param name="damage_info">ダメージ情報</param>
	virtual void TakeDamage(const DamageInfo& damage_info) override;

	//~ End Actor interface

	//~Begin Character interface
protected:
	/// <summary>
	/// 他のキャラクターを踏みつける
	/// </summary>
	/// <param name="stomped_character">踏みつけ対象</param>
	virtual void StompCharacter(Character* const stomped_character);

	/// <summary>
	/// 他のキャラクターに踏みつけられた時の処理
	/// </summary>
	virtual void OnStompedByOtherCharacter(Character* const stomper);
	virtual void OnFacingDirectionChanged(const Direction new_facing_dir);
	virtual void OnJump();
	virtual void OnLanded();
	virtual void OnDead(const CharacterDeathInfo* death_info);

private:
	virtual void SetupBodyCollider(BoxCollider* const body_collider) = 0;
	//~ End Character interface

public:
	void KillCharacter(const CharacterDeathInfo* death_info);
	bool IsUndamageable() const { return _is_undamageable; }
	bool IsDead() const { return _is_dead; }
	int GetMaxHP() const;
	int GetHP() const;
	void SetHP(const int new_hp);
	void KnockBack(const Vector2D& knock_back_impulse);
	Vector2D GetImpulseToAddVelocity(const Vector2D& velocity) const;
	void AddMovementInput(const Vector2D& world_dir);
	void SetMovementInputEnabled(const bool is_enabled);
	bool GetMovementInputEnabled() const;
	void Jump(const bool ignore_jump_count_limit = false);
	void UpdateWalkingDirectionAndGroundNormal();
	void GetWalkingDirection(Vector2D& out_walking_direction_left, Vector2D& out_walking_direction_right) const;
	Vector2D GetWalkingDirectionLeft() const { return _walking_direction_left; }
	Vector2D GetWalkingDirectionRight() const { return _walking_direction_right; }
	Vector2D GetGroundNormal() const { return _ground_normal; }
	Vector2D _walking_direction_left;
	Vector2D _walking_direction_right;
	Vector2D _ground_normal;
	BoxCollider* GetBodyCollider() const { return _body_collider; }
	bool IsFalling() const;
	CharacterMovementMode GetMovementMode() const;
	enum class Direction : int
	{
		LEFT = -1,
		RIGHT = 1,
	};
	Direction GetFacingDirection() const;
	Vector2D GetFacingDirectionAsVector2D() const;
	void SetFacingDirection(const Direction new_facing_dir);
	void FlipFacingDirection();
	void SetForcedFacingDirection(const Direction forced_facing_dir);
	void SetIsFacingDirectionForced(const bool is_forced) { _is_facing_direction_forced = is_forced; }

	CharacterMovementComponent* GetCharacterMovementComponent();
	bool _is_facing_direction_forced;	// trueの間はAddMovementInputで向きが入力方向に変わらない
	Vector2D _last_ground_normal;

protected:
	InGameScene* _ingame_scene_ref;

	void StopJumping();
	void SetIsUndamageable(const bool is_undamageable) { _is_undamageable = is_undamageable; }

private:
	
	CharacterMovementComponent* _character_movement;
	BoxCollider* _body_collider;
	Direction _facing_dir;	// NOTE: SetFacingDirection()を介して設定する
	bool _is_dead;
	int _hp;
	int _max_hp;
	bool _is_undamageable;			// ダメージ無効化状態か. (ex)アイテムによる無敵状態, ダメージを受けた後のダメージ無効化
	bool _is_movement_input_enabled;
};

template<>
struct initial_params_of_actor<Character>
{
	using type = CharacterInitialParams;
};