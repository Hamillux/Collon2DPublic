#include "StageInteractiveScene.h"
#include "StagePerimeterColliderHolder.h"
#include "Scene/AllScenesInclude.h"
#include "Actor/AllActorsInclude_generated.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "GameSystems/FontManager.h"
#include "GameSystems/CollisionManager.h"
#include "Component/Collider/SegmentCollider.h"
#include <fstream>
#include <nlohmann/json.hpp>

StageInteractiveScene::StageInteractiveScene()
	: _player_ref(nullptr)
{
}

StageInteractiveScene::~StageInteractiveScene()
{
}

void StageInteractiveScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	typedef SceneBase::traits<StageInteractiveScene>::initial_params_type InitialParamsType;
	const InitialParamsType* const stage_interactive_scene_params = dynamic_cast<const InitialParamsType*>(scene_params);

	const std::string file_path = stage_interactive_scene_params->stage_id.GetJsonFilePath();
	std::ifstream stage_json_file(file_path);
	assert(stage_json_file.is_open());

	const nlohmann::json stage_json = nlohmann::json::parse(stage_json_file);
	_stage = std::make_unique<Stage>();
	_stage->FromJsonObject(stage_json);

	BuildStage(*_stage);

	_stage->stage_events.OnStageSizeChanged.Bind([this]() 
		{
			SetWorldArea(_stage->GetStageLeftTop(), _stage->GetStageRightBottom());
		});
}

SceneType StageInteractiveScene::Tick(const float delta_seconds)
{
	const SceneType result_scene_type = __super::Tick(delta_seconds);

	// 衝突判定
	if (IsWorldTimerActive())
	{
		CollisionManager::GetInstance().HandleCollisions();
	}

	return GetSceneType();
}

void StageInteractiveScene::Finalize()
{
	actor_spawn_info_map.clear();
	_stage.reset();
	_player_ref = nullptr;

	__super::Finalize();
}

std::unique_ptr<const SceneBaseInitialParams> StageInteractiveScene::GetInitialParamsForNextScene(const SceneType next_scene) const
{
	switch (next_scene)
	{
	case SceneType::EDITOR_SCENE:
	{
		auto params = std::make_unique<initial_params_of_scene_t<StageEditorScene>>();
		params->prev_scene_type = GetSceneType();
		params->stage_id = _stage->GetStageId();
		return params;
	}
	case SceneType::INGAME_SCENE:
	{
		auto params = std::make_unique<initial_params_of_scene_t<InGameScene>>();
		params->prev_scene_type = GetSceneType();
		params->stage_id = _stage->GetStageId();
		return params;
	}
	case SceneType::SELECT_SCENE:
	{
		auto params = std::make_unique<initial_params_of_scene_t<StageSelectScene>>();
		params->prev_scene_type = GetSceneType();
		params->next_scene = GetSceneType();
		params->initially_selected_stage = _stage->GetStageId();
		return params;
	}
	case SceneType::TITLE_SCENE: 
	{
		auto params = std::make_unique<initial_params_of_scene_t<TitleScene>>();
		params->prev_scene_type = GetSceneType();
		return params;

	}
	}
	
	throw std::runtime_error("Undefined scene transition");
}

void StageInteractiveScene::PreDestroyActor(Actor* destroyee)
{
	RemoveActorFromStage(destroyee);

	__super::PreDestroyActor(destroyee);
}

void StageInteractiveScene::BuildStage(const Stage& stage)
{
	SetWorldArea(stage.GetStageLeftTop(), stage.GetStageRightBottom());

	SetupBackgrounds();

	CreateStagePerimeterColliders();

	CreateActorsInStage();

	// 全Actorの初期化が終了したので空間分割する
	const Vector2D root_cell_left_top = stage.GetStageLeftTop() - Vector2D(UNIT_TILE_SIZE, UNIT_TILE_SIZE);
	const Vector2D root_cell_right_bottom = stage.GetStageRightBottom() + Vector2D(UNIT_TILE_SIZE, UNIT_TILE_SIZE);
	CollisionManager::GetInstance().ConstructKdTree(root_cell_left_top, root_cell_right_bottom);

	// 描画優先度をもとに, アクターをソートする
	SortActorsByDrawPriority();
}

