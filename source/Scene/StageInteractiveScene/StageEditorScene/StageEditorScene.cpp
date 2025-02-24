#include "StageEditorScene.h"
#include "StageEditorColor.h"
#include "StageEditorSceneStatesInclude.h"
#include "EditorMessageManager.h"
#include "Input/DeviceInput.h"
#include "GameSystems/GameConfig/GameConfig.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "GameSystems/FontManager.h"
#include "GameSystems/Sound/SoundManager.h"
#include "Actor/AllActorsInclude_generated.h"
#include <tuple>

using namespace DxLib;

StageEditorScene::StageEditorScene()
	: _should_show_controls(true)
	, _is_unsaved(false)
	, _is_control_window_expanded(false)
{
}

StageEditorScene::~StageEditorScene()
{
}

namespace
{
	template<typename Base, typename Derived>
	std::shared_ptr<Base> upper_pointer_cast(const std::shared_ptr<Derived>& derived)
	{
		static_assert(std::is_base_of<Base, Derived>::value, "Base is not a base class of Derived");
		return std::static_pointer_cast<Base>(derived);
	}
}

void StageEditorScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);
	typedef SceneBase::traits<StageEditorScene>::initial_params_type InitialParamsType;
	const InitialParamsType* const editor_scene_params = dynamic_cast<const InitialParamsType* const>(scene_params);
	assert(editor_scene_params);

	// コマンド履歴の初期化
	{
		_command_history = std::make_shared<CommandHistory>();

		_command_history->command_history_events.OnStateChanged += [this]() {MarkAsUnsaved(); };

		_command_history->command_history_events.OnNewCommandPushed += [this](const std::shared_ptr<CommandBase>& new_command)
			{
				OnNewCommandPushed(new_command);
			};
	}

	// エディターメッセージマネージャーの初期化
	{
		_editor_message_manager = std::make_unique<EditorMessageManager>();
		EditorMessageManagerDesc msg_manager_desc = {};
		msg_manager_desc.max_message_count = 3;
		msg_manager_desc.display_time = 6.f;
		msg_manager_desc.message_card_size = ImVec2(200, 100);
		msg_manager_desc.card_back_color = ImVec4(0,0,0,0.5);
		msg_manager_desc.card_front_color = ImVec4(1,1,1,1);
		msg_manager_desc.card_border_color = ImVec4(0.5, 0.5, 0.5, 1.0);
		msg_manager_desc.window_pos = ImVec2(4, WINDOW_SIZE_Y - 4);
		msg_manager_desc.window_pivot = ImVec2(0, 1);
		msg_manager_desc.message_type_icon_size = ImVec2(32, 32);
		_editor_message_manager->Initialize(msg_manager_desc);
	}

	MakeSidebarInfo();

	LoadSummonInfos();

	LoadEditorSceneSounds();
	_editor_scene_sounds->bgm->Play();

	// ステートの初期化
	_state_stack = std::make_shared<SceneStateStack<StageEditorScene>>();
	assert(_state_stack->empty());
	_state_stack->ChangeState(*this, std::make_shared<StageEditorSceneState_Edit_Summon>());

	SetWorldTimerActive(false);

	LoadStageEditorConfig(*GameConfig::GetInstance().GetConfigItem<StageEditorConfig>());
}

SceneType StageEditorScene::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);
	SceneType ret = GetSceneType();

	ImGui::PushFont(FontManager::GetInstance().GetFont(1));

	// 全ステート共通の処理
	if (_is_end_scene_requested)
	{
		ImGui::PopFont();
		return destination_scene;
	}
	
	// ステートの更新
	_state_stack->Tick(*this, delta_seconds);

	// エディターメッセージの更新
	_editor_message_manager->Tick(delta_seconds);

	sidebar_info->OnLoopEnd();

	ImGui::PopFont();

	return ret;
}

void StageEditorScene::LoadStageEditorConfig(const StageEditorConfig& config)
{
	SetSoundsVolume(config);
}

