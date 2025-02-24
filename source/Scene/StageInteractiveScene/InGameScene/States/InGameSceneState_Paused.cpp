#include "InGameSceneState_Paused.h"
#include "GameSystems/FontManager.h"

namespace {
	constexpr const char* STR_ID_PAUSED_MODAL = "ingame_scene_paused_modal";
	constexpr MasterDataID PAUSE_MODAL_FONT_ID = 4;
	constexpr ImVec2 PAUSE_MODAL_BUTTON_SIZE = { 512, 128 };
	constexpr float PAUSE_MODAL_BUTTON_SPACE = 64;
}

void InGameSceneState_Paused::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	parent_scene.SetWorldTimerActive(false);

	parent_scene._sound_instance_bgm->Stop();

	ImGui::OpenPopup(STR_ID_PAUSED_MODAL);
}

void InGameSceneState_Paused::OnLeaveState(ParentSceneClass& parent_scene)
{
	__super::OnLeaveState(parent_scene);

	parent_scene.SetWorldTimerActive(true);

	parent_scene._sound_instance_bgm->Play(false);

	if (ImGui::IsPopupOpen(STR_ID_PAUSED_MODAL))
	{
		ImGui::BeginPopupModal(STR_ID_PAUSED_MODAL);
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

std::shared_ptr<SceneState<InGameSceneState_Paused::ParentSceneClass>> InGameSceneState_Paused::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	ImGuiWindowFlags flags = ImGuiWindowFlags_None;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowSize({ WINDOW_SIZE_X, WINDOW_SIZE_Y });
	ImGui::SetNextWindowPos({ 0,0 });

	constexpr ImVec2 CHILD_SIZE = {
		PAUSE_MODAL_BUTTON_SIZE.x,
		PAUSE_MODAL_BUTTON_SIZE.y * 3 + PAUSE_MODAL_BUTTON_SPACE * 2
	};

	// ポーズモーダル
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(PAUSE_MODAL_FONT_ID));

		if (ImGui::BeginPopupModal(STR_ID_PAUSED_MODAL, NULL, flags))
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
					parent_scene._destination_scene = SceneType::SELECT_SCENE;
					parent_scene.EndInGameScene();
				}

				if (ImGui::Button("RETURN TO TITLE", PAUSE_MODAL_BUTTON_SIZE))
				{
					parent_scene._destination_scene = SceneType::TITLE_SCENE;
					parent_scene.EndInGameScene();
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

		ImGui::PopFont();
	}
	
	return ret;
}
