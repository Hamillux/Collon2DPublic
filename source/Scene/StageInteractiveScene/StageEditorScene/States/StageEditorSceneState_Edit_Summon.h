#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState_Edit.h"

class Actor;

class StageEditorSceneState_Edit_Summon : public StageEditorSceneState_Edit
{
public:
	StageEditorSceneState_Edit_Summon();
	virtual ~StageEditorSceneState_Edit_Summon();

    //~ Begin SceneState interface
public:
    // virtual void OnEnterState(ParentSceneClass& parent_scene) override;
    virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
    // virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
    virtual void Draw(ParentSceneClass& parent_scene) override;
    virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
    // virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
    virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
    //~ End SceneState interface

	//~ Begin StageEditorSceneState_Edit
private:
	virtual void ShowRightbarBody(StageEditorScene& parent_editor_scene) override;
	virtual EditMode GetEditMode() const override { return EditMode::SUMMON; }
    // virtual void OnUndo(StageEditorScene& parent_editor_scene) override;
    // virtual void OnRedo(StageEditorScene& parent_editor_scene) override;
	//~ End StageEditorSceneState_Edit

private:
    void OnSummonIconClicked(StageEditorScene& parent_editor_scene, const std::shared_ptr<StageEditorScene::SummonEntityInfo> selected_info);
    void ResetActorToSummon(StageEditorScene& parent_editor_scene, const std::shared_ptr<StageEditorScene::SummonEntityInfo> summon_info);
    void SummonActor(StageEditorScene& parent_editor_scene);

    Actor* _created_actor_to_summon;
	int _idx_summon_icon;
	std::shared_ptr<StageEditorScene::SummonEntityInfo> _summoning_entity_info;
};