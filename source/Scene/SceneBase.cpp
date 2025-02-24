#include "SceneBase.h"
#include "Scene/SceneManager.h"
#include "Input/DeviceInput.h"
#include "AllScenesInclude.h"
#include "Actor/SceneAnimRendererActor.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "GameSystems/SystemTimer.h"
#include "GameSystems/CollisionManager.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "GameSystems/Sound/SoundManager.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <fstream>

const Vector2D SceneBase::BASIC_GRAVITY_FORCE = Vector2D(0, 600);

SceneBase::SceneBase()
	: should_sort_actors(true)
	, _game_speed_rate(1.0f)
	, _world_area_left_top(0, 0)
	, _world_area_right_bottom(FLT_MAX, WINDOW_SIZE_Y)
	, _world_timer(0.f)
	, _scene_anim_actor(nullptr)
	, _should_clamp_camera_in_world_area(true)
{}

SceneBase::~SceneBase()
{}

void SceneBase::Initialize(const SceneBaseInitialParams* const scene_params)
{
	_camera_params = CameraParams();
	SetWorldArea(Vector2D{ 0.f,0.f }, Vector2D{ FLT_MAX, _camera_params.GetScreenExtent().y});

	if(ShouldSpawnSceneAnimRendererActor())
	{
		initial_params_of_actor_t<SceneAnimRendererActor> anim_actor_params{};
		anim_actor_params.physics.gravity_scale = 0.f;
		anim_actor_params.transform.position = Vector2D{ 0, 0 };
		anim_actor_params._draw_priority = INT_MAX;
		_scene_anim_actor = CreateActor<SceneAnimRendererActor>(&anim_actor_params);
	}
	else
	{
		_scene_anim_actor = nullptr;
	}

	CanvasInfo foreground_canvas_info = {};
	foreground_canvas_info.width = 1280;
	foreground_canvas_info.height = 720;
	_foreground_canvas = std::make_unique<Canvas>(foreground_canvas_info);
	
	CanvasInfo debug_world_canvas_info = {};
	debug_world_canvas_info.width = WINDOW_SIZE_X;
	debug_world_canvas_info.height = WINDOW_SIZE_Y;
	_debug_world_canvas = std::make_unique<Canvas>(debug_world_canvas_info);

	CollisionManager::GetInstance().Initialize();
}

SceneType SceneBase::Tick(float delta_seconds)
{
	if(_is_world_timer_active)
	{
		UpdateWorldDelayedEvents(delta_seconds);

		UpdateWorldRepeatingEvents(delta_seconds);

		// アクターの更新
		for (auto iterator = _actors.begin(); iterator != _actors.end(); ++iterator)
		{
			Actor*& actor = *iterator;
			if (actor && actor->ShouldCallTickActor() && !actor->IsHidden())
			{
				actor->TickActor(delta_seconds);
			}
		}

		// パーティクル更新
		ParticleManager::GetInstance().Tick(delta_seconds);

		_world_timer += delta_seconds;
	}

	// カメラパラメータの更新
	{
		UpdateCameraParams(delta_seconds);

		if (_should_clamp_camera_in_world_area)
		{
			_camera_params.ClampWorldOffset(_world_area_left_top, _world_area_right_bottom);
		}
	}

	return GetSceneType();
}

void SceneBase::Draw()
{
	// 1. アクターの描画処理
	for (const auto& actor : _actors)
	{
		if (!actor->ShouldCallDraw()) { continue; }

		if (actor->IsHidden()) { continue; }

		if (!actor->IsDrawAreaCheckIgnored())
		{
			const FCircle draw_area = GetSceneDrawArea();
			if (!IsActorInDrawArea(actor, draw_area))
			{
				continue;
			}
		}

		actor->Draw(_camera_params);
	}

	// 2. パーティクル描画
	ParticleManager::GetInstance().Draw(_camera_params);

	// 3. デバッグ描画
	DxLib::DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, _debug_world_canvas->handle, TRUE);
	_debug_world_canvas->ClearScreen();
}

