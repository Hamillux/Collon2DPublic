#include "PlayerState_Playing.h"
#include "Component/CharacterMovementComponent.h"
#include "Actor/Character/Player/PlayerAnimState.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "Scene/SceneBase.h"
#include "GameSystems/FontManager.h"

void PlayerState_Playing::OnEnter(Player& player)
{
	__super::OnEnter(player);

	_should_show_controls = true;

	player.character_events.OnBeginFalling.Bind
	(
		[&player]() 
		{
			player.SetIsFacingDirectionForced(false);
			player.SetMovementInputEnabled(true);
		},
		this
	);

	player.character_events.OnLanded.Bind
	(
		[&player]()
		{
			player.SetIsFacingDirectionForced(false);
			player.SetMovementInputEnabled(true);
		},
		this
	);
}

void PlayerState_Playing::OnLeave(Player& player)
{
	__super::OnLeave(player);

	player.character_events.OnBeginFalling.UnBind(this);
	player.character_events.OnLanded.UnBind(this);
}

void PlayerState_Playing::Tick(Player& player, float delta_seconds)
{
	__super::Tick(player, delta_seconds);

	if (_is_jump_key_active)
	{
		_stomping_jump_timer += delta_seconds;
	}
	else
	{
		_stomping_jump_timer = 0.f;
	}

	if (_should_show_controls)
	{
		ShowControls();
	}
}

void PlayerState_Playing::DrawForeground(Player& player, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(player, canvas_info);
	player.DrawPlayerUI(canvas_info);
}

void PlayerState_Playing::HandleInput(Player& player)
{
	using CharaDirection = Character::Direction;
	const CharaDirection facing_direction = player.GetFacingDirection();
	const Vector2D wasd = DeviceInput::GetInputDir_WASD();

	// 操作説明表示・非表示
	if (DeviceInput::IsPressed(KEY_INPUT_TAB))
	{
		_should_show_controls = !_should_show_controls;
	}

	if(player.GetMovementInputEnabled())
	{
		const BoxCollider* body_collider = player.GetBodyCollider();
		const Vector2D center_of_body = body_collider->GetCenterWorldPosition();
		const float half_body_width = body_collider->GetBoxExtent().x * 0.5f;

		// WASDで移動
		if (fabsf(wasd.x) > EPSIRON)
		{
			CollisionQueryParams_SingleLineTrace query_params{};
			query_params.ignore_actors.push_back(&player);
			query_params.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
			const float sgn_to_end = wasd.x > 0.f ? 1.f : -1.f;
			query_params.segment = FSegment{ center_of_body, center_of_body + Vector2D{half_body_width + 1.f, 0.f} * sgn_to_end };

			QueryResult_SingleLineTrace query_result{};
			player.GetScene()->SingleLineTrace(query_result, query_params);
			if (
					!query_result.has_hit || 
					query_result.hit_collider->GetCollisionTypeBetween(body_collider) != CollisionType::BLOCK ||
					Vector2D::Dot(query_result.hit_normal, wasd) >= EPSIRON
				)
			{
				player.AddMovementInput(wasd);
			}
		}

		// ジャンプ
		constexpr int JUMP_INPUT_KEY = KEY_INPUT_SPACE;
		_is_jump_key_active = DeviceInput::IsActive(JUMP_INPUT_KEY);

		if (DeviceInput::IsPressed(JUMP_INPUT_KEY))
		{
			// 壁蹴りジャンプ
			bool done_wall_kick_jump = false;
			const bool is_falling = player.IsFalling();
			const bool has_input_toward_facing_dir = 
				facing_direction == CharaDirection::LEFT && DeviceInput::IsActive(KEY_INPUT_A) ||
				facing_direction == CharaDirection::RIGHT && DeviceInput::IsActive(KEY_INPUT_D);
			if (is_falling && has_input_toward_facing_dir)
			{
				const bool wall_jump_to_right = facing_direction == CharaDirection::LEFT;
				// 右に壁ジャンプするには, 左側に壁が必要
				const Vector2D query_rect_center =
					body_collider->GetCenterWorldPosition() + (wall_jump_to_right ? Vector2D{ -half_body_width, 0 } : Vector2D{ half_body_width, 0 });
				const Vector2D query_rect_extent{ 6.f, body_collider->GetBoxExtent().y * 0.3f };

				QueryResult_MultiAARectTrace query_result{};
				CollisionQueryParams_RectAA query_params{};
				query_params.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::GROUND);
				query_params.ignore_actors.push_back(&player);
				query_params.rect = FRectAA(query_rect_center, query_rect_extent.x, query_rect_extent.y);
				player.GetScene()->MultiAARectTrace(query_result, query_params);

				// 壁蹴りジャンプ実行
				if (query_result.has_hit)
				{
					CharacterMovementComponent* movement = player.GetCharacterMovementComponent();
					const float jump_speed = movement->_jump_speed;
					const float initial_speed_x = 400;

					const Vector2D delta_velocity = wall_jump_to_right ? Vector2D{ initial_speed_x, -jump_speed } : Vector2D{ -initial_speed_x, -jump_speed };
					movement->SetVelocity(Vector2D{ movement->GetVelocity().x, 0.f });

					const Vector2D impulse = movement->GetImpulseToAddVelocity(delta_velocity);
					movement->AddImpulse(impulse);

					player.SetForcedFacingDirection(wall_jump_to_right ? CharaDirection::RIGHT : CharaDirection::LEFT);

					player.SetMovementInputEnabled(false);

					player._sound_effects->wallkick->Play();

					done_wall_kick_jump = true;
				}
			}
			
			if (!done_wall_kick_jump)
			{
				player.Jump();
			}
		}
	}

	if (DeviceInput::IsReleased(KEY_INPUT_SPACE))
	{
		player.StopJumping();
	}

	if (DeviceInput::IsActive(KEY_INPUT_LSHIFT))
	{
		player.EnableSprint();
	}
	else
	{
		player.DisableSprint();
	}

	// アイテム切り替え
	if (DeviceInput::IsPressed(KEY_INPUT_RIGHT))
	{
		player.SwitchItem();
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_LEFT))
	{
		player.SwitchItem(true);
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_UP))
	{
		player.UseCurrentItem();
	}

	// 攻撃
	using PlayerAttackInfo = Player::PlayerAttackInfo;
	if (DeviceInput::IsPressed(KEY_INPUT_RSHIFT) && !DeviceInput::IsActive(KEY_INPUT_S))
	{
		Vector2D wasd = DeviceInput::GetInputDir_WASD();
		PlayerAttackInfo attack_info{};
		if (wasd.y < -EPSIRON)
		{
			attack_info.attack_direction = PlayerAttackInfo::AttackDirection_Up;
		}
		else if (fabsf(wasd.x) > EPSIRON) 
		{
			attack_info.attack_direction =
				wasd.x < 0 ? PlayerAttackInfo::AttackDirection_Left : PlayerAttackInfo::AttackDirection_Right;
		}
		else
		{
			attack_info.attack_direction =
				facing_direction == CharaDirection::LEFT ? PlayerAttackInfo::AttackDirection_Left : PlayerAttackInfo::AttackDirection_Right;
		}

		player.Attack(attack_info);
	}

	// リフレクタ―
	if (DeviceInput::IsPressed(KEY_INPUT_RSHIFT) && DeviceInput::IsActive(KEY_INPUT_S))
	{
		player.TryToActivateReflector();
	}
	else if (player.IsReflectorActive() && !DeviceInput::IsActive(KEY_INPUT_RSHIFT))
	{
		player.DeactivateReflector();
	}
}

