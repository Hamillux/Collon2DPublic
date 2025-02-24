#include "Character.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include "Component/CharacterMovementComponent.h"

Character::Character()
	: _body_collider(nullptr)
	, _is_dead(false)
	, _hp(1)
	, _max_hp(100)
	, _facing_dir(Direction::RIGHT)
	, _character_movement(nullptr)
	, _is_movement_input_enabled(true)
	, _ingame_scene_ref(nullptr)
	, _is_facing_direction_forced(false)
	, _is_undamageable(false)
	, _last_ground_normal(Vector2D{})
{
}

void Character::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);
	using InitialParamsType = initial_params_of_actor_t<Character>;
	const InitialParamsType* character_params = dynamic_cast<const InitialParamsType*>(actor_params);
	assert(character_params);

	_max_hp = character_params->max_hp;
	_hp = _max_hp;
	SetFacingDirection(character_params->look_right ? Direction::RIGHT: Direction::LEFT);

	if (GetScene()->GetSceneType() == SceneType::INGAME_SCENE)
	{
		_ingame_scene_ref = dynamic_cast<InGameScene*>(GetScene());
	}
	else
	{
		_ingame_scene_ref = nullptr;
	}

	_character_movement = CreateComponent<CharacterMovementComponent>(this);
	_character_movement->_max_walk_speed = character_params->max_walk_speed;
	_character_movement->_max_fly_speed = character_params->max_fly_speed;
	_character_movement->character_movement_events.OnBeginFallingDown += [this]() 
		{
			character_events.OnBeginFalling.Dispatch();
		};

	_is_movement_input_enabled = true;

	_body_collider = CreateComponent<BoxCollider>(this);
	SetupBodyCollider(_body_collider);
	_body_collider->SetLocalPosition(Vector2D{});
}

void Character::TickActor(float delta_seconds)
{
	__super::TickActor(delta_seconds);

	UpdateWalkingDirectionAndGroundNormal();

	if (!_is_dead)
	{
		constexpr float FALL_DEATH_HEIGHT_OFFSET = 256.f;

		if (GetHP() <= 0)
		{
			CharacterDeathInfo death_info{};
			death_info.cause_of_death = ECauseOfCharacterDeath::ZERO_HP;
			KillCharacter(&death_info);
		}
		else if (GetActorWorldPosition().y > _ingame_scene_ref->GetStageBottomY() + FALL_DEATH_HEIGHT_OFFSET)
		{
			CharacterDeathInfo death_info{};
			death_info.cause_of_death = ECauseOfCharacterDeath::FALL_FROM_STAGE;
			KillCharacter(&death_info);
		}
	}
}

void Character::GetOccupyingTiles(int& out_tiles_x, int& out_tiles_y, Vector2D& out_snap_position_to_actor_position) const
{
	const Vector2D actor_pos = GetActorWorldPosition();
	const Vector2D body_extent = GetBodyCollider()->GetBoxExtent();

	const int tile_x = static_cast<int>(std::ceil(body_extent.x / UNIT_TILE_SIZE));
	const int tile_y = static_cast<int>(std::ceil(body_extent.y / UNIT_TILE_SIZE));

	out_tiles_x = tile_x;
	out_tiles_y = tile_y;

	const float snap_pos_to_tiles_center_x = tile_x % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0;
	const float snap_pos_to_tiles_center_y = tile_y % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0;

	// NOTE: タイル群の中心とキャラクターの位置のx座標は同じ
	const float tiles_center_to_actor_pos_y = UNIT_TILE_SIZE * tile_y * 0.5f - body_extent.y * 0.5f;

	out_snap_position_to_actor_position.x = snap_pos_to_tiles_center_x;
	out_snap_position_to_actor_position.y = snap_pos_to_tiles_center_y + tiles_center_to_actor_pos_y;
}

void Character::Finalize()
{
	_body_collider = nullptr;
	_is_dead = false;
	_hp = 1;
	_max_hp = 100;
	_facing_dir = Direction::RIGHT;
	_character_movement = nullptr;
	_is_movement_input_enabled = true;
	_ingame_scene_ref = nullptr;
	_is_facing_direction_forced = false;
	_is_undamageable = false;
	_last_ground_normal = Vector2D{};

	__super::Finalize();
}


void Character::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	if (!hit_result.has_hit || !hit_result.self_collider || !hit_result.other_collider)
	{
		return;
	}

	Character* const other_character = dynamic_cast<Character*>(hit_result.other_collider->GetOwnerActor());
	if (other_character)
	{
		const bool is_stomped_other = hit_result.normal_from_other.y < -0.5;
		if(is_stomped_other)
		{
			StompCharacter(other_character);
		}
	}

	if (
		GetMovementMode() == CharacterMovementMode::Falling &&
		hit_result.collision_type == CollisionType::BLOCK && 
		hit_result.other_collider->GetCollisionObjectType() == CollisionObjectType::GROUND &&
		hit_result.normal_from_other.y < 0 && 
		hit_result.total_push_back_distance > EPSIRON
		)
	{
		UpdateWalkingDirectionAndGroundNormal();
		_character_movement->ProcessLanded();
		OnLanded();
	}
}

