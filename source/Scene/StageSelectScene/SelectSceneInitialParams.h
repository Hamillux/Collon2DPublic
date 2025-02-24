#pragma once
#include "Scene/SceneBaseInitialParams.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"

struct StageSelectSceneInitialParams : public SceneBaseInitialParams
{
	StageSelectSceneInitialParams()
		: next_scene(SceneType::NONE)
		, initially_selected_stage(StageId(StageId::NONE))
	{}
	StageSelectSceneInitialParams(const SceneType prev_scene_in, const SceneType next_scene_in, const StageId initially_selected_stage_in = StageId(StageId::NONE))
		: SceneBaseInitialParams(prev_scene_in)
		, next_scene(next_scene_in)
		, initially_selected_stage(initially_selected_stage_in)
	{}
	virtual ~StageSelectSceneInitialParams()
	{}

	SceneType next_scene;
	StageId initially_selected_stage;
};