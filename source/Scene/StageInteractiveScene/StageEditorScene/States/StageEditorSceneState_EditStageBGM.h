#pragma once

#include "StageEditorSceneState.h"
#include <vector>
#include <string>
#include <memory>

class StageEditorSceneState_EditStageBGM : public StageEditorSceneState
{
public:
    StageEditorSceneState_EditStageBGM();
    virtual ~StageEditorSceneState_EditStageBGM();

    //~ Begin SceneState interface
public:
    virtual void OnEnterState(ParentSceneClass& parent_scene) override;
    virtual void OnLeaveState(ParentSceneClass& parent_scene) override;

    virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
    virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
    //~ End SceneState interface

    //~ Begin StackableSceneState interface
private:
    virtual bool ShouldDestroyPreviousState() const override
    {
        return false;
    }
    //~ End StackableSceneState interface

private:
    void ShowStageBGMModal(ParentSceneClass& parent_scene);
    void DoChangeBGMCommand(StageEditorScene& parent_scene, const MasterDataID bgm_id);

    size_t _selected_bgm_index = 0;

    std::shared_ptr<SoundInstance> _preview_sound_instance;
	void PlayPreviewSound(const std::string& bgm_file);
};
