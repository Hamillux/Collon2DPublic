#include "TestScene.h"
#include "Scene/AllScenesInclude.h"
#include "Input/DeviceInput.h"
#include <imgui.h>
#include "Scene/TestScene/TestSceneImpl/AllTestSceneImplInclude.h"
#include "Utility/ImGui/ImGuiInclude.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/IEditableParameter.h"
#include "Scene/TestScene/TestSceneImpl/TestSceneImplBase.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "Component/EmitterComponent.h"
#include "Actor/AllActorsInclude_generated.h"

static nlohmann::json particle_json;
constexpr MasterDataID PARTICLE_ID = 1;
TestScene::TestScene()
{
}
TestScene::~TestScene()
{
}
void TestScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	const TestSceneInitialParams* test_scene_params = dynamic_cast<const TestSceneInitialParams*>(scene_params);

	CreateTestSceneImpl(test_scene_params->test_id);

	SceneBaseInitialParams dummy = {};
	_test_scene_impl->Initialize(&dummy);
}

SceneType TestScene::Tick(float delta_seconds)
{
	SceneType ret = _test_scene_impl->Tick(delta_seconds);
	if (ret == _test_scene_impl->GetSceneType())
	{
		return GetSceneType();
	}
	
	return ret;
}

void TestScene::Draw()
{
	_test_scene_impl->Draw();
}

void TestScene::DrawForeground(const CanvasInfo& canvas_info)
{
	_test_scene_impl->DrawForeground(canvas_info);
}

void TestScene::Finalize()
{
	_test_scene_impl->Finalize();
	__super::Finalize();
}

void TestScene::UpdateCameraParams(const float delta_seconds)
{
	_test_scene_impl->UpdateCameraParams(delta_seconds);
}

std::unique_ptr<const SceneBaseInitialParams> TestScene::GetInitialParamsForNextScene(const SceneType next_scene) const
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

void TestScene::PreDestroyActor(Actor* destroyee)
{
	_test_scene_impl->PreDestroyActor(destroyee);
}

void TestScene::OnDestroyedActor(Actor* destroyed)
{
	_test_scene_impl->OnDestroyedActor(destroyed);
}


#define SWITCH_CASE(N) case N: _test_scene_impl = std::make_unique<TestSceneImpl_##N>(); break

void TestScene::CreateTestSceneImpl(const int test_id)
{
	switch (test_id)
	{
		SWITCH_CASE(1);
		SWITCH_CASE(2);
		SWITCH_CASE(3);
		SWITCH_CASE(4);
		// TODO: TestSceneImpl_Nを追加した場合、ここに追記
	default:
		throw std::runtime_error("Unknown test id");
	}
}
