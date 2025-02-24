#include "StageEditorSceneState_Edit_Move.h"
#include "Actor/AllActorsInclude_generated.h"

StageEditorSceneState_Edit_Move::StageEditorSceneState_Edit_Move()
	: _current_substate(SubState::Default)
{
}

StageEditorSceneState_Edit_Move::~StageEditorSceneState_Edit_Move()
{
}

void StageEditorSceneState_Edit_Move::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	DeviceInput::input_events.OnDragDropBegin.Bind([this]() {OnBeginDragDrop(); }, this);
	DeviceInput::input_events.OnDragDropEnd.Bind([this]() {OnEndDragDrop(); }, this);
}

void StageEditorSceneState_Edit_Move::OnLeaveState(ParentSceneClass& parent_scene)
{
	if (_moving_info != nullptr)
	{
		CancelMovingActor();
	}

	if (_duplication_info != nullptr)
	{
		CancelDuplication(parent_scene);
	}

	DeviceInput::input_events.OnDragDropBegin.UnBind(this);
	DeviceInput::input_events.OnDragDropEnd.UnBind(this);

	_moving_info.reset();
	_current_substate = SubState::Default;
	_drag_drop_start_viewport_pos.reset();
	_drag_drop_end_viewport_pos.reset();
	_selected_actors.clear();

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorScene>> StageEditorSceneState_Edit_Move::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	return ret;
}

void StageEditorSceneState_Edit_Move::Draw(ParentSceneClass& parent_scene)
{
	constexpr int convex_r = 255;
	constexpr int convex_g = 255;
	constexpr int convex_b = 0;
	for (auto& actor : _selected_actors)
	{
		parent_scene.DrawActorConvex(actor, DxLib::GetColor(convex_r, convex_g, convex_b));
	}

	if (_moving_info || _duplication_info)
	{
		const std::vector<Actor*> actor_list = _moving_info ? _moving_info->moving_actors : _duplication_info->duplicated_actors;
		for (auto& actor : actor_list)
		{
			actor->Draw(parent_scene._camera_params);
			parent_scene.DrawActorConvex(actor, DxLib::GetColor(convex_r, convex_g, convex_b));
		}
	}

	if (_current_substate == SubState::RectSelecting)
	{
		constexpr int rect_r = 32;
		constexpr int rect_g = 32;
		constexpr int rect_b = 255;
		constexpr int rect_a = 128;

		const Vector2D& start = *_drag_drop_start_viewport_pos;
		const Vector2D& end = DeviceInput::GetMousePosition();

		BlendDrawHelper::DrawBox(
			DrawBlendInfo(DX_BLENDMODE_ALPHA, rect_a),
			start.x, start.y, end.x, end.y,
			DxLib::GetColor(rect_r, rect_g, rect_b),
			TRUE
		);

	}
}

void StageEditorSceneState_Edit_Move::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);
}

void StageEditorSceneState_Edit_Move::UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
{
	switch (_current_substate)
	{
	case SubState::Default:
	case SubState::MovingActors:
	case SubState::MovingDuplicatedActors:
		__super::UpdateCameraParams(parent_scene, delta_seconds);
		return;
	}
}

void StageEditorSceneState_Edit_Move::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	__super::HandleInput(parent_scene, delta_seconds);

	switch (_current_substate)
	{
	case SubState::Default:
		HandleInputImpl_Default(parent_scene, delta_seconds);
		break;
	case SubState::MovingActors:
		HandleInputImpl_MovingActors(parent_scene, delta_seconds);
		break;
	case SubState::RectSelecting:
		HandleInputImpl_RectSelecting(parent_scene, delta_seconds);
		break;
	case SubState::MovingDuplicatedActors:
		HandleInputImpl_MovingDuplicatedActors(parent_scene, delta_seconds);
		break;
	}
}

void StageEditorSceneState_Edit_Move::ShowRightbarBody(StageEditorScene& parent_editor_scene)
{
	// 操作説明を説明画像とともに表示

	// エンティティの選択

	// エンティティの移動

	// エンティティの複製

	// エンティティの削除
}

void StageEditorSceneState_Edit_Move::OnUndo(StageEditorScene& parent_editor_scene)
{
	__super::OnUndo(parent_editor_scene);

	UnselectAllActors();
}

void StageEditorSceneState_Edit_Move::OnRedo(StageEditorScene& parent_editor_scene)
{
	__super::OnRedo(parent_editor_scene);

	UnselectAllActors();
}