void SceneBase::DrawForeground(const CanvasInfo& canvas_info)
{
	// アクターのUI描画処理 
	for (const auto& actor : _actors)
	{
		if (actor->IsHidden())
		{
			continue;
		}
		actor->DrawForeground(canvas_info);
	}
}

void SceneBase::Finalize()
{
	GraphicResourceManager::GetInstance().Finalize();
	SoundManager::GetInstance().Finalize();
	CollisionManager::GetInstance().Finalize();

	// パーティクルを全破壊
	ParticleManager::GetInstance().DeactivateAllParticles();

	// 全てのオブジェクトを破棄
	DestroyAllActors();

	_world_delayed_events.clear();
	_world_repeating_events.clear();

	_bg_layer.clear();
	_bg_layer.shrink_to_fit();

	_actors_to_add.clear();
	_actors_to_remove.clear();

	_game_speed_rate = 1.0f;
}

void SceneBase::UpdateCameraParams(const float delta_seconds)
{
}

Vector2D SceneBase::GetGravityForce() const
{
	return BASIC_GRAVITY_FORCE;
}

void SceneBase::OnAddedActor(Actor* new_actor)
{
	new_actor->actor_events.on_draw_priority_changed.Bind
	(
		[this]() {OnActorDrawPriorityChanged(); },
		this
	);
}

void SceneBase::OnRemovedActor(Actor* removed_actor)
{
	removed_actor->actor_events.on_draw_priority_changed.UnBind(this);
}

void SceneBase::PreDestroyActor(Actor* destroyee)
{}

void SceneBase::OnDestroyedActor(Actor * destroyed)
{}

bool SceneBase::ShouldSpawnSceneAnimRendererActor() const
{
	return true;
}

SceneType SceneBase::ExecuteTick(const float delta_seconds)
{
	// NOTE: Tick()内でImGuiのウィンドウ表示を行っているため、
	// Tick()を呼ばないとImGuiのウィンドウが表示されない.
	// そのため, is_world_tick_enabledのフラグ処理はTick()内で行う
	SceneType ret = Tick(delta_seconds);

	// 破壊されたアクターの処理
	std::vector<Actor*> destroy_targets;
	for (auto& actor : _actors)
	{
		if (actor->ShouldDestroy())
		{
			destroy_targets.push_back(actor);
		}
	}
	for (auto& destroyee : destroy_targets)
	{
		DestroyActor(destroyee);
	}

	// シーンに追加されたアクターの処理
	if (_actors_to_add.size() != 0)
	{
		for (auto& actor : _actors_to_add)
		{
			_actors.push_back(actor);
		}
		_actors_to_add.clear();
		should_sort_actors = true;
	}

	// シーンから除外されたアクターの処理
	if (_actors_to_remove.size() != 0)
	{
		for (auto& actor : _actors_to_remove)
		{
			auto it = std::find(_actors.begin(), _actors.end(), actor);
			if (it == _actors.end())
			{
				throw std::runtime_error("Attempted to remove actor that is not in the scene");
			}

			_actors.erase(it);
		}

		_actors_to_remove.clear();
		should_sort_actors = true;
	}
	
	return ret;
}

void SceneBase::ExecuteDrawProcess()
{
	// アクターを描画優先度でソート
	if (should_sort_actors)
	{
		SortActorsByDrawPriority();
		should_sort_actors = false;
	}

	// 1. 背景描画
	DrawBackground();

	// 2. ワールド描画
	Draw();
	
	// 3. 前景描画
	if(should_call_draw_foreground)
	{
		const int last_draw_screen = DxLib::GetDrawScreen();

		DxLib::SetDrawScreen(_foreground_canvas->handle);
		DxLib::ClearDrawScreen();

		DrawForeground(_foreground_canvas->info);

		DxLib::SetDrawScreen(last_draw_screen);
		DxLib::DrawExtendGraph(
			0,0,
			WINDOW_SIZE_X, WINDOW_SIZE_Y,
			_foreground_canvas->handle,
			true
		);
	}
}

void SceneBase::SetGameSpeed(const float new_speed_rate)
{
	if (new_speed_rate <= 0.f)
	{
		return;
	}

	_game_speed_rate = new_speed_rate;
}

