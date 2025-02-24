#pragma once

#include "Actor/Character/Enemy/EnemyBase.h"

enum class TacklingEnemyMoveState
{
	Idle,
	Tackling,
};

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// プレイヤーを見つけると突進してくる敵
/// </summary>
class TacklingEnemy : public EnemyBase
{
public:
	TacklingEnemy();
	virtual ~TacklingEnemy();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void Finalize() override;
	virtual void OnHitCollision(const HitResult& hit_result) override;
	//~ End Actor interface

	//~ Begin Character interface
protected:
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	virtual void OnDead(const CharacterDeathInfo* death_info) override;
	//~ End Character interface

	//~ Begin EnemyBase interface
protected:
	virtual void TickEnemy(const float delta_seconds) override;
	virtual AnimRendererComponent* CreateAnimRenderer() override;
	virtual Vector2D GetBodySize() const override;
	//~ End EnemyBase interface

public:
	TacklingEnemyMoveState GetTacklingEnemyMoveState() const { return _move_state; }

private:
	TacklingEnemyMoveState _move_state;
	void ChangeMoveState(TacklingEnemyMoveState new_state);

	bool IsPlayerInFrontOfMe();
	float _next_flip_time;
	float _player_lost_time;
	float _default_walk_speed;
	float _tackling_speed;
};

template<> struct initial_params_of_actor<TacklingEnemy> { using type = initial_params_of_actor_t<EnemyBase>; };