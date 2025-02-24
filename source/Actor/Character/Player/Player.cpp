#include "Player.h"
#include "Component/Renderer/Animator/PlayerAnimatorComponent.h"
#include "PlayerAnimState.h"
#include "PlayerStatesInclude.h"
#include "Component/CharacterMovementComponent.h"
#include "Actor/Mapchip/Gimmick/GoalFlag.h"
#include "Input/DeviceInput.h"
#include "GameSystems/Sound/SoundManager.h"
#include "Scene/StageInteractiveScene/InGameScene/InGameScene.h"
#include <iomanip>
#include <random>

// キャラクター移動関係
namespace
{
	constexpr float GRAVITY_SCALE = 2.25f;
	constexpr float JUMP_PEAK_HEIGHT = UNIT_TILE_SIZE * 5;
	constexpr float WALK_SPEED_SPRINT = 450.f;
	constexpr float ACCELLERATION_SPRINT = 750.f;
	constexpr float AIR_CONTROL_SPRINT = 0.5f;
	constexpr float WALK_SPEED_NORMAL = 300.f;
	constexpr float ACCELLERATION_NORMAL = 500.f;
	constexpr float AIR_CONTROL_NORMAL = 0.5f;
	constexpr float ATTACK_WIDTH_SPEED_THRESHOLD = 350.f;
}

// UI関係
namespace
{
	constexpr int ITEM_BOX_SIZE = 96;
	constexpr int ITEM_BOX_LEFT = 20;
	constexpr int ITEM_BOX_TOP = 20;
	constexpr int ITEM_BOX_BORDER_COLOR = 0xFFFF00;
	constexpr int ITEM_BOX_BORDER_THICKNESS = 2;
	constexpr int BOX_ALPHA = 96;
	constexpr DrawBlendInfo ITEM_BOX_BLEND_INFO{ DX_BLENDMODE_ALPHA, static_cast<uint8_t>(BOX_ALPHA) };
	constexpr int ITEM_NAME_FONT_SIZE = 16;
	constexpr int item_name_cx = ITEM_BOX_LEFT + ITEM_BOX_SIZE / 2;
	constexpr int item_name_cy = ITEM_BOX_TOP + ITEM_NAME_FONT_SIZE / 2;

	// HP, SP
	constexpr int BAR_HEIGHT = 18;
	constexpr int BAR_LENGTH = 256;
	constexpr int BAR_LEFT = 128;
	constexpr int BAR_TOP_HP = 20;
	constexpr int BAR_TOP_SP = BAR_TOP_HP + BAR_HEIGHT + 8;
	constexpr int BAR_BORDER_COLOR = 0xAAAAAA;
	constexpr int BAR_BORDER_THICKNESS = 2;
	constexpr int BAR_BG_COLOR = 0x0;
	constexpr int BAR_BG_ALPHA = 128;
	constexpr int HP_COLOR = 0x55FF55;
	constexpr int SP_COLOR = 0xFFFF00;
	constexpr DrawBlendInfo bar_bg_blend_info = DrawBlendInfo(DX_BLENDMODE_ALPHA, BAR_BG_ALPHA);

	constexpr int COIN_ICON_LEFT = BAR_LEFT;
	constexpr int COIN_TOP = BAR_TOP_SP + BAR_HEIGHT + 8;
	constexpr int COIN_SIZE = 24;
	constexpr int FONT_SIZE = 24;
}

// プレイヤーのアニメーション関係
namespace
{
	constexpr PlayerAnimPlayInfos anim_infos_default{
		AnimPlayInfo{MasterDataID(33), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(34), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(35), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(36), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(37), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(38), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(39), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(65), 1.f, 1.f, FALSE, FALSE}
	};

	constexpr PlayerAnimPlayInfos anim_infos_picohammer
	{
		AnimPlayInfo{MasterDataID(40), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(41), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(42), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(43), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(44), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(45), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(46), 1.f, 1.f, FALSE, FALSE},
		AnimPlayInfo{MasterDataID(66), 1.f, 1.f, FALSE, FALSE}
	};

