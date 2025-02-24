#include "StageSelectScene.h"
#include "Scene/AllScenesInclude.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "GameSystems/FontManager.h"
#include "GameSystems/Sound/SoundManager.h"
#include "Input/DeviceInput.h"
#include "Utility/UIElements/UIElements.h"
#include <fstream>
#include <stdio.h>
#include <Windows.h>	// ファイル・ディレクトリ操作のため


namespace ModeChangeButtonsImageInfo
{
	const std::string image_path = std::string(ResourcePaths::Dir::IMAGES) + "ui/buttons/action_button_a.png";
	const Vector2D divisions(4, 2);
	const Vector2D unit_size(256, 256);
	constexpr uint8_t graph_index_left_unselected = 0;
	constexpr uint8_t graph_index_left_selected = 4;
	constexpr uint8_t graph_index_right_unselected = 1;
	constexpr uint8_t graph_index_right_selected = 5;
	constexpr float ratio_image_to_rect = 1.5;
	constexpr uint8_t rect_size = 64;
}


namespace SmallThumbNailsGrid
{
	// 96*72単位のグリッド
	constexpr int grids_left = 100;
	constexpr int grids_top = 130;
	constexpr int cell_size_x = 160;
	constexpr int cell_size_y = 90;
	constexpr int num_cells_x = 3;
	constexpr int num_cells_y = 5;
	constexpr int line_color = 0x111133;
	constexpr int line_thickness = 3;
	constexpr int padding = 1;

	constexpr int grids_right = grids_left + cell_size_x * num_cells_x;
	constexpr int grids_bottom = grids_top + cell_size_y * num_cells_y;

	constexpr int num_cells_total = num_cells_x * num_cells_y;

	const Vector2D grids_lefttop(grids_left, grids_top);
	const Vector2D cell_extent(cell_size_x, cell_size_y);

	constexpr int thumbnails_left = grids_left + (line_thickness - 1) / 2 + padding + 1;
	constexpr int thumbnails_top = grids_top + (line_thickness - 1) / 2 + padding + 1;
	constexpr int thumbnail_size_x = cell_size_x - (line_thickness - 1) - padding * 2;
	constexpr int thumbnail_size_y = cell_size_y - (line_thickness - 1) - padding * 2;

	const Vector2D new_stage_panel_lefttop(thumbnails_left, thumbnails_top);
	const Vector2D new_stage_extent(thumbnail_size_x, thumbnail_size_y);
	constexpr const uint16_t new_stage_panel_font_size = thumbnail_size_y / 4;
}

namespace
{
	int GetCellIndex(const int page, const int cell_x, const int cell_y)
	{
		return page * SmallThumbNailsGrid::num_cells_total + cell_y * SmallThumbNailsGrid::num_cells_x + cell_x;
	}

	void GetPageAndCell(const int cell_index, int& out_page, int& out_cell_x, int& out_cell_y)
	{
		out_page = cell_index / SmallThumbNailsGrid::num_cells_total;
		const int i = cell_index % SmallThumbNailsGrid::num_cells_total;
		out_cell_x = i % SmallThumbNailsGrid::num_cells_x;
		out_cell_y = i / SmallThumbNailsGrid::num_cells_x;
	}
}

namespace LargeThumbNail
{
	constexpr int rect_left = 700;
	constexpr int rect_top = SmallThumbNailsGrid::grids_top;
	constexpr int rect_size_x = 160 * 3;
	constexpr int rect_size_y = 90 * 3;
	constexpr int line_color = 0x111133;
	constexpr int line_thickness = 3;
	constexpr int padding = 2;

	constexpr int rect_right = rect_left + rect_size_x;
	constexpr int rect_bottom = rect_top + rect_size_y;

	constexpr int thumbnail_size_x = rect_size_x - (line_thickness - 1) - padding * 2;
	constexpr int thumbnail_size_y = rect_size_y - (line_thickness - 1) - padding * 2;

	constexpr int thumbnail_left = rect_left + (line_thickness - 1) / 2 + padding + 1;
	constexpr int thumbnail_right = thumbnail_left + thumbnail_size_x - 1;
	constexpr int thumbnail_top = rect_top + (line_thickness - 1) / 2 + padding + 1;
	constexpr int thumbnail_bottom = thumbnail_top + thumbnail_size_y - 1;

	const Vector2D rect_lefttop(rect_left, rect_top);
	const Vector2D rect_extent(rect_size_x, rect_size_y);

	const Vector2D thumbnail_lefttop(thumbnail_left, thumbnail_top);
	const Vector2D thumbnail_extent(thumbnail_size_x, thumbnail_size_y);

	constexpr const uint16_t new_stage_panel_font_size = thumbnail_size_y/ 4;
}
// 小サムネイル, 大サムネイル共通
namespace
{
	UIElements::Rectangle new_stage_panel(0x888888);
	constexpr const char* new_stage_panel_text = u8"新規作成";
	constexpr const int new_stage_panel_text_fill_color = 0xFFFFFF;
	constexpr const int new_stage_panel_text_alpha = UIElements::MAX_ALPHA;
	constexpr const int new_stage_panel_text_border_color = 0x0;
}

