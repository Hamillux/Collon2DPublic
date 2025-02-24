#pragma once

#include "MovementComponent.h"

class Character;

class CharacterMovementComponent : public MovementComponent
{
public:
	struct CharacterMovementEvents
	{
		Event<> OnBeginFallingDown;	// Falling状態でY速度が上から下に変化した時
	};
	CharacterMovementEvents character_movement_events;

	CharacterMovementComponent();
	virtual ~CharacterMovementComponent() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Tick(const float delta_seconds) override;
	//~ End ComponentBase interface

	//~ Begin MovementComponent interface
public:
	virtual Vector2D GetVelocity() const override;
	virtual void SetVelocity(const Vector2D& new_velocity) override;
	//~ End MovementComponent interface

public:
	bool Jump(const bool ignore_jump_count_limit = false);
	bool IsJumpableMovementMode(const CharacterMovementMode movement_mode);
	void AddMovementInput(const Vector2D& world_direction);
	void AddImpulse(const Vector2D& impulse);

	/// <summary>
	/// 指定の速度変化を生じさせるためのインパルスを取得する
	/// </summary>
	/// <param name="delta_velocity">速度変化</param>
	/// <returns>AddImpulseに渡すとdelta_velocityの速度変化を生じさせるインパルス</returns>
	Vector2D GetImpulseToAddVelocity(const Vector2D& delta_velocity);

	/// <summary>
	/// 移動方法の変更
	/// </summary>
	/// <param name="new_movement_mode"></param>
	void SetMovementMode(const CharacterMovementMode new_movement_mode);
	CharacterMovementMode GetMovementMode() const { return _current_movement_mode; }

	/// <summary>
	/// 着地処理. 親キャラクターから呼び出す
	/// </summary>
	void ProcessLanded();

	float _max_accelleration;	// 最大加速度
	float _max_walk_speed;		// 最大歩行速度
	float _max_fly_speed;		// 最大飛行速度
	float _ground_friction;		// 
	float _brake_friction;
	bool _use_brake_friction;
	float _constant_deceleration;
	int _max_jump_count;
	int _jump_count;
	bool _pressed_jump;
	float _jump_speed;
	float _air_control;	// Falling時の水平方向の加速度倍率
	float _max_fall_speed;
	float _gravity_scale;

private:
	void CalculateAcceleration(const float delta_seconds);

	/// <summary>
	/// 移動入力を加速度に変換して_accelerationに加算する
	/// </summary>
	/// <param name="delta_seconds"></param>
	/// <param name="normalized_movement_input"></param>
	void AddAccelerationByMovementInput(const Vector2D& normalized_movement_input);
	void ApplyBrake(const float delta_seconds, const float friction, const float constant_deceleration, const bool should_apply_brake_x = true, const bool should_apply_brake_y = false);
	Vector2D ConsumeMovementInput();
	Vector2D ConsumeImpulse();

	Character* _character_ref;
	CharacterMovementMode _current_movement_mode;
	Vector2D _velocity;
	Vector2D _last_velocity;
	Vector2D _acceleration;
	Vector2D _movement_input;
	Vector2D _accumulated_impulse;

	static constexpr float TIME_STEP = 1.f / 30.f;
	static constexpr float TIME_TO_BEGIN_FALL = 0.1f;
	static constexpr float IMPULSE_DELTA_TIME = 0.1f;
	bool _applied_input;
	float _begin_fall_timer;
};