	std::unordered_map<uint8_t,AnimState<PlayerAnimatorComponent>> MakePlayerAnimStateMapping(const PlayerAnimPlayInfos& anim_infos)
	{
		std::unordered_map<uint8_t, AnimState<PlayerAnimatorComponent>> anim_state_mapping;
		anim_state_mapping[PlayerAnimStateID_Idle] = AnimState<PlayerAnimatorComponent>{ anim_infos.idle, PlayerAnimStateUpdate_Idle };
		anim_state_mapping[PlayerAnimStateID_Run] = AnimState<PlayerAnimatorComponent>{ anim_infos.run, PlayerAnimStateUpdate_Run };
		anim_state_mapping[PlayerAnimStateID_Jump] = AnimState<PlayerAnimatorComponent>{ anim_infos.jump, PlayerAnimStateUpdate_Jump };
		anim_state_mapping[PlayerAnimStateID_Fall] = AnimState<PlayerAnimatorComponent>{ anim_infos.fall, PlayerAnimStateUpdate_Fall };
		anim_state_mapping[PlayerAnimStateID_Magic] = AnimState<PlayerAnimatorComponent>{ anim_infos.magic, PlayerAnimStateUpdate_Magic };
		return anim_state_mapping;
	}
}

// その他
namespace
{
	constexpr int REFLECTOR_ACTIVATION_SP = 1;	// リフレクタ―起動に必要なSP
	constexpr int REFOECTOR_SP_CONSUMPTION_SPEED = 10;	// リフレクタ―が1秒あたりに消費するSP
}

Player::Player()
	: _player_animator(nullptr)
	, _num_coins(0)
	, _max_sp(1)
	, _sp(1)
	, _current_equipment(EquipmentType::None)
	, current_item_type(0)
	, _ui_graph_handles()
	, _can_attack(true)
	, _can_apply_movement_input(true)
	, _is_sprinting(false)
	, _reflector_collider(nullptr)
	, _reflector_scene_anim_index(0)
{}

Player::~Player()
{
}

void Player::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	auto player_params = dynamic_cast<const initial_params_of_actor_t<Player>*>(actor_params);
	_max_sp = player_params->_max_sp;
	_sp = _max_sp;

	LoadIconHandles();

	LoadPlayerSoundEffects();

	CharacterMovementComponent* movement = GetCharacterMovementComponent();
	const float jump_speed = sqrt(2.f * GetScene()->GetGravityForce().y * GRAVITY_SCALE * JUMP_PEAK_HEIGHT) + 50;

	movement->_gravity_scale = GRAVITY_SCALE;
	movement->_jump_speed = jump_speed;

	_player_animator = CreateComponent<PlayerAnimatorComponent>(this);
	using AnimStateType = AnimState<PlayerAnimatorComponent>;
	_player_animator->SetAnimStateMapping(MakePlayerAnimStateMapping(anim_infos_default), PlayerAnimStateID_Idle);
	_player_animator->SetLocalPosition(GetBodyCollider()->GetLocalPosition() + Vector2D{0, -20});
	_player_animator->SetReverseX(GetFacingDirection() == Direction::LEFT);

	// リフレクタ―
	{
		const Vector2D reflector_size{ 96, 96 };
		_reflector_collider = CreateComponent<BoxCollider>(this);
		_reflector_collider->SetBoxColliderParams(
			CollisionType::OVERLAP,
			CollisionObjectType::PLAYER,
			{ CollisionObjectType::ENEMY },
			false,
			reflector_size
		);

		const Vector2D body_pos = GetBodyCollider()->GetLocalPosition();
		const Vector2D body_size = GetBodyCollider()->GetBoxExtent();
		const Vector2D reflector_local_pos{ body_pos.x, body_pos.y + (body_size.y - reflector_size.y) * 0.5f };
		_reflector_collider->SetLocalPosition(reflector_local_pos);
	}

	CreatePlayerStates(_player_states);
	_state_stack = std::make_unique<PlayerStateStack>();
	_state_stack->ChangeState(*this, _player_states.emerging);

	_last_position = GetActorWorldPosition();
	_delta_position = Vector2D{};
}

void Player::TickActor(float delta_seconds)
{
	__super::TickActor(delta_seconds);
	_state_stack->Tick(*this, delta_seconds);
	_delta_position = GetActorWorldPosition() - _last_position;
	_last_position = GetActorWorldPosition();
	_player_animator->SetReverseX(GetFacingDirection() == Direction::LEFT);
}

void Player::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);
	_state_stack->Draw(*this, camera_params);
}

void Player::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);
	_state_stack->DrawForeground(*this, canvas_info);
}

void Player::Finalize()
{
	_player_animator = nullptr;
	_sound_effects.reset();
	_num_coins = 0;
	_max_sp = 1;
	_sp = 1;
	_current_equipment = EquipmentType::None;
	current_item_type = 0;
	_ui_graph_handles = {};
	_can_attack = true;
	_can_apply_movement_input = true;
	_is_sprinting = false;
	_reflector_collider = nullptr;

	__super::Finalize();
}