// その他
namespace
{
	const Vector2D _mode_str_center = Vector2D
	{
		SmallThumbNailsGrid::grids_left + SmallThumbNailsGrid::num_cells_x * SmallThumbNailsGrid::cell_size_x / 2.f,
		75.f
	};
	const int _length_mode_str_to_button = 150;

	const Vector2D current_page_str_center =	Vector2D
	{
		SmallThumbNailsGrid::grids_left + SmallThumbNailsGrid::num_cells_x * SmallThumbNailsGrid::cell_size_x / 2.f,
		625
	};

	constexpr const char* str_id_confirm_deletion_window = "select_scene_confirm_deletion";

	constexpr float TRANSITION_TIME = 0.5f;	// シーン遷移にかかる時間

	constexpr const char* STAGE_LIST_FILE_NAME = "stage_list.txt";
}

StageSelectScene::StageSelectScene()
	: _current_control_mode(ControlMode::KEYBOARD)
	, _bg_handle(-1)
	, _current_page(0)
	, _current_select_mode(StageSelectMode::PLAY)
	, _current_cell_x(NONE_HOVERED)
	, _current_cell_y(NONE_HOVERED)
	, _last_cell_x(NONE_HOVERED)
	, _last_cell_y(NONE_HOVERED)
	, _selected_stage_id(StageId::NONE)
{}

StageSelectScene::~StageSelectScene()
{}

void StageSelectScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	auto select_scene_params = dynamic_cast<const initial_params_of_scene_t<StageSelectScene>*const>(scene_params);

	assert(select_scene_params->next_scene == SceneType::INGAME_SCENE || select_scene_params->next_scene== SceneType::EDITOR_SCENE);

	_selected_stage_id = select_scene_params->initially_selected_stage;

	if (select_scene_params->next_scene == SceneType::EDITOR_SCENE)
	{
		_current_select_mode = StageSelectMode::EDIT;
	}
	else
	{
		_current_select_mode = StageSelectMode::PLAY;
	}

	GraphicResourceManager& graphic_manager = GraphicResourceManager::GetInstance();
	_bg_handle = graphic_manager.GetGraphForDxLib("resources/images/backgrounds/bg_select_edit_scene.png");

	LoadStageListFromFile(_stage_id_list);
	for(const auto& stage_id : _stage_id_list)
	{
		_id_thumbnail_map[stage_id] = GetThumbnailHandle(stage_id);
		LoadStage(stage_id);
	}

	LoadSelectSceneSounds();

	FocusStage(_selected_stage_id);

	ChangeControlMode(ControlMode::KEYBOARD);

	_sounds.bgm->Play();
}

