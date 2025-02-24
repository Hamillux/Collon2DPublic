#include "TitleScene.h"
#include "Scene/AllScenesInclude.h"
#include "Input/DeviceInput.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "GameSystems/Sound/SoundManager.h"

namespace
{
	constexpr float FADE_TIME = 0.5f;
	constexpr int BGM_VOLUME = 50;
}

TitleScene::TitleScene()
	: _state(TitleSceneState::Default)
	, _si_bgm(nullptr)
	, _fade_start_time(0.f)
{}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(const SceneBaseInitialParams* const scene_params)
{
	__super::Initialize(scene_params);

	SoundManager& sound_manager = SoundManager::GetInstance();

	_si_bgm = sound_manager.MakeSoundInstance("resources/sounds/bgm/bgm_title_scene.ogg");
	_si_bgm->SetLoopEnabled(true);
	_si_bgm->SetVolume(BGM_VOLUME);
	_si_bgm->Play();

	GraphicResourceManager& graph_manager = GraphicResourceManager::GetInstance();

	std::vector<BackgroundParams> title_bgs = {
		BackgroundParams(graph_manager.GetGraphForDxLib(MasterDataID(55)), 100.f),
		BackgroundParams(graph_manager.GetGraphForDxLib(MasterDataID(56)), 50.f),
		BackgroundParams(graph_manager.GetGraphForDxLib(MasterDataID(57)), 25.f)
	};

	for (auto bg : title_bgs)
	{
		AddBackgroundToLayer(bg);
	}
}

SceneType TitleScene::Tick(float delta_seconds)
{
	__super::Tick(delta_seconds);

	if (_state == TitleSceneState::Default && DeviceInput::IsPressed(KEY_INPUT_RETURN))
	{
		const int delta_volume = 10;
		const float interval = FADE_TIME / (_si_bgm->GetVolume() / delta_volume);

		MakeRepeatingEventWorld(this, interval, [this]() 
			{
				_si_bgm->SetVolume(_si_bgm->GetVolume() - delta_volume);
				const int current_volume = _si_bgm->GetVolume();
				if (_si_bgm->GetVolume() == 0)
				{
					_state = TitleSceneState::End;
					return false;
				}

				return true;
			}
		);

		_fade_start_time = GetWorldTime();
		_state = TitleSceneState::InTransition;
	}

	if (_state == TitleSceneState::End)
	{
		return SceneType::SELECT_SCENE;
	}

#ifdef _DEBUG
	if (DeviceInput::IsActive(KEY_INPUT_T))
	{
		return SceneType::TEST_SELECT_SCENE;
	}
#endif

	return GetSceneType();
}

void TitleScene::Draw()
{
	__super::Draw();

	if (_state == TitleSceneState::Default)
	{
		const tstring text_title = to_tstring(u8"コロンのアクション");
		constexpr uint8_t TITLE_FONT_SIZE = 50;
		const int title_width = DrawStringHelper::GetDrawStringWidth(TITLE_FONT_SIZE, text_title.c_str(), strlenDx(text_title.c_str()));
		const int x = WINDOW_SIZE_X / 2 - title_width / 2;
		DrawStringHelper::DrawString(TITLE_FONT_SIZE, x, 100, text_title.c_str(), 0xFFFFFF);

		SetFontSize(32);
		const tstring text_press_enter = to_tstring(u8"Press Enter");
		int str_width = GetDrawStringWidth(text_press_enter.c_str(), strlenDx(text_press_enter.c_str()));
		DrawString(WINDOW_SIZE_X / 2 - str_width / 2, WINDOW_SIZE_Y - 128, text_press_enter.c_str(), 0xFFFFFF);
		SetFontSize(DEFAULT_FONT_SIZE);
	}
}

void TitleScene::DrawForeground(const CanvasInfo& canvas_info)
{
	__super::DrawForeground(canvas_info);

	if (_state == TitleSceneState::InTransition || _state == TitleSceneState::End)
	{
		const int alpha = static_cast<int>(clamp(255.f * (GetWorldTime() - _fade_start_time) / FADE_TIME, 0.f, 255.f));
		BlendDrawHelper::DrawBox(DrawBlendInfo(DX_BLENDMODE_ALPHA, alpha), 0, 0, canvas_info.width, canvas_info.height, 0x0, TRUE);
	}

}

void TitleScene::Finalize()
{
	_state = TitleSceneState::Default;
	_si_bgm.reset();

	__super::Finalize();
}

std::unique_ptr<const SceneBaseInitialParams> TitleScene::GetInitialParamsForNextScene(const SceneType next_scene) const
{
	switch (next_scene)
	{
	case SceneType::SELECT_SCENE:
	{
		auto params = std::make_unique<SceneBase::traits<StageSelectScene>::initial_params_type>();
		params->prev_scene_type = GetSceneType();
		params->next_scene = SceneType::INGAME_SCENE;
		return params;
	}
	case SceneType::TEST_SELECT_SCENE:
	{
		auto params = std::make_unique<SceneBase::traits<TestScene>::initial_params_type>();
		params->prev_scene_type = GetSceneType();
		return params;
	}
	}

	throw std::runtime_error("Undefined scene transition");
}

void TitleScene::UpdateCameraParams(const float delta_second)
{
	_camera_params.world_offset = _camera_params.world_offset + Vector2D(WINDOW_SIZE_X, 0) * delta_second;
}