void Player::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	// ゴールとの接触
	Actor* other_actor = hit_result.other_collider->GetOwnerActor();
	if (other_actor->IsDerivedFrom<GoalFlag>())
	{
		const int goal_draw_priority = other_actor->GetDrawPriority();
		if (goal_draw_priority >= GetDrawPriority())
		{
			SetDrawPriority(goal_draw_priority + 1);
		}

		const Vector2D pos = other_actor->GetActorWorldPosition() + Vector2D{0.f, -32.f};
		GetScene()->PlayAnimation(
			AnimPlayInfo{ MasterDataID(20),1.f,1.5f,FALSE,FALSE },
			Transform{ pos, 0.f }
		);

		// ゴール位置にプレイヤーを固定
		SetActorWorldPosition(other_actor->GetActorWorldPosition() + Vector2D{0, -10});
		GetCharacterMovementComponent()->SetMovementMode(CharacterMovementMode::Walking);
		GetCharacterMovementComponent()->Sleep();

		Goal();
	}

	// リフレクターとの衝突
	if (hit_result.self_collider == _reflector_collider)
	{
		Character* other_character = dynamic_cast<Character*>(hit_result.other_collider->GetOwnerActor());
		if (other_character)
		{
			const Vector2D to_other_character = other_character->GetActorWorldPosition() - GetActorWorldPosition();
			const Vector2D other_velocity = other_character->GetVelocity();

			if (Vector2D::Dot(to_other_character, other_velocity) < 0.f)
			{
				const Vector2D new_velocity = other_velocity * -1.f;
				other_character->SetVelocity(new_velocity);
				other_character->SetFacingDirection(new_velocity.x > 0.f ? Direction::RIGHT : Direction::LEFT);
			}
		}
	}
}

void Player::RequestToSetActorHidden(const bool new_hidden)
{
	// プレイヤーは非表示化しない
}

void Player::TakeDamage(const DamageInfo& damage_info)
{
	__super::TakeDamage(damage_info);

	SetIsUndamageable(true);
	GetBodyCollider()->RemoveHitTarget(CollisionObjectType::ENEMY);
	_player_animator->SetBlendValue(128);

	GetScene()->MakeDelayedEventWorld(this, 1.f, [this]() 
		{
			SetIsUndamageable(false);
			GetBodyCollider()->AddHitTarget(CollisionObjectType::ENEMY);
			_player_animator->SetBlendValue(255);
		}
	);

	_player_animator->SetOverrideAnimation(GetAnimPlayInfoForCurrentEquipment().damaged);

	_sound_effects->damaged->Play();
}

void Player::StompCharacter(Character* const stomped_character)
{
	_state_stack->StompCharacter(*this, stomped_character);

	_sound_effects->stomp->Play();
}

void Player::OnJump()
{
	__super::OnJump();

	_player_animator->ChangeState(PlayerAnimStateID_Jump);

	_sound_effects->jump->Play();

	_can_attack = false;
}

void Player::OnLanded()
{
	_can_attack = true;
}

void Player::OnDead(const CharacterDeathInfo* death_info)
{
	__super::OnDead(death_info);

	_state_stack->ChangeState(*this, _player_states.dead);

	_sound_effects->dead->Play();
}

void Player::SetupBodyCollider(BoxCollider* const body_collider)
{
	body_collider->SetBoxColliderParams(
		CollisionType::BLOCK,
		CollisionObjectType::PLAYER,
		{
			CollisionObjectType::GROUND, 
			CollisionObjectType::BARRIER,
			CollisionObjectType::ENEMY,
			CollisionObjectType::ITEM,
			CollisionObjectType::GIMMICK,
			CollisionObjectType::GOAL_FLAG
		},
		true,
		Vector2D(32.f, 54.f)
	);
	body_collider->SetLocalPosition(Vector2D(0, 5 - 16));
}

void Player::LoadIconHandles()
{
	constexpr MasterDataID ICON_ID_COIN = 360;
	_ui_graph_handles.icon_coin = MasterHelper::GetGameIconHandleForDxLib(ICON_ID_COIN);
}

void Player::DrawPlayerUI(const CanvasInfo& canvas_info)
{
	// HP, SP
	DrawHPAndSPBars(canvas_info);

	// アイテム
	DrawItemIcon(canvas_info);

	// コイン
	{

		DrawExtendGraph(COIN_ICON_LEFT, COIN_TOP, COIN_ICON_LEFT + COIN_SIZE, COIN_TOP + COIN_SIZE, _ui_graph_handles.icon_coin, TRUE);
		DrawStringHelper::DrawStringC(
			FONT_SIZE,
			COIN_ICON_LEFT + COIN_SIZE * 1.5,
			COIN_TOP + COIN_SIZE / 2,
			to_tstring(_num_coins).c_str(),
			0xFFFFFF);
	}
}

