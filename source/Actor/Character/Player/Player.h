#pragma once

#include "Actor/Character/Character.h"
#include "PlayerInitialParams.h"
#include <functional>
#include "GameSystems/MasterData/MasterDataInclude.h"	// for struct ItemCallbackArgs

class PlayerAnimatorComponent;
struct PlayerAnimPlayInfos;
class PlayerState;
class PlayerStateStack;
class ItemActor;
class InGameScene;
class BoxCollider;

CLN2D_GEN_DEFINE_ACTOR()
class Player : public Character
{
	friend class PlayerState_Emerging;
	friend class PlayerState_Playing;
	friend class PlayerState_Dead;
	friend class PlayerState_Goaled;
	// TODO: プレイヤーのステートを追加する場合はここに追記
public:
	constexpr static const int16_t MAX_NUM_COINS = 100;

	// イベント
	struct PlayerEvents
	{
		Event<> OnPlayerEmergenceSequenceFinished;
		Event<> OnPlayerDeathSequenceFinished;
		Event<> OnPlayerReachedGoal;
		Event<> OnPlayerGoalSequenceFinished;
	};
	PlayerEvents player_events;

public:
	Player();
	virtual ~Player();

public:
	//~ Begin Actor interface
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) override;
	virtual void Draw(const CameraParams& camera_params) override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual void OnHitCollision(const HitResult& hit_result) override;
	virtual void RequestToSetActorHidden(const bool new_hidden) override;
	//~ End Actor interface

	//~ Begin Character interface
protected:
	virtual void StompCharacter(Character* const stomped_character) override;
	virtual void TakeDamage(const DamageInfo& damage_info) override;
	virtual void OnJump() override;
	virtual void OnLanded() override;
	virtual void OnDead(const CharacterDeathInfo* death_info) override;

private:
	virtual void SetupBodyCollider(BoxCollider* const body_collider) override;
	//~ End Character interface

public:
	int GetMaxSP() const;
	int GetSP() const;
	void SetSP(int new_sp);

	Vector2D GetDeltaPosition() const;

	void AddScore(int score);

	int GetNumCoins() const;
	// コインを増やしてスコアを加算
	void AddCoin(int num_additional_coins);

	void GetItem(const MasterDataID item_id);
	void EquipHammer();

private:
	void SetNumCoins(int num_coins);

	void Goal();

	// UIに使用するアイコンの画像ハンドルを取得
	void LoadIconHandles();

	// アイテム, HP, SP, コイン数
	void DrawPlayerUI(const CanvasInfo& canvas_info);

	// アイテムアイコンの描画
	void DrawItemIcon(const CanvasInfo& canvas_info) const;

	// HPバーの描画
	void DrawHPAndSPBars(const CanvasInfo& canvas_info) const;

	struct PlayerAttackInfo
	{
		enum AttackDirection
		{
			AttackDirection_Left,
			AttackDirection_Right,
			AttackDirection_Up,
		};

		AttackDirection attack_direction;
	};
	void Attack(const PlayerAttackInfo& attack_info);
	bool _can_attack;
	bool _can_apply_movement_input;

	BoxCollider* _reflector_collider;
	void TryToActivateReflector();
	void DeactivateReflector();
	bool IsReflectorActive() const;
	size_t _reflector_scene_anim_index;

	bool _is_sprinting;
	void EnableSprint();
	void DisableSprint();

	// 無敵状態
	// void EndInvincible();
	// bool IsInvincible() const;

	// レンダラー
	PlayerAnimatorComponent* _player_animator;
	void SetDeadAnimation();

	// ステート
	std::unique_ptr<PlayerStateStack> _state_stack;
	struct PlayerStates
	{
		std::shared_ptr<PlayerState> playing;
		std::shared_ptr<PlayerState> emerging;
		std::shared_ptr<PlayerState> dead;
		std::shared_ptr<PlayerState> goaled;
	};
	PlayerStates _player_states;
	static void CreatePlayerStates(PlayerStates& player_states);

	Vector2D _last_position;
	Vector2D _delta_position;

	// 魔法に使用するSP
	int _max_sp;
	int _sp;

	// コイン所持数
	int _num_coins;

	/// <summary>
	/// 所有アイテム情報
	/// </summary>
	struct PosessingItemInfo
	{
		std::function<void(const ItemCallbackArgs&)> use_action;	// 使用時の処理
		int graphic_handle;	// アイテムスロットに表示する画像のハンドル
		uint16_t count;	// 所持数

		PosessingItemInfo()
			: use_action(decltype(use_action){})
			, graphic_handle(0)
			, count(0)
		{
		}

		/// <param name="use_action_">アイテム使用時の処理</param>
		/// <param name="graphic_handle_">アイコンのグラフィックハンドル</param>
		/// <param name="count_">所持数</param>
		PosessingItemInfo(
			const decltype(use_action)& use_action_,
			const int graphic_handle_,
			const uint16_t count_
		)
			: use_action(use_action_)
			, graphic_handle(graphic_handle_)
			, count(count_)
		{
		}

	};
	std::map<MasterDataID, PosessingItemInfo> item_inventory;	// 所有アイテム情報
	MasterDataID current_item_type;

	// インベントリにアイテムを追加
	void AddItemToInventory(const MasterDataID item_id);

	// アイテムの切換え
	void SwitchItem(const bool select_prev = false);

	// 選択中のアイテムを使用する. 使い切ったアイテム(個数が0)はインベントリから削除される
	void UseCurrentItem();

	enum class EquipmentType
	{
		None,
		PicoHammer
	};
	EquipmentType _current_equipment;
	void ChangeEquipment(const EquipmentType new_equipment_type);
	const PlayerAnimPlayInfos& GetAnimPlayInfoForCurrentEquipment() const;

	DamageInfo GetAttackDamageInfoForCurrentEquipment();
	DamageInfo GetStompDamageInfo();

	struct PlayerUIGraphHandles
	{
		int icon_coin;
	};
	PlayerUIGraphHandles _ui_graph_handles;

	struct PlayerSoundEffects
	{
		std::shared_ptr<SoundInstance> attack;
		std::shared_ptr<SoundInstance> damaged;
		std::shared_ptr<SoundInstance> dead;
		std::shared_ptr<SoundInstance> get_coin;
		std::shared_ptr<SoundInstance> get_item;
		std::shared_ptr<SoundInstance> reach_goal;
		std::shared_ptr<SoundInstance> jump;
		std::shared_ptr<SoundInstance> reflector;
		std::shared_ptr<SoundInstance> stomp;
		std::shared_ptr<SoundInstance> tackled;
		std::shared_ptr<SoundInstance> use_item;
		std::shared_ptr<SoundInstance> wallkick;
		// TODO: プレイヤー効果音の追加

		std::vector<std::shared_ptr<SoundInstance>> GetList() const
		{
			return std::vector<std::shared_ptr<SoundInstance>>
			{
				attack,
				damaged,
				dead,
				get_coin,
				get_item,
				reach_goal,
				jump,
				reflector,
				stomp,
				tackled,
				use_item,
				wallkick,
				// TODO: プレイヤー効果音の追加
			};
		}
	};
	std::unique_ptr<PlayerSoundEffects> _sound_effects;
	void LoadPlayerSoundEffects();
};

template<> struct initial_params_of_actor<Player> { using type = PlayerInitialParams; };