#include "StageEditorSceneState_Edit.h"
#include "Input/DeviceInput.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorSceneStatesInclude.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"
#include "GameSystems/GameConfig/GameConfig.h"

namespace
{
	constexpr const char* id_modal_stage_saved_alert = "modal_stage_saved_alert";
}

StageEditorSceneState_Edit::StageEditorSceneState_Edit()
	: _is_sidebar_hidden(false)
	, _is_grid_shown(true)
{
}

StageEditorSceneState_Edit::~StageEditorSceneState_Edit()
{
}

Vector2D StageEditorSceneState_Edit::GetTileCenterPosition(const Vector2D& q_world, const int offset_tiles_x, const int offset_tiles_y)
{
	return StageInteractiveScene::GetTileCenterPosition(q_world, offset_tiles_x, offset_tiles_y);
}

Vector2D StageEditorSceneState_Edit::GetTileCenterPosition(const int tile_index_x, const int tile_index_y)
{
	return StageInteractiveScene::GetTileCenterPosition(tile_index_x, tile_index_y);
}

void StageEditorSceneState_Edit::GetTileIndex(const Vector2D& q_world, int& tile_index_x, int& tile_index_y)
{
	StageInteractiveScene::GetTileIndex(q_world, tile_index_x, tile_index_y);
}

void StageEditorSceneState_Edit::Undo(StageEditorScene& parent_editor_scene)
{
	const int last_state = parent_editor_scene._command_history->GetCurrentState();
	parent_editor_scene._command_history->Undo();
	if (last_state != parent_editor_scene._command_history->GetCurrentState())
	{
		OnUndo(parent_editor_scene);
	}
}

void StageEditorSceneState_Edit::Redo(StageEditorScene& parent_editor_scene)
{
	const int last_state = parent_editor_scene._command_history->GetCurrentState();
	parent_editor_scene._command_history->Redo();
	if (last_state != parent_editor_scene._command_history->GetCurrentState())
	{
		OnRedo(parent_editor_scene);
	}
}

void StageEditorSceneState_Edit::SaveStage(StageEditorScene& parent_editor_scene)
{
	parent_editor_scene.SaveStage();
	parent_editor_scene.PushEditorMessage(u8"セーブ完了");
}

void StageEditorSceneState_Edit::ShowSidebars(StageEditorScene& parent_editor_scene)
{
	//Sidebar共通スタイル
	constexpr float WINDOW_ALPHA = 1.f;
	constexpr int POP_STYLE_COLOR_COUNT = 3;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, StageEditorColor::BackColor(0));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, StageEditorColor::BackColor(0));
	ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));

	ImGui::PushStyleColors_Button(StageEditorColor::BackColor(1));
	ImGui::PushSidebarDefaultStyle();

	// Leftbar
	const bool leftbar_fully_open = ImGui::BeginSideBar
		(
			"editor_scene_leftbar",
			parent_editor_scene.sidebar_info->left.desc,
			parent_editor_scene.sidebar_info->left.is_open,
			parent_editor_scene.sidebar_info->left.current_width
		);
	{
		ShowLeftbarContent(parent_editor_scene);
	}
	ImGui::EndSideBar();

	// Rightbar
	ImGui::BeginSideBar
		(
			"editor_scene_rightbar",
			parent_editor_scene.sidebar_info->right.desc,
			parent_editor_scene.sidebar_info->right.is_open,
			parent_editor_scene.sidebar_info->right.current_width
		);
	{
		ShowRightbarContent(parent_editor_scene);
	}
	ImGui::EndSideBar();

	ImGui::PopSidebarDefaultStyle();
	ImGui::PopStyleColor(POP_STYLE_COLOR_COUNT);
	ImGui::PopStyleColors_Button();
}

void StageEditorSceneState_Edit::ShowPopups(StageEditorScene& parent_editor_scene)
{
	if (ImGui::BeginPopupModal(id_modal_stage_saved_alert))
	{
		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::Text("Stage Saved");
		ImGui::EndPopup();
	}
}