void Player::DrawItemIcon(const CanvasInfo& canvas_info) const
{
	// アイテムボックスの枠
	{
		BlendDrawHelper::DrawBox(
			ITEM_BOX_BLEND_INFO,
			ITEM_BOX_LEFT,
			ITEM_BOX_TOP,
			ITEM_BOX_LEFT + ITEM_BOX_SIZE,
			ITEM_BOX_TOP + ITEM_BOX_SIZE,
			0x0,
			TRUE
		);

		DxLib::DrawBoxAA(
			ITEM_BOX_LEFT,
			ITEM_BOX_TOP,
			ITEM_BOX_LEFT + ITEM_BOX_SIZE,
			ITEM_BOX_TOP + ITEM_BOX_SIZE,
			ITEM_BOX_BORDER_COLOR,
			FALSE,
			ITEM_BOX_BORDER_THICKNESS
		);
	}

	if (current_item_type == 0)
	{
		// アイテムなしの場合は枠だけ表示
		return;
	}

	// アイテム画像の表示
	constexpr const int item_graph_padding = 4;	// アイテム枠とアイテム画像の間の余白
	const int handle = item_inventory.at(current_item_type).graphic_handle;
	DxLib::DrawExtendGraph(
		ITEM_BOX_LEFT + item_graph_padding,
		ITEM_BOX_TOP + item_graph_padding,
		ITEM_BOX_LEFT + ITEM_BOX_SIZE - item_graph_padding,
		ITEM_BOX_TOP + ITEM_BOX_SIZE - item_graph_padding,
		item_inventory.at(current_item_type).graphic_handle,
		true
	);

	// アイテム名表示
	/*const std::string&  str = MdItem::Get(current_item_type).item_name;
	DrawStringHelper::DrawStringC(ITEM_NAME_FONT_SIZE, item_name_cx, item_name_cy, str.c_str(), 0xFFFFFF);*/

	// 所持数
	DxLib::DrawFormatString(ITEM_BOX_LEFT + ITEM_BOX_SIZE - 10, ITEM_BOX_TOP + ITEM_BOX_SIZE - 10, 0xFFFFFF, _T("x%d"), item_inventory.at(current_item_type).count);
}

void Player::DrawHPAndSPBars(const CanvasInfo& canvas_info) const
{
	// HPバー
	{
		// HP border
		DxLib::DrawBoxAA(
			BAR_LEFT - BAR_BORDER_THICKNESS + 1,
			BAR_TOP_HP - BAR_BORDER_THICKNESS + 1,
			BAR_LEFT + BAR_LENGTH + BAR_BORDER_THICKNESS - 1,
			BAR_TOP_HP + BAR_HEIGHT + BAR_BORDER_THICKNESS - 1,
			BAR_BORDER_COLOR,
			FALSE,
			BAR_BORDER_THICKNESS
		);

		// HP background
		BlendDrawHelper::DrawBox(
			bar_bg_blend_info,
			BAR_LEFT,
			BAR_TOP_HP,
			BAR_LEFT + BAR_LENGTH,
			BAR_TOP_HP + BAR_HEIGHT,
			BAR_BG_COLOR,
			TRUE
		);

		// HP bar
		const int hp_width = static_cast<int>(BAR_LENGTH * static_cast<float>(GetHP()) / GetMaxHP());
		DxLib::DrawBox(
			BAR_LEFT,
			BAR_TOP_HP,
			BAR_LEFT + hp_width,
			BAR_TOP_HP + BAR_HEIGHT,
			HP_COLOR,
			TRUE
		);

		// (現在HP) / (MAXHP)
		const tstring hp_str = to_tstring(GetHP()) + _T(" / ") + to_tstring(GetMaxHP());
		constexpr int str_left = BAR_LEFT + BAR_LENGTH + 10;
		constexpr int str_top = BAR_TOP_HP + BAR_HEIGHT / 2 - FONT_SIZE / 2;
		DrawString(str_left, str_top, hp_str.c_str(), 0xFFFFFF);
	}

	// SPバー
	{
		// SP border
		DxLib::DrawBoxAA(
			BAR_LEFT - BAR_BORDER_THICKNESS + 1,
			BAR_TOP_SP - BAR_BORDER_THICKNESS + 1,
			BAR_LEFT + BAR_LENGTH + BAR_BORDER_THICKNESS - 1,
			BAR_TOP_SP + BAR_HEIGHT + BAR_BORDER_THICKNESS - 1,
			BAR_BORDER_COLOR,
			FALSE,
			BAR_BORDER_THICKNESS
		);

		// SP background
		BlendDrawHelper::DrawBox(
			bar_bg_blend_info,
			BAR_LEFT,
			BAR_TOP_SP,
			BAR_LEFT + BAR_LENGTH,
			BAR_TOP_SP + BAR_HEIGHT,
			BAR_BG_COLOR,
			TRUE
		);

		// SP bar
		const int sp_width = static_cast<int>(BAR_LENGTH * static_cast<float>(_sp) / _max_sp);
		DxLib::DrawBox(
			BAR_LEFT,
			BAR_TOP_SP,
			BAR_LEFT + sp_width,
			BAR_TOP_SP + BAR_HEIGHT,
			SP_COLOR,
			TRUE
		);

		// (現在SP) / (MAXSP)
		const tstring sp_str = to_tstring(_sp) + _T(" / ") + to_tstring(_max_sp);
		constexpr int str_left = BAR_LEFT + BAR_LENGTH + 10;
		constexpr int str_top = BAR_TOP_SP + BAR_HEIGHT / 2 - FONT_SIZE / 2;
		DrawString(str_left, str_top, sp_str.c_str(), 0xFFFFFF);
	}
}