void StageEditorSceneState_Edit_Move::HandleInputImpl_Default(StageEditorScene& parent_scene, const float delta_seconds)
{
	const Vector2D& mouse_pos = DeviceInput::GetMousePosition();
	const bool is_mouse_on_stage = parent_scene.IsMouseOnStage(mouse_pos);
	Actor* hovered_actor = parent_scene.GetActorAt(mouse_pos);

	if (_moving_info == nullptr && IsAnyActorSelected() && DeviceInput::IsPressed(KEY_INPUT_DELETE))
	{
		RemoveSelectedActors(parent_scene);
		return;
	}

	if (!is_mouse_on_stage)
	{
		return;
	}

	if (IsDoingDragDrop())
	{
		Actor* const drag_start_hovered_actor = parent_scene.GetActorAt(*_drag_drop_start_viewport_pos);
		const bool was_hovered_selected_actor =  drag_start_hovered_actor != nullptr && IsSelectedActor(drag_start_hovered_actor);

		const bool is_lshift_active = DeviceInput::IsActive(KEY_INPUT_LSHIFT);

		if (drag_start_hovered_actor == nullptr)
		{
			// 矩形選択開始
			_current_substate = SubState::RectSelecting;
			return;
		}
		else if (was_hovered_selected_actor && !is_lshift_active)
		{
			BeginMovingSelectedActors(parent_scene);
			return;
		}
		else if (was_hovered_selected_actor && is_lshift_active)
		{
			// アクター複製
			BeginDuplicationOfSelectedActors(parent_scene);
			return;
		}
	}

	// ステージ上を左クリック(左Ctrlなし)
	if (DeviceInput::IsPressed(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE) && !DeviceInput::IsActive(KEY_INPUT_LCONTROL))
	{
		if (hovered_actor == nullptr)
		{
			UnselectAllActors();
		}
		else if(!IsSelectedActor(hovered_actor))
		{
			// 今回クリックされたアクターのみ選択
			UnselectAllActors();
			SelectActor(hovered_actor);
		}
	}

	// ステージ上をLCtrl+左クリック
	else if (DeviceInput::IsPressed(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE) && DeviceInput::IsActive(KEY_INPUT_LCONTROL))
	{
		Actor* hovered_actor = parent_scene.GetActorAt(mouse_pos);

		if (hovered_actor)
		{
			if (IsSelectedActor(hovered_actor))
			{
				UnSelectActor(hovered_actor);
			}
			else
			{
				SelectActor(hovered_actor);
			}
		}
	}
}

