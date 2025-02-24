#pragma once

#include "StageEditorSceneState.h"

class StageEditorSceneState_ThumbnailShooting : public StageEditorSceneState
{
public:
	StageEditorSceneState_ThumbnailShooting();
	virtual ~StageEditorSceneState_ThumbnailShooting();

	//~ Begin SceneState interface
public:
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	// virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StageEditorSceneState interface
public:
	virtual void ShowControls(StageEditorScene& parent_editor_scene) override;
	//~End StageEditorSceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface

	/// <summary>
	///	キャプチャ結果をステージのサムネイルに設定する
	/// </summary>
	/// <param name="stage"></param>
	void UpdateStageThumbnail(const Stage& stage) const;

	enum class ThunmailShootingSubState
	{
		Default,
		ReleasingShutter,
		Confirm,
	};
	void ChangeSubState(const ThunmailShootingSubState next_substate);
	ThunmailShootingSubState _current_substate;

	std::shared_ptr<DxLibScreenCapture> _capture_result;
	float _shuttering_phase;
	float _shuttering_speed;

	bool _should_show_grid;
};