Actor* StageEditorScene::GetActorAt(const Vector2D& viewport_position) const
{
	if (!IsMouseOnStage(viewport_position))
	{
		return nullptr;
	}

	const Vector2D world_position = _camera_params.TransformPosition_ViewportToWorld(viewport_position);

	for (auto& actor : _actors)
	{
		std::vector<Vector2D> vertices;
		actor->GetWorldConvexPolygonVertices(vertices);

		if (GeometricUtility::DoesConvexPolygonContainsPoint(world_position, vertices))
		{
			return actor;
		}
	}

	return nullptr;
}

bool StageEditorScene::IsMouseOnStage(const Vector2D& mouse_pos) const
{
	const bool mouse_is_out_of_window = (mouse_pos.x < 0) || (mouse_pos.x > WINDOW_SIZE_X) || (mouse_pos.y < 0) || (mouse_pos.y > WINDOW_SIZE_Y);
	if (mouse_is_out_of_window)
	{
		return false;
	}

	if (ImGui::IsAnyWindowHovered())
	{
		return false;
	}

	if (ImGui::IsAnyPopupOpen())
	{
		return false;
	}

	return true;
}

void StageEditorScene::DrawActorConvex(Actor* actor, int color, int alpha)
{
	if (actor == nullptr)
	{
		return;
	}
	std::vector<Vector2D> vertices;
	actor->GetWorldConvexPolygonVertices(vertices);
	for (auto& v : vertices)
	{
		v = Vector2D::WorldToViewport(v, _camera_params);
	}

	int last_blend_mode, last_blend_value;
	GetDrawBlendMode(&last_blend_mode, &last_blend_value);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	GraphDrawHelper::DrawConvexPolygon(vertices, color);

	SetDrawBlendMode(last_blend_mode, last_blend_value);
}

void StageEditorScene::Draw()
{
	__super::Draw();

	_state_stack->Draw(*this);
}

void StageEditorScene::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);

	_state_stack->DrawForeground(*this, canvas_info);
}

void StageEditorScene::Finalize()
{
	_state_stack->Finalize(*this);

	_editor_scene_sounds.reset();

	for (auto& actor : _actor_waiting_room)
	{
		DestroyActor(actor);
	}
	_actor_waiting_room.clear();

	_command_history.reset();

	sidebar_info.reset();

	_summon_infos.clear();

	_editor_message_manager->Finalize();
	_editor_message_manager.reset();

	__super::Finalize();
}

void StageEditorScene::UpdateCameraParams(const float delta_seconds)
{
	_state_stack->UpdateCameraParams(*this, delta_seconds);

	_camera_params.screen_scale = std::min(GetMaxScreenScale(), _camera_params.screen_scale);
	ClampCameraPositionInStage(_camera_params);
}