void StageEditorSceneState_Edit::ShowLeftbarContent(StageEditorScene& parent_editor_scene)
{
	const std::unique_ptr<StageEditorScene::SidebarInfo>& sidebar_info = parent_editor_scene.sidebar_info;

	static ImVec2 group_size = ImGui::GetWindowSize();
	const ImVec2 button_size = ImVec2(sidebar_info->common_button_size, sidebar_info->common_button_size);

	ImGui::BeginAlignedChild("leftsidebar", { 0.5, 0.5 }, { 0.5, 0.5 }, group_size);
	{
		if (sidebar_info->is_first_loop)
		{
			ImGui::BeginDisabled();
		}

		// 左サイドバーに表示するボタンのグループ
		ImGui::BeginGroup();
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 16);

		// 各ボタンの表示と押下時処理
		if (ImGui::ImageButton("editor_settings", sidebar_info->img_editor_settings, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_EditorSettings>());
		}
		ImGui::SetItemTooltip(u8"エディタ設定");

		if (ImGui::ImageButton("stage_settings", sidebar_info->img_stage_settings, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_StageSettings>());
		}
		ImGui::SetItemTooltip(u8"ステージ設定");

		if (ImGui::ImageButton("change_stage_length", sidebar_info->img_enter_change_stage_length, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_ResizeStage>());
		}
		ImGui::SetItemTooltip(u8"ステージの長さを変更");

		if(ImGui::ImageButton("change_stage_background", sidebar_info->img_open_modal_background, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_EditStageBg>());
		}
		ImGui::SetItemTooltip(u8"ステージの背景を変更");

		if (ImGui::ImageButton("change_stage_bgm", sidebar_info->img_open_modal_bgm, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_EditStageBGM>());
		}
		ImGui::SetItemTooltip(u8"ステージのBGMを変更");

		if (ImGui::ImageButton("shoot_thumbnail", sidebar_info->img_enter_thumbnail_shooting, button_size))
		{
			SetNextState(std::make_shared<StageEditorSceneState_ThumbnailShooting>());
		}
		ImGui::SetItemTooltip(u8"サムネイル撮影");

		if (ImGui::ImageButton("undo", sidebar_info->img_undo, button_size))
		{
			Undo(parent_editor_scene);
		}
		ImGui::SetItemTooltip(u8" Undo ");

		if (ImGui::ImageButton("redo", sidebar_info->img_redo, button_size))
		{
			Redo(parent_editor_scene);
		}
		ImGui::SetItemTooltip(u8" Redo ");

		ImGui::PopStyleVar();
		ImGui::EndGroup();

		if (sidebar_info->is_first_loop)
		{
			ImGui::EndDisabled();
			group_size = ImGui::GetItemRectSize();
			sidebar_info->left.desc.width = group_size.x + 32;
			sidebar_info->left.desc.height = group_size.y + 32;
			sidebar_info->left.current_width = group_size.x + 32;
			sidebar_info->left.is_open = true;
		}
	}
	ImGui::EndAlignedChild();
}

void StageEditorSceneState_Edit::ShowRightbarContent(StageEditorScene& parent_editor_scene)
{
	ShowRightbarHead(parent_editor_scene);
	ShowRightbarBody(parent_editor_scene);
}

