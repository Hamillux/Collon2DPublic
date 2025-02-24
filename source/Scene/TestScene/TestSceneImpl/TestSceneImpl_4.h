#pragma once
#include "Scene/TestScene/TestSceneImpl/TestSceneImplBase.h"

class TestSceneImpl_4 : public TestSceneImplBase
{
public:
	TestSceneImpl_4();
	virtual ~TestSceneImpl_4();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	virtual void Draw() override;
	//virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	//virtual void UpdateCameraParams(const float delta_seconds) override;
protected:
	//virtual void PreDestroyActor(Actor* destroyee) override;
	//virtual void OnDestroyedActor(Actor* destroyed) override;
	// End SceneBase interface

private:
	class Actor* _actor;
	class AnimRendererComponent* _renderer;
	class BoxCollider* _actor_collider;
	class ColliderBase* _ground_collider;
	int _debug_screen_handle;
};