float SceneBase::GetWorldTime() const
{
	return _world_timer;
}

void SceneBase::DrawDebugLine(const Vector2D& start_world, const Vector2D& end_world, const int line_color, const int line_thickness, const DrawBlendInfo& blend_info)
{
	const int last_screen = DxLib::GetDrawScreen();

	DxLib::SetDrawScreen(_debug_world_canvas->handle);

	const Vector2D start_screen = Vector2D::WorldToViewport(start_world, _camera_params);
	const Vector2D end_screen = Vector2D::WorldToViewport(end_world, _camera_params);
	BlendDrawHelper::DrawLine(blend_info, start_screen.x, start_screen.y, end_screen.x, end_screen.y, line_color, line_thickness);

	DxLib::SetDrawScreen(last_screen);
}

void SceneBase::DrawDebugRect(const FRect& rect, const int line_color, const int line_thickness, const DrawBlendInfo& blend_info)
{
	std::array<Vector2D, 4> vertices;
	rect.GetVertices(vertices);

	const int last_screen = DxLib::GetDrawScreen();
	DxLib::SetDrawScreen(_debug_world_canvas->handle);

	for (size_t i = 0; i < 4; i++)
	{
		const Vector2D start = Vector2D::WorldToViewport(vertices[i], _camera_params);
		const Vector2D end = Vector2D::WorldToViewport(vertices[(i + 1) % 4], _camera_params);
		BlendDrawHelper::DrawLine(blend_info, start.x, start.y, end.x, end.y, line_color, line_thickness);
	}

	DxLib::SetDrawScreen(last_screen);
}

Actor* SceneBase::CreateAndInitializeActorByEntityType(const EEntityType entity_type, const ActorInitialParams* actor_params)
{
	Actor* created_actor = ActorFactory::CreateAndInitializeActorByEntityType(entity_type, actor_params, this);
	if (created_actor != nullptr)
	{
		AddActor(created_actor);
	}
	return created_actor;
}

void SceneBase::DestroyActor(Actor*& destroyee)
{
	if (destroyee == nullptr)
	{
		return;
	}

	// 破壊前処理
	PreDestroyActor(destroyee);
	destroyee->Finalize();

	// 破壊対象をactorsから除外
	std::vector<Actor*>::iterator it_destroyee;
	if (ExistsInScene(destroyee, &it_destroyee))
	{
		_actors.erase(it_destroyee);
	}

	// 破壊
	ActorFactory::DestroyActor(destroyee);
}

FCircle SceneBase::GetSceneDrawArea() const
{
	const Vector2D& center = _camera_params.world_offset;
	const float radius = _camera_params.GetWorldViewHalfExtent().Length();
	return FCircle(center, radius);
}

bool SceneBase::IsActorInDrawArea(const Actor* const actor, const FCircle& draw_area)
{
	const FCircle actor_bounding_circle = actor->GetBoundingCircle();
	return GeometricUtility::DoesCircleOverlapWithAnother(draw_area, actor_bounding_circle);
}

bool SceneBase::ExistsInScene(Actor* actor, std::vector<Actor*>::iterator* outIt)
{
	auto it = std::find(_actors.begin(), _actors.end(), actor);
	if (outIt != nullptr)
	{
		*outIt = it;
	}

	return it != _actors.end();
}

void SceneBase::RemoveActor(Actor* actor_to_remove)
{
	if (ExistsInScene(actor_to_remove))
	{
		_actors_to_remove.insert(actor_to_remove);
		OnRemovedActor(actor_to_remove);
	}
}

void SceneBase::AddActor(Actor* actor_to_add)
{
	if (!actor_to_add->IsInitialized())
	{
		throw std::runtime_error("Attempted to add uninitialized actor");
	}

	if(!ExistsInScene(actor_to_add))
	{
		_actors_to_add.insert(actor_to_add);
		OnAddedActor(actor_to_add);
	}
}

size_t SceneBase::PlayAnimation(const AnimPlayInfo& anim_play_info, const Transform& transform, Actor* const attach_to)
{
	return _scene_anim_actor->PlayAnimation(anim_play_info, transform, attach_to);
}

