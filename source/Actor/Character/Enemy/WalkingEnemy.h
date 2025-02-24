#pragma once

#include "Actor/Character/Enemy/EnemyBase.h"
#include "WalkingEnemyInitialParams.h"

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// ただ歩くだけの敵
/// </summary>
class WalkingEnemy : public EnemyBase
{
public:
	WalkingEnemy();
	virtual ~WalkingEnemy();
	
public:
	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void Draw(const CameraParams& camera_params) override;
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
};

template<> struct initial_params_of_actor<WalkingEnemy> { using type = WalkingEnemyInitialParams; };