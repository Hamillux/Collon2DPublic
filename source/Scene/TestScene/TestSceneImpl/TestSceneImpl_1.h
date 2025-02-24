#pragma once

#include "Scene/TestScene/TestSceneImpl/TestSceneImplBase.h"

class Actor;
class EmitterComponent;

class TestSceneImpl_1 : public TestSceneImplBase
{
public:
	TestSceneImpl_1();
	virtual ~TestSceneImpl_1();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params);
	virtual SceneType Tick(float delta_seconds);
	virtual void Draw();
	virtual void UpdateCameraParams(const float delta_seconds);
	//~ End SceneBase interface

	Actor* test_actor;
	EmitterComponent* emitter_component;
};