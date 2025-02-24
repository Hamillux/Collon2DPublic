#pragma once

#include "Actor/Character/Character.h"
#include "Actor/Character/Enemy/EnemyBaseInitialParams.h"

struct CameraParams;
class AnimRendererComponent;
class Player;

/// <summary>
/// 敵の基底クラス
/// </summary>
class EnemyBase : public Character
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) final override;
	// virtual void Draw(const CameraParams& camera_params) override;
	virtual void Finalize() override;

	// 衝突したプレイヤーにSideHitダメージを与える
	virtual void OnHitCollision(const HitResult& hit_result) override;

protected:
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	//~ End Actor interface

	//~Begin Character interface
protected:
	// 移動入力を無効化, コリジョンを無効化
	virtual void OnDead(const CharacterDeathInfo* death_info) override;

private:
	virtual void SetupBodyCollider(BoxCollider* const created_body_collider) final override;
	//~ End Character interface


	//~ Begin EnemyBase interface
public:
	/// <summary>
	/// 倒して得られるスコアを取得する
	/// </summary>
	/// <returns></returns>
	virtual int GetBeatenScore() const;

protected:
	virtual std::vector<CollisionObjectType> GetHitTargetTypes() const;
	virtual void TickEnemy(const float delta_seconds);
	virtual AnimRendererComponent* CreateAnimRenderer() = 0;

	/// <summary>
	/// ボディーコライダーのサイズを取得する
	/// </summary>
	/// <returns></returns>
	virtual Vector2D GetBodySize() const = 0;

	/// <summary>
	/// 敵を消滅させる
	/// <para>EnemyBase::VanishEnemy()ではスコア加算と消滅エフェクトの再生を行う</para>
	/// </summary>
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