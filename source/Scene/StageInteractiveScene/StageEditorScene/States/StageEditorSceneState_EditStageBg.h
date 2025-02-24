#pragma once

#include "StageEditorSceneState.h"

class StageEditorSceneState_EditStageBg : public StageEditorSceneState
{
public:
	StageEditorSceneState_EditStageBg();
	virtual ~StageEditorSceneState_EditStageBg();

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;

	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	// virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	// virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	// virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface

private:
	void ShowStageBgModal(ParentSceneClass& parent_scene);
	void DoChangeBgLayerCommand(StageEditorScene& parent_scene, const StageBGLayer& bg_layer);
	std::vector<StageBGLayer> _loaded_bg_layers;
	size_t _selected_bg_layer_index = 0;
	float _preview_cx = 0.f;
};