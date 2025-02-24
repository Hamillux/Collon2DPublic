#include "InGameScene.h"
#include "InGameSceneStatesInclude.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "Scene/StageSelectScene/StageSelectScene.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "GameSystems/Sound/SoundManager.h"
#include "Actor/AllActorsInclude_generated.h"
#include "Component/Collider/SegmentCollider.h"
#include "Input/DeviceInput.h"

namespace
{
	constexpr float DEFAULT_SPAWN_AREA_RADIUS = UNIT_TILE_SIZE * 32;
}

InGameScene::InGameScene()
	: _destination_scene(SceneType::NONE)
	, _total_score(0)
	, _spawn_area_radius(DEFAULT_SPAWN_AREA_RADIUS)
	, _is_end_scene_requested(false)
	, _is_retry_requested(false)
	, _remaining_time(0.f)
	, _is_timer_stopped(false)
	, _has_hurried_player(false)
{}

InGameScene::~InGameScene()
{}

void InGameScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	_remaining_time = GetStageRef().GetTimeLimit();

	if (scene_params == nullptr)
	{
		std::cerr << "InGameScene::Initialize: scene_params is nullptr" << std::endl;
		return;
	}

	// UIに使用するアイコンをロード
	_state_stack = std::make_unique<InGameSceneStateStack>();
	_state_stack->ChangeState(*this, std::make_shared<InGameSceneState_Playing>());

	SetupBGM();

	GetPlayerRef()->player_events.OnPlayerEmergenceSequenceFinished.Bind(
		[this]() 
		{
			_sound_instance_bgm->Play();
		},
		this
	);
}

SceneType InGameScene::Tick(float delta_seconds)
{
	SceneType result_scene_type = __super::Tick(delta_seconds);

	if (_is_end_scene_requested)
	{
		return _destination_scene;
	}

	if (_is_retry_requested)
	{
		_is_retry_requested = false;
		
		return SceneType::MSG_RELOAD;
	}

	_state_stack->Tick(*this, delta_seconds);

	for (auto& actor : _actors)
	{
		// スポーン/デスポーンはステージへの生成情報があるアクターに対してのみ行う
		if (!IsActorInStage(actor))
		{
			continue;
		}

		// スポーンチェック
		const Transform spawn_transform = GetSpawnActorInfo(actor)->initial_params->transform;
		FCircle actor_spawn_circle(spawn_transform.position, actor->GetBoundingCircleRadius());
		const bool is_init_pos_in_spawn_area = GeometricUtility::DoesCircleOverlapWithAnother(actor_spawn_circle, GetSpawnArea());
		const bool should_spawn = actor->IsHidden() && is_init_pos_in_spawn_area;
		_was_actor_init_pos_in_spawn_area.at(actor) = is_init_pos_in_spawn_area;
		if (should_spawn)
		{
			actor->SetActorWorldPosition(spawn_transform.position);
			actor->SetActorWorldRotation(spawn_transform.rotation);
			actor->SetVelocity(Vector2D{});
			actor->RequestToSetActorHidden(false);
			continue;
		}

		// デスポーンチェック
		const bool is_actor_in_spawn_area = GeometricUtility::DoesCircleOverlapWithAnother(actor->GetBoundingCircle(), GetSpawnArea());
		const bool should_despawn = !actor->IsHidden() && !is_actor_in_spawn_area;
		if (should_despawn)
		{
			actor->RequestToSetActorHidden(true);
			continue;
		}
	}

	return result_scene_type;
}

void InGameScene::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);

	_state_stack->DrawForeground(*this, canvas_info);
}

void InGameScene::Finalize()
{
	_state_stack->Finalize(*this);
	_state_stack.reset();

	_was_actor_init_pos_in_spawn_area.clear();

	_destination_scene = SceneType::NONE;
	_total_score = 0;
	_is_end_scene_requested = false;
	_is_retry_requested = false;
	_remaining_time = 0.f;
	_is_timer_stopped = false;
	_has_hurried_player = false;
	_sound_instance_bgm.reset();

	__super::Finalize();
}

void InGameScene::AddScore(int score)
{
	_total_score += score;
}

int InGameScene::GetScore() const
{
	return _total_score;
}

void InGameScene::UpdateCameraParams(const float delta_seconds)
{
	__super::UpdateCameraParams(delta_seconds);

	_state_stack->UpdateCameraParams(*this, delta_seconds);

	ClampCameraPositionInStage(_camera_params);
}

void InGameScene::OnAddedActor(Actor* new_actor)
{
	__super::OnAddedActor(new_actor);

	_was_actor_init_pos_in_spawn_area[new_actor] = false;
}

void InGameScene::OnRemovedActor(Actor* removed_actor)
{
	_was_actor_init_pos_in_spawn_area.erase(removed_actor);

	__super::OnRemovedActor(removed_actor);
}

void InGameScene::PreDestroyActor(Actor* destroyee)
{
	_was_actor_init_pos_in_spawn_area.erase(destroyee);

	__super::PreDestroyActor(destroyee);
}

FCircle InGameScene::GetSpawnArea() const
{
	return FCircle(_camera_params.world_offset, _spawn_area_radius);
}

void InGameScene::EndInGameScene()
{
	_is_end_scene_requested = true;
}

void InGameScene::RetryStage()
{
	_is_retry_requested = true;
}

void InGameScene::HurryPlayer()
{
	if (_has_hurried_player)
	{
		return;
	}

	_has_hurried_player = true;

	const int bgm_volume = _sound_instance_bgm->GetVolume();
	_sound_instance_bgm->SetVolume(10);
	_sound_instance_bgm->SetPlaySpeed(1.5f);

	auto se_hurry_player = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/se/ingame_scene/se_hurry_player.ogg");
	se_hurry_player->SetPlayToEndWhenDestroyed(true);
	se_hurry_player->SetVolume(50);
	se_hurry_player->Play();

	MakeDelayedEventSystem(this, 1.f,
		[this, bgm_volume]()
		{
			_sound_instance_bgm->SetVolume(bgm_volume);
		}
	);
}

void InGameScene::SetupBGM()
{
	const std::string& bgm_file = MdStageBGM::Get(GetStageRef().GetBgmId()).file_path;
	_sound_instance_bgm = SoundManager::GetInstance().MakeSoundInstance(bgm_file);
	_sound_instance_bgm->SetLoopEnabled(true);
	_sound_instance_bgm->SetVolume(50);
}