void Character::RequestToSetActorHidden(const bool new_hidden)
{
	if (!IsDead())
	{
		__super::RequestToSetActorHidden(new_hidden);
	}
}

void Character::TakeDamage(const DamageInfo& damage_info)
{
	SetHP(_hp - damage_info.damage_value);
}

void Character::OnFacingDirectionChanged(const Direction new_facing_dir)
{
}

void Character::OnJump()
{
}

void Character::OnLanded()
{
	character_events.OnLanded.Dispatch();
}

void Character::OnDead(const CharacterDeathInfo* death_info)
{
}

void Character::StompCharacter(Character* const stomped_character)
{

}

void Character::OnStompedByOtherCharacter(Character* const stomper)
{
}

void Character::ApplyDamage(const DamageInfo& damage_info)
{
	if (IsDead() || IsUndamageable())
	{
		return;
	}

	TakeDamage(damage_info);
}

void Character::KillCharacter(const CharacterDeathInfo* death_info)
{
	if (_is_dead)
	{
		return;
	}

	_is_dead = true;
	_hp = 0;
	OnDead(death_info);
	character_events.OnDead.Dispatch(death_info);
}

int Character::GetMaxHP() const
{
	return _max_hp;
}

int Character::GetHP() const
{
	return _hp;
}

void Character::SetHP(const int new_hp)
{
	_hp = clamp(new_hp, 0, _max_hp);
}

bool Character::IsFalling() const
{
	return _character_movement->GetMovementMode() == CharacterMovementMode::Falling;
}

CharacterMovementMode Character::GetMovementMode() const
{
	return _character_movement->GetMovementMode();
}

void Character::Jump(const bool ignore_jump_count_limit)
{
	if(_character_movement->Jump(ignore_jump_count_limit))
	{
		OnJump();
	}
}

void Character::StopJumping()
{
	const Vector2D current_velocity = _character_movement->GetVelocity();
	if (current_velocity.y > 0.f)
	{
		return;
	}

	_character_movement->SetVelocity(Vector2D{current_velocity.x, current_velocity.y * 0.2f});
}

void Character::KnockBack(const Vector2D& knock_back_impulse)
{
	_character_movement->SetMovementMode(CharacterMovementMode::Falling);
	_character_movement->AddImpulse(knock_back_impulse);
}

Vector2D Character::GetImpulseToAddVelocity(const Vector2D& velocity) const
{
	return _character_movement->GetImpulseToAddVelocity(velocity);
}

void Character::AddMovementInput(const Vector2D& world_dir)
{
	if (!_is_movement_input_enabled)
	{
		return;
	}

	_character_movement->AddMovementInput(world_dir);

	// 移動入力方向に応じて向きを変える
	if (!_is_facing_direction_forced)
	{
		const float dot = Vector2D::Dot(world_dir, Vector2D{ 1,0 });

		if (dot > 0 && _facing_dir == Direction::LEFT)
		{
			SetFacingDirection(Direction::RIGHT);
		}
		else if (dot < 0 && _facing_dir == Direction::RIGHT)
		{
			SetFacingDirection(Direction::LEFT);
		}
	}
}

void Character::SetMovementInputEnabled(const bool is_enabled)
{
	_is_movement_input_enabled = is_enabled;
}

bool Character::GetMovementInputEnabled() const
{
	return _is_movement_input_enabled;
}

