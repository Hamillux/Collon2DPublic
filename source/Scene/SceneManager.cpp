#include "SceneManager.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <cassert>
#include "Input/DeviceInput.h"
#include "AllScenesInclude.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "GameSystems/SystemTimer.h"
#include "GameSystems/GameObjectManager.h"

SceneManager::SceneManager()
	: _current_scene(nullptr)
{
}

SceneManager::~SceneManager()
{}

void SceneManager::Initialize()
{
#if defined(_DEBUG) || defined(DEBUG)
	_debug_screen_handle = DxLib::MakeScreen(WINDOW_SIZE_X, WINDOW_SIZE_Y, TRUE);
#endif

	SystemTimer::GetInstance().Init();

	_draw_scene_screen_handle = DxLib::MakeScreen(WINDOW_SIZE_X, WINDOW_SIZE_Y, TRUE);

	auto scene_params = std::make_unique<const SceneBaseInitialParams>(SceneType::NONE);
	ChangeScene(SceneType::TITLE_SCENE, scene_params);
}

SceneManagerMessage SceneManager::Tick(float DeltaSeconds)
{
	SystemTimer::GetInstance().Update();

	NewImGuiFrame();

	// シーンの更新
	static std::deque<float> delta_seconds_buffer;
	delta_seconds_buffer.push_back(DeltaSeconds);
	if (delta_seconds_buffer.size() > 60)
	{
		delta_seconds_buffer.pop_front();
	}
	const float average_delta_seconds =
		std::accumulate(delta_seconds_buffer.begin(), delta_seconds_buffer.end(), 0.f) / delta_seconds_buffer.size();

	SceneType next_scene_type = _current_scene->ExecuteTick(DeltaSeconds * _current_scene->GetGameSpeed());
	SceneType current_scene_type = _current_scene->GetSceneType();
	if (next_scene_type == SceneType::MSG_RELOAD)
	{
		std::unique_ptr<const SceneBaseInitialParams> scene_params
			= _current_scene->GetInitialParamsForNextScene(current_scene_type);
		ChangeScene(current_scene_type, scene_params);

		ImGui::EndFrame();
		return SceneManagerMessage::CHANGED_SCENE;
	}
	else if (next_scene_type != current_scene_type)
	{
		std::unique_ptr<const SceneBaseInitialParams> scene_params 
			= _current_scene->GetInitialParamsForNextScene(next_scene_type);
		ChangeScene(next_scene_type, scene_params);

		ImGui::EndFrame();
		return SceneManagerMessage::CHANGED_SCENE;
	}

	// タイトルシーンでESCを押すとゲーム終了
	if (_current_scene->GetSceneType() == SceneType::TITLE_SCENE && DeviceInput::IsActive(KEY_INPUT_ESCAPE))
	{
		ImGui::EndFrame();
		return SceneManagerMessage::QUITTED_GAME;
	}

	// 描画
	Draw();

	return SceneManagerMessage::CONTINUE;
}

void SceneManager::Finalize()
{
	// CurrentSceneの解放
	if (_current_scene != nullptr)
	{
		ImGui::NewFrame();

		_current_scene->Finalize();
		delete _current_scene;
		_current_scene = nullptr;

		ImGui::EndFrame();
	}
	
	DxLib::DeleteGraph(_draw_scene_screen_handle);

	SystemTimer::GetInstance().Finalize();
}

void SceneManager::NewImGuiFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void SceneManager::Draw()
{
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();

	// シーンの描画
	_current_scene->ExecuteDrawProcess();

	RefreshDxLibDirect3DSetting();

	// ImGui
	{
		// Rendering
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	// 画面を更新
	ScreenFlip();
}

void SceneManager::DrawLoadingScreen() const
{
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	const int saved_font_size = GetFontSize();

	// 黒背景に白文字"Now Loading..."の素朴なロード画面
	SetFontSize(64);
	DrawString(WINDOW_SIZE_X - 500, WINDOW_SIZE_Y - 100, _T("Now Loading..."), 0xFFFFFF);

	SetFontSize(saved_font_size);
	ScreenFlip();
}

void SceneManager::ChangeScene(SceneType next_scene_type, std::unique_ptr<const SceneBaseInitialParams>& scene_params)
{
	// シーンの生成
	SceneBase* new_scene = CreateScene(next_scene_type);
	assert(new_scene != nullptr);

	// 現在のシーンの解放
	// 最初のシーン生成時のみCurrentSceneがnullptr
	if (_current_scene != nullptr)
	{
		_current_scene->Finalize();
		delete _current_scene;
	}

	const int load_start_time = GetNowCount();

	DrawLoadingScreen();

	// 新しいシーンの開始
	new_scene->Initialize(scene_params.get());
	_current_scene = new_scene;

	DeviceInput::ResetAll();

	if (GetNowCount() - load_start_time < MIN_LOADING_TIME_MS)
	{
		Sleep(MIN_LOADING_TIME_MS - (GetNowCount() - load_start_time));
	}
}

SceneBase* SceneManager::CreateScene(SceneType new_scene_type)
{
	switch (new_scene_type)
	{
		case SceneType::INGAME_SCENE:
			return GameObjectManager::GetInstance().CreateObject<InGameScene>();
		case SceneType::SELECT_SCENE:
			return GameObjectManager::GetInstance().CreateObject<StageSelectScene>();
		case SceneType::TEST_SCENE:
			return GameObjectManager::GetInstance().CreateObject<TestScene>();
		case SceneType::TEST_SELECT_SCENE:
			return GameObjectManager::GetInstance().CreateObject<TestSelectScene>();
		case SceneType::TITLE_SCENE:
			return GameObjectManager::GetInstance().CreateObject<TitleScene>();
		case SceneType::EDITOR_SCENE:
			return GameObjectManager::GetInstance().CreateObject<StageEditorScene>();
		default:
			throw std::runtime_error("Unknown SceneType");
			return nullptr;
	}
}