void StageEditorScene::MakeSidebarInfo()
{
	sidebar_info = std::make_unique<SidebarInfo>();

	constexpr int OPEN_CLOSE_BUTTON_SIZE = 32;
	constexpr int GENERAL_BUTTON_SIZE = 48;

	// Load icon textures
	using ArrayElement = std::tuple<MasterDataID, ImGui::Texture*>;
	const std::vector<ArrayElement> icon_textures =
	{
		std::make_tuple(MasterDataID(328), &sidebar_info->img_editmode_add),
		std::make_tuple(MasterDataID(336), &sidebar_info->img_editmode_modify),
		std::make_tuple(MasterDataID(354), &sidebar_info->img_editmode_move),
		std::make_tuple(MasterDataID(346), &sidebar_info->img_editmode_remove),
		std::make_tuple(MasterDataID(352), &sidebar_info->img_undo),
		std::make_tuple(MasterDataID(351), &sidebar_info->img_redo),
		std::make_tuple(MasterDataID(333), &sidebar_info->img_editor_settings),
		std::make_tuple(MasterDataID(350), &sidebar_info->img_stage_settings),
		std::make_tuple(MasterDataID(353), &sidebar_info->img_enter_change_stage_length),
		std::make_tuple(MasterDataID(304), &sidebar_info->img_enter_thumbnail_shooting),
		std::make_tuple(MasterDataID(331), &sidebar_info->img_open_modal_background),
		std::make_tuple(MasterDataID(375), &sidebar_info->img_open_modal_bgm),
		std::make_tuple(MasterDataID(311), &sidebar_info->img_open_leftbar),
		std::make_tuple(MasterDataID(310), &sidebar_info->img_close_leftbar),
		// TODO: エディターで使用するアイコン画像を追加する場合はここに追記
	};

	for (auto& elm : icon_textures)
	{
		const MasterDataID icon_id = std::get<0>(elm);
		ImGui::Texture* texture = std::get<1>(elm);
		MasterHelper::GetGameIconImguiImage(
			icon_id,
			texture->im_tex_id,
			texture->u0(),
			texture->v0(),
			texture->u1(),
			texture->v1()
		);
	}

	sidebar_info->common_button_size = GENERAL_BUTTON_SIZE;

	// Left sidebar
	constexpr int LEFT_BAR_WIDTH = 200;
	constexpr int LEFT_BAR_HEIGHT = 300;
	constexpr float LEFT_BAR_TOGGLE_TIME = 0.25;
	constexpr int LEFT_BAR_UPPER_PADDING = 0;

	sidebar_info->left.is_open = true;
	sidebar_info->left.current_width = LEFT_BAR_WIDTH;
	sidebar_info->left.desc.type = ImGui::SidebarType::LEFT;
	sidebar_info->left.desc.width = LEFT_BAR_WIDTH;
	sidebar_info->left.desc.height = LEFT_BAR_HEIGHT;
	sidebar_info->left.desc.screen_width = WINDOW_SIZE_X;
	sidebar_info->left.desc.screen_height = WINDOW_SIZE_Y;
	sidebar_info->left.desc.delta_width =
		static_cast<float>(LEFT_BAR_WIDTH) / (static_cast<float>(FRAME_RATE) * LEFT_BAR_TOGGLE_TIME);
	sidebar_info->left.desc.upper_padding = LEFT_BAR_UPPER_PADDING;
	sidebar_info->left.desc.button_size = OPEN_CLOSE_BUTTON_SIZE;

	const ImGui::Texture& left_button_open = sidebar_info->img_open_leftbar;
	sidebar_info->left.desc.texture_id_open = left_button_open.im_tex_id;
	sidebar_info->left.desc.uv0_open = left_button_open.uv0;
	sidebar_info->left.desc.uv1_open = left_button_open.uv1;

	const ImGui::Texture& left_button_close = sidebar_info->img_close_leftbar;
	sidebar_info->left.desc.texture_id_close = left_button_close.im_tex_id;
	sidebar_info->left.desc.uv0_close = left_button_close.uv0;
	sidebar_info->left.desc.uv1_close = left_button_close.uv1;

	// Right sidebar
	constexpr int RIGHT_BAR_WIDTH = 360;
	constexpr int RIGHT_BAR_HEIGHT = 800;
	constexpr float RIGHT_BAR_TOGGLE_TIME = 0.25;
	constexpr float RIGHT_BAR_UPPER_PADDING = 0;

	sidebar_info->right.is_open = true;
	sidebar_info->right.current_width = RIGHT_BAR_WIDTH;
	sidebar_info->right.desc.type = ImGui::SidebarType::RIGHT;
	sidebar_info->right.desc.width = RIGHT_BAR_WIDTH;
	sidebar_info->right.desc.height = RIGHT_BAR_HEIGHT;
	sidebar_info->right.desc.screen_width = WINDOW_SIZE_X;
	sidebar_info->right.desc.screen_height = WINDOW_SIZE_Y;
	sidebar_info->right.desc.delta_width =
		static_cast<float>(RIGHT_BAR_WIDTH) / (static_cast<float>(FRAME_RATE) * RIGHT_BAR_TOGGLE_TIME);
	sidebar_info->right.desc.upper_padding = RIGHT_BAR_UPPER_PADDING;
	sidebar_info->right.desc.button_size = OPEN_CLOSE_BUTTON_SIZE;

	const ImGui::Texture& right_button_open = sidebar_info->img_close_leftbar;
	sidebar_info->right.desc.texture_id_open = right_button_open.im_tex_id;
	sidebar_info->right.desc.uv0_open = right_button_open.uv0;
	sidebar_info->right.desc.uv1_open = right_button_open.uv1;

	const ImGui::Texture& right_button_close = sidebar_info->img_open_leftbar;
	sidebar_info->right.desc.texture_id_close = right_button_close.im_tex_id;
	sidebar_info->right.desc.uv0_close = right_button_close.uv0;
	sidebar_info->right.desc.uv1_close = right_button_close.uv1;
}

