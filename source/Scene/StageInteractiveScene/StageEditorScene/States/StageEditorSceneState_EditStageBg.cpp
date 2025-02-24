#include "StageEditorSceneState_EditStageBg.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"

namespace
{
	constexpr const char* str_id_modal_stage_bg = "modal_stage_bg";

	constexpr ImVec2 modal_size = 
		ImVec2(WINDOW_SIZE_X * 0.8, WINDOW_SIZE_Y * 0.8);

	constexpr ImVec2 modal_pos = 
		ImVec2(WINDOW_SIZE_X / 2 - modal_size.x / 2, WINDOW_SIZE_Y / 2 - modal_size.y / 2);

	constexpr float PREVIEW_U0 = 0.3f;
	constexpr float PREVIEW_V0 = 0.2f;
	constexpr float PREVIEW_U1 = PREVIEW_U0 + 0.575f;
	constexpr float PREVIEW_V1 = PREVIEW_V0 + 0.575f;

	constexpr float OK_BUTTON_CENTER_U = PREVIEW_U0 + (PREVIEW_U1 - PREVIEW_U0) * 0.25f;
	constexpr float CANCEL_BUTTON_CENTER_U = PREVIEW_U0 + (PREVIEW_U1 - PREVIEW_U0) * 0.75f;
	constexpr float BUTTON_CENTER_V = 0.925f;
}

StageEditorSceneState_EditStageBg::StageEditorSceneState_EditStageBg()
	: _selected_bg_layer_index(0)
{
}

StageEditorSceneState_EditStageBg::~StageEditorSceneState_EditStageBg()
{
}

void StageEditorSceneState_EditStageBg::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	parent_scene._should_show_controls = false;

	StageBGLayer::LoadStageBGLayers(_loaded_bg_layers);

	for (size_t i = 0; i < _loaded_bg_layers.size(); i++)
	{
		if (_loaded_bg_layers.at(i).bg_layer_id == parent_scene.GetStageRef().GetBgLayerId())
		{
			_selected_bg_layer_index = i;
			break;
		}
	}

	ImGui::OpenPopup(str_id_modal_stage_bg);
}

void StageEditorSceneState_EditStageBg::OnLeaveState(ParentSceneClass& parent_scene)
{
	if (ImGui::IsPopupOpen(str_id_modal_stage_bg))
	{
		ImGui::BeginPopupModal(str_id_modal_stage_bg);
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorSceneState_EditStageBg::ParentSceneClass>> StageEditorSceneState_EditStageBg::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	ShowStageBgModal(parent_scene);

	return ret;
}

void StageEditorSceneState_EditStageBg::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	const int x0 = canvas_info.width * PREVIEW_U0;
	const int y0 = canvas_info.height * PREVIEW_V0;
	const int x1 = canvas_info.width * PREVIEW_U1;
	const int y1 = canvas_info.height * PREVIEW_V1;
	const int height = y1 - y0;

	RECT last_draw_area;
	DxLib::GetDrawArea(&last_draw_area);
	DxLib::SetDrawArea(x0, y0, x1, y1);
	
	std::vector<StageBGInfo> bg_infos = _loaded_bg_layers.at(_selected_bg_layer_index).bg_infos;
	std::sort(bg_infos.begin(), bg_infos.end(), [](const StageBGInfo& a, const StageBGInfo& b) 
		{
			return a.distance > b.distance;
		});

	for (auto& bg_info : bg_infos)
	{
		int left = 0;
		const int handle = GraphicResourceManager::GetInstance().GetGraphForDxLib(bg_info.image_id);
		while (left < x1)
		{
			int graph_size_x = 1, graph_size_y = 1;
			DxLib::GetGraphSize(handle, &graph_size_x, &graph_size_y);
			const int width = (height * graph_size_x) / graph_size_y;
			DrawExtendGraph(left, y0, left + width, y1, handle, TRUE);
			left += width;
		}
	}

	DxLib::SetDrawArea(last_draw_area.left, last_draw_area.top, last_draw_area.right, last_draw_area.bottom);
}