void StageEditorSceneState_Edit::ShowRightbarHead(StageEditorScene& parent_editor_scene)
{
	const ImVec2 button_size = ImVec2(parent_editor_scene.sidebar_info->common_button_size, parent_editor_scene.sidebar_info->common_button_size);

	// 編集モード切替ボタン
	static ImVec2 group_size = ImVec2(1, 1);
	ImGui::BeginAlignedChild("radiobuttons_editmode", { 0.5, 0 }, { 0.5, 0 }, group_size, { 0, 32 });
	{
		// 最初のループはgroup_sizeの計算をするため非表示にする
		if (parent_editor_scene.sidebar_info->is_first_loop) { ImGui::BeginDisabled(); }

		// EditMode選択ボタンを横に並べたグループ
		ImGui::BeginGroup();
		{
			const std::array<ImGui::Texture, 4> images = 
			{
				parent_editor_scene.sidebar_info->img_editmode_add,
				parent_editor_scene.sidebar_info->img_editmode_modify,
				parent_editor_scene.sidebar_info->img_editmode_move,
				parent_editor_scene.sidebar_info->img_editmode_remove
			};

			int selected_edit_mode = static_cast<int>(GetEditMode());
			for (size_t i = 0; i < EDIT_MODE_LIST.size(); i ++)
			{
				const ImGui::Texture& img = images.at(i);
				ImGui::PushID(i);
				ImGui::ImageRadioButton("##radio_editmode", selected_edit_mode, static_cast<int>(EDIT_MODE_LIST.at(i)), img.im_tex_id, button_size, img.uv0, img.uv1, StageEditorColor::BackColor(1));
				ImGui::PopID();
				if (i < EDIT_MODE_LIST.size() - 1)
				{
					ImGui::SameLine();
				}
			}
			if (selected_edit_mode != static_cast<int>(GetEditMode()))
			{
				SetNextState(MakeSubState(static_cast<EditMode>(selected_edit_mode)));
			}

		}
		ImGui::EndGroup();

		if (parent_editor_scene.sidebar_info->is_first_loop)
		{
			ImGui::EndDisabled();
			group_size = ImGui::GetItemRectSize();
		}
	}
	ImGui::EndAlignedChild();
}

std::shared_ptr<SceneState<StageEditorScene>> StageEditorSceneState_Edit::MakeSubState(const EditMode edit_mode)
{
	switch (edit_mode)
	{
	case EditMode::SUMMON:
		return std::make_shared<StageEditorSceneState_Edit_Summon>();
	case EditMode::MODIFY:
		return std::make_shared<StageEditorSceneState_Edit_Modify>();
	case EditMode::MOVE:
		return std::make_shared<StageEditorSceneState_Edit_Move>();
	}

	throw std::runtime_error("Invalid EditMode");
	return nullptr;
}

StageEditorSceneState_Edit::EditMode StageEditorSceneState_Edit::GetNextEditMode(const EditMode current_edit_mode)
{
	for (int i = 0; i < EDIT_MODE_LIST.size(); i++)
	{
		if (current_edit_mode == EDIT_MODE_LIST.at(i))
		{
			return EDIT_MODE_LIST.at((i + 1) % EDIT_MODE_LIST.size());
		}
	}

	throw std::runtime_error("Invalid EditMode");
}

StageEditorSceneState_Edit::EditMode StageEditorSceneState_Edit::GetPrevEditMode(const EditMode current_edit_mode)
{
	for (int i = 0; i < EDIT_MODE_LIST.size(); i++)
	{
		if (current_edit_mode == EDIT_MODE_LIST.at(i))
		{
			return EDIT_MODE_LIST.at((i + EDIT_MODE_LIST.size() - 1) % EDIT_MODE_LIST.size());
		}
	}

	throw std::runtime_error("Invalid EditMode");
}


std::shared_ptr<SceneState<StageEditorSceneState_Edit::ParentSceneClass>> StageEditorSceneState_Edit::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);
	if(!_is_sidebar_hidden)
	{
		ShowSidebars(parent_scene);
	}
	ShowPopups(parent_scene);

	return ret;
}