void StageEditorSceneState_Edit_Move::HandleInputImpl_MovingActors(StageEditorScene& parent_scene, const float delta_seconds)
{
	const Vector2D& mouse_pos = DeviceInput::GetMousePosition();
	const bool is_mouse_on_stage = parent_scene.IsMouseOnStage(mouse_pos);

	if (!is_mouse_on_stage)
	{
		return;
	}

	int stage_tile_index_left, stage_tile_index_top, stage_tile_index_right, stage_tile_index_bottom;
	parent_scene.GetStageTileIndex(stage_tile_index_left, stage_tile_index_top, stage_tile_index_right, stage_tile_index_bottom);

	const Vector2D mouse_pos_world = parent_scene._camera_params.TransformPosition_ViewportToWorld(mouse_pos);
	int mouse_tile_index_x, mouse_tile_index_y;
	GetTileIndex(mouse_pos_world, mouse_tile_index_x, mouse_tile_index_y);

	const int moving_actors_origin_tile_index_x_min = stage_tile_index_left - _moving_info->offset_tiles_to_occupying_area_edge.to_left;
	const int moving_actors_origin_tile_index_x_max = stage_tile_index_right - _moving_info->offset_tiles_to_occupying_area_edge.to_right;

	const int moving_actors_origin_tile_index_y_min = stage_tile_index_top - _moving_info->offset_tiles_to_occupying_area_edge.to_top;
	const int moving_actors_origin_tile_index_y_max = stage_tile_index_bottom - _moving_info->offset_tiles_to_occupying_area_edge.to_bottom;

	// 移動アクター群の原点
	// NOTE: 通常はマウス位置だが, いずれかのアクターのスナップ位置がステージ外に出る場合は,
	//	     アクターがギリギリステージ内に収まるように原点の位置を調整する
	const int moving_actors_origin_tile_index_x = clamp(
		mouse_tile_index_x,
		moving_actors_origin_tile_index_x_min,
		moving_actors_origin_tile_index_x_max
	);

	const int moving_actors_origin_tile_index_y = clamp(
		mouse_tile_index_y,
		moving_actors_origin_tile_index_y_min,
		moving_actors_origin_tile_index_y_max
	);

	// 移動中アクターをスナップ
	for (auto& actor : _moving_info->moving_actors)
	{
		const auto& offset_tiles_to_actor_snap_pos = _moving_info->offset_tiles_to_actor_snap_pos.at(actor);
		const int snap_tile_index_x = moving_actors_origin_tile_index_x + offset_tiles_to_actor_snap_pos.x;
		const int snap_tile_index_y = moving_actors_origin_tile_index_y + offset_tiles_to_actor_snap_pos.y;

		const Vector2D& snap_position = GetTileCenterPosition(snap_tile_index_x, snap_tile_index_y);
		const Vector2D& actor_pos = snap_position + _moving_info->snap_pos_to_actor_pos.at(actor);
		actor->SetActorWorldPosition(actor_pos);
	}

	if (DeviceInput::IsPressed(MOUSE_INPUT_MIDDLE, DeviceInput::Device::MOUSE))
	{
		CancelMovingActor();
	}

	else if (!DeviceInput::IsActive(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE))
	{
		// 移動完了
		int mouse_tile_index_x, mouse_tile_index_y;
		GetTileIndex(parent_scene._camera_params.TransformPosition_ViewportToWorld(mouse_pos), mouse_tile_index_x, mouse_tile_index_y);

		const int mouse_tile_from_x = _moving_info->mouse_tile_from_x;
		const int mouse_tile_from_y = _moving_info->mouse_tile_from_y;

		if (mouse_tile_index_x != mouse_tile_from_x || mouse_tile_index_y != mouse_tile_from_y)
		{
			std::vector<std::shared_ptr<CommandBase>> move_commands;
			for (auto& actor : _moving_info->moving_actors)
			{
				const Vector2D from = _moving_info->moved_from.at(actor);
				const Vector2D to = actor->GetActorWorldPosition();
				move_commands.push_back(std::make_shared<stage_editor_scene::CmdChangeActorPosition>(&parent_scene, actor, from, to));
			}

			parent_scene._command_history->ExecuteAndPush
			(
				std::make_shared<CmdCommandList>(move_commands)
			);
		}

		_selected_actors = _moving_info->moving_actors;
		_moving_info.reset();
		_current_substate = SubState::Default;
	}
}

void StageEditorSceneState_Edit_Move::HandleInputImpl_RectSelecting(StageEditorScene& parent_scene, const float delta_seconds)
{
	if (_drag_drop_end_viewport_pos != nullptr)
	{
		const float rect_left_viewport = std::min(_drag_drop_start_viewport_pos->x, _drag_drop_end_viewport_pos->x);
		const float rect_top_viewport = std::min(_drag_drop_start_viewport_pos->y, _drag_drop_end_viewport_pos->y);
		const float rect_right_viewport = std::max(_drag_drop_start_viewport_pos->x, _drag_drop_end_viewport_pos->x);
		const float rect_bottom_viewport = std::max(_drag_drop_start_viewport_pos->y, _drag_drop_end_viewport_pos->y);
		const FRectAA rect{
			parent_scene._camera_params.TransformPosition_ViewportToWorld(Vector2D{ rect_left_viewport, rect_top_viewport }),
			parent_scene._camera_params.TransformPosition_ViewportToWorld(Vector2D{ rect_right_viewport, rect_bottom_viewport })
		};

		std::vector<Actor*> actors_inside_rect;
		GetActorsInsideAARect(parent_scene, rect, actors_inside_rect);

		for (auto& actor : actors_inside_rect)
		{
			SelectActor(actor);
		}

		_current_substate = SubState::Default;
		_drag_drop_start_viewport_pos.reset();
		_drag_drop_end_viewport_pos.reset();
		return;
	}
}