SceneType StageSelectScene::Tick(float delta_seconds)
{
	SceneType result_scene_type = __super::Tick(delta_seconds);

	if (IsInTransition())
	{
		_transition_info->remain_time -= delta_seconds;

		if (_transition_info->remain_time <= 0.f)
		{
			return _transition_info->destination_scene_type;
		}

		return result_scene_type;
	}

	if (!ImGui::IsAnyPopupOpen())
	{
		result_scene_type = HandleInput();

		if ((_current_cell_x != NONE_HOVERED && _current_cell_y != NONE_HOVERED) && (_current_cell_x != _last_cell_x || _current_cell_y != _last_cell_y))
		{
			_sounds.se_move_cursor->Play();
		}
		_last_cell_x = _current_cell_x;
		_last_cell_y = _current_cell_y;
	}

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowPos(ImVec2(WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal(str_id_confirm_deletion_window, NULL, flags))
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(5));

		ImGui::SetCursorPos(ImVec2(30, 30));
		ImGui::Text(u8"本当に削除しますか？");

		ImGui::SetCursorPos(ImVec2(70, 100));
		if (ImGui::Button(u8" はい "))
		{
			RemoveSelectedStage();
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::SetCursorPos(ImVec2(220, 100));
		if (ImGui::Button(u8"いいえ"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::PopFont();
		ImGui::EndPopup();
	}
	
	if(!_transition_info)
	{
		ShowControlInfo();
	}

	return result_scene_type;
}

void StageSelectScene::DrawForeground(const CanvasInfo& canvas_info)
{
	DrawExtendGraph(0, 0, canvas_info.width, canvas_info.height, _bg_handle, TRUE);

	const tstring mode_str = [this]()
	{
		switch (_current_select_mode)
		{
		case StageSelectMode::PLAY:
			return to_tstring(u8"遊ぶ");
		case StageSelectMode::EDIT:
			return to_tstring(u8"作る");
		case StageSelectMode::REMOVE:
			return to_tstring(u8"消す");
		}
		return tstring();
	}();
	const int mode_str_fill_color = [this]()
	{
		switch (_current_select_mode)
		{
		case StageSelectMode::PLAY:
			return 0x4169E1;
		case StageSelectMode::EDIT:
			return 0x228b22;
		case StageSelectMode::REMOVE:
			return 0xFF0000;
		}
		return 0xFFFFFF;
	}();
	
	DrawStringHelper::DrawStringC(64, _mode_str_center.x, _mode_str_center.y, mode_str.c_str(), mode_str_fill_color);

	DrawSmallThumbNails();
	DrawSelectedStageThumbnail();

	// ステージ情報
	if (GetSelectedStage() != nullptr)
	{
		constexpr int FONT_COLOR = 0xFFFFFF;
		constexpr int CENTER_X = LargeThumbNail::rect_left + LargeThumbNail::rect_size_x / 2;
		constexpr int TOP = LargeThumbNail::rect_top + LargeThumbNail::rect_size_y + 32;
		// ステージ名
		const tstring stage_name_str = to_tstring(GetSelectedStage()->GetStageName());
		DrawStringHelper::DrawStringC(32, CENTER_X, TOP + 12, stage_name_str.c_str(), FONT_COLOR);

		// 制限時間
		const tstring time_limit_str = to_tstring(u8"制限時間: ") + to_tstring(GetSelectedStage()->GetTimeLimit()) + to_tstring(u8"秒");
		DrawStringHelper::DrawStringC(24, CENTER_X, TOP + 12 + 32, time_limit_str.c_str(), FONT_COLOR);

		// ステージ説明
		{
			// SetDrawAreaで説明の描画範囲を制限
			RECT last_draw_area;
			GetDrawArea(&last_draw_area);
			SetDrawArea(LargeThumbNail::rect_left, LargeThumbNail::rect_bottom + 100, LargeThumbNail::rect_right, WINDOW_SIZE_Y - 64);

			RECT desc_area;
			GetDrawArea(&desc_area);

			const int last_font_size = DxLib::GetFontSize();
			DxLib::SetFontSize(24);

			const std::string description = GetSelectedStage()->GetDescription();
			std::string description_substr;
			GetUtf8Substring(description, 14 * 5, description_substr); // 14文字 * 5行(最大5行まで表示)

			const tstring display_str = description_substr.empty() ? to_tstring(u8"ステージの説明はありません") : to_tstring(description_substr);
			DrawObtainsString_CharClip(desc_area.left, desc_area.top, 32, display_str.c_str(), FONT_COLOR);

			SetDrawArea(last_draw_area.left, last_draw_area.top, last_draw_area.right, last_draw_area.bottom);

			if (description.size() > description_substr.size())
			{
				DrawString(desc_area.left, 630, to_tstring(u8"…").c_str(), 0xFFFFFF);
			}

			DxLib::SetFontSize(last_font_size);
		}
	}

	// ページ番号表示
	const tstring page_str = to_tstring(_current_page + 1) + _T(" / ") + to_tstring(GetNumPages());
	DrawStringHelper::DrawStringC(32, current_page_str_center.x, current_page_str_center.y, page_str.c_str(), 0xFFFFFF);

	if (_transition_info)
	{
		const float alpha = 255.f * ((TRANSITION_TIME - _transition_info->remain_time) / TRANSITION_TIME);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha));
		DrawBox(0, 0, canvas_info.width, canvas_info.height, 0x000000, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void StageSelectScene::Finalize()
{
	SetMouseDispFlag(true);

	_transition_info.reset();
	_current_select_mode = StageSelectMode::PLAY;
	_current_control_mode = ControlMode::KEYBOARD;
	_current_page = 0;
	_current_cell_x = NONE_HOVERED;
	_current_cell_y = NONE_HOVERED;
	_last_cell_x = NONE_HOVERED;
	_last_cell_y = NONE_HOVERED;
	_id_thumbnail_map.clear();
	_stage_id_list.clear();
	_stage_map.clear();
	_sounds = SelectSceneSounds{};

	__super::Finalize();
}

std::unique_ptr<const SceneBaseInitialParams> StageSelectScene::GetInitialParamsForNextScene(const SceneType next_scene) const
{
	switch (next_scene)
	{
	case SceneType::EDITOR_SCENE:
	{
		auto params = std::make_unique<initial_params_of_scene_t<StageEditorScene>>();
		params->prev_scene_type = GetSceneType();
		params->stage_id = _selected_stage_id;
		return params;
	}
	case SceneType::INGAME_SCENE:
	{
		auto params = std::make_unique<initial_params_of_scene_t<InGameScene>>();
		params->prev_scene_type = GetSceneType();
		params->stage_id = _selected_stage_id;
		return params;
	}
	case SceneType::TITLE_SCENE:
		auto params = std::make_unique<initial_params_of_scene_t<TitleScene>>();
		params->prev_scene_type = GetSceneType();
		return params;
	}

	throw std::runtime_error("Undefined scene transition");
}

void StageSelectScene::BeginTransition(const SceneType destination_scene_type)
{
	if (_transition_info)
	{
		return;
	}

	_transition_info = std::make_unique<TransitionInfo>();
	_transition_info->destination_scene_type = destination_scene_type;
	_transition_info->remain_time = TRANSITION_TIME;
}

void StageSelectScene::FocusStage(const StageId& stage_id_to_hover)
{
	if (!_selected_stage_id.IsValid())
	{
		return;
	}

	const auto it_stage = std::find(_stage_id_list.begin(), _stage_id_list.end(), stage_id_to_hover);
	if (it_stage == _stage_id_list.end())
	{
		return;
	}

	const int i_stage = it_stage - _stage_id_list.begin();
	const int i = i_stage + IsSelectingStageToEdit();
	const int i_cell = i % SmallThumbNailsGrid::num_cells_total;

	_current_page = i / SmallThumbNailsGrid::num_cells_total;
	_current_cell_x = i_cell % SmallThumbNailsGrid::num_cells_x;
	_current_cell_y = i_cell / SmallThumbNailsGrid::num_cells_x;

	_last_cell_x = _current_cell_x;
	_last_cell_y = _current_cell_y;
}

void StageSelectScene::RemoveSelectedStage()
{
	// 選択ステージのインデックスを取得
	auto it_stage_to_remove = std::find(_stage_id_list.begin(), _stage_id_list.end(), _selected_stage_id);
	if (it_stage_to_remove == _stage_id_list.end())
	{
		return;
	}
	const int i_selected_stage = it_stage_to_remove - _stage_id_list.begin();

	// リストからステージを削除
	_id_thumbnail_map.erase(_selected_stage_id);
	_stage_id_list.erase(it_stage_to_remove);
	UpdateStageIDListFile();

	// ステージのJSONファイルとサムネイルファイルの削除
	remove(_selected_stage_id.GetJsonFilePath().c_str());
	remove(_selected_stage_id.GetThumbNailFilePath().c_str());

	// ステージ削除によるページ数減少の処理
	const int num_pages = GetNumPages();
	if (_current_page >= num_pages)
	{
		_current_page = num_pages - 1;
	}

	// 削除したステージの次のステージを選択する
	const int max_i_selected_stage = _stage_id_list.size() - 1;
	const int i_new_selected_stage = std::min(i_selected_stage, max_i_selected_stage);
	if (i_new_selected_stage < 0)
	{
		_selected_stage_id = StageId::NONE;
	}
	else
	{
		_selected_stage_id = _stage_id_list.at(i_new_selected_stage);
	}
}

void StageSelectScene::CreateNewStageThumnail(const StageId& new_stage_id)
{
	if (!new_stage_id.IsValid())
	{
		return;
	}
	
	// TODO: 指定されたステージテンプレート番号に応じてコピーするサムネイルも変更
	const std::string copy_src_file_path = std::string(ResourcePaths::Dir::STAGE_TEMPLATES) + "stage_template_1.png";
	const std::string copy_dest_file_path = new_stage_id.GetThumbNailFilePath();

	FILE* p_src_file = nullptr, * p_dest_file = nullptr;
	errno_t es, ed;
	es = fopen_s(&p_src_file, copy_src_file_path.c_str(), "rb");
	if (es != 0 || !p_src_file)
	{
		if (p_src_file)
		{
			fclose(p_src_file);
		}
		return;
	}

	ed = fopen_s(&p_dest_file, copy_dest_file_path.c_str(), "wb");
	if (ed != 0 || !p_dest_file)
	{
		if (p_dest_file)
		{
			fclose(p_dest_file);
		}
		return;
	}

	constexpr int buf_size = 1024;
	char buf[buf_size];
	size_t read_size;
	while (read_size = fread(buf, sizeof(char), buf_size, p_src_file))
	{
		fwrite(buf, sizeof(char), read_size, p_dest_file);
	}

	fclose(p_src_file);
	fclose(p_dest_file);
}

StageId StageSelectScene::CreateNewStage()
{
	// 新ステージのIDを生成
	uint64_t high_bits, low_bits;
	RandomNumberGenerator::GenerateUUID(high_bits, low_bits);
	const StageId new_stage_id(high_bits, low_bits);

	// 新ステージのJSONファイルのパス
	const std::string file_path = new_stage_id.GetJsonFilePath();

	// 新ステージと同名のファイルが存在しないかをチェック
	std::ifstream json_ifs(file_path);
	if (json_ifs.is_open())
	{
		throw std::runtime_error("File already exists!");
		return StageId::NONE;
	}
	else if (std::find(_stage_id_list.begin(), _stage_id_list.end(), new_stage_id) != _stage_id_list.end())
	{
		throw std::runtime_error("ID is already registered");
		return StageId::NONE;
	}
	json_ifs.close();

	// ステージを生成
	Stage new_stage = Stage::MakeFromTemplate(0);
	new_stage.SetStageId(new_stage_id);

	// 新ステージのJSONファイルを作成
	new_stage.SaveToFile(new_stage_id.GetJsonFilePath());

	CreateNewStageThumnail(new_stage_id);
	
	// リストに新ステージのIDを追加
	_stage_id_list.emplace(_stage_id_list.begin(), new_stage_id);
	_id_thumbnail_map[new_stage_id] = GetThumbnailHandle(new_stage_id);
	UpdateStageIDListFile();

	return new_stage_id;
}

SceneType StageSelectScene::HandleInput()
{
	// タイトルに戻る
	if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		return SceneType::TITLE_SCENE;
	}

	// 選択モード切替
	if (DeviceInput::IsPressed(KEY_INPUT_A))
	{
		const int next_select_mode = (static_cast<int>(_current_select_mode) - 1 + 3) % 3;
		ChangeStageSelectMode(static_cast<StageSelectMode>(next_select_mode));
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_D))
	{
		const int next_select_mode = (static_cast<int>(_current_select_mode) + 1) % 3;
		ChangeStageSelectMode(static_cast<StageSelectMode>(next_select_mode));
	}

	// 入力モードの変更をチェック
	if (_current_control_mode == ControlMode::MOUSE &&
		(
			DeviceInput::IsPressed(KEY_INPUT_LEFT) || DeviceInput::IsPressed(KEY_INPUT_RIGHT) ||
			DeviceInput::IsPressed(KEY_INPUT_UP) || DeviceInput::IsPressed(KEY_INPUT_DOWN)
			)
		)
	{
		ChangeControlMode(ControlMode::KEYBOARD);
	}
	else if (_current_control_mode == ControlMode::KEYBOARD && DeviceInput::IsMouseMoved())
	{
		ChangeControlMode(ControlMode::MOUSE);
	}

	if (_current_control_mode == ControlMode::MOUSE)
	{
		const Vector2D mouse_pos = DeviceInput::GetMousePosition();
		const Vector2D small_thumbnails_lefttop(SmallThumbNailsGrid::grids_left, SmallThumbNailsGrid::grids_top);
		const Vector2D small_thumbnails_rightbottom(SmallThumbNailsGrid::grids_right, SmallThumbNailsGrid::grids_bottom);
		if (GeometricUtility::DoesAARectContainPoint(mouse_pos, small_thumbnails_lefttop, small_thumbnails_rightbottom))
		{
			const Vector2D thumbnails_lefttop_to_mouse_pos = mouse_pos - small_thumbnails_lefttop;
			_current_cell_x = thumbnails_lefttop_to_mouse_pos.x / SmallThumbNailsGrid::cell_size_x;
			_current_cell_y = thumbnails_lefttop_to_mouse_pos.y / SmallThumbNailsGrid::cell_size_y;
		}
		else
		{
			_current_cell_x = _current_cell_y = NONE_HOVERED;
		}
	}
	else if (_current_control_mode == ControlMode::KEYBOARD)
	{
		if (DeviceInput::IsPressed(KEY_INPUT_LEFT))
		{
			_current_cell_x = std::max(_current_cell_x - 1, 0);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_RIGHT))
		{
			_current_cell_x = std::min(_current_cell_x + 1, SmallThumbNailsGrid::num_cells_x - 1);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_UP))
		{
			_current_cell_y = std::max(_current_cell_y - 1, 0);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_DOWN))
		{
			_current_cell_y = std::min(_current_cell_y + 1, SmallThumbNailsGrid::num_cells_y - 1);
		}
	}

	// 選択ステージの更新
	_selected_stage_id = GetHoveredStageId();

	// ステージ選択処理
	if
		(
			(_current_control_mode == ControlMode::KEYBOARD && DeviceInput::IsPressed(KEY_INPUT_RETURN)) ||
			(_current_control_mode == ControlMode::MOUSE && DeviceInput::IsPressed(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE))
			)
	{
		if (_selected_stage_id.IsValid())
		{
			_sounds.se_select->Play();

			switch (_current_select_mode)
			{
			case StageSelectMode::PLAY:
				UpdateStageIDListFile();
				BeginTransition(SceneType::INGAME_SCENE);
				break;
			case StageSelectMode::EDIT:
				UpdateStageIDListFile();
				BeginTransition(SceneType::EDITOR_SCENE);
				break;
			case StageSelectMode::REMOVE:
				ImGui::OpenPopup(str_id_confirm_deletion_window);
				DxLib::SetMouseDispFlag(true);
				break;
			}

		}

		// 「作る」モードで新規作成セルを選択中
		else if (IsSelectingStageToEdit() && _current_cell_x == 0 && _current_cell_y == 0)
		{
			_sounds.se_select->Play();

			_selected_stage_id = CreateNewStage();
			return SceneType::EDITOR_SCENE;
		}
	}

	const int num_pages = GetNumPages();
	if (DeviceInput::IsPressed(KEY_INPUT_PGUP) || DeviceInput::WheelIsUp())
	{
		// 前のページへ
		if (_current_page == 0)
		{
			_current_page = num_pages - 1;
		}
		else
		{
			_current_page--;
		}

		_sounds.se_switch_page->Play();
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_PGDN) || DeviceInput::WheelIsDown())
	{
		// 次のページへ
		if (_current_page == num_pages - 1)
		{
			_current_page = 0;
		}
		else
		{
			_current_page++;
		}

		_sounds.se_switch_page->Play();
	}

	return GetSceneType();
}

