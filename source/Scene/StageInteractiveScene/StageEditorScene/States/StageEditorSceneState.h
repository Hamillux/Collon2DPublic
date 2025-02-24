#pragma once

#include "Scene/SceneState/SceneState.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorScene.h"

/// <summary>
/// StageEditorSceneの状態クラスの基底クラス. 
/// ポップアップ表示中, ImGuiアイテムアクティブ時はHandleInputが呼ばれない
/// </summary>
class StageEditorSceneState : public StackableSceneState<StageEditorScene>
{
public:
	StageEditorSceneState() {}
	virtual ~StageEditorSceneState() {}

	//~ Begin SceneState interface
public:
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;

	// 操作説明の表示・非表示
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StageEditorSceneState interface
public:
	virtual void ShowControls(StageEditorScene& parent_editor_scene);
	//~End StageEditorSceneState interface


};