void Player::SwitchItem(const bool select_prev)
{
	if (item_inventory.size() <= 1)
	{
		return;
	}

	if (select_prev) 
	{
		auto it = item_inventory.find(current_item_type);
		if (it == item_inventory.begin())
		{
			it = item_inventory.end();
		}
		current_item_type = (--it)->first;
	}
	else
	{
		auto it = item_inventory.find(current_item_type);
		++it;
		if (it == item_inventory.end())
		{
			it = item_inventory.begin();
		}
		current_item_type = it->first;
	}
}

void Player::UseCurrentItem()
{
	if (current_item_type == 0)
	{
		return;
	}

	if (item_inventory.at(current_item_type).count <= 0)
	{
		throw std::runtime_error("Player::UseCurrentItem: No item");
	}

	// 現在のアイテムを使い切った場合、次のアイテムを選択する
	auto it_next_item = ++(item_inventory.find(current_item_type));
	if (it_next_item == item_inventory.end())
	{
		it_next_item = item_inventory.begin();
	}

	ItemCallbackArgs args = {};
	args.world = _ingame_scene_ref;
	args.player = this;

	item_inventory.at(current_item_type).use_action(args);

	item_inventory.at(current_item_type).count--;

	_sound_effects->use_item->Play();

	if(item_inventory.at(current_item_type).count == 0)
	{
		const MasterDataID next_item_id = it_next_item->first;
		item_inventory.erase(current_item_type);
		current_item_type = current_item_type != next_item_id ? next_item_id : 0;
	}
}

void Player::ChangeEquipment(const EquipmentType new_equipment_type)
{
	_current_equipment = new_equipment_type;
	_player_animator->SetAnimStateMapping(
		MakePlayerAnimStateMapping(GetAnimPlayInfoForCurrentEquipment()),
		PlayerAnimStateID_Idle
	);
}

const PlayerAnimPlayInfos& Player::GetAnimPlayInfoForCurrentEquipment() const
{
	switch (_current_equipment)
	{
	case EquipmentType::None:
		return anim_infos_default;
	case EquipmentType::PicoHammer:
		return anim_infos_picohammer;
	}

	throw std::runtime_error("Player::GetAnimPlayInfoForCurrentEquipment: Unknown equipment type");
}

DamageInfo Player::GetAttackDamageInfoForCurrentEquipment()
{
	switch (_current_equipment)
	{
	case EquipmentType::None:
	{
		DamageInfo damage_info;
		damage_info.damage_causer = this;
		damage_info.damage_value = 100;		// TODO: ダメージ量のパラメータ化
		damage_info.damage_type = DamageType::Punch;
		damage_info.punch_damage_info = DamageInfo::PunchDamageInfo();
		return damage_info;
	}

	case EquipmentType::PicoHammer:
	{
		DamageInfo damage_info;
		damage_info.damage_causer = this;
		damage_info.damage_value = 200;		// TODO: ダメージ量のパラメータ化
		damage_info.damage_type = DamageType::Crush;
		return damage_info;
	}
	}

	throw std::runtime_error("Player::GetAttackDamageInfoForCurrentEquipment: Unknown equipment type");
}

