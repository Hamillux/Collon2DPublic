#pragma once
#include "Scene/SceneBaseInitialParams.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"

class StageEditorScene;

struct EditorSceneInitialParams : public SceneBaseInitialParams
{
	EditorSceneInitialParams()
		: stage_id(StageId::NONE)
	{}

	EditorSceneInitialParams(const SceneType prev_scene_in, const StageId stage_id_in)
		: SceneBaseInitialParams(prev_scene_in)
		, stage_id(stage_id_in)
	{}
	virtual ~EditorSceneInitialParams() {}

	StageId stage_id;
};