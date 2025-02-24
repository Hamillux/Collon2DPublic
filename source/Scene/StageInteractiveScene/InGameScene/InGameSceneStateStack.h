#pragma once

#include "Scene/SceneState/SceneState.h"
#include "InGameScene.h"

class InGameSceneStateStack : public SceneStateStack<InGameScene>
{
public:
	InGameSceneStateStack() {}
	virtual ~InGameSceneStateStack() {};
};