void StageEditorSceneState_Edit_Move::HandleInputImpl_MovingDuplicatedActors(StageEditorScene& parent_scene, const float delta_seconds)
{
	if (_drag_drop_end_viewport_pos)
	{
		int mouse_tile_to_x, mouse_tile_to_y;
		GetTileIndex(
			parent_scene._camera_params.TransformPosition_ViewportToWorld(*_drag_drop_end_viewport_pos),
			mouse_tile_to_x,
			mouse_tile_to_y
		);

		if (mouse_tile_to_x != _duplication_info->mouse_tile_from_x || mouse_tile_to_y != _duplication_info->mouse_tile_from_y)
		{
			std::vector<std::shared_ptr<CommandBase>> summon_commands;

			for (auto& actor : _duplication_info->duplicated_actors)
			{
				_duplication_info->spawn_actor_infos.at(actor)
					->initial_params->transform.position = actor->GetActorWorldPosition();

				summon_commands.push_back(
					std::make_shared<stage_editor_scene::CmdSummonActor>(
						&parent_scene,
						actor,
						_duplication_info->spawn_actor_infos.at(actor)
					)
				);
			}
		
			parent_scene._command_history->ExecuteAndPush(
				std::make_shared<CmdCommandList>(summon_commands)
			);
		}

		_duplication_info.reset();
		_current_substate = SubState::Default;
		return;
	}

	for (auto& actor : _duplication_info->duplicated_actors)
	{
		int mouse_tile_index_x, mouse_tile_index_y;
		GetTileIndex(
			parent_scene._camera_params.TransformPosition_ViewportToWorld(DeviceInput::GetMousePosition()),
			mouse_tile_index_x,
			mouse_tile_index_y
		);

		const std::pair<int, int> offset_tiles_to_actor_snap_pos = _duplication_info->offset_tiles_to_actor_snap_pos.at(actor);

		const Vector2D snap_position = GetTileCenterPosition(
			parent_scene._camera_params.TransformPosition_ViewportToWorld(DeviceInput::GetMousePosition()),
			offset_tiles_to_actor_snap_pos.first,
			offset_tiles_to_actor_snap_pos.second
		);

		const Vector2D snap_pos_to_actor_pos = _duplication_info->snap_pos_to_actor_pos.at(actor);
		
		actor->SetActorWorldPosition(snap_position + snap_pos_to_actor_pos);
	}
}

void StageEditorSceneState_Edit_Move::HandleInputImpl_ChangingStageLength(StageEditorScene& parent_scene, const float delta_seconds)
{
	if (DeviceInput::IsPressed(KEY_INPUT_LEFT))
	{
		parent_scene.GetStageRef().SetStageLength(parent_scene.GetStageRef().GetStageLength() - 1);
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_RIGHT))
	{
		parent_scene.GetStageRef().SetStageLength(parent_scene.GetStageRef().GetStageLength() + 1);
	}

}

void StageEditorSceneState_Edit_Move::UnselectAllActors()
{
	_selected_actors.clear();
}

void StageEditorSceneState_Edit_Move::SelectActor(Actor* actor)
{
	if (IsSelectedActor(actor))
	{
		return;
	}

	_selected_actors.push_back(actor);
}

void StageEditorSceneState_Edit_Move::UnSelectActor(Actor* actor)
{
	if (!IsSelectedActor(actor))
	{
		return;
	}

	auto it = std::find(_selected_actors.begin(), _selected_actors.end(), actor);
	_selected_actors.erase(it);
}

bool StageEditorSceneState_Edit_Move::IsAnyActorSelected() const
{
	return !_selected_actors.empty();
}

bool StageEditorSceneState_Edit_Move::IsSelectedActor(Actor* actor) const
{
	return std::find(_selected_actors.begin(), _selected_actors.end(), actor) != _selected_actors.end();
}