void StageEditorScene::ToggleSidebarOpenClosed(SidebarInfo::SidebarState& sidebar_state)
{
	sidebar_state.is_open = !sidebar_state.is_open;
}

void StageEditorScene::OpenSidebar(SidebarInfo::SidebarState& sidebar_state)
{
	sidebar_state.is_open = true;
}

void StageEditorScene::PushEditorMessage(const std::string& message, const SoundInstance* const sound)
{
	_editor_message_manager->EnqueueMessage(message);

	if (sound != nullptr)
	{
		sound->Play();
	}
}

bool StageEditorScene::BeginControlsImGuiWindow()
{
	constexpr const char* str_id_controls_window = "editor_scene_controls_window";

	const ImVec2 window_size(700.f, _is_control_window_expanded ? Collon2D::WINDOW_SIZE_Y * 0.5f : 100.f);
	const ImVec2 window_pos = ImVec2{210, Collon2D::WINDOW_SIZE_Y - 4};
	constexpr ImVec2 window_pivot{ 0.f, 1.f };
	constexpr float window_bg_alpha = 0.75f;

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	// 操作説明のフォントID
	constexpr MasterDataID controls_font_id = MasterDataID(2);

	// 操作説明ウィンドウの拡大・縮小アイコン
	constexpr MasterDataID icon_id_expand_control_window = 323;
	constexpr MasterDataID icon_id_collapse_control_window = 309;
	ImGui::Texture icon_texture_expand;
	ImGui::Texture icon_texture_collapse;
	MasterHelper::GetGameIconImguiImage(icon_id_expand_control_window, icon_texture_expand);
	MasterHelper::GetGameIconImguiImage(icon_id_collapse_control_window, icon_texture_collapse);

	ImGui::PushFont(FontManager::GetInstance().GetFont(controls_font_id));
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pivot);
	ImGui::SetNextWindowBgAlpha(window_bg_alpha);

	const bool ret = ImGui::Begin(str_id_controls_window, NULL, flags);
	{
		const ImVec2 initial_cursor_pos = ImGui::GetCursorPos();

		constexpr ImVec2 expand_collapse_button_size{ 16,16 };
		const ImGui::Texture& button_icon_texture = _is_control_window_expanded ? icon_texture_collapse : icon_texture_expand;
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - expand_collapse_button_size.x);
		ImGui::SetCursorPosY(0);
		if (ImGui::ImageButton("button_id_expand_collapse_control_window", button_icon_texture, expand_collapse_button_size))
		{
			_is_control_window_expanded = !_is_control_window_expanded;
		}

		ImGui::SetCursorPos(initial_cursor_pos);
	}
	return ret;
}

void StageEditorScene::EndControlsImGuiWindow()
{
	ImGui::End();
	ImGui::PopFont();
}

