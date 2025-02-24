#pragma once

#include "SceneObject/Actor/Character/Character.h"
#include "SceneObject/Actor/Character/Enemy/EnemyBaseInitialParams.h"

struct ScreenParams;
class AnimRendererComponent;
class Player;

class EnemyBase : public Character
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) final override;
	// virtual void Draw(const ScreenParams& screen_params) override;
	virtual void Finalize() override;

	// 衝突したプレイヤーにSideHitダメージを与える
	virtual void OnHitCollision(const HitResult& hit_result) override;
	//~ End Actor interface

	//~Begin Character interface
protected:
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	// 移動入力を無効化, コリジョンを無効化
	virtual void OnDead(const CharacterDeathInfo* death_info) override;

private:
	virtual void SetupBodyCollider(BoxCollider* const created_body_collider) final override;
	//~ End Character interface


	//~ Begin EnemyBase interface
public:
	// 倒されるスコア
	virtual int GetBeatenScore() const;

protected:
	virtual std::vector<CollisionObjectType> GetHitTargetTypes() const;
	virtual void TickEnemy(const float delta_seconds);
	virtual AnimRendererComponent* CreateAnimRenderer() = 0;
	virtual Vector2D GetBodySize() const = 0;
	virtual void VanishEnemy();
	//~ End EnemyBase interface

protected:
	AnimRendererComponent* GetAnimRenderer() const { return _anim_renderer; }
	// 前方が奈落かどうか
	bool IsFacingAbyss();
	int GetPlayerHitDamage() const { return _player_hit_damage; }

private:
	void BindEvents();
	void UnbindEvents();

	AnimRendererComponent* _anim_renderer;
	int _player_hit_damage;

	struct EnemySoundEffects
	{
		std::shared_ptr<SoundInstance> damaged;
	};
	std::unique_ptr<EnemySoundEffects> _sound_effects;
	void LoadEnemySoundEffects();
};

template<> struct initial_params_of_actor<EnemyBase> { using type = EnemyBaseInitialParams; };