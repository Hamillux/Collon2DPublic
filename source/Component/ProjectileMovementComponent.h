#pragma once

#include "Component/MovementComponent.h"

/// <summary>
/// 飛翔体の移動コンポーネント
/// </summary>
class ProjectileMovementComponent : public MovementComponent
{
public:
	ProjectileMovementComponent();
	virtual ~ProjectileMovementComponent();

public:
	//~ Begin ComponentBase interface
	virtual void  Tick(const float delta_seconds) override;
	//~ End ComponentBase interface

	//~ Begin MovementComponent interface
	virtual Vector2D GetVelocity() const override;
	virtual void SetVelocity(const Vector2D& new_velocity) override;
	//~ End MovementComponent interface

	/// <summary>
	/// ホーミングの有効・無効を設定する
	/// </summary>
	void SetHomingEnabled();

	/// <summary>
	/// ホーミング対象を設定する
	/// </summary>
	/// <param name="in_homing_target">ホーミング対象</param>
	/// <param name="enable_homing">ホーミングを有効にするかどうか</param>
	void SetHomingTarget(const Actor* const in_homing_target, const bool enable_homing = true);

public:
	float max_speed;				// 最高速
	bool is_homing_enabled;			// ホーミングが有効かどうか
	const Actor* homing_target;		// ホーミング対象
	float homing_acceleration;		// ホーミング対象に向かう加速度の大きさ
	float coef_restitution;			// 反発係数
	float directional_acceleration;	// 進行方向の加速度の大きさ
	float gravity_scale;			// 重力の大きさ

private:
	void UpdateVelocity(const float delta_seconds);
	Vector2D _current_velocity;
};