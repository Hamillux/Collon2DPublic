#include "TestSelectScene.h"
#include <imgui.h>
#include "Input/DeviceInput.h"
#include "Scene/TestScene/TestScene.h"
#include "Scene/TestScene/TestSceneImpl/AllTestSceneImplInclude.h"
#include "Utility/ImGui/ImGuiInclude.h"

TestSelectScene::TestSelectScene()
	: _selected_test_id(1)
{
}

TestSelectScene::~TestSelectScene()
{
}

SceneType TestSelectScene::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);

	SceneType ret = GetSceneType();

	ImGui::SetNextWindowSize(ImVec2(600, 400));
	ImGui::SetNextWindowPos(ImVec2(WINDOW_SIZE_X, WINDOW_SIZE_Y) * 0.5f, 0, ImVec2(0.5, 0.5));
	if (ImGui::Begin("SelectTest"))
	{
		for(int i = 1; i <= NUM_TEST_SCENE_IMPL; i ++)
		{
			if (ImGui::Button(std::to_string(i).c_str(), ImVec2(64, 64)))
			{
				_selected_test_id = i;
				ret = SceneType::TEST_SCENE;
			}
		}
	}
	ImGui::End();

	if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		ret = SceneType::TITLE_SCENE;
	}

	return ret;
}

std::unique_ptr<const SceneBaseInitialParams> TestSelectScene::GetInitialParamsForNextScene(const SceneType next_scene) const
{
	std::unique_ptr<TestSceneInitialParams> ret = std::make_unique<TestSceneInitialParams>();
	ret->test_id = _selected_test_id;
	return std::move(ret);
}

SceneType TestSelectScene::GetSceneType() const
{
	return SceneType::TEST_SELECT_SCENE;
}
