#include "CharacterMovementComponent.h"
#include "Actor/Character/Character.h"
#include "Scene/SceneBase.h"

CharacterMovementComponent::CharacterMovementComponent()
	: _character_ref(nullptr)
	, _velocity(Vector2D{})
	, _acceleration(Vector2D{})
	, _accumulated_impulse(Vector2D{})
	, _movement_input(Vector2D{})
	, _max_accelleration(1000.f)
	, _max_walk_speed(300.f)
	, _max_fly_speed(300.f)
	, _ground_friction(100.f)
	, _brake_friction(1000.f)
	, _use_brake_friction(false)
	, _constant_deceleration(2000.f)
	, _air_control(0.2f)
	, _max_fall_speed(1000.f)
	, _gravity_scale(1.f)
	, _current_movement_mode(CharacterMovementMode::Walking)
	, _jump_count(0)
	, _jump_speed(500.f)
	, _max_jump_count(1)
	, _pressed_jump(false)
	, _applied_input(false)
	, _begin_fall_timer(0.f)
{
}

void CharacterMovementComponent::Initialize()
{
	__super::Initialize();
	_character_ref = dynamic_cast<Character*>(GetOwnerActor());
	if (_character_ref == nullptr)
	{
		throw std::runtime_error("CharacterMovementComponent can only be attached to Character.");
	}
}
 
