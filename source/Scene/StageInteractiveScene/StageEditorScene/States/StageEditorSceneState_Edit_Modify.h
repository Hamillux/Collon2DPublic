#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState_Edit.h"

class StageEditorSceneState_Edit_Modify : public StageEditorSceneState_Edit
{
public:
	StageEditorSceneState_Edit_Modify();
	virtual ~StageEditorSceneState_Edit_Modify();

    //~ Begin SceneState interface
public:
    // virtual void OnEnterState(ParentSceneClass& parent_scene) override;
    // virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
    // virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
    // virtual void Draw(ParentSceneClass& parent_scene) override;
    virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
    // virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
    virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
    //~ End SceneState interface

	//~ Begin StageEditorSceneState interface
public:
	virtual void ShowControls(StageEditorScene& parent_editor_scene) override;
	//~End StageEditorSceneState interface

	//~ Begin StageEditorSceneState_Edit
public:
	virtual void ShowRightbarBody(StageEditorScene& parent_editor_scene) override;
	virtual EditMode GetEditMode() const override { return EditMode::MODIFY; }
	virtual void OnUndo(StageEditorScene& parent_editor_scene) override;
	virtual void OnRedo(StageEditorScene& parent_editor_scene) override;
	//~ End StageEditorSceneState_Edit

private:
	struct ActorModificationContext
	{
		ActorModificationContext(Actor* const in_actor, const std::shared_ptr<SpawnActorInfo>& in_spawn_info, std::shared_ptr<CommandHistory> command_history);
		Actor* actor;		// パラメータ編集中のアクター
		std::shared_ptr<SpawnActorInfo> spawn_info;
		std::shared_ptr<ParamEditGroup> edit_param_group_root;
	};

	// 生存期間はアクターを選択した直後から, そのアクターのパラメータを表示している間.
	std::unique_ptr<ActorModificationContext> actor_modification_context;
	bool IsModifyingActor() const { return actor_modification_context != nullptr; }

	// アクターのパラメータ編集を開始する
	// - 編集中に別のアクターの編集を開始しようとした場合は, そのアクターの編集を終了してから新しいアクターの編集を開始する
	// - 無効なアクターを指定した場合は, 編集中であればその編集を終了し, そうでなければ何もしない      
	void BeginActorModification(StageEditorScene& parent_scene, Actor* target_actor);

	// アクターのパラメータ編集を終了する
	void EndActorModification();
};