Stage& StageInteractiveScene::GetStageRef() const
{
	return *_stage;
}

Player* StageInteractiveScene::GetPlayerRef() const
{
	return _player_ref;
}

std::shared_ptr<SpawnActorInfo> StageInteractiveScene::GetSpawnActorInfo(Actor* actor) const
{
	return actor_spawn_info_map.at(actor);
}

bool StageInteractiveScene::IsActorInStage(Actor* actor) const
{
	return actor_spawn_info_map.find(actor) != actor_spawn_info_map.end();
}

void StageInteractiveScene::ClampCameraPositionInStage(CameraParams& camera_params) const
{
	camera_params.ClampWorldOffset(GetStageRef().GetStageLeftTop(), GetStageRef().GetStageRightBottom());
}

float StageInteractiveScene::GetMaxScreenScale() const
{
	return _camera_params.GetMaxScale(GetStageRef().GetStageSize());
}

void StageInteractiveScene::AddActorToStage(Actor* new_actor, std::shared_ptr<SpawnActorInfo> spawn_info)
{
	if (actor_spawn_info_map.find(new_actor) != actor_spawn_info_map.end())
	{
		throw std::runtime_error("Actor is already in stage");
	}

	actor_spawn_info_map[new_actor] = spawn_info;
	GetStageRef().AddSpawnActor(spawn_info);
	ReloadActorInStage(new_actor);	// 追加しようとしたアクターが未初期化でもここで初期化される
}

std::shared_ptr<SpawnActorInfo> StageInteractiveScene::RemoveActorFromStage(Actor* actor)
{
	auto it = actor_spawn_info_map.find(actor);
	if (it == actor_spawn_info_map.end())
	{
		return nullptr;
	}

	const std::shared_ptr<SpawnActorInfo> spawn_info = it->second;
	GetStageRef().RemoveSpawnActorInfo(spawn_info);
	actor_spawn_info_map.erase(it);

	return spawn_info;
}

float StageInteractiveScene::GetStageBottomY() const
{
	return _stage->GetStageRightBottom().y;
}

void StageInteractiveScene::GetActorOccupyingArea(Actor* const actor, int& tile_index_left, int& tile_index_top, int& tile_index_right, int& tile_index_bottom)
{
	int num_tiles_x, num_tiles_y;
	Vector2D snap_pos_to_actor_pos;
	actor->GetOccupyingTiles(num_tiles_x, num_tiles_y, snap_pos_to_actor_pos);

	const Vector2D snap_pos = actor->GetActorWorldPosition() - snap_pos_to_actor_pos;
	int snap_tile_x, snap_tile_y;
	GetTileIndex(snap_pos, snap_tile_x, snap_tile_y);

	tile_index_left = snap_tile_x - ((num_tiles_x - 1) / 2);
	tile_index_right = snap_tile_x + (num_tiles_x / 2);
	tile_index_top = snap_tile_y - ((num_tiles_y - 1) / 2);
	tile_index_bottom = snap_tile_y + (num_tiles_y / 2);
}

void StageInteractiveScene::CreateActorsInStage()
{
	if (!actor_spawn_info_map.empty())
	{
		// 
		throw std::runtime_error("Actor spawn info map is not empty");
	}

	for (const auto& actor_info : _stage->GetSpawnActorInfosRef())
	{
		Actor* spawned_actor = CreateAndInitializeActorByEntityType(actor_info->entity_type, actor_info->initial_params.get());
		if (actor_info->entity_type == EEntityType::Player)
		{
			if (_player_ref != nullptr)
			{
				// プレイヤーは1体のみ
				throw std::runtime_error("Player is already spawned");
			}

			_player_ref = dynamic_cast<Player*>(spawned_actor);

			if (_player_ref == nullptr)
			{
				throw std::runtime_error("Spawned player is invalid");
			}
		}
		actor_spawn_info_map[spawned_actor] = actor_info;
	}
}