void CharacterMovementComponent::Tick(const float delta_seconds)
{
	__super::Tick(delta_seconds);

	if(_pressed_jump)
	{
		_velocity.y = -_jump_speed;
		SetMovementMode(CharacterMovementMode::Falling);
		_pressed_jump = false;
	}

	// キャラクターの足下に地面があるかどうかを判定
	QueryResult_MultiAARectTrace query_result_bottom;
	{
		auto body_collider = _character_ref->GetBodyCollider();
		constexpr float half_line_length = 1.f;
		const Vector2D body_position = body_collider->GetWorldPosition();
		const float body_left = body_position.x - body_collider->GetBoxExtent().x * 0.5f;
		const float body_right = body_position.x + body_collider->GetBoxExtent().x * 0.5f;
		const float body_bottom = body_position.y + body_collider->GetBoxExtent().y * 0.5f;

		CollisionQueryParams_RectAA query_params_bottom;
		query_params_bottom.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
		query_params_bottom.ignore_actors.push_back(_character_ref);
		const float aa_width = body_collider->GetBoxExtent().x - 2.f;
		const float aa_height = 2.f;
		const Vector2D aa_center = body_position + Vector2D{ 0, body_collider->GetBoxExtent().y * 0.5f + aa_height / 2.f };
		
		query_params_bottom.rect = FRectAA(aa_center, aa_width, aa_height);
		GetScene()->MultiAARectTrace(query_result_bottom, query_params_bottom);
	}

	// 歩行時の落下チェック
	if (_current_movement_mode == CharacterMovementMode::Walking)
	{
		if (query_result_bottom.has_hit)
		{
			_begin_fall_timer = 0.f;
		}
		else
		{
			_begin_fall_timer += delta_seconds;
			_character_ref->AddWorldPosition(_character_ref->GetGroundNormal() * -1.f);
		}

		if (_begin_fall_timer >= TIME_TO_BEGIN_FALL)
		{
			SetMovementMode(CharacterMovementMode::Falling);
			_begin_fall_timer = 0.f;
			_jump_count += 1;	// 歩いて空中に飛び出すのもジャンプとしてカウント
			character_movement_events.OnBeginFallingDown.Dispatch();
		}
	}

	_acceleration = Vector2D{ 0,0 };
	bool is_brake_enabled = false;

	// 加速度計算
	{
		if (_current_movement_mode == CharacterMovementMode::Walking)
		{
			// 地面方向へ加速
			const float to_ground_acceleration = fabsf(_velocity.x) * 10.f;
			_acceleration += _character_ref->GetGroundNormal() * -1.f * clamp(to_ground_acceleration, 100.f, 1000.f);
		}

		// 移動入力を加速度に適用
		if (_movement_input.IsZeroVector())
		{
			switch (_current_movement_mode)
			{
			case CharacterMovementMode::Walking:
				is_brake_enabled = true;
				break;
			case CharacterMovementMode::Falling:
				is_brake_enabled = false;
				break;
			case CharacterMovementMode::Flying:
				is_brake_enabled = true;
				break;
			}
			_applied_input = false;
		}
		else if (_current_movement_mode == CharacterMovementMode::Walking && !query_result_bottom.has_hit)
		{
			ConsumeMovementInput();
			_applied_input = false;
			is_brake_enabled = false;
		}
		else
		{
			const Vector2D movement_input = ConsumeMovementInput();
			float dot = Vector2D::Dot(_velocity.Normalize(), movement_input.Normalize());
			is_brake_enabled = dot < -EPSIRON;

			AddAccelerationByMovementInput(movement_input.Normalize());
			_applied_input = true;
		}
	}

	// 速度更新
	{
		_velocity += _acceleration * delta_seconds;

		// インパルスによる速度変化
		if (!_accumulated_impulse.IsZeroVector())
		{
			_velocity += _accumulated_impulse * IMPULSE_DELTA_TIME;
			_accumulated_impulse = Vector2D{};
			is_brake_enabled = false;
		}

		// 落下中は重力を適用
		if (_current_movement_mode == CharacterMovementMode::Falling)
		{
			_velocity += GetScene()->GetGravityForce() * _gravity_scale * delta_seconds;
		}

		if (is_brake_enabled)
		{
			const float actual_friction = _use_brake_friction ? _brake_friction : _ground_friction;
			ApplyBrake(delta_seconds, actual_friction, _constant_deceleration);
		}
	}

	// 速度制限
	{
		if (_current_movement_mode == CharacterMovementMode::Walking)
		{
			// 地面に対して垂直な方向の速度を0にする
			const Vector2D ground_normal = _character_ref->GetGroundNormal();
			const Vector2D vel_perpendicular_to_ground = ground_normal * Vector2D::Dot(_velocity, ground_normal);
			_velocity -= vel_perpendicular_to_ground;

			// 地面に対して水平な方向の最大速度が最大歩行速度を超えないようにする
			const Vector2D vel_parallel_to_ground = _velocity - vel_perpendicular_to_ground;
			const Vector2D vel_parallel_to_ground_normalized = vel_parallel_to_ground.Normalize();
			const float vel_parallel_to_ground_length = vel_parallel_to_ground.Length();
			if (vel_parallel_to_ground_length > _max_walk_speed)
			{
				_velocity = vel_perpendicular_to_ground + vel_parallel_to_ground_normalized * _max_walk_speed;
			}

		}

		else if (_current_movement_mode == CharacterMovementMode::Falling)
		{
			// 落下速度制限
			if (_velocity.y > _max_fall_speed)
			{
				_velocity.y = _max_fall_speed;
			}

			if (std::abs(_velocity.x) > _max_walk_speed)
			{
				_velocity.x = (_velocity.x > 0.f) ? _max_walk_speed : -_max_walk_speed;
			}

		}

		else if (_current_movement_mode == CharacterMovementMode::Flying)
		{
			if (_velocity.Length() > _max_fly_speed)
			{
				_velocity = _velocity.Normalize() * _max_fly_speed;
			}
		}
	}

	if (_current_movement_mode == CharacterMovementMode::Falling && _last_velocity.y < 0.f && _velocity.y > 0.f)
	{
		character_movement_events.OnBeginFallingDown.Dispatch();
	}

	_last_velocity = _velocity;
	_character_ref->SetVelocity(_velocity);
}

Vector2D CharacterMovementComponent::GetVelocity() const
{
	return _velocity;
}

void CharacterMovementComponent::SetVelocity(const Vector2D& new_velocity)
{
	_velocity = new_velocity;
}

bool CharacterMovementComponent::Jump(const bool ignore_jump_count_limit)
{
	if (!IsJumpableMovementMode(_current_movement_mode))
	{
		return false;
	}

	if (ignore_jump_count_limit)
	{
		_pressed_jump = true;
		return true;
	}

	if (_jump_count < _max_jump_count)
	{
		_jump_count++;
		_pressed_jump = true;
		return true;
	}

	return false;
}

