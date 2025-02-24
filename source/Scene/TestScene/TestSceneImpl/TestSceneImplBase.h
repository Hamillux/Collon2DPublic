#pragma once

#include "Scene/SceneBase.h"

class TestScene;
class Actor;
class EmitterComponent;

/// <summary>
/// NOTE: TestSceneImplBase派生のシーンはTestSceneが生成・破棄する
/// </summary>
class TestSceneImplBase : public SceneBase
{
	friend class TestScene;
public:
	TestSceneImplBase();
	virtual ~TestSceneImplBase();

	//~ Begin SceneBase interface
	virtual SceneType GetSceneType() const { return SceneType::TEST_SCENE_IMPL; };
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const ;
	//~ End SceneBase interface
};