void StageInteractiveScene::CreateStagePerimeterColliders()
{
	if (_stage == nullptr)
	{
		return;
	}

	const Vector2D left_top = _stage->GetStageLeftTop();
	const Vector2D right_bottom = _stage->GetStageRightBottom();

	// ステージの外壁コライダーを所有するアクター
	ActorInitialParams actor_params;
	actor_params.transform.position = left_top;
	CreateActor<StagePerimeterColliderHolder>(&actor_params)->CreateColliders(left_top, right_bottom);
}

void StageInteractiveScene::SetupBackgrounds()
{
	if (_stage == nullptr)
	{
		return;
	}

	const MasterDataID bg_layer_id = _stage->GetBgLayerId();
	std::vector<StageBGLayer> bg_layers;
	StageBGLayer::LoadStageBGLayers(bg_layers);

	for (auto& bg_layer : bg_layers)
	{
		if (bg_layer.bg_layer_id != bg_layer_id)
		{
			continue;
		}

		SetStageBackground(bg_layer);
	}
}

void StageInteractiveScene::ReloadActorInStage(Actor* actor)
{
	const std::shared_ptr<SpawnActorInfo>& spawn_info = actor_spawn_info_map.at(actor);
	actor->Finalize();
	actor->Initialize(spawn_info->initial_params.get());
}

void StageInteractiveScene::SetStageBackground(const StageBGLayer& stage_bg_layer)
{
	ClearBackgroundLayer();

	for (auto& bg_info : stage_bg_layer.bg_infos)
	{
		const int handle = GraphicResourceManager::GetInstance().GetGraphForDxLib(bg_info.image_id);
		AddBackgroundToLayer(BackgroundParams{ handle, bg_info.distance });
	}
}

void StageInteractiveScene::GetStageTileIndex(int& left, int& top, int& right, int& bottom) const
{
	const Vector2D stage_left_top = GetStageRef().GetStageLeftTop();
	const Vector2D stage_right_bottom = GetStageRef().GetStageRightBottom();
	GetTileIndex(stage_left_top + Vector2D{ UNIT_TILE_SIZE / 2, UNIT_TILE_SIZE / 2 }, left, top);
	GetTileIndex(stage_right_bottom - Vector2D{ UNIT_TILE_SIZE / 2, UNIT_TILE_SIZE / 2 }, right, bottom);
}

Vector2D StageInteractiveScene::GetTileCenterPosition(const Vector2D& q_world, const int offset_tiles_x, const int offset_tiles_y)
{
	int ix, iy;
	GetTileIndex(q_world, ix, iy);

	return GetTileCenterPosition(ix + offset_tiles_x, iy + offset_tiles_y);
}

Vector2D StageInteractiveScene::GetTileCenterPosition(const int tile_index_x, const int tile_index_y)
{
	static_assert(UNIT_TILE_SIZE % 2 == 0, "UNIT_TILE_SIZE must be even number");

	return Vector2D
	(
		tile_index_x * UNIT_TILE_SIZE + UNIT_TILE_SIZE / 2,
		tile_index_y * UNIT_TILE_SIZE + UNIT_TILE_SIZE / 2
	);
}

void StageInteractiveScene::GetTileIndex(const Vector2D& q_world, int& tile_index_x, int& tile_index_y)
{
	const double x = static_cast<double>(q_world.x) / static_cast<double>(UNIT_TILE_SIZE);
	const double y = static_cast<double>(q_world.y) / static_cast<double>(UNIT_TILE_SIZE);
	tile_index_x = x > 0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
	tile_index_y = y > 0 ? static_cast<int>(y) : static_cast<int>(y) - 1;
}

void StageInteractiveScene::ReloadAllActorsInStage()
{
	for (auto actor_info_pair : actor_spawn_info_map)
	{
		ReloadActorInStage(actor_info_pair.first);
	}
}