void StageEditorScene::OnNewCommandPushed(const std::shared_ptr<CommandBase>& new_command)
{
	// コマンド追加時に呼ばれる処理
	// - アクターのスナップ位置がタイルの中心になるように調整する
	// - アクターがステージ外に出ないように調整する. 完全に外に出ている場合は削除する

	// TODO: 必要であれば、変更が加わったアクターのみリロードしてパフォーマンス改善
	ReloadAllActorsInStage();

	int stage_left, stage_top, stage_right, stage_bottom;
	GetStageTileIndex(stage_left, stage_top, stage_right, stage_bottom);
	const int stage_width = stage_right - stage_left + 1;
	const int stage_height = stage_bottom - stage_top + 1;

	// 完全にステージ外に出ているアクターを格納するリスト
	std::vector<Actor*> remove_list;

	for (auto& actor : _actors)
	{
		// NOTE: スポーン情報を持つのはステージデータに含まれるアクターのみ
		if (!IsActorInStage(actor))
		{
			continue;
		}

		// スナップ位置がタイルの中心になるように調整
		const Vector2D actor_snap_position = actor->GetSnapPosition();
		const int snap_position_x = static_cast<int>(actor_snap_position.x);
		const int snap_position_y = static_cast<int>(actor_snap_position.y);
		constexpr int HALF_TILE_SIZE = UNIT_TILE_SIZE / 2;
		const bool is_snap_position_located_on_tile_center = 
			positive_modulo(snap_position_x, UNIT_TILE_SIZE) == HALF_TILE_SIZE &&
			positive_modulo(snap_position_y, UNIT_TILE_SIZE) == HALF_TILE_SIZE;


		if (!is_snap_position_located_on_tile_center)
		{
			// 一番近いタイルの中心にスナップする.
			// この段階でステージ内に収まっておらずとも、後続の処理でステージ内に収まるように調整されるので問題ない
			const int nearest_tile_center_x = (snap_position_x / UNIT_TILE_SIZE) * UNIT_TILE_SIZE + (snap_position_x < 0 ? -HALF_TILE_SIZE : HALF_TILE_SIZE);
			const int nearest_tile_center_y = (snap_position_y / UNIT_TILE_SIZE) * UNIT_TILE_SIZE + (snap_position_y < 0 ? -HALF_TILE_SIZE : HALF_TILE_SIZE);
			const Vector2D new_actor_position = actor->GetSnapPositionToActorPosition() + Vector2D(nearest_tile_center_x, nearest_tile_center_y);
			
			auto cmd_move_to_nearest_tile_center = std::make_shared<stage_editor_scene::CmdChangeActorPosition>(
				this,
				actor,
				actor->GetActorWorldPosition(),
				new_actor_position
			);

			cmd_move_to_nearest_tile_center->CallDo();

			new_command->AddSubsequentCommand(cmd_move_to_nearest_tile_center);
		}


		int actor_left, actor_top, actor_right, actor_bottom;
		GetActorOccupyingArea(actor, actor_left, actor_top, actor_right, actor_bottom);

		const int actor_width = actor_right - actor_left + 1;
		const int actor_height = actor_bottom - actor_top + 1;
		if (actor_width > stage_width || actor_height > stage_height)
		{
			throw std::runtime_error("Actor is larger than stage");
		}

		// アクターとステージの重複区間を求め, サブコマンドが必要か判定する
		const int overlapping_x = std::min(stage_right, actor_right) - std::max(stage_left, actor_left) + 1;
		const int overlapping_y = std::min(stage_bottom, actor_bottom) - std::max(stage_top, actor_top) + 1;

		if (overlapping_x <= 0 || overlapping_y <= 0)
		{
			remove_list.push_back(actor);
		}
		else if (overlapping_x < actor_width || overlapping_y < actor_height)
		{
			const int abs_delta_tiles_x = actor_width - overlapping_x;
			const int abs_delta_tiles_y = actor_height - overlapping_y;
			const int sign_x = (actor_left < stage_left) ? 1 : -1;
			const int sign_y = (actor_top < stage_top) ? 1 : -1;

			const int delta_position_x = sign_x * abs_delta_tiles_x * UNIT_TILE_SIZE;
			const int delta_position_y = sign_y * abs_delta_tiles_y * UNIT_TILE_SIZE;

			const Vector2D move_to =
				actor->GetActorWorldPosition() + Vector2D(delta_position_x, delta_position_y);

			auto move_command = std::make_shared<stage_editor_scene::CmdChangeActorPosition>(
				this,
				actor,
				actor->GetActorWorldPosition(),
				move_to
			);

			move_command->CallDo();

			new_command->AddSubsequentCommand(move_command);
		}
	}

	if (!remove_list.empty())
	{
		auto cmd_remove_actors_out_of_stage = std::make_shared<stage_editor_scene::CmdRemoveActors>(this, remove_list);
		cmd_remove_actors_out_of_stage->CallDo();
		new_command->AddSubsequentCommand(cmd_remove_actors_out_of_stage);
	}
}
void StageEditorScene::AddActorToWaitingRoom(Actor* const actor)
{
	if (std::find(_actor_waiting_room.begin(), _actor_waiting_room.end(), actor) != _actor_waiting_room.end())
	{
		throw std::runtime_error("Actor is already in waiting room");
	}

	_actor_waiting_room.push_back(actor);
}