void StageSelectScene::ShowControlInfo() const
{
	// ImGuiで操作説明を表示
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoScrollbar;
	flags |= ImGuiWindowFlags_NoScrollWithMouse;
	flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	flags |= ImGuiWindowFlags_NoSavedSettings;
	flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	flags |= ImGuiWindowFlags_NoNav;
	flags |= ImGuiWindowFlags_NoNavFocus;
	flags |= ImGuiWindowFlags_NoNavInputs;
	flags |= ImGuiWindowFlags_NoNavFocus;
	ImGui::SetNextWindowPos(ImVec2{ 0, WINDOW_SIZE_Y - 64 });
	ImGui::SetNextWindowSize(ImVec2{ WINDOW_SIZE_X, 64 });
	ImGui::SetNextWindowBgAlpha(0.75f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x0);
	if (ImGui::Begin("stage_select_control_info", NULL, flags))
	{
		constexpr MasterDataID font_id = 5;
		const int font_size = MdFont::Get(font_id).font_size;
		const ImVec2 icon_size = ImVec2(font_size, font_size);
		constexpr float space = 64.f;

		ImGui::PushFont(FontManager::GetInstance().GetFont(font_id));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		// 選択中のステージで遊ぶ / 選択中のステージを編集する (右クリック/ Enter)
		{
			constexpr MasterDataID icon_id_mouse_click_right = 278;
			constexpr MasterDataID icon_id_key_enter = 242;
			ImGui::Texture texture_mouse_click_right;
			ImGui::Texture texture_key_enter;
			MasterHelper::GetGameIconImguiImage(icon_id_mouse_click_right, texture_mouse_click_right);
			MasterHelper::GetGameIconImguiImage(icon_id_key_enter, texture_key_enter);
			const char* text = [this]() -> const char*
				{
					switch (_current_select_mode)
					{
					case StageSelectMode::PLAY:
						return u8"遊ぶ    ";
					case StageSelectMode::EDIT:
						return u8"編集する";
					case StageSelectMode::REMOVE:
						return u8"削除する";
					}

					throw std::runtime_error("Undefined select mode");
				}();
			const ImGui::Texture& icon_texture =
				_current_control_mode == ControlMode::MOUSE ? texture_mouse_click_right : texture_key_enter;

			ImGui::Image(icon_texture, icon_size);
			ImGui::SameLine();
			ImGui::Text(text);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space);
		}

		// 選択モード切替(A,D)
		{
			constexpr MasterDataID icon_id_key_A = 88;
			constexpr MasterDataID icon_id_key_D = 148;
			ImGui::Texture texture_key_A;
			ImGui::Texture texture_key_D;
			MasterHelper::GetGameIconImguiImage(icon_id_key_A, texture_key_A);
			MasterHelper::GetGameIconImguiImage(icon_id_key_D, texture_key_D);
			ImGui::Image(texture_key_A, icon_size);
			ImGui::SameLine();
			ImGui::Image(texture_key_D, icon_size);
			ImGui::SameLine();
			ImGui::Text(u8"選択モード切替");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space);
		}

		// ステージを選択(マウス / 上下左右キー)
		{
			constexpr MasterDataID icon_id_mouse_move = 280;
			constexpr MasterDataID icon_id_key_left = 104;
			ImGui::Texture texture_mouse_move;
			ImGui::Texture texture_key_arrows;
			MasterHelper::GetGameIconImguiImage(icon_id_mouse_move, texture_mouse_move);
			MasterHelper::GetGameIconImguiImage(icon_id_key_left, texture_key_arrows);
			const ImGui::Texture& icon_texture =
				_current_control_mode == ControlMode::MOUSE ? texture_mouse_move : texture_key_arrows;
			ImGui::Image(icon_texture, icon_size);
			ImGui::SameLine();
			ImGui::Text(u8"ステージを選択");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space);
		}

		// ページ切替 (ホイール / PgUp, PgDn)
		{
			constexpr MasterDataID icon_id_mouse_wheel_updn = 289;
			constexpr MasterDataID icon_id_key_pgup = 226;
			constexpr MasterDataID icon_id_key_pgdn = 224;
			ImGui::Texture texture_mouse_wheel_updn;
			ImGui::Texture texture_key_pgup;
			ImGui::Texture texture_key_pgdn;
			MasterHelper::GetGameIconImguiImage(icon_id_mouse_wheel_updn, texture_mouse_wheel_updn);
			MasterHelper::GetGameIconImguiImage(icon_id_key_pgup, texture_key_pgup);
			MasterHelper::GetGameIconImguiImage(icon_id_key_pgdn, texture_key_pgdn);
			if (_current_control_mode == ControlMode::MOUSE)
			{
				ImGui::Image(texture_mouse_wheel_updn, icon_size);
			}
			else
			{
				ImGui::Image(texture_key_pgup, icon_size);
				ImGui::SameLine();
				ImGui::Image(texture_key_pgdn, icon_size);
			}
			ImGui::SameLine();
			ImGui::Text(u8"ページ切替");
		}

		ImGui::PopStyleVar();
		ImGui::PopFont();
	}
	ImGui::End();
	ImGui::PopStyleColor();
}