void StageEditorSceneState_EditStageBg::ShowStageBgModal(ParentSceneClass& parent_scene)
{
	ImGui::PushFont(FontManager::GetInstance().GetFont(2));
	constexpr int NUM_POP_STYLE_COLOR = 3;
	ImGui::PushStyleColor(ImGuiCol_PopupBg, StageEditorColor::BackColor(0));
	ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColors_Button(StageEditorColor::BackColor(1));

	ImGui::SetNextWindowSize(modal_size);
	ImGui::SetNextWindowPos(modal_pos);
	ImGui::SetNextWindowBgAlpha(0.f);


	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_NoTitleBar;

	ImGui::SetNextWindowPos(modal_pos);
	ImGui::SetNextWindowSize(modal_size);
	if (ImGui::BeginPopupModal(str_id_modal_stage_bg, NULL, flags))
	{
		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			ImGui::CloseCurrentPopup();
			ResumeLastState(parent_scene);
		}

		const ImVec2 child_wnd_size_bg_layer_list = ImGui::GetWindowSize() * ImVec2 { 0.225, 0.9 };
		constexpr const char* str_id_child_wnd_bg_layer_list = "child_wnd_bg_layer_list";
		ImGui::BeginAlignedChild(str_id_child_wnd_bg_layer_list, ImVec2(0, 0.5), ImVec2(0, 0.5), child_wnd_size_bg_layer_list);
		{
			if (ImGui::BeginListBox("bg_layer_list_box", ImVec2{ -1.f, -1.f }))
			{
				for (size_t i = 0; i < _loaded_bg_layers.size(); i++)
				{
					const StageBGLayer& bg_layer = _loaded_bg_layers.at(i);
					const bool is_selected = i == _selected_bg_layer_index;
					if (ImGui::Selectable(bg_layer.name.c_str(), is_selected))
					{
						_selected_bg_layer_index = i;
					}

					// リストボックスを開いたときに選択されているアイテムをデフォルトでフォーカスする
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}

		}
		ImGui::EndAlignedChild();

		// 背景レイヤーのプレビュー部分をくりぬいた部分を描画
		{
			const ImVec4 bg_color = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);

			constexpr ImVec2 left_max = ImVec2{WINDOW_SIZE_X * PREVIEW_U0, modal_pos.y + modal_size.y};
			constexpr ImVec2 top_max = ImVec2{modal_pos.x + modal_size.x, WINDOW_SIZE_Y * PREVIEW_V0};
			constexpr ImVec2 modal_max = modal_pos + modal_size;
			constexpr ImVec2 bottom_min = ImVec2{ modal_pos.x, WINDOW_SIZE_Y * PREVIEW_V1 };
			constexpr ImVec2 right_min = ImVec2{WINDOW_SIZE_X * PREVIEW_U1, modal_pos.y};
			ImGui::GetBackgroundDrawList()->AddRectFilled(modal_pos, left_max, ImGui::GetColorU32(bg_color));
			ImGui::GetBackgroundDrawList()->AddRectFilled(modal_pos, top_max, ImGui::GetColorU32(bg_color));
			ImGui::GetBackgroundDrawList()->AddRectFilled(right_min, modal_max, ImGui::GetColorU32(bg_color));
			ImGui::GetBackgroundDrawList()->AddRectFilled(bottom_min, modal_max, ImGui::GetColorU32(bg_color));

			const ImU32 behind_modal_color = ImGui::GetColorU32(ImVec4{ 0,0,0,0.75 });
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ 0,0 }, {modal_pos.x, WINDOW_SIZE_Y}, behind_modal_color);
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ modal_max.x, 0 }, ImVec2{ WINDOW_SIZE_X, WINDOW_SIZE_Y }, behind_modal_color);
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ modal_pos.x,0 }, ImVec2{ modal_max.x, modal_pos.y }, behind_modal_color);
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{ modal_pos.x,modal_max.y }, ImVec2{ modal_max.x, WINDOW_SIZE_Y }, behind_modal_color);

		}

		// OK/Cancelボタン
		{
			ImVec2 button_pos_ok = ImGui::GetWindowSize() * ImVec2 { OK_BUTTON_CENTER_U, BUTTON_CENTER_V };
			ImVec2 button_pos_cancel = ImGui::GetWindowSize() * ImVec2 { CANCEL_BUTTON_CENTER_U, BUTTON_CENTER_V };
			ImVec2 button_size = ImVec2(150, 60);
			ImGui::SetCursorPos(button_pos_ok - button_size * 0.5);
			if (ImGui::Button(u8"OK", button_size))
			{
				DoChangeBgLayerCommand(parent_scene, _loaded_bg_layers.at(_selected_bg_layer_index));
				ImGui::CloseCurrentPopup();
				ResumeLastState(parent_scene);
			}

			ImGui::SetCursorPos(button_pos_cancel - button_size * 0.5);
			if (ImGui::Button(u8"Cancel", button_size))
			{
				ImGui::CloseCurrentPopup();
				ResumeLastState(parent_scene);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(NUM_POP_STYLE_COLOR);
	ImGui::PopStyleColors_Button();
	ImGui::PopFont();
}

void StageEditorSceneState_EditStageBg::DoChangeBgLayerCommand(StageEditorScene& parent_scene, const StageBGLayer& new_bg_layer)
{
	StageBGLayer current_bg_layer;
	for (auto& bg_layer : _loaded_bg_layers)
	{
		if (bg_layer.bg_layer_id == parent_scene.GetStageRef().GetBgLayerId())
		{
			current_bg_layer = bg_layer;
			break;
		}
	}

	if (current_bg_layer.bg_layer_id == new_bg_layer.bg_layer_id)
	{
		return;
	}

	parent_scene._command_history->ExecuteAndPush(
		std::make_shared<stage_editor_scene::CmdChangeStageBackground>(
			&parent_scene,
			current_bg_layer,
			new_bg_layer
		)
	);
}
