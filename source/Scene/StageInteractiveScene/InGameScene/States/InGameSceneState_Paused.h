#pragma once

#include "InGameSceneState.h"

class InGameSceneState_Paused : public InGameSceneState
{
	//~ Begin SceneState interface
public:
	/// <summary>
	/// <para>- SetWorldTimerActive(false)を呼び出す</para>
	/// <para>- ポーズメニューのポップアップを表示する</para>
	/// </summary>
	/// <param name="parent_scene"></param>
	virtual void OnEnterState(ParentSceneClass& parent_scene) override;

	/// <summary>
	/// <para>- SetWorldTimerActive(true)を呼び出す</para>
	/// <para>- ポーズメニューのポップアップを非表示にする</para>
	/// </summary>
	/// <param name="parent_scene"></param>
	virtual void OnLeaveState(ParentSceneClass& parent_scene) override;

	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	//virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	//virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	virtual bool ShouldDestroyPreviousState() const override
	{
		return false;
	}
	//~ End StackableSceneState interface
};