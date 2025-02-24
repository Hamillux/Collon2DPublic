#pragma once
#include "Scene/SceneBase.h"
#include <memory>

class TestSceneImplBase;

class TestScene : public SceneBase
{
public:
	TestScene();
	virtual ~TestScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	virtual void Draw() override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::TEST_SCENE; };
	virtual void UpdateCameraParams(const float delta_seconds) override;
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const override;
protected:
	virtual void PreDestroyActor(Actor* destroyee) override;
	virtual void OnDestroyedActor(Actor* destroyed) override;
	// End SceneBase interface

private:
	void CreateTestSceneImpl(const int test_id);

	std::unique_ptr<TestSceneImplBase> _test_scene_impl;
};

struct TestSceneInitialParams : public SceneBaseInitialParams
{
	TestSceneInitialParams()
		: test_id(1)
	{}
	virtual ~TestSceneInitialParams() {}

	int test_id;
};