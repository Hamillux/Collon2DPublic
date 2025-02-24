#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState.h"

class StageEditorSceneState_EditorSettings : public StageEditorSceneState
{
public:
	StageEditorSceneState_EditorSettings();
	virtual ~StageEditorSceneState_EditorSettings();

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface

private:
	void ShowEditorSettingsPopup(ParentSceneClass& parent_scene);
	void ShowSettings_General(ParentSceneClass& parent_scene);
	void ShowSettings_Audio(ParentSceneClass& parent_scene);
	void ShowSettings_KeyConfig(ParentSceneClass& parent_scene);

	bool _should_reload_config;
	std::unique_ptr<ParamEditGroup> _param_edit_group_general;
};