void StageEditorSceneState_Edit::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);

	if (_is_grid_shown)
	{
		const FColor4 grid_color = GameConfig::GetInstance().GetConfigItem<StageEditorConfig>()->grid_color;

		parent_scene.DrawWorldGrid(grid_color.Get00RRGGBB(), grid_color.a * 255);
	}

	// 右サイドバーが非表示の場合は現在の編集モードアイコンを表示
	if (parent_scene.sidebar_info->right.current_width == 0)
	{
		ImGui::Texture icon_texture;
		switch (GetEditMode())
		{
		case EditMode::SUMMON:
			icon_texture = parent_scene.sidebar_info->img_editmode_add;
			break;
		case EditMode::MODIFY:
			icon_texture = parent_scene.sidebar_info->img_editmode_modify;
			break;
		case EditMode::MOVE:
			icon_texture = parent_scene.sidebar_info->img_editmode_move;
			break;
		}
		
		constexpr ImVec2 size = { 32,32 };
		constexpr ImVec2 pos = { WINDOW_SIZE_X - 32, 32 };
		
		ImGui::GetForegroundDrawList()->AddRectFilled(
			pos,
			pos + size,
			ImGui::GetColorU32(StageEditorColor::BackColor(2))
		);

		ImGui::GetForegroundDrawList()->AddImage(
			icon_texture.im_tex_id,
			pos,
			pos+size,
			icon_texture.uv0,
			icon_texture.uv1
		);
	}
}

void StageEditorSceneState_Edit::UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (ImGui::IsAnyPopupOpen())
	{
		return;
	}

	CameraParams& camera_params = parent_scene._camera_params;

	// キーボード操作
	{
		// WASDでカメラ移動
		if (!DeviceInput::IsActive(KEY_INPUT_LCONTROL) && !ImGui::IsAnyItemActive())
		{
			const float camera_speed = 1000.f * camera_params.screen_scale;
			const Vector2D input_dir = DeviceInput::GetInputDir_WASD();
			const float multiplier = DeviceInput::IsActive(KEY_INPUT_LSHIFT) ? 2.f : 1.f;
			camera_params.world_offset += input_dir * camera_speed * delta_seconds * multiplier;
		}
	}

	//マウス操作
	const Vector2D mouse_pos = DeviceInput::GetMousePosition();
	if (DeviceInput::IsActive(MOUSE_INPUT_RIGHT, DeviceInput::Device::MOUSE))
	{
		const float HALF_SCREEN_WIDTH = camera_params.GetWorldViewHalfExtent().x;
		const float  HALF_SCREEN_HEIGHT = camera_params.GetWorldViewHalfExtent().y;
		constexpr int VIEWPORT_WIDTH = WINDOW_SIZE_X;
		constexpr int VIEWPORT_HEIGHT = WINDOW_SIZE_Y;
		const Vector2D mouse_move = DeviceInput::GetMouseDeltaPosition();
			
		camera_params.world_offset -= Vector2D
		(
			2 * mouse_move.x * HALF_SCREEN_WIDTH / (float)VIEWPORT_WIDTH,
			2 * mouse_move.y * HALF_SCREEN_HEIGHT / (float)VIEWPORT_HEIGHT
		);
	}

	if (parent_scene.IsMouseOnStage(mouse_pos))
	{
		// ホイール上下で画角変更
		if (!DeviceInput::WheelIsStopped())
		{
			const float stage_height =
				parent_scene.GetStageRef().GetStageRightBottom().y - parent_scene.GetStageRef().GetStageLeftTop().y;
			
			// 拡大率クランプ
			constexpr float MIN_SCALE = 0.25f;
			constexpr float DELTA_SCALE = 0.25f;
			const float max_scale = parent_scene.GetMaxScreenScale();
			int n = -2;
			while (powf(2.f, n) < camera_params.screen_scale)
			{
				n++;
			}

			const float new_scale = DeviceInput::WheelIsUp() ? (powf(2, n - 1)) : (powf(2, n + 1));
			
			if (camera_params.screen_scale != new_scale && clamp(new_scale, MIN_SCALE, max_scale) == new_scale)
			{
				camera_params.ChangeScale(new_scale, DeviceInput::GetMousePosition());
			}
		}

		// 右押下+中クリックで画角リセット
		else if 
		(
			DeviceInput::IsActive(MOUSE_INPUT_RIGHT, DeviceInput::Device::MOUSE) &&
			DeviceInput::IsPressed(MOUSE_INPUT_MIDDLE, DeviceInput::Device::MOUSE)			
		)
		{
			camera_params.ChangeScale(1.f, GetWindowSize() * 0.5f);
		}
	}
}