void StageSelectScene::ChangeStageSelectMode(const StageSelectMode new_mode)
{
	if (_current_select_mode == new_mode)
	{
		return;
	}

	// NOTE:「編集」では先頭セルが「新規作成」になるので,「遊ぶ」「消す」との遷移時に選択ステージがずれないようにする
	// なお, マウス操作時にこれをやるとマウスカーソルがテレポートするので, キーボード操作時のみ行う
	if (_current_control_mode == ControlMode::KEYBOARD)
	{
		const int current_cell_index = GetCellIndex(_current_page, _current_cell_x, _current_cell_y);
		if (_current_select_mode == StageSelectMode::EDIT)
		{
			GetPageAndCell(std::max(0, current_cell_index - 1), _current_page, _current_cell_x, _current_cell_y);
		}
		else if (new_mode == StageSelectMode::EDIT)
		{
			GetPageAndCell(std::min(GetMaxCellIndex(), current_cell_index + 1), _current_page, _current_cell_x, _current_cell_y);
		}
	}

	_current_select_mode = new_mode;
	_sounds.se_switch_select_mode->Play();
}

int StageSelectScene::GetThumbnailHandle(const StageId& stage_id) const
{
	GraphicResourceManager& graphic_manager = GraphicResourceManager::GetInstance();
	return graphic_manager.GetGraphForDxLib(stage_id.GetThumbNailFilePath());
}

