#pragma once

#include "Scene/StageInteractiveScene/StageEditorScene/States/StageEditorSceneState_Edit.h"

class StageEditorSceneState_Edit_Move : public StageEditorSceneState_Edit
{
public:
	StageEditorSceneState_Edit_Move();
	virtual ~StageEditorSceneState_Edit_Move();

    //~ Begin SceneState interface
public:
    virtual void OnEnterState(ParentSceneClass& parent_scene) override;
    virtual void OnLeaveState(ParentSceneClass& parent_scene) override;
    virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override;
    virtual void Draw(ParentSceneClass& parent_scene) override;
    virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
    virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
    virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
    //~ End SceneState interface

	//~ Begin StageEditorSceneState_Edit
public:
	virtual void ShowRightbarBody(StageEditorScene& parent_editor_scene) override;
	virtual EditMode GetEditMode() const override { return EditMode::MOVE; }
    virtual void OnUndo(StageEditorScene& parent_editor_scene) override;
    virtual void OnRedo(StageEditorScene& parent_editor_scene) override;
	//~ End StageEditorSceneState_Edit

private:
    enum class SubState
    {
        Default,
        MovingActors,
        RectSelecting,
		MovingDuplicatedActors,
    };
    SubState _current_substate;
    void HandleInputImpl_Default(StageEditorScene& parent_scene, const float delta_seconds);
	void HandleInputImpl_MovingActors(StageEditorScene& parent_scene, const float delta_seconds);
	void HandleInputImpl_RectSelecting(StageEditorScene& parent_scene, const float delta_seconds);
	void HandleInputImpl_MovingDuplicatedActors(StageEditorScene& parent_scene, const float delta_seconds);
	void HandleInputImpl_ChangingStageLength(StageEditorScene& parent_scene, const float delta_seconds);

    struct MovingInfo
    {
        int mouse_tile_from_x;
		int mouse_tile_from_y;

        // マウス位置からアクター全体の占有領域の端までのタイル数オフセット
        struct OffsetTiles4
        {
            int to_left;
            int to_top;
            int to_right;
            int to_bottom;
        };
        OffsetTiles4 offset_tiles_to_occupying_area_edge;

        std::vector<Actor*> moving_actors;

        std::unordered_map<Actor*, Vector2D> moved_from;

        // マウスオーバーしているタイルからスナップ位置を含むタイルまでのタイル数オフセット
        struct OffsetTiles2
        {
            int x;
            int y;
        };
        std::unordered_map<Actor*, OffsetTiles2> offset_tiles_to_actor_snap_pos;

		// スナップ位置からアクター位置までのオフセット
		std::unordered_map<Actor*, Vector2D> snap_pos_to_actor_pos;
    };
	std::unique_ptr<MovingInfo> _moving_info;   // 選択されたアクターの移動開始時に生成、移動完了時に破棄

	struct DuplicationInfo
	{
        int mouse_tile_from_x;
        int mouse_tile_from_y;

        // マウス位置からアクター全体の占有領域の端までのタイル数オフセット
        struct OffsetTiles4
        {
			int to_left;
			int to_top;
			int to_right;
			int to_bottom;
        };
        OffsetTiles4 offset_tiles_to_occupying_area_edge;

        std::vector<Actor*> duplicated_actors;

        // マウスオーバーしているタイルからスナップ位置を含むタイルまでのタイル数オフセット
        std::unordered_map<Actor*, std::pair<int, int>> offset_tiles_to_actor_snap_pos;

        // スナップ位置→アクター位置
        std::unordered_map<Actor*, Vector2D> snap_pos_to_actor_pos;

		std::unordered_map<Actor*, std::shared_ptr<SpawnActorInfo>> spawn_actor_infos;
	};
	std::unique_ptr<DuplicationInfo> _duplication_info;

    std::vector<Actor*> _selected_actors;   // 選択リスト

    // 選択リストをクリアする
	void UnselectAllActors();

	// actorを選択リストに追加する. すでに選択されている場合は何もしない
	void SelectActor(Actor* actor);

	// actorを選択リストから削除する. 選択されていない場合は何もしない
    void UnSelectActor(Actor* actor);
    bool IsAnyActorSelected() const;
	bool IsSelectedActor(Actor* actor) const;

    void BeginMovingSelectedActors(StageEditorScene& parent_scene);
    void CancelMovingActor();

	void BeginDuplicationOfSelectedActors(StageEditorScene& parent_scene);
	void CancelDuplication(StageEditorScene& parent_scene);
	bool IsDuplicatableActor(Actor* actor) const;

    void RemoveSelectedActors(StageEditorScene& parent_editor_scene);
    bool IsDeletableActor(StageEditorScene& parent_editor_scene, Actor* actor, const bool should_push_message = true) const;

    std::unique_ptr<Vector2D> _drag_drop_start_viewport_pos;
    std::unique_ptr<Vector2D> _drag_drop_end_viewport_pos;
    void OnBeginDragDrop();
	void OnEndDragDrop();
    bool IsDoingDragDrop();

    // 境界円が軸平行な矩形に含まれるアクターを取得する
    void GetActorsInsideAARect(StageEditorScene& parent_editor_scene, const FRectAA world_rect, std::vector<Actor*>& contained_actors);
};