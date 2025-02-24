#pragma once

#include "SceneObject/Component/MovementComponent.h"

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

	void EnableHoming(const Actor* const homing_target);
public:
	float max_speed;				// 最高速
	bool is_homing_enabled;
	const Actor* homing_target;		// ホーミング対象
	float homing_acceleration;		// ホーミング多少に向かう加速度の大きさ
	float coef_restitution;			// 反発係数
	float directional_acceleration;	// 進行方向の加速度の大きさ
	float gravity_scale;

private:
	void UpdateVelocity(const float delta_seconds);
	Vector2D _current_velocity;
};