void StageSelectScene::LoadStageListFromFile(std::vector<StageId>& out_stage_list) const
{
	const std::string stage_list_file_path = std::string(ResourcePaths::Dir::STAGES) + STAGE_LIST_FILE_NAME;

	std::ifstream stage_list_file(stage_list_file_path);
	
	if (stage_list_file.is_open())
	{
		// リストファイルからステージIDを読み込む

		// 重複チェック用
		std::unordered_set<StageId> loaded_ids;

		std::string buf_line;
		while (std::getline(stage_list_file, buf_line))
		{
			const StageId id(buf_line);
			if (id.IsValid() && loaded_ids.find(id) == loaded_ids.end())
			{
				out_stage_list.push_back(id);
			}
		}
	}
	else
	{
		// リストファイルを新規作成

		if ((GetFileAttributesA(ResourcePaths::Dir::STAGES) & INVALID_FILE_ATTRIBUTES) != 0)
		{
			CreateDirectoryA(ResourcePaths::Dir::STAGES, NULL);
		}

		std::ofstream new_stage_list_file(stage_list_file_path);
		if (!new_stage_list_file.is_open())
		{
			throw std::runtime_error("Failed to create " + std::string(stage_list_file_path));
		}

		new_stage_list_file.close();

		return;
	}
}