bool CharacterMovementComponent::IsJumpableMovementMode(const CharacterMovementMode movement_mode)
{
	switch (movement_mode)
	{
	case CharacterMovementMode::Walking:
	case CharacterMovementMode::Falling:
		return true;
	}

	return false;
}

void CharacterMovementComponent::CalculateAcceleration(const float delta_seconds)
{
}

void CharacterMovementComponent::AddAccelerationByMovementInput(const Vector2D& normailzed_movement_input)
{
	if (_current_movement_mode == CharacterMovementMode::Walking)
	{
		const Vector2D walk_dir = normailzed_movement_input.x >= 0.f ? _character_ref->GetWalkingDirectionRight() : _character_ref->GetWalkingDirectionLeft();
		_acceleration += walk_dir * _max_accelleration * fabsf(normailzed_movement_input.x);
	}
	else if (_current_movement_mode == CharacterMovementMode::Falling)
	{
		_acceleration += normailzed_movement_input.GetX() * _max_accelleration * _air_control;
	}
	else if (_current_movement_mode == CharacterMovementMode::Flying)
	{
		_acceleration += normailzed_movement_input * _max_accelleration;
	}
}

void CharacterMovementComponent::ApplyBrake(const float delta_seconds, const float friction, const float constant_deceleration, const bool should_apply_brake_x, const bool should_apply_brake_y)
{
	const Vector2D old_velocity = _velocity;

	const Vector2D reverse_velocity 
		= (constant_deceleration == 0.f) ? Vector2D{} : _velocity.Normalize() * (-constant_deceleration);

	// 分割計算
	// こちらで計算する場合は一括計算をコメントアウト
	/*{
		float remain_time = delta_seconds;
		while (remain_time > 0.f)
		{
			const float dt = (remain_time > TIME_STEP) ? TIME_STEP : remain_time;
			remain_time -= dt;

			_velocity += (_velocity.Normalize() * (-friction) + reverse_velocity) * dt;
		}
	}*/

	// 一括計算
 	_velocity += (_velocity.Normalize() * (-friction) + reverse_velocity) * delta_seconds;

	if(!should_apply_brake_x)
	{
		_velocity.x = old_velocity.x;
	}
	if (!should_apply_brake_y)
	{
		_velocity.y = old_velocity.y;
	}

	// 速度が逆方向に変わった場合, 速度を0にする
	if (Vector2D::Dot(old_velocity, _velocity) < 0.f)
	{
		_velocity = Vector2D{};
	}
}

void CharacterMovementComponent::AddMovementInput(const Vector2D& world_direction)
{
	_movement_input += world_direction;
}

void CharacterMovementComponent::AddImpulse(const Vector2D& impulse)
{
	_accumulated_impulse += impulse;
}

Vector2D CharacterMovementComponent::GetImpulseToAddVelocity(const Vector2D& delta_velocity)
{
	return delta_velocity / IMPULSE_DELTA_TIME;
}

void CharacterMovementComponent::SetMovementMode(const CharacterMovementMode new_movement_mode)
{
	_current_movement_mode = new_movement_mode;
}

void CharacterMovementComponent::ProcessLanded()
{
	if (_current_movement_mode == CharacterMovementMode::Falling)
	{
		_jump_count = 0;
		SetMovementMode(CharacterMovementMode::Walking);

		Vector2D new_velocity = _character_ref->GetWalkingDirectionRight() * _last_velocity.x;
 		_velocity = new_velocity;
	}
}

Vector2D CharacterMovementComponent::ConsumeMovementInput()
{
	if (_movement_input.x > -EPSIRON && _movement_input.x < EPSIRON)
	{
		_movement_input.x = 0.f;
	}
	if (_movement_input.y > -EPSIRON && _movement_input.y < EPSIRON)
	{
		_movement_input.y = 0.f;
	}
	const Vector2D ret = _movement_input;
	_movement_input = Vector2D{};
	return ret;
}

Vector2D CharacterMovementComponent::ConsumeImpulse()
{
	const Vector2D ret = _accumulated_impulse;
	_accumulated_impulse = Vector2D{};
	return ret;
}
