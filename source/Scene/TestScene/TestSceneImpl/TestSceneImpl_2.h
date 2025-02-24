#pragma once

#include "Scene/TestScene/TestSceneImpl/TestSceneImplBase.h"

class TestSceneImpl_2 : public TestSceneImplBase
{
public:
	TestSceneImpl_2();
	virtual ~TestSceneImpl_2();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	//virtual void Draw() override;
	//virtual void DrawForeground() override;
	//virtual void Finalize() override;
	//virtual void UpdateCameraParams(const float delta_seconds) override;
protected:
	//virtual void PreDestroyActor(Actor* destroyee) override;
	//virtual void OnDestroyedActor(Actor* destroyed) override;
	// End SceneBase interface

};