void SceneBase::StopAnimation(const size_t scene_anim_index)
{
	_scene_anim_actor->StopAnimation(scene_anim_index);
}

void SceneBase::SingleLineTrace(QueryResult_SingleLineTrace& out_query_result, const CollisionQueryParams_SingleLineTrace& query_params)
{
	CollisionManager::GetInstance().SingleLineTrace(out_query_result, query_params);
}

void SceneBase::MultiAARectTrace(QueryResult_MultiAARectTrace& out_query_result, const CollisionQueryParams_RectAA& query_params)
{
	CollisionManager::GetInstance().MultiAARectTrace(out_query_result, query_params);
}

void SceneBase::DestroyAllActors()
{
	for (auto& actor : _actors)
	{
		PreDestroyActor(actor);
		actor->Finalize();
		ActorFactory::DestroyActor(actor);
	}
	_actors.clear();
}

void SceneBase::UpdateWorldDelayedEvents(const float delta_seconds)
{
	// NOTE: イテレーション中に要素が追加される可能性があるため, 逆順で処理
	for (int i = _world_delayed_events.size() - 1; i >= 0; i--)
	{
		WorldDelayedEvent& delayed_event = *_world_delayed_events.at(i);

		if (!IsValid(delayed_event.object))
		{
			_world_delayed_events.erase(_world_delayed_events.begin() + i);
			continue;
		}

		if (delayed_event.dispatch_world_time <= _world_timer)
		{
			delayed_event.process();
			_world_delayed_events.erase(_world_delayed_events.begin() + i);
		}
	}
}

void SceneBase::UpdateWorldRepeatingEvents(const float delta_seconds)
{
	// NOTE: イテレーション中に要素が追加される可能性があるため, 逆順で処理
	for (int i = _world_repeating_events.size() - 1; i >= 0; i--)
	{
		WorldRepeatingEvent& repeating_event = *_world_repeating_events.at(i);

		if (!IsValid(repeating_event.object))
		{
			_world_repeating_events.erase(_world_repeating_events.begin() + i);
			continue;
		}

		bool is_timer_alive = true;
		if (GetWorldTime() >= repeating_event.next_dispatch_time)
		{
			while (is_timer_alive && GetWorldTime() >= repeating_event.next_dispatch_time)
			{
				is_timer_alive &= repeating_event.process();
				repeating_event.next_dispatch_time += repeating_event.interval;
			}
		}

		if (!is_timer_alive)
		{
			_world_repeating_events.erase(_world_repeating_events.begin() + i);
		}
	}
}

std::shared_ptr<DxLibScreenCapture> SceneBase::CaptureScene()
{
	const int last_screen = DxLib::GetDrawScreen();

	std::shared_ptr<DxLibScreenCapture> capture_screen 
		= std::make_shared<DxLibScreenCapture>(WINDOW_SIZE_X, WINDOW_SIZE_Y, TRUE);

	DxLib::SetDrawScreen(capture_screen->GetHandle());
	DxLib::ClearDrawScreen();
	DrawBackground();
	Draw();

	DxLib::SetDrawScreen(last_screen);
	
	return capture_screen;
}

void SceneBase::OnActorDrawPriorityChanged()
{
	if (!should_sort_actors)
	{
		should_sort_actors = true;
	}
}

void SceneBase::MakeDelayedEventWorld(GameObject* const object, const float delay_time, const std::function<void()> process)
{
	_world_delayed_events.push_back(std::make_unique<WorldDelayedEvent>(object, _world_timer + delay_time, process));
}

void SceneBase::MakeDelayedEventSystem(const GameObject* const object, const float delay_time, const std::function<void()> process)
{
	SystemTimer::GetInstance().MakeDelayedEventSystem(object, delay_time, process);
}

void SceneBase::MakeRepeatingEventWorld(const GameObject* const object, const float interval, const std::function<bool()> process)
{
	_world_repeating_events.push_back(std::make_unique<WorldRepeatingEvent>(object, _world_timer + interval, interval, process));
}

