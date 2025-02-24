#pragma once

#include "Scene/StageInteractiveScene/StageInteractiveScene.h"
#include "Scene/SceneState/SceneState.h"
#include <vector>
#include <string>
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <array>
#include <memory>

class Stage;
class Actor;
class Player;
class InGameSceneStateStack;

class InGameScene : public StageInteractiveScene
{
	friend class InGameSceneState_Playing;
	friend class InGameSceneState_Paused;
	friend class InGameSceneState_GameOver;
	friend class InGameSceneState_StageCleared;

public:
	InGameScene();
	virtual ~InGameScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::INGAME_SCENE; }
	virtual void UpdateCameraParams(const float delta_seconds) override;
protected:
	virtual void OnAddedActor(Actor* new_actor) override;
	virtual void OnRemovedActor(Actor* removed_actor) override;
	virtual void PreDestroyActor(Actor* destroyee) override;
	//~ End SceneBase interface

	//~ Begin StageInteractiveScene interface
protected:
	// virtual void BuildStage(const Stage& stage) override;
	//~ End StageInteractiveScene interface

public:
	void AddScore(int score);
	int GetScore() const;
	void SetTimerStopped(const bool is_stopped) { _is_timer_stopped = is_stopped; }

private:
	/// <summary>
	/// <para>スポーンエリア: カメラを中心とする円形のエリア.</para>
	/// <para>アクターは, 初期位置を中心とした境界円が初めてスポーンエリアと重なったときにスポーンされ, </para>
	/// <para>初期位置を現在位置とした境界円がスポーンエリアから外れたときにデスポーンされる.</para>
	/// <para>なお, Actor::IsDespawnable()がfalseの場合, スポーンエリアから外れてもデスポーンされない.</para>
	/// </summary>
	const float _spawn_area_radius;
	FCircle GetSpawnArea() const;
	// 前フレームでアクターのスポーン位置がスポーンエリア内にあったか
	std::unordered_map<Actor*, bool> _was_actor_init_pos_in_spawn_area;

	bool _is_end_scene_requested;
	SceneType _destination_scene;
	void EndInGameScene();

	bool _is_retry_requested;
	void RetryStage();

	std::unique_ptr<InGameSceneStateStack> _state_stack;

	int _total_score;

	float _remaining_time;
	bool _is_timer_stopped;
	bool _has_hurried_player;
	static constexpr float HURRY_TIME = 30.f;
	void HurryPlayer();

	std::shared_ptr<SoundInstance> _sound_instance_bgm;
	void SetupBGM();
};

template<>
struct SceneBase::traits<InGameScene>
{
	typedef StageInteractiveSceneInitialParams initial_params_type;
};