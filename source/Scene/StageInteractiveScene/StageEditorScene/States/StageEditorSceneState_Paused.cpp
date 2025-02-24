#include "StageEditorSceneState_Paused.h"
#include "GameSystems/FontManager.h"
#include "GameSystems/Sound/SoundInstance.h"

namespace {
	constexpr MasterDataID PAUSE_MODAL_FONT_ID = 4;
	constexpr ImVec2 PAUSE_MODAL_BUTTON_SIZE = { 512, 128 };
	constexpr float PAUSE_MODAL_BUTTON_SPACE = 64;

	constexpr const char* STR_ID_CONFIRM_SAVE_MODAL = "stage_editor_scene_confirm_save_modal";
	constexpr MasterDataID CONFIRM_MODAL_FONT_ID = 3;
	constexpr ImVec2 CONFIRM_BUTTON_SIZE = { 120, 40 };
	constexpr ImVec2 CONFIRM_MODAL_SIZE = { 400,300 };
	constexpr float TEXT_CURSOR_POS_Y = 0.3f * CONFIRM_MODAL_SIZE.y;
	constexpr float BUTTONS_CURSOR_POS_Y = 0.7f * CONFIRM_MODAL_SIZE.y;
}

StageEditorSceneState_Paused::StageEditorSceneState_Paused()
{
}

StageEditorSceneState_Paused::~StageEditorSceneState_Paused()
{}

void StageEditorSceneState_Paused::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	parent_scene._editor_scene_sounds->bgm->Stop();

	// ポーズモーダルを表示
	ImGui::OpenPopup(STR_ID_PAUSE_MODAL);
}

void StageEditorSceneState_Paused::OnLeaveState(ParentSceneClass& parent_scene)
{
	parent_scene._editor_scene_sounds->bgm->Play(false);

	// ポーズモーダルを閉じる
	if(ImGui::IsPopupOpen(STR_ID_PAUSE_MODAL))
	{
		ImGui::BeginPopupModal(STR_ID_PAUSE_MODAL);
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorSceneState_Paused::ParentSceneClass>> StageEditorSceneState_Paused::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowSize({WINDOW_SIZE_X, WINDOW_SIZE_Y});
	ImGui::SetNextWindowPos({ 0,0 });

	constexpr ImVec2 CHILD_SIZE = {
		PAUSE_MODAL_BUTTON_SIZE.x,
		PAUSE_MODAL_BUTTON_SIZE.y * 3 + PAUSE_MODAL_BUTTON_SPACE * 2
	};

	// ポーズモーダル
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(PAUSE_MODAL_FONT_ID));

		// BeginModal ~ EndModalの中でOpenPopupを呼んでもうまくいかなかったので、フラグで制御
		bool should_open_confirm_save_modal = false;
		if (ImGui::BeginPopupModal(STR_ID_PAUSE_MODAL, NULL, flags))
		{
			constexpr int NUM_POP_STYLE_VAR = 2;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, PAUSE_MODAL_BUTTON_SPACE));

			ImGui::SetNextWindowFocus();
			ImGui::BeginAlignedChild("buttons", { 0.5, 0.5 }, { 0.5, 0.5 }, CHILD_SIZE);
			{
				ImGui::PushAllowKeyboardFocus(false);
				if (ImGui::Button("RESUME", PAUSE_MODAL_BUTTON_SIZE))
				{
					ResumeLastState(parent_scene);
				}

				if (ImGui::Button("RETURN TO STAGE SELECT", PAUSE_MODAL_BUTTON_SIZE))
				{
					parent_scene.destination_scene = SceneType::SELECT_SCENE;
					if (parent_scene.IsUnsaved())
					{
						ImGui::CloseCurrentPopup();
						should_open_confirm_save_modal = true;
					}
					else
					{
						parent_scene.EndScene();
					}
				}

				if (ImGui::Button("RETURN TO TITLE", PAUSE_MODAL_BUTTON_SIZE))
				{
					parent_scene.destination_scene = SceneType::TITLE_SCENE;
					if (parent_scene.IsUnsaved())
					{
						ImGui::CloseCurrentPopup();
						should_open_confirm_save_modal = true;
					}
					else
					{
						parent_scene.EndScene();
					}
				}
				ImGui::PopAllowKeyboardFocus();
			}
			ImGui::EndAlignedChild();
			
			ImGui::PopStyleVar(NUM_POP_STYLE_VAR);

			if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
			{
				ResumeLastState(parent_scene);
			}

			ImGui::EndPopup();
		}

		if (should_open_confirm_save_modal)
		{
			ImGui::OpenPopup(STR_ID_CONFIRM_SAVE_MODAL);
		}

		ImGui::PopFont();
	}

	// 保存確認モーダル
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(CONFIRM_MODAL_FONT_ID));

		// 上と同様に、BeginModal ~ EndModalの中でOpenPopupを呼んでもうまくいかなかったので、フラグで制御
		bool should_open_pause_modal = false;
		ImVec2 screen_size = ImGui::GetScreenSize();
		ImGui::SetNextWindowSize(CONFIRM_MODAL_SIZE);
		ImGui::SetNextWindowPos(ImGui::GetScreenSize() * 0.5f, ImGuiCond_Always, { 0.5, 0.5 });
		if (ImGui::BeginPopupModal(STR_ID_CONFIRM_SAVE_MODAL, NULL, flags))
		{
			ImGui::SetCursorPosY(TEXT_CURSOR_POS_Y);
			ImGui::Text(u8"作業内容を保存しますか？");

			constexpr int NUM_POP_STYLE_VAR = 2;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, PAUSE_MODAL_BUTTON_SPACE));

			ImGui::SetCursorPosY(BUTTONS_CURSOR_POS_Y);
			if (ImGui::Button(u8"保存する", CONFIRM_BUTTON_SIZE))
			{
				ImGui::CloseCurrentPopup();
				parent_scene.SaveStage();
				parent_scene.EndScene();
			}
			ImGui::SameLine();

			if (ImGui::Button(u8"保存しない", CONFIRM_BUTTON_SIZE))
			{
				ImGui::CloseCurrentPopup();
				parent_scene.EndScene();
			}
			ImGui::SameLine();

			if (ImGui::Button(u8"キャンセル", CONFIRM_BUTTON_SIZE))
			{
				parent_scene.destination_scene = SceneType::NONE;
				ImGui::CloseCurrentPopup();
				should_open_pause_modal = true;
			}

			ImGui::PopStyleVar(NUM_POP_STYLE_VAR);

			ImGui::EndPopup();
		}

		if (should_open_pause_modal)
		{
			ImGui::OpenPopup(STR_ID_PAUSE_MODAL);
		}
	}

	ImGui::PopFont();
	return ret;
}