void SceneBase::MakeRepeatingEventSystem(const GameObject* const object, const float interval, const std::function<bool()> process)
{
	SystemTimer::GetInstance().MakeRepeatingEventSystem(object, interval, process);
}

void SceneBase::SortActorsByDrawPriority()
{	
	std::sort(_actors.begin(), _actors.end(), [](const Actor* obj1, const Actor* obj2)
	{
		if (obj1->GetDrawPriority() != obj2->GetDrawPriority())
		{
			// draw_sort_priorityが小さい順にソートする
			return obj1->GetDrawPriority() < obj2->GetDrawPriority();
		}

		// priorityが同じ場合は、画面下の方から描画する
		return  obj1->GetActorWorldPosition().y > obj2->GetActorWorldPosition().y;
	});
}

void SceneBase::AddBackgroundToLayer(BackgroundParams bg_params)
{
	// レイヤーが空
	if (_bg_layer.size() == 0)
	{
		_bg_layer.push_back(bg_params);
		return;
	}

	// 追加対象よりもdistance_scaleが小さい背景を先頭から線形探査し, 
	// 発見した場合はその直前に挿入する.
	auto iterator = _bg_layer.begin();
	while (iterator != _bg_layer.end())
	{
		if (bg_params.z >= iterator->z)
		{
			_bg_layer.insert(iterator, bg_params);
			return;
		}

		++iterator;
	}

	// 追加対象のdistance_scaleが, どの挿入済み背景のdistance_scaleよりも小さい場合, 追加対象が上のwhileループでは挿入されない.
	_bg_layer.push_back(bg_params);
}

void SceneBase::ClearBackgroundLayer()
{
	_bg_layer.clear();
}

void SceneBase::DrawBackground() const
{
	RefreshDxLibDirect3DSetting();

	const float cx = _camera_params.world_offset.x;
	const float cz = _camera_params.GetCameraZ();
	const Vector2D half_screen_size = _camera_params.GetScreenHalfExtent();

	for (const auto& bg_params : _bg_layer)
	{
		int bg_graph_size_x, bg_graph_size_y;
		DxLib::GetGraphSize(bg_params.handle, &bg_graph_size_x, &bg_graph_size_y);
		const float bg_width_per_height = static_cast<float>(bg_graph_size_x) / bg_graph_size_y;

		const float bg_z = bg_params.z;	// 背景平面のz座標
		const float d = bg_z - cz;	// カメラと背景の距離
		const float bg_top_world_y = _world_area_left_top.y - half_screen_size.y * (bg_z - 1.f);
		const float bg_bottom_world_y = _world_area_right_bottom.y + half_screen_size.y * (bg_z - 1.f);
		const float bg_world_size_y = bg_bottom_world_y - bg_top_world_y;
		const float bg_world_size_x = bg_world_size_y * bg_width_per_height;

		// 描画する背景のインデックス範囲を計算
		// NOTE: 誤差を考慮し, 理論値より左右に1枚ずつ余分に描画するため, -1, +1している
		const int bgi_start = floor((cx - half_screen_size.x * d) / bg_world_size_x) - 1;
		const int bgi_end = ceil((cx + half_screen_size.x * d) / bg_world_size_x) + 1;

		const auto get_bg_left_world = [&](const int i) -> float
			{
				const float left_0 = -half_screen_size.x * (bg_z - cz);
				return i * bg_world_size_x + left_0;
			};

		for (int bgi = bgi_start; bgi < bgi_end; bgi++)
		{
			const float left = get_bg_left_world(bgi);
			const float right = get_bg_left_world(bgi + 1);
			const Vector2D left_top_viewport = _camera_params.TransformPosition_WorldToViewport(Vector2D{ left, bg_top_world_y }, bg_z);
			const Vector2D right_bottom_viewport = _camera_params.TransformPosition_WorldToViewport(Vector2D{ right, bg_bottom_world_y }, bg_z);
			DxLib::DrawExtendGraph(
				left_top_viewport.x,
				left_top_viewport.y,
				right_bottom_viewport.x,
				right_bottom_viewport.y,
				bg_params.handle,
				TRUE
			);
		}
	}
}

