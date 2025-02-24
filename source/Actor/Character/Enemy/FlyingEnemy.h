#pragma once

#include "EnemyBase.h"
#include "FlyingEnemyInitialParams.h"

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// 飛行する敵
/// </summary>
class FlyingEnemy : public EnemyBase
{
public:
	FlyingEnemy();
	virtual ~FlyingEnemy();

public:
	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	//virtual void Draw(const CameraParams& camera_params) override;
	virtual void Finalize() override;
	//virtual void OnHitCollision(const HitResult& hit_result) override;
	//~ End Actor interface

	//~ Begin Character interface
protected:
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	virtual void OnDead(const CharacterDeathInfo* death_info) override;
	//~ End Character interface


	//~ Begin EnemyBase interface
protected:
	virtual std::vector<CollisionObjectType> GetHitTargetTypes() const override;
	virtual void TickEnemy(const float delta_seconds) override;
	virtual AnimRendererComponent* CreateAnimRenderer() override;
	virtual Vector2D GetBodySize() const override;
	//~ End EnemyBase interface

private:
	std::unique_ptr<Vector2D> _destination;	// 移動目標
	float _destination_update_time;			// 移動目標の更新時刻

};

template<> struct initial_params_of_actor<FlyingEnemy> { using type = FlyingEnemyInitialParams; };