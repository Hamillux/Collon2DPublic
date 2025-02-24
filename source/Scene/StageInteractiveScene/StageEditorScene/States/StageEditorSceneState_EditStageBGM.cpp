#include "StageEditorSceneState_EditStageBGM.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorColor.h"
#include "GameSystems/Sound/SoundManager.h"
#include "GameSystems/GameConfig/GameConfig.h"

namespace
{
    constexpr const char* str_id_modal_stage_bgm = "modal_stage_bgm";

    constexpr ImVec2 MODAL_SIZE = 
        ImVec2(WINDOW_SIZE_X * 0.8f, WINDOW_SIZE_Y * 0.8f);

    constexpr ImVec2 MODAL_POSITION = 
        ImVec2(WINDOW_SIZE_X / 2 - MODAL_SIZE.x / 2, WINDOW_SIZE_Y / 2 - MODAL_SIZE.y / 2);

    constexpr float OK_BUTTON_CENTER_U = 0.3f;
    constexpr float CANCEL_BUTTON_CENTER_U = 0.7f;
    constexpr float BUTTON_CENTER_V = 0.9f;
}

StageEditorSceneState_EditStageBGM::StageEditorSceneState_EditStageBGM()
    : _selected_bgm_index(0)
{
}

StageEditorSceneState_EditStageBGM::~StageEditorSceneState_EditStageBGM()
{
}

void StageEditorSceneState_EditStageBGM::OnEnterState(ParentSceneClass& parent_scene)
{
    __super::OnEnterState(parent_scene);

    parent_scene._should_show_controls = false;
    parent_scene._editor_scene_sounds->bgm->Stop();

    const MdStageBGM& current_bgm = MdStageBGM::Get(parent_scene.GetStageRef().GetBgmId());
    
    const std::vector<MdStageBGM>& bgm_data = MdStageBGM::GetData();
    for (int i = 0; i < bgm_data.size(); i++)
    {
        if (bgm_data.at(i).id == current_bgm.id)
        {
            _selected_bgm_index = i;
            break;
        }
    }

    PlayPreviewSound(current_bgm.file_path);

    ImGui::OpenPopup(str_id_modal_stage_bgm);
}

void StageEditorSceneState_EditStageBGM::OnLeaveState(ParentSceneClass& parent_scene)
{
    if (ImGui::IsPopupOpen(str_id_modal_stage_bgm))
    {
        ImGui::BeginPopupModal(str_id_modal_stage_bgm);
        ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (_preview_sound_instance)
    {
        _preview_sound_instance->Stop();
        _preview_sound_instance.reset();
    }

    parent_scene._editor_scene_sounds->bgm->Play(false);

    __super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<StageEditorSceneState_EditStageBGM::ParentSceneClass>> StageEditorSceneState_EditStageBGM::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
    auto ret = __super::Tick(parent_scene, delta_seconds);

    ShowStageBGMModal(parent_scene);

    return ret;
}

void StageEditorSceneState_EditStageBGM::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
    // 必要に応じて前景描画を実装
}

void StageEditorSceneState_EditStageBGM::ShowStageBGMModal(ParentSceneClass& parent_scene)
{
    ImGui::PushFont(FontManager::GetInstance().GetFont(2));
    constexpr int NUM_POP_STYLE_COLOR = 3;
    ImGui::PushStyleColor(ImGuiCol_PopupBg, StageEditorColor::BackColor(0));
    ImGui::PushStyleColor(ImGuiCol_Text, StageEditorColor::ForeColor(0));
    ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.75));
    ImGui::PushStyleColors_Button(StageEditorColor::BackColor(1));

    ImGui::SetNextWindowSize(MODAL_SIZE);
    ImGui::SetNextWindowPos(MODAL_POSITION);

    ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoCollapse | 
        ImGuiWindowFlags_NoTitleBar;

    if (ImGui::BeginPopupModal(str_id_modal_stage_bgm, NULL, flags))
    {
        if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
        {
            ImGui::CloseCurrentPopup();
            ResumeLastState(parent_scene);
        }

        // BGMリスト
        const ImVec2 child_wnd_size_bgm_list = ImGui::GetWindowSize() * ImVec2 { 0.9, 0.7 };
        constexpr const char* str_id_child_wnd_bgm_list = "child_wnd_bgm_list";
        constexpr ImVec2 offset_bgm_list{0, 32};
        ImGui::BeginAlignedChild(str_id_child_wnd_bgm_list, ImVec2(0.5, 0), ImVec2(0.5, 0), child_wnd_size_bgm_list, offset_bgm_list);
        {
            if (ImGui::BeginListBox("bgm_list_box", ImVec2{ -1.f, -1.f }))
            {
                const std::vector<MdStageBGM>& bgm_data = MdStageBGM::GetData();
                for (int i = 0; i < bgm_data.size(); i++)
                {
                    const std::string& label = std::to_string(i + 1) + u8". " + bgm_data.at(i).title;
                    const std::string& bgm_file = bgm_data.at(i).file_path;
                    const bool is_selected = i == _selected_bgm_index;

                    if (ImGui::Selectable(label.c_str(), is_selected))
                    {
                        _selected_bgm_index = i;

                        // プレビュー再生
                        PlayPreviewSound(bgm_file);
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

        // OK/Cancelボタン
        {
            ImVec2 button_pos_ok = ImGui::GetWindowSize() * ImVec2 { OK_BUTTON_CENTER_U, BUTTON_CENTER_V };
            ImVec2 button_pos_cancel = ImGui::GetWindowSize() * ImVec2 { CANCEL_BUTTON_CENTER_U, BUTTON_CENTER_V };
            ImVec2 button_size = ImVec2(150, 60);
            ImGui::SetCursorPos(button_pos_ok - button_size * 0.5);
            if (ImGui::Button(u8"OK", button_size))
            {
                DoChangeBGMCommand(parent_scene, MdStageBGM::GetData().at(_selected_bgm_index).id);
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

void StageEditorSceneState_EditStageBGM::DoChangeBGMCommand(StageEditorScene& parent_scene, const MasterDataID bgm_id)
{
    // 現在のBGMを取得
    const MasterDataID current_bgm = parent_scene.GetStageRef().GetBgmId();

    if (current_bgm == bgm_id)
    {
        return;
    }

    // BGM変更コマンドを実行
    parent_scene._command_history->ExecuteAndPush(
        std::make_shared<stage_editor_scene::CmdChangeStageBGM>(
            &parent_scene,
            current_bgm,
            bgm_id
        )
    );
}

void StageEditorSceneState_EditStageBGM::PlayPreviewSound(const std::string& bgm_file)
{
    if (_preview_sound_instance)
    {
        _preview_sound_instance->Stop();
    }

    const int bgm_volume = std::max(25, GameConfig::GetInstance().GetConfigItem<StageEditorConfig>()->sound_volume.bgm);

    _preview_sound_instance = SoundManager::GetInstance().MakeSoundInstance(bgm_file);
    _preview_sound_instance->SetVolume(bgm_volume);
    _preview_sound_instance->Play(true);
}