void StageSelectScene::LoadStage(const StageId& stage_id)
{
	// ステージのJSONファイルを開く
	std::ifstream json_file(stage_id.GetJsonFilePath());
	if (!json_file.is_open())
	{
		throw std::runtime_error("Failed to open " + stage_id.GetJsonFilePath());
	}

	// JSONファイルを読み込む
	nlohmann::json stage_json = nlohmann::json::parse(json_file);
	json_file.close();
	Stage stage;
	stage.FromJsonObject(stage_json);

	// ステージをマップに登録
	_stage_map[stage_id] = stage;
}

void StageSelectScene::UpdateStageIDListFile() const
{
	std::ofstream stage_list_file(std::string(ResourcePaths::Dir::STAGES) + STAGE_LIST_FILE_NAME);
	if (!stage_list_file.is_open())
	{
		return;
	}

	for (const auto& id : _stage_id_list)
	{
		stage_list_file << id.ToUUIDFormatString() << std::endl;
	}

	stage_list_file.close();
}

void StageSelectScene::DrawSmallThumbNails() const
{
	using namespace SmallThumbNailsGrid;

	// グリッドの描画
	for (int x = 0; x <= num_cells_x; x++)
	{
		DrawLine(grids_left + x * cell_size_x, grids_top, grids_left + x * cell_size_x, grids_bottom, line_color, line_thickness);
	}
	for (int y = 0; y <= num_cells_y; y++)
	{
		DrawLine(grids_left, grids_top + y * cell_size_y, grids_right, grids_top + y * cell_size_y, line_color, line_thickness);
	}

	const int i_first_stage_in_page = std::max(_current_page * num_cells_total - (IsSelectingStageToEdit()), 0);
	int i_stage_id = i_first_stage_in_page;
	for (int i_cell = 0; i_cell < num_cells_total; i_cell++)
	{
		if (i_cell == 0 && IsSelectingStageToEdit() && _current_page == 0)
		{
			new_stage_panel.Draw(
				new_stage_panel_lefttop,
				new_stage_extent,
				UIElements::RectLabel(
					new_stage_panel_text,
					new_stage_panel_text_fill_color,
					new_stage_panel_text_border_color,
					new_stage_panel_text_alpha,
					new_stage_panel_font_size
				)
			);
			continue;
		}

		if (i_stage_id == _stage_id_list.size())
		{
			break;
		}

		const int handle = _id_thumbnail_map.at(_stage_id_list.at(i_stage_id));
		const int thumbnail_left = thumbnails_left + (i_cell % num_cells_x) * cell_size_x;
		const int thumbnail_right = thumbnail_left + thumbnail_size_x - 1;
		const int thumbnail_top = thumbnails_top + (i_cell / num_cells_x) * cell_size_y;
		const int thumbnail_bottom = thumbnail_top + thumbnail_size_y - 1;
		DrawExtendGraph(thumbnail_left, thumbnail_top, thumbnail_right, thumbnail_bottom, handle, true);

		i_stage_id++;
	}

	// マウスオーバーしているサムネイルに太枠を表示
	if(!NoStageIsHovered())
	{
		const Vector2D hovered_cell_lefttop(grids_left + cell_size_x * _current_cell_x, grids_top + cell_size_y * _current_cell_y);
		UIElements::Rectangle bold_frame(0, 0, 6, 0xFF0000, DrawBlendInfo::MAX_BLEND_VALUE);
		bold_frame.Draw(hovered_cell_lefttop, Vector2D(cell_size_x, cell_size_y));
	}
}