void SceneBase::SetWorldArea(const Vector2D& left_top, const Vector2D& right_bottom)
{
	_world_area_left_top = left_top;
	_world_area_right_bottom = right_bottom;
}

void SceneBase::DrawWorldGrid(const int color, const uint8_t alpha) const
{
	RECT last_draw_area;
	DxLib::GetDrawArea(&last_draw_area);

	// 背景の描画エリアを設定
	const Vector2D bg_area_left_top_viewport = Vector2D::WorldToViewport(_world_area_left_top, _camera_params);
	Vector2D bg_area_right_bottom_viewport = Vector2D::WorldToViewport(_world_area_right_bottom, _camera_params);
	if (_world_area_right_bottom.x == FLT_MAX)
	{
		bg_area_right_bottom_viewport.x = last_draw_area.right;
	}
	SetDrawArea(bg_area_left_top_viewport.x, bg_area_left_top_viewport.y, bg_area_right_bottom_viewport.x, bg_area_right_bottom_viewport.y);

	const Vector2D& screen_world_offset = _camera_params.world_offset;
	int left, right, top, bottom;
	left = UNIT_TILE_SIZE * static_cast<int>(floor(screen_world_offset.x / UNIT_TILE_SIZE));
	right = UNIT_TILE_SIZE * static_cast<int>(ceil((screen_world_offset.x + WINDOW_SIZE_X) / UNIT_TILE_SIZE));
	top = UNIT_TILE_SIZE * static_cast<int>(floor(screen_world_offset.y / UNIT_TILE_SIZE));
	bottom = UNIT_TILE_SIZE * static_cast<int>(ceil((screen_world_offset.y + WINDOW_SIZE_Y) / UNIT_TILE_SIZE));

	int s_left, s_top, s_right, s_bottom;
	Vector2D::WorldToViewport(Vector2D(left, top), _camera_params).ToIntRound(s_left, s_top);
	Vector2D::WorldToViewport(Vector2D(right, bottom), _camera_params).ToIntRound(s_right, s_bottom);

	const uint16_t scaled_unit_tile_size =
		Vector2D::WorldToViewport(Vector2D{ UNIT_TILE_SIZE,0 }, _camera_params).x -
		Vector2D::WorldToViewport(Vector2D{ 0,0 }, _camera_params).x;
	while (s_left > 0)
	{
		s_left -= scaled_unit_tile_size;
	}
	while (s_top > 0)
	{
		s_top -= scaled_unit_tile_size;
	}
	while (s_right < WINDOW_SIZE_X)
	{
		s_right += scaled_unit_tile_size;
	}
	while (s_bottom < WINDOW_SIZE_Y)
	{
		s_bottom += scaled_unit_tile_size;
	}

	BlendDrawHelper::ExecuteDrawProcess(DrawBlendInfo(DX_BLENDMODE_ALPHA, alpha), [&]() {
		int x = s_left;
		int y = s_top;
		while (x <= s_right)
		{
			DrawLine(x, 0, x, WINDOW_SIZE_Y, color);
			x += scaled_unit_tile_size;
		}
		while (y <= s_bottom)
		{
			DrawLine(0, y, WINDOW_SIZE_X, y, color);
			y += scaled_unit_tile_size;

		}
	});

	SetDrawArea(last_draw_area.left, last_draw_area.top, last_draw_area.right, last_draw_area.bottom);
}

void SceneBase::SetWorldTimerActive(const bool new_world_timer_active)
{
	_is_world_timer_active = new_world_timer_active;
}

SceneBase::Canvas::Canvas(const CanvasInfo& canvas_info)
	: info(canvas_info)
{
	handle = MakeScreen(canvas_info.width, canvas_info.height, TRUE);
}

void SceneBase::Canvas::ClearScreen()
{
	const int last_screen = DxLib::GetDrawScreen();

	DxLib::SetDrawScreen(handle);
	DxLib::ClearDrawScreen();

	DxLib::SetDrawScreen(last_screen);
}
