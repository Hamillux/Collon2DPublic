#pragma once

#include "Scene/SceneBase.h"

class TestSelectScene : public SceneBase
{
public:
	TestSelectScene();
	virtual ~TestSelectScene();

	//~ Begin SceneBase interface
public:
	virtual SceneType Tick(float delta_seconds);
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const;
	virtual SceneType GetSceneType() const;
	//~ End SceneBase interface

private:
	int _selected_test_id;
};