void StageSelectScene::DrawSelectedStageThumbnail() const
{
	using namespace LargeThumbNail;
	
	DrawLine(rect_left, rect_top, rect_left, rect_bottom, line_color, line_thickness);
	DrawLine(rect_left, rect_bottom, rect_right, rect_bottom, line_color, line_thickness);
	DrawLine(rect_right, rect_bottom, rect_right, rect_top, line_color, line_thickness);
	DrawLine(rect_right, rect_top, rect_left, rect_top, line_color, line_thickness);

	if (_selected_stage_id.IsValid())
	{
		const int thumbnail_handle = _id_thumbnail_map.at(_selected_stage_id);
		DrawExtendGraph(thumbnail_left, thumbnail_top, thumbnail_right, thumbnail_bottom, thumbnail_handle, true);
	}
	else
	{
		if (HoveringNewStageCell())
		{
			new_stage_panel.Draw(
				thumbnail_lefttop,
				thumbnail_extent,
				UIElements::RectLabel(
					new_stage_panel_text,
					new_stage_panel_text_fill_color,
					new_stage_panel_text_border_color,
					new_stage_panel_text_alpha,
					new_stage_panel_font_size
				)
			);
		}
		else
		{
			DrawStringHelper::DrawStringC(32, rect_left + rect_size_x / 2, rect_top + rect_size_y / 2, _T("Select Stage"), 0xFFFFFF);
		}
	}
}

StageId StageSelectScene::GetHoveredStageId() const
{
	using namespace SmallThumbNailsGrid;

	if (NoStageIsHovered())
	{
		return StageId::NONE;
	}
	
	// セル番号
	const int i_cell = _current_cell_y * num_cells_x + _current_cell_x;

	// ステージIDの通し番号
	int i_stage_id = _current_page * num_cells_total + i_cell - (IsSelectingStageToEdit());

	// ステージID通し番号が範囲外
	if (i_stage_id < 0 || i_stage_id >= _stage_id_list.size())
	{
		return StageId::NONE;
	}

	return _stage_id_list.at(i_stage_id);
	return _stage_id_list.at(i_stage_id);
}

bool StageSelectScene::HoveringNewStageCell() const
{
	return (
		_current_cell_x == 0 &&
		_current_cell_y == 0 &&
		IsSelectingStageToEdit() &&
		_current_page == 0
	);
}

void StageSelectScene::ChangeControlMode(const ControlMode new_control_mode)
{
	switch (new_control_mode)
	{
	case ControlMode::MOUSE:
	{
		SetMousePoint(
			SmallThumbNailsGrid::grids_left + (_current_cell_x + 0.5) * SmallThumbNailsGrid::cell_size_x,
			SmallThumbNailsGrid::grids_top + (_current_cell_y + 0.5) * SmallThumbNailsGrid::cell_size_y
		);
		SetMouseDispFlag(true);
		break;
	}
	
	case ControlMode::KEYBOARD:
	{
		if (NoStageIsHovered())
		{
			_current_cell_x = _current_cell_y = 0;
		}
		SetMouseDispFlag(false);
		break;
	}
	}

	_current_control_mode = new_control_mode;
}

int StageSelectScene::GetNumPages() const
{
	// 編集ステージを選択中の場合, 先頭ページの先頭セルには「新規作成」が表示されるが、
	// ページ数の計算においては、ステージ数が1増えたとすれば良い.
	const int N = _stage_id_list.size() + (IsSelectingStageToEdit());

	// ceil(N / num_cells_total)
	return (N - 1) / SmallThumbNailsGrid::num_cells_total + 1;
}

int StageSelectScene::GetMaxCellIndex() const
{
	const int num_pages = GetNumPages();
	return num_pages * SmallThumbNailsGrid::num_cells_total - 1;
}

void StageSelectScene::LoadSelectSceneSounds()
{
	constexpr int BGM_VOLUME = 50;
	constexpr int SE_VOLUME = 75;

	_sounds = SelectSceneSounds();
	_sounds.bgm = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/bgm/bgm_select_scene.ogg");
	_sounds.bgm->SetVolume(BGM_VOLUME);
	_sounds.bgm->SetLoopEnabled(true);

	const std::string select_scene_se_dir = "resources/sounds/se/select_scene/";
	_sounds.se_move_cursor = SoundManager::GetInstance().MakeSoundInstance(select_scene_se_dir + "se_move_cursor.mp3");
	_sounds.se_move_cursor->SetVolume(SE_VOLUME);

	_sounds.se_select = SoundManager::GetInstance().MakeSoundInstance(select_scene_se_dir + "se_select.ogg");
	_sounds.se_select->SetVolume(SE_VOLUME);

	_sounds.se_switch_page = SoundManager::GetInstance().MakeSoundInstance(select_scene_se_dir + "se_switch_page.wav");
	_sounds.se_switch_page->SetVolume(SE_VOLUME);

	_sounds.se_switch_select_mode = SoundManager::GetInstance().MakeSoundInstance(select_scene_se_dir + "se_switch_select_mode.ogg");
	_sounds.se_switch_select_mode->SetVolume(SE_VOLUME);
}

bool StageSelectScene::NoStageIsHovered() const
{
	return _current_cell_x == NONE_HOVERED || _current_cell_y == NONE_HOVERED;
}

bool StageSelectScene::IsSelectingStageToEdit() const
{
	return _current_select_mode == StageSelectMode::EDIT;
}

Stage* StageSelectScene::GetSelectedStage()
{
	if (!_selected_stage_id.IsValid())
	{
		return nullptr;
	}

	if (_stage_map.find(_selected_stage_id) == _stage_map.end())
	{
		LoadStage(_selected_stage_id);
	}

	return &_stage_map.at(_selected_stage_id);
}