void PlayerState_Playing::StompCharacter(Player& player, Character * const stomped_character)
{
	CharacterMovementComponent* movement = player.GetCharacterMovementComponent();
	if (CanStompingJump())
	{
		player.Jump(true);
	}
	else
	{
		movement->SetMovementMode(CharacterMovementMode::Falling);
		movement->AddImpulse(movement->GetImpulseToAddVelocity(Vector2D{ 0.f, -500.f }));
		player.SetMovementInputEnabled(false);
	}

	stomped_character->ApplyDamage(player.GetStompDamageInfo());
}

void PlayerState_Playing::ShowControls()
{
	constexpr ImVec2 CONTROLS_WINDOW_SIZE{ 300,300 };
	constexpr ImVec2 CONTROLS_WINDOW_POS{ WINDOW_SIZE_X, 100};
	constexpr ImVec2 CONTROLS_WINDOW_PIVOT{ 1, 0 };
	constexpr ImGuiWindowFlags CONTROLS_WINDOW_FLAGS =
		ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowPos(CONTROLS_WINDOW_POS, ImGuiCond_Always, CONTROLS_WINDOW_PIVOT);
	ImGui::SetNextWindowSize(CONTROLS_WINDOW_SIZE, ImGuiCond_Always);
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(MasterDataID(1)));

		ImGui::Begin("str_id_ingame_player_controls", NULL, CONTROLS_WINDOW_FLAGS);
		ImGui::Text(u8"Tab: 操作説明を非表示");
		ImGui::Text(u8"R: ステージをやり直す");
		ImGui::Text(u8"Esc: ポーズ");
		ImGui::Text(u8"W,D: 移動");
		ImGui::Text(u8"Space: ジャンプ");
		ImGui::Text(u8"Shift: ダッシュ");
		ImGui::Text(u8"RShift: 攻撃");
		ImGui::Text(u8"W + RShift: 上方向攻撃");
		ImGui::Text(u8"S + RShift: リフレクター");
		ImGui::Text(u8"Up: アイテム使用");
		ImGui::Text(u8"Left, Right: アイテム切り替え");

		ImGui::PopFont();
	}
	ImGui::End();
}