void StageEditorSceneState_Edit_Move::BeginMovingSelectedActors(StageEditorScene& parent_scene)
{
	const Vector2D mouse_pos = DeviceInput::GetMousePosition();

	// 移動開始
	_moving_info = std::make_unique<MovingInfo>();

	int mouse_tile_index_x, mouse_tile_index_y;
	GetTileIndex(
		parent_scene._camera_params.TransformPosition_ViewportToWorld(mouse_pos),
		mouse_tile_index_x,
		mouse_tile_index_y
	);
	_moving_info->mouse_tile_from_x = mouse_tile_index_x;
	_moving_info->mouse_tile_from_y = mouse_tile_index_y;

	struct OccupyingArea 
	{
		int left{ INT_MAX }, top{ INT_MAX }, right{ INT_MIN }, bottom{ INT_MIN };
	};
	OccupyingArea actors_occupying_area;

	for (auto& actor : _selected_actors)
	{
		_moving_info->moving_actors.push_back(actor);

		int actor_tile_index_x, actor_tile_index_y;
		GetTileIndex(
			actor->GetSnapPosition(),
			actor_tile_index_x,
			actor_tile_index_y
		);

		_moving_info->moved_from[actor] = actor->GetActorWorldPosition();
		_moving_info->offset_tiles_to_actor_snap_pos[actor] = MovingInfo::OffsetTiles2
		{
			actor_tile_index_x - mouse_tile_index_x,
			actor_tile_index_y - mouse_tile_index_y
		};
		_moving_info->snap_pos_to_actor_pos[actor] = actor->GetSnapPositionToActorPosition();

		OccupyingArea actor_occupying_area{};
		StageInteractiveScene::GetActorOccupyingArea(
			actor,
			actor_occupying_area.left,
			actor_occupying_area.top,
			actor_occupying_area.right,
			actor_occupying_area.bottom
		);
		
		actors_occupying_area.left = std::min(actors_occupying_area.left, actor_occupying_area.left);
		actors_occupying_area.top = std::min(actors_occupying_area.top, actor_occupying_area.top);
		actors_occupying_area.right = std::max(actors_occupying_area.right, actor_occupying_area.right);
		actors_occupying_area.bottom = std::max(actors_occupying_area.bottom, actor_occupying_area.bottom);
	}

	auto& offset_to_area_edge = _moving_info->offset_tiles_to_occupying_area_edge;
	offset_to_area_edge.to_left = actors_occupying_area.left - mouse_tile_index_x;
	offset_to_area_edge.to_top = actors_occupying_area.top - mouse_tile_index_y;
	offset_to_area_edge.to_right = actors_occupying_area.right - mouse_tile_index_x;
	offset_to_area_edge.to_bottom = actors_occupying_area.bottom - mouse_tile_index_y;

	UnselectAllActors();
	_current_substate = SubState::MovingActors;
}

void StageEditorSceneState_Edit_Move::CancelMovingActor()
{
	if (_current_substate != SubState::MovingActors)
	{
		return;
	}

	for (auto& actor : _moving_info->moving_actors)
	{
		actor->SetActorWorldPosition(_moving_info->moved_from.at(actor));
	}

	_moving_info.reset();
	_current_substate = SubState::Default;
}

void StageEditorSceneState_Edit_Move::BeginDuplicationOfSelectedActors(StageEditorScene& parent_scene)
{
	const Vector2D mouse_pos = DeviceInput::GetMousePosition();

	_duplication_info = std::make_unique<DuplicationInfo>();
	int mouse_tile_index_x, mouse_tile_index_y;
	GetTileIndex(
		parent_scene._camera_params.TransformPosition_ViewportToWorld(mouse_pos),
		mouse_tile_index_x,
		mouse_tile_index_y
	);

	_duplication_info->mouse_tile_from_x = mouse_tile_index_x;
	_duplication_info->mouse_tile_from_y = mouse_tile_index_y;

	for (auto& actor : _selected_actors)
	{
		if (!IsDuplicatableActor(actor))
		{
			continue;
		}

		const SpawnActorInfo& copy_src_spawn_info = *parent_scene.GetSpawnActorInfo(actor);
		const EEntityType entity_type = copy_src_spawn_info.entity_type;

		// 初期化パラメータを複製
		std::shared_ptr<ActorInitialParams> duplicated_initial_params = ActorFactory::CreateInitialParamsByEntityType(entity_type);
		nlohmann::json json{};
		copy_src_spawn_info.initial_params->ToJsonObject(json);
		duplicated_initial_params->FromJsonObject(json);

		// stage_editor_scene::CmdSummonActorコマンドの前準備として, 生成したアクターを待機リストに追加
		Actor* const duplicated_actor = ActorFactory::CreateAndInitializeActorByEntityType(entity_type, duplicated_initial_params.get(), &parent_scene);
		parent_scene.AddActorToWaitingRoom(duplicated_actor);

		const Vector2D snap_pos_to_actor_pos = duplicated_actor->GetSnapPositionToActorPosition();
		const Vector2D snap_position = duplicated_actor->GetActorWorldPosition() - snap_pos_to_actor_pos;
		int snap_tile_index_x, snap_tile_index_y;
		GetTileIndex(snap_position, snap_tile_index_x, snap_tile_index_y);

		_duplication_info->duplicated_actors.push_back(duplicated_actor);
		_duplication_info->offset_tiles_to_actor_snap_pos[duplicated_actor] = std::make_pair<int, int>(snap_tile_index_x - mouse_tile_index_x, snap_tile_index_y - mouse_tile_index_y);
		_duplication_info->snap_pos_to_actor_pos[duplicated_actor] = snap_pos_to_actor_pos;
		_duplication_info->spawn_actor_infos[duplicated_actor] = std::make_shared<SpawnActorInfo>();
		_duplication_info->spawn_actor_infos.at(duplicated_actor)->entity_type = entity_type;
		_duplication_info->spawn_actor_infos.at(duplicated_actor)->initial_params = duplicated_initial_params;
	}

	UnselectAllActors();
	_current_substate = SubState::MovingDuplicatedActors;
}