DamageInfo Player::GetStompDamageInfo()
{
	DamageInfo stomp_damage_info{};
	stomp_damage_info.damage_type = DamageType::Stomp;
	stomp_damage_info.damage_value = 100;	// TODO: ダメージ量のパラメータ化
	stomp_damage_info.damage_causer = this;
	stomp_damage_info.stomp_damage_info = DamageInfo::StompDamageInfo();
	return stomp_damage_info;
}

void Player::LoadPlayerSoundEffects()
{
	_sound_effects = std::make_unique<PlayerSoundEffects>();

	// 下のsound_volume_mapに登録されていないサウンドのデフォルトボリューム
	constexpr int DEFAULT_SE_VOLUME = 50;

	const std::unordered_map<std::shared_ptr<SoundInstance>, int> sound_volume_map =
	{
		{_sound_effects->get_item, 30},
		{_sound_effects->jump, 30},
		{_sound_effects->get_coin, 40},
		{_sound_effects->stomp, 100},
		{_sound_effects->attack, 100},
		{_sound_effects->wallkick, 30},
		{_sound_effects->use_item, 100},
	};

	const std::string ingame_scene_se_dir = "resources/sounds/se/ingame_scene/";
	_sound_effects->attack = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_attack_2.mp3");
    _sound_effects->damaged = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_damaged.ogg");
    _sound_effects->dead = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_dead.ogg");
    _sound_effects->get_coin = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_get_coin.ogg");
    _sound_effects->get_item = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_get_item.ogg");
    _sound_effects->reach_goal = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_reach_goal.ogg");
    _sound_effects->jump = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_jump.ogg");
    _sound_effects->reflector = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_reflector.ogg");
    _sound_effects->stomp = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_stomp.ogg");
    _sound_effects->tackled = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_tackled.ogg");
    _sound_effects->use_item = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_use_item.ogg");
    _sound_effects->wallkick = SoundManager::GetInstance().MakeSoundInstance(ingame_scene_se_dir + "se_player_wallkick.ogg");
	// TODO: プレイヤー効果音の追加

	for (auto& sound : _sound_effects->GetList())
	{
		if (sound_volume_map.find(sound) != sound_volume_map.end())
		{
			sound->SetVolume(sound_volume_map.at(sound));
		}
		else
		{
			sound->SetVolume(DEFAULT_SE_VOLUME);
		}
	}
}

