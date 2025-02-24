#include "TestSceneImplBase.h"
#include "Scene/AllScenesInclude.h"

TestSceneImplBase::TestSceneImplBase()
{
}

TestSceneImplBase::~TestSceneImplBase()
{
}

std::unique_ptr<const SceneBaseInitialParams> TestSceneImplBase::GetInitialParamsForNextScene(const SceneType next_scene) const
{
	switch (next_scene)
	{
	case SceneType::TITLE_SCENE:
	{
		auto params = std::make_unique<SceneBase::traits<TitleScene>::initial_params_type>();
		params->prev_scene_type = GetSceneType();
		return params;
	}
	}

	throw std::runtime_error("Undefined scene transition");
}