void StageEditorSceneState_Edit_Move::CancelDuplication(StageEditorScene& parent_scene)
{
	if (!_duplication_info)
	{
		return;
	}

	for (auto& actor : _duplication_info->duplicated_actors)
	{
		parent_scene.DestroyActor(actor);
	}

	_duplication_info.reset();
	_current_substate = SubState::Default;
}

bool StageEditorSceneState_Edit_Move::IsDuplicatableActor(Actor* actor) const
{
	if
	(
		actor->IsDerivedFrom<Player>() ||
		actor->IsDerivedFrom<GoalFlag>()
	)
	{
		return false;
	}

	return true;
}

void StageEditorSceneState_Edit_Move::RemoveSelectedActors(StageEditorScene& parent_editor_scene)
{
	if (!IsAnyActorSelected())
	{
		return;
	}

	std::vector<Actor*> deletion_targets;
	for (auto& actor : _selected_actors)
	{
		if (IsDeletableActor(parent_editor_scene, actor))
		{
			deletion_targets.push_back(actor);
		}
	}

	if (!deletion_targets.empty())
	{
		parent_editor_scene._command_history->ExecuteAndPush(
			std::make_shared<stage_editor_scene::CmdRemoveActors>(&parent_editor_scene, deletion_targets)
		);
	}

	parent_editor_scene._editor_scene_sounds->se_remove_actor->Play();

	UnselectAllActors();
}

bool StageEditorSceneState_Edit_Move::IsDeletableActor(StageEditorScene& parent_editor_scene, Actor* actor, const bool should_push_message) const
{
	const bool is_player = dynamic_cast<Player*>(actor) != nullptr;
	if (is_player)
	{
		parent_editor_scene.PushEditorMessage(u8"プレイヤーを削除することはできません", parent_editor_scene._editor_scene_sounds->se_warning.get());
		return false;
	}

	const bool is_goal = dynamic_cast<GoalFlag*>(actor) != nullptr;
	if (is_goal)
	{
		parent_editor_scene.PushEditorMessage(u8"ゴールを削除することはできません", parent_editor_scene._editor_scene_sounds->se_warning.get());
		return false;
	}

	return true;
}

void StageEditorSceneState_Edit_Move::OnBeginDragDrop()
{
	_drag_drop_start_viewport_pos = std::make_unique<Vector2D>(DeviceInput::GetMousePosition());
	_drag_drop_end_viewport_pos.reset();
}

void StageEditorSceneState_Edit_Move::OnEndDragDrop()
{
	_drag_drop_end_viewport_pos = std::make_unique<Vector2D>(DeviceInput::GetMousePosition());
}

bool StageEditorSceneState_Edit_Move::IsDoingDragDrop()
{
	return _drag_drop_start_viewport_pos != nullptr && _drag_drop_end_viewport_pos == nullptr;
}

void StageEditorSceneState_Edit_Move::GetActorsInsideAARect(StageEditorScene& parent_editor_scene, const FRectAA world_rect, std::vector<Actor*>& contained_actors)
{
	for (auto& actor : parent_editor_scene._actors)
	{
		bool is_contained = true;

		std::vector<Vector2D> convex_vertices;
		actor->GetWorldConvexPolygonVertices(convex_vertices);
		
		for (auto& vertex : convex_vertices)
		{
			is_contained &= GeometricUtility::DoesAARectContainPoint(vertex, world_rect);
		}

		if (is_contained)
		{
			contained_actors.push_back(actor);
		}
	}
}