void Player::Attack(const PlayerAttackInfo& attack_info)
{
	if (!_can_attack)
	{
		return;
	}

	// TODO: 必要であれば歩行時以外の状態での攻撃を実装
	if (GetCharacterMovementComponent()->GetMovementMode() != CharacterMovementMode::Walking)
	{
		return;
	}

	const Direction facing_direction = GetFacingDirection();
	const Vector2D body_collider_center = GetBodyCollider()->GetCenterWorldPosition();
	const Vector2D body_collider_extent = GetBodyCollider()->GetBoxExtent();

	// 攻撃のクールタイム設定
	{
		constexpr float COOL_TIME = 0.3f;
		_can_attack = false;
		_can_apply_movement_input = false;
		SetMovementInputEnabled(false);
		const Direction forced_facing_direction = [&]() -> Direction
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Left:
					return Direction::LEFT;
				case PlayerAttackInfo::AttackDirection_Right:
					return Direction::RIGHT;
				default:
					return facing_direction;
				}
			}();
		SetForcedFacingDirection(forced_facing_direction);
		GetScene()->MakeDelayedEventWorld(this, COOL_TIME, [this]()
			{
				_can_attack = true;
				_can_apply_movement_input = true;
				SetIsFacingDirectionForced(false);
				SetMovementInputEnabled(true);
			});
	}

	// 攻撃モーション
	{
		switch (attack_info.attack_direction)
		{
		case PlayerAttackInfo::AttackDirection_Left:
		case PlayerAttackInfo::AttackDirection_Right:
			_player_animator->SetOverrideAnimation(GetAnimPlayInfoForCurrentEquipment().attack_right);
			break;
		case PlayerAttackInfo::AttackDirection_Up:
			_player_animator->SetOverrideAnimation(GetAnimPlayInfoForCurrentEquipment().attack_upper);
		}

	}

	// 攻撃エフェクト
	{
		constexpr MasterDataID ATTACK_EFFECT_ID = 3;
		constexpr float OFFSET_FROM_BODY = 40;
		const Vector2D body_local_pos = GetBodyCollider()->GetLocalPosition();

		const float effect_local_x = [&]() -> float
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Left:
					return body_local_pos.x - body_collider_extent.x * 0.5f - OFFSET_FROM_BODY;
				case PlayerAttackInfo::AttackDirection_Right:
					return body_local_pos.x + body_collider_extent.x * 0.5f + OFFSET_FROM_BODY;
				default:
					return body_local_pos.x;
				}
			}();

		const float effect_local_y = [&]() -> float
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Up:
					return body_local_pos.y - body_collider_extent.y * 0.5f;
				default:
					return body_local_pos.y;
				}
			}();

		const float effect_world_rotation = [&]() -> float
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Left:
					return CLN2D_HALF_PI;
				case PlayerAttackInfo::AttackDirection_Right:
					return -CLN2D_HALF_PI;
				case PlayerAttackInfo::AttackDirection_Up:
					return CLN2D_PI;
				default:
					return 0.f;
				}
			}();
		
		const Transform this_transform = GetActorWorldTransform();
		GetScene()->PlayAnimation(
			AnimPlayInfo{ MasterDataID(ATTACK_EFFECT_ID),1.f,1.f,FALSE, FALSE},
			Transform{ Vector2D{effect_local_x, effect_local_y}, this_transform.InverseTransformRotation(effect_world_rotation) },
			this
		);
	}

	// 攻撃音
	_sound_effects->attack->Play();

	// 攻撃対象の取得
	QueryResult_MultiAARectTrace query_result{};
	{
		// TODO: パラメータ化
		const Vector2D query_rect_extent =
			attack_info.attack_direction == PlayerAttackInfo::AttackDirection_Up ?
			Vector2D(64, 64) : Vector2D(96, 64);

		// クエリ矩形はbody_colliderに接するように配置
		const float query_rect_center_y = [&]() -> float
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Up:
					return body_collider_center.y - body_collider_extent.y * 0.5f - query_rect_extent.y * 0.5f;
				default:
					return body_collider_center.y;
				}
			}();

		const float query_rect_center_x = [&]() -> float
			{
				switch (attack_info.attack_direction)
				{
				case PlayerAttackInfo::AttackDirection_Left:
					return body_collider_center.x - body_collider_extent.x * 0.5f - query_rect_extent.x * 0.5f;
				case PlayerAttackInfo::AttackDirection_Right:
					return body_collider_center.x + body_collider_extent.x * 0.5f + query_rect_extent.x * 0.5f;
				default:
					return body_collider_center.x;
				}
			}();

		const Vector2D query_rect_center{ query_rect_center_x, query_rect_center_y };

		CollisionQueryParams_RectAA query_params;
		query_params.hit_object_types = static_cast<CollisionObjectType_UnderlyingType>(CollisionObjectType::WILDCARD);
		query_params.ignore_actors.push_back(this);
		query_params.rect = FRectAA(query_rect_center, query_rect_extent.x, query_rect_extent.y);
		GetScene()->MultiAARectTrace(query_result, query_params);
	}

	// 攻撃対象へのダメージ適用
	std::unordered_set<Actor*> hit_actors;
	for (auto& collider : query_result.hit_colliders)
	{
		hit_actors.insert(collider->GetOwnerActor());
	}

	for (auto& actor : hit_actors)
	{
		DamageInfo damage_info = GetAttackDamageInfoForCurrentEquipment();

		// キャラクターに対してはノックバックを適用
		if (Character* as_character = dynamic_cast<Character*>(actor))
		{
			CharacterMovementComponent* movement = as_character->GetCharacterMovementComponent();
			const float sign_impulse_x = facing_direction == Direction::LEFT ? -1.f : 1.f;
			const float knock_back_speed_x = fabsf(GetVelocity().x) + 900.f;		// TODO: パラメータ化
			constexpr float knock_back_velocity_y = -300.f;	// TODO: パラメータ化
			const Vector2D impulse = movement->GetImpulseToAddVelocity(Vector2D{ sign_impulse_x * knock_back_speed_x, knock_back_velocity_y });

			if(damage_info.damage_type == DamageType::Punch)
			{
				damage_info.punch_damage_info.impulse[0] = impulse.x;
				damage_info.punch_damage_info.impulse[1] = impulse.y;
			}
			else if (damage_info.damage_type == DamageType::Crush)
			{
				damage_info.crush_damage_info.impulse[0] = impulse.x;
				damage_info.crush_damage_info.impulse[1] = impulse.y;
			}
		}
		
		actor->ApplyDamage(damage_info);
	}
}