void StageEditorScene::LoadSummonInfos()
{
	// メモリ確保
	std::unordered_map<EEntityCategory, size_t> entity_counts;
	MdEntity::CountEntitiesInCategory(entity_counts);
	for (auto& pair : entity_counts)
	{
		_summon_infos[pair.first] = std::vector<std::shared_ptr<SummonEntityInfo>>();
		_summon_infos.at(pair.first).reserve(pair.second);
	}

	
	const std::vector<MdEntity>& entity_data_list = MdEntity::GetData();
	for (auto& entity_data : entity_data_list)
	{
		EEntityCategory category = EnumInfo<EEntityCategory>::StringToEnum(entity_data.category_str);
		_summon_infos.at(category).push_back(std::make_shared<SummonEntityInfo>());
		std::shared_ptr<SummonEntityInfo> new_summon_info = _summon_infos.at(category).at(_summon_infos.at(category).size() - 1);

		const std::string json_path = std::string(ResourcePaths::Dir::PARAMS) + entity_data.initial_params_json_name;
		std::ifstream json_file(json_path);

		if (!json_file)
		{
			throw std::runtime_error("Failed to open file: " + json_path);
		}

		nlohmann::json params_json = nlohmann::json::parse(json_file);

		new_summon_info->initial_params_json = params_json;
		new_summon_info->entity_id = entity_data.id;
	}
}

void StageEditorScene::SaveStage()
{
	const std::string path = GetStageRef().GetStageId().GetJsonFilePath();
	GetStageRef().SaveToFile(path);
	MarkAsSaved();
}

void StageEditorScene::LoadEditorSceneSounds()
{
	_editor_scene_sounds = std::make_unique<EditorSceneSounds>();

	_editor_scene_sounds->bgm = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/bgm/bgm_editor_scene.ogg");
	_editor_scene_sounds->bgm->SetLoopEnabled(true);

	const std::string EDITOR_SCENE_SE_DIR = "resources/sounds/se/editor_scene/";
	_editor_scene_sounds->se_place_actor = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_place_actor.mp3");
	_editor_scene_sounds->se_camera_shutter = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_camera_shutter.mp3");
	_editor_scene_sounds->se_warning = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_warning.ogg");
	_editor_scene_sounds->se_undo = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_undo.ogg");
	_editor_scene_sounds->se_redo = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_redo.ogg");
	_editor_scene_sounds->se_remove_actor = SoundManager::GetInstance().MakeSoundInstance(EDITOR_SCENE_SE_DIR + "se_remove_actor.ogg");
}

void StageEditorScene::SetSoundsVolume(const StageEditorConfig& config)
{
	_editor_scene_sounds->bgm->SetVolume(config.sound_volume.bgm);

	for (auto& se : _editor_scene_sounds->GetSEList())
	{
		se->SetVolume(config.sound_volume.se);
	}
}

void StageEditorScene::EndScene()
{
	_is_end_scene_requested = true;
}