void StageEditorSceneState_Edit::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	__super::HandleInput(parent_scene, delta_seconds);

	// Ctrl+Z: Undo
	if (DeviceInput::IsActive(KEY_INPUT_LCONTROL) && DeviceInput::IsPressed(KEY_INPUT_Z))
	{
		Undo(parent_scene);
	}

	// Ctrl+Y: Redo
	else if (DeviceInput::IsActive(KEY_INPUT_LCONTROL) && DeviceInput::IsPressed(KEY_INPUT_Y))
	{
		Redo(parent_scene);
	}

	// 左Ctrl+S: セーブ
	if (DeviceInput::IsActive(KEY_INPUT_LCONTROL) && DeviceInput::IsPressed(KEY_INPUT_S))
	{
		SaveStage(parent_scene);
	}

	// Esc: ポーズ
	else if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		SetNextState(std::make_shared<StageEditorSceneState_Paused>());
	}

	// F2: グリッド表示・非表示
	else if (DeviceInput::IsPressed(KEY_INPUT_F2))
	{
		_is_grid_shown = !_is_grid_shown;
	}

	// Q: 左サイドバー開閉
	else if (DeviceInput::IsPressed(KEY_INPUT_Q))
	{
		parent_scene.ToggleSidebarOpenClosed(parent_scene.sidebar_info->left);
	}

	// E: 右サイドバー開閉
	else if (DeviceInput::IsPressed(KEY_INPUT_E))
	{
		parent_scene.ToggleSidebarOpenClosed(parent_scene.sidebar_info->right);
	}

	// SPACE: 次の編集モードへ
	else if (DeviceInput::IsPressed(KEY_INPUT_SPACE) && !DeviceInput::IsActive(KEY_INPUT_LSHIFT))
	{
		SetNextState(MakeSubState(GetNextEditMode(GetEditMode())));
	}

	// LSHIFT+SPACE: 前の編集モードへ
	else if (DeviceInput::IsPressed(KEY_INPUT_SPACE) && DeviceInput::IsActive(KEY_INPUT_LSHIFT))
	{
		SetNextState(MakeSubState(GetPrevEditMode(GetEditMode())));
	}

	// 右クリック中はサイドバー非表示
	_is_sidebar_hidden = DeviceInput::IsActive(MOUSE_INPUT_RIGHT, DeviceInput::Device::MOUSE);
}

void StageEditorSceneState_Edit::ShowControls(StageEditorScene& parent_editor_scene)
{
	__super::ShowControls(parent_editor_scene);

	ImGui::Text(u8"F2: グリッド表示・非表示");
	ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"Q/E: 左/右サイドバー開閉");

	ImGui::Text(u8"WASD: カメラ移動");
	ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"Shift: カメラ移動を加速");

	ImGui::Text(u8"Esc: ポーズ");
	ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"Ctrl+S: 保存");

	ImGui::Text(u8"Ctrl+Z: Undo");
	ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"Ctrl+Y: Redo");

	ImGui::Text(u8"Shift+Space: 前の編集モードへ");
	ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"Space: 次の編集モードへ");
	
	ImGui::Text(u8"ホイール: ズームイン・ズームアウト");
	ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f);
	ImGui::Text(u8"右+中クリック: ズームリセット");
}

void StageEditorSceneState_Edit::OnUndo(StageEditorScene& parent_editor_scene)
{
	parent_editor_scene._editor_scene_sounds->se_undo->Play();
}

void StageEditorSceneState_Edit::OnRedo(StageEditorScene& parent_editor_scene)
{
	parent_editor_scene._editor_scene_sounds->se_redo->Play();
}