void Player::TryToActivateReflector()
{
	if (IsReflectorActive() || GetSP() < REFLECTOR_ACTIVATION_SP)
	{
		return;
	}

	SetSP(GetSP() - REFLECTOR_ACTIVATION_SP);

	_reflector_collider->Activate();

	_reflector_scene_anim_index = GetScene()->PlayAnimation(
		AnimPlayInfo{ MasterDataID(30),1.f,1.f,FALSE,FALSE },
		Transform{ Vector2D{0, -32}, 0.f },
		this
	);

	SetIsUndamageable(true);

	GetScene()->MakeRepeatingEventWorld(this, 1.f / REFOECTOR_SP_CONSUMPTION_SPEED, [this]() 
		{
			if (!_reflector_collider->IsActive())
			{
				return false;
			}

			SetSP(GetSP() - 1);
			if (GetSP() <= 0)
			{
				SetSP(0);
				DeactivateReflector();
				return false;
			}

			return true;
		});

	_sound_effects->reflector->Play();
}

void Player::DeactivateReflector()
{
	if (!IsReflectorActive())
	{
		return;
	}

	_reflector_collider->Deactivate();

	SetIsUndamageable(false);

	GetScene()->StopAnimation(_reflector_scene_anim_index);
}

bool Player::IsReflectorActive() const
{
	return _reflector_collider->IsActive();
}

void Player::EnableSprint()
{
	_is_sprinting = true;
	GetCharacterMovementComponent()->_max_walk_speed = WALK_SPEED_SPRINT;
	GetCharacterMovementComponent()->_max_accelleration = ACCELLERATION_SPRINT;
	GetCharacterMovementComponent()->_air_control = AIR_CONTROL_SPRINT;
}

void Player::DisableSprint()
{
	_is_sprinting = false;
	GetCharacterMovementComponent()->_max_walk_speed = WALK_SPEED_NORMAL;
	GetCharacterMovementComponent()->_max_accelleration = ACCELLERATION_NORMAL;
	GetCharacterMovementComponent()->_air_control = AIR_CONTROL_NORMAL;
}

void Player::SetDeadAnimation()
{
	_player_animator->SetOverrideAnimation(GetAnimPlayInfoForCurrentEquipment().damaged);
	_player_animator->SetPlaySpeed(0.f);
}


Vector2D Player::GetDeltaPosition() const
{
	return _delta_position;
}

void Player::AddScore(int score)
{
	if (_ingame_scene_ref)
	{
		_ingame_scene_ref->AddScore(score);
	}
}

int Player::GetNumCoins() const
{
	return _num_coins;
}

void Player::AddCoin(int num_additional_coins)
{
	SetNumCoins(GetNumCoins() + num_additional_coins);

	constexpr int COIN_SCORE = 100;
	AddScore(COIN_SCORE);

	_sound_effects->get_coin->Play();
}

void Player::GetItem(const MasterDataID item_id)
{
	AddScore(MdItem::Get(item_id).score);
	AddItemToInventory(item_id);
}

void Player::AddItemToInventory(const MasterDataID item_id)
{
	if (item_inventory.empty())
	{
		current_item_type = item_id;
	}

	if (item_inventory.find(item_id) == item_inventory.end())
	{
		const MdItem md_item = MdItem::Get(item_id);
		const int g_handle = MasterHelper::GetGameIconHandleForDxLib(md_item);
		constexpr int INITIAL_COUNT = 1;
		item_inventory[item_id] = PosessingItemInfo{md_item.GetUseAction(), g_handle, INITIAL_COUNT};
	}
	else
	{
		item_inventory.at(item_id).count++;
	}

	_sound_effects->get_item->Play();
}

void Player::SetNumCoins(int num_coins)
{
	_num_coins = num_coins;
}

void Player::Goal()
{
	player_events.OnPlayerReachedGoal.Dispatch();

	_sound_effects->reach_goal->Play();

	_state_stack->ChangeState(*this, _player_states.goaled);
}

void Player::EquipHammer()
{
	ChangeEquipment(EquipmentType::PicoHammer);
}

void Player::CreatePlayerStates(PlayerStates& player_states)
{
	player_states.emerging = std::make_shared<PlayerState_Emerging>();
	player_states.playing = std::make_shared<PlayerState_Playing>();
	player_states.dead = std::make_shared<PlayerState_Dead>();
	player_states.goaled = std::make_shared<PlayerState_Goaled>();
	// TODO: プレイヤーのステートを追加する場合はここに追記
}

int Player::GetMaxSP() const
{
	return _max_sp;
}

int Player::GetSP() const
{
	return _sp;
}

void Player::SetSP(int new_sp)
{
	_sp = new_sp;
}
