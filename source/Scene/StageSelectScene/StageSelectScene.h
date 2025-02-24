#pragma once

#include "Scene/SceneBase.h"
#include "SelectSceneInitialParams.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"
#include <memory>
#include <unordered_set>

class StageSelectScene : public SceneBase
{
public:
	StageSelectScene();
	virtual ~StageSelectScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::SELECT_SCENE; }
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const override;
	//~ End SceneBase interface

private:
	struct TransitionInfo 
	{
		SceneType destination_scene_type;
		float remain_time;
	};
	std::unique_ptr<TransitionInfo> _transition_info;	// 遷移開始時に生成
	void BeginTransition(const SceneType destination_scene_type);
	bool IsInTransition() const { return _transition_info != nullptr; }

	void FocusStage(const StageId& stage_id_to_hover);
	void RemoveSelectedStage();
	void CreateNewStageThumnail(const StageId& new_stage_id);
	StageId CreateNewStage();
	SceneType HandleInput();
	void ShowControlInfo() const;

	enum class StageSelectMode : int
	{
		PLAY = 0, EDIT = 1, REMOVE = 2
	};
	StageSelectMode _current_select_mode;
	void ChangeStageSelectMode(const StageSelectMode new_mode);

	int GetThumbnailHandle(const StageId& stage_id) const;

	void LoadStageListFromFile(std::vector<StageId>& out_stage_list) const;
	void LoadStage(const StageId& stage_id);
	void UpdateStageIDListFile() const;

	void DrawSmallThumbNails() const;
	void DrawSelectedStageThumbnail() const;
	StageId GetHoveredStageId() const;
	bool HoveringNewStageCell() const;
	bool NoStageIsHovered() const;
	bool IsSelectingStageToEdit() const;

	StageId _selected_stage_id;
	Stage* GetSelectedStage();

	enum class ControlMode{MOUSE, KEYBOARD};
	void ChangeControlMode(const ControlMode new_control_mode);
	ControlMode _current_control_mode;

	int GetNumPages() const;
	int GetMaxCellIndex() const;
	int _current_page;
	int _current_cell_x;
	int _current_cell_y;
	int _last_cell_x;
	int _last_cell_y;
	static constexpr int NONE_HOVERED = -1;

	std::map<StageId, int> _id_thumbnail_map;
	std::vector<StageId> _stage_id_list;
	std::unordered_map<StageId, Stage> _stage_map;

	int _bg_handle;

	struct SelectSceneSounds
	{
		std::shared_ptr<SoundInstance> bgm;
		std::shared_ptr<SoundInstance> se_move_cursor;
		std::shared_ptr<SoundInstance> se_select;
		std::shared_ptr<SoundInstance> se_switch_page;
		std::shared_ptr<SoundInstance> se_switch_select_mode;
	};
	SelectSceneSounds _sounds;
	void LoadSelectSceneSounds();
};


template<>
struct SceneBase::traits<StageSelectScene>
{
	typedef StageSelectSceneInitialParams initial_params_type;
};