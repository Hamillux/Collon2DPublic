#pragma once
#include "Scene/SceneType.h"

struct SceneBaseInitialParams
{
	SceneBaseInitialParams()
		: prev_scene_type(SceneType::NONE)
	{}

	SceneBaseInitialParams(const SceneType prev_scene)
		: prev_scene_type(prev_scene)
	{}
	virtual ~SceneBaseInitialParams() {}

	SceneType prev_scene_type;
};