void Character::UpdateWalkingDirectionAndGroundNormal()
{
	const float line_length = std::min(16.f, _body_collider->GetBoxExtent().x);
	constexpr float LINE_START_Y_OFFSET = -4.f;
	const Vector2D body_position = _body_collider->GetWorldPosition();
	const float body_left = body_position.x - _body_collider->GetBoxExtent().x * 0.5f;
	const float body_right = body_position.x + _body_collider->GetBoxExtent().x * 0.5f;
	const float body_bottom = body_position.y + _body_collider->GetBoxExtent().y * 0.5f;
	const float line_start_y = body_bottom + LINE_START_Y_OFFSET;
	const float line_end_y = body_bottom + line_length;

	QueryResult_SingleLineTrace query_result_left;
	CollisionQueryParams_SingleLineTrace query_params_left;
	query_params_left.ignore_actors.push_back(this);
	query_params_left.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
	query_params_left.segment = FSegment{ Vector2D{body_left, line_start_y}, Vector2D{body_left, line_end_y} };
	GetScene()->SingleLineTrace(query_result_left, query_params_left);
	const float hit_sq_length_left = (query_result_left.hit_location.y - line_start_y);

	QueryResult_SingleLineTrace query_result_left_long{};
	CollisionQueryParams_SingleLineTrace query_params_left_long(query_params_left);
	query_params_left_long.segment.end.y += _body_collider->GetBoxExtent().x;
	GetScene()->SingleLineTrace(query_result_left_long, query_params_left_long);

	QueryResult_SingleLineTrace query_result_right;
	CollisionQueryParams_SingleLineTrace query_params_right;
	query_params_right.ignore_actors.push_back(this);
	query_params_right.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
	query_params_right.segment = FSegment{ Vector2D{body_right, line_start_y}, Vector2D{body_right, line_end_y} };
	GetScene()->SingleLineTrace(query_result_right, query_params_right);
	const float hit_sq_length_right = (query_result_right.hit_location.y - line_start_y);

	QueryResult_SingleLineTrace query_result_right_long{};
	CollisionQueryParams_SingleLineTrace query_params_right_long(query_params_right);
	query_params_right_long.segment.end.y += _body_collider->GetBoxExtent().x;
	GetScene()->SingleLineTrace(query_result_right_long, query_params_right_long);

	QueryResult_SingleLineTrace query_result_center{};
	CollisionQueryParams_SingleLineTrace query_params_center{};
	const float center_start_y = body_bottom + LINE_START_Y_OFFSET;
	const float center_end_y = body_bottom + _body_collider->GetBoxExtent().x;
	query_params_center.ignore_actors.push_back(this);
	query_params_center.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
	query_params_center.segment = FSegment{ Vector2D{body_position.x, center_start_y}, Vector2D{body_position.x, center_end_y} };
	GetScene()->SingleLineTrace(query_result_center, query_params_center);

	const Vector3D normal_left = Vector3D::MakeFromXY(query_result_left.hit_normal);
	const Vector3D walk_dir_left = normal_left.Cross(Vector3D{ -1,0,0 }.Cross(normal_left));
	const Vector3D normal_right = Vector3D::MakeFromXY(query_result_right.hit_normal);
	const Vector3D walk_dir_right = normal_right.Cross(Vector3D{ 1,0,0 }.Cross(normal_right));
	
	if (query_result_left.has_hit && query_result_right.has_hit)
	{
		_ground_normal = (query_result_left.hit_normal + query_result_right.hit_normal).Normalize();
		_walking_direction_left = Vector3D::MakeFromXY(_ground_normal).Cross(Vector3D{ -1,0,0 }.Cross(Vector3D::MakeFromXY(_ground_normal))).XY();
		_walking_direction_right = _walking_direction_left * -1.f;
	}
	else if (query_result_left.has_hit && !query_result_right.has_hit)
	{
		_walking_direction_left = walk_dir_left.XY();
		_walking_direction_right = walk_dir_left.XY() * -1.f;
		_ground_normal = query_result_left.hit_normal;
	}
	else if (!query_result_left.has_hit && query_result_right.has_hit)
	{
		_walking_direction_left = walk_dir_right.XY() * -1.f;
		_walking_direction_right = walk_dir_right.XY();
		_ground_normal = query_result_right.hit_normal;
	}

	if (GetCharacterMovementComponent()->GetMovementMode() == CharacterMovementMode::Walking&& _last_ground_normal != _ground_normal)
	{
		// 地面の法線が変わった
		const Vector2D old_velocity = GetCharacterMovementComponent()->GetVelocity();
		const float old_speed = old_velocity.Length();
		const Vector2D v_n = _ground_normal * Vector2D::Dot(_ground_normal, old_velocity);
		const Vector2D v_t = GetCharacterMovementComponent()->GetVelocity() - v_n;
		const Vector3D new_velocity = Vector3D::MakeFromXY(_ground_normal).Cross(Vector3D::MakeFromXY(v_t).Cross(Vector3D::MakeFromXY(_ground_normal)));
		GetCharacterMovementComponent()->SetVelocity(new_velocity.XY().Normalize() * old_speed);
	}
}

void Character::GetWalkingDirection(Vector2D& out_walking_direction_left, Vector2D& out_walking_direction_right) const
{
	out_walking_direction_left = _walking_direction_left;
	out_walking_direction_right = _walking_direction_right;
}

Character::Direction Character::GetFacingDirection() const
{
	return _facing_dir;
}

Vector2D Character::GetFacingDirectionAsVector2D() const
{
	if (_facing_dir == Direction::LEFT)
	{
		return Vector2D{ -1.f, 0.f };
	}
	else if (_facing_dir == Direction::RIGHT)
	{
		return Vector2D{ 1.f, 0.f };
	}

	return Vector2D{ 0,0 };
}

void Character::SetForcedFacingDirection(const Direction forced_facing_dir)
{
	SetFacingDirection(forced_facing_dir);
	_is_facing_direction_forced = true;
}

void Character::SetFacingDirection(const Direction new_facing_dir)
{
	const bool is_changed = _facing_dir != new_facing_dir;

	_facing_dir = new_facing_dir;

	OnFacingDirectionChanged(new_facing_dir);
}

void Character::FlipFacingDirection()
{
	SetFacingDirection(GetFacingDirection() == Direction::LEFT ? Direction::RIGHT : Direction::LEFT);
}

CharacterMovementComponent* Character::GetCharacterMovementComponent()
{
	return _character_movement;
}
