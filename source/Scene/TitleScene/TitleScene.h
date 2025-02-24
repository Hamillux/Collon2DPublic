#pragma once

#include "Scene/SceneBase.h"
#include <tchar.h>
#include <stdint.h>

class TitleScene : public SceneBase
{
public:
	TitleScene();
	virtual ~TitleScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(float delta_seconds) override;
	virtual void Draw() override;
	virtual void DrawForeground(const CanvasInfo& canvas_info) override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::TITLE_SCENE; }
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const;
protected:
	virtual void UpdateCameraParams(const float delta_seconds) override;
	//~ End SceneBase interface

private:
	enum class TitleSceneState 
	{
		Default, InTransition, End
	};
	TitleSceneState _state;

	std::shared_ptr<SoundInstance> _si_bgm;

	float _fade_start_time;
};