#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorScene.h"

class StageEditorSceneState_Edit : public StageEditorSceneState
{
public:
	StageEditorSceneState_Edit();
	virtual ~StageEditorSceneState_Edit();

	enum class EditMode : int
	{
		SUMMON = 0,
		MODIFY = 1,
		MOVE = 2,
	};
	static constexpr std::array<EditMode, 3> EDIT_MODE_LIST = { EditMode::SUMMON, EditMode::MODIFY, EditMode::MOVE};

	//~ Begin SceneState interface
public:
	// virtual void OnEnterState(ParentSceneClass& parent_scene) override;
	// virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
	// virtual void Draw(ParentSceneClass& parent_scene) override;
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StageEditorSceneState interface
public:
	virtual void ShowControls(StageEditorScene& parent_editor_scene) override;
	//~End StageEditorSceneState interface

	//~ Begin StageEditorSceneState_Edit interface
public:
	virtual void ShowRightbarBody(StageEditorScene& parent_editor_scene) = 0;
	virtual EditMode GetEditMode() const = 0;

	/// <summary>
	/// Undo時の処理. StageEditorScene_Edit::OnUndo()では効果音を再生
	/// </summary>
	virtual void OnUndo(StageEditorScene& parent_editor_scene);

	/// <summary>
	/// Redo時の処理. StageEditorScene_Edit::OnRedo()では効果音を再生
	/// </summary>
	virtual void OnRedo(StageEditorScene& parent_editor_scene);
	//~ End StageEditorSceneState_Edit interface

protected:
	/// <summary>
	/// 点Qを含むタイルの中心座標を取得
	/// </summary>
	static Vector2D GetTileCenterPosition
	(
		const Vector2D& q_world,
		const int offset_tiles_x,
		const int offset_tiles_y
	);

	static Vector2D GetTileCenterPosition(const int tile_index_x, const int tile_index_y);

	static void GetTileIndex
	(
		const Vector2D& q_world,
		int& tile_index_x,
		int& tile_index_y
	);
	

private:
	void Undo(StageEditorScene& parent_editor_scene);
	void Redo(StageEditorScene& parent_editor_scene);
	void SaveStage(StageEditorScene& parent_editor_scene);
	void ShowSidebars(StageEditorScene& parent_editor_scene);
	void ShowPopups(StageEditorScene& parent_editor_scene);
	void ShowLeftbarContent(StageEditorScene& parent_editor_scene);
	void ShowRightbarContent(StageEditorScene& parent_editor_scene);
	void ShowRightbarHead(StageEditorScene& parent_editor_scene);
	static std::shared_ptr<SceneState<StageEditorScene>> MakeSubState(const EditMode edit_mode);
	static EditMode GetNextEditMode(const EditMode current_edit_mode);
	static EditMode GetPrevEditMode(const EditMode current_edit_mode);

	bool _is_sidebar_hidden;
	bool _is_grid_shown;
};