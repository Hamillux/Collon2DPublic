#include "InGameSceneState_GameOver.h"
#include "Actor/Character/Player/Player.h"
#include "GameSystems/Sound/SoundManager.h"
#include <DxLib.h>

namespace {
	constexpr float FADE_DURATION = 1.0f;

	constexpr float JINGLE_START_TIME = 0.75f;

	const std::string GetCauseOfDeathString(const CharacterDeathInfo& death_info)
	{
		switch (death_info.cause_of_death)
		{
		case ECauseOfCharacterDeath::ZERO_HP:
			return u8"力尽きた...";
		case ECauseOfCharacterDeath::FALL_FROM_STAGE:
			return u8"奈落の底へ落っこちた";
		case ECauseOfCharacterDeath::TIME_UP:
			return u8"時間切れだ";
		}
		return "ゲームオーバー";
	}
}

InGameSceneState_GameOver::InGameSceneState_GameOver()
	: _timer(0.0f)
	, _selected_button(BUTTON::BUTTON_RETRY)
	, _fade_start_time(FLT_MAX)
{
}

void InGameSceneState_GameOver::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	_timer = 0.f;
	_fade_start_time = FLT_MAX;
	_selected_button = BUTTON::BUTTON_RETRY;

	parent_scene._sound_instance_bgm->Stop();

	_sound_instance_jingle = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/jingle/ji_failed.ogg");
	_sound_instance_jingle->SetVolume(50);
	parent_scene.MakeDelayedEventWorld(&parent_scene, JINGLE_START_TIME, [this]() 
		{
			_sound_instance_jingle->Play(); 
		});

	parent_scene.GetPlayerRef()->player_events.OnPlayerDeathSequenceFinished += [this]() 
		{
			_fade_start_time = _timer;
		};
}

std::shared_ptr<SceneState<InGameSceneState_GameOver::ParentSceneClass>> InGameSceneState_GameOver::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	_timer += delta_seconds;

	return ret;
}

void InGameSceneState_GameOver::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);

	if (_timer < _fade_start_time)
	{
		return;
	}

	const float fade_end_time = _fade_start_time + FADE_DURATION;

	DrawBlendInfo blend_info;
	blend_info.dx_blend_mode = DX_BLENDMODE_ALPHA;
	if (_timer < fade_end_time)
	{
		blend_info.blend_value = static_cast<int>(255 * (_timer - _fade_start_time) / FADE_DURATION);
	}
	else
	{
		blend_info.blend_value = 255;
	}

	// 画面全体を黒くする
	BlendDrawHelper::DrawBox(blend_info, 0, 0, canvas_info.width, canvas_info.height, GetColor(0, 0, 0), TRUE);

	if (_timer >= fade_end_time)
	{
		constexpr int GAME_OVER_FONT_SIZE = 64;
		const int x = canvas_info.width / 2;
		const int y = canvas_info.height * 0.3;
		DrawStringHelper::DrawStringC(GAME_OVER_FONT_SIZE, x, y, _T("GAME OVER"), GetColor(255, 255, 255));

		// ゲーム―オーバー理由
		constexpr int CAUSE_OF_DEATH_FONT_SIZE = 24;
		const int cause_of_death_y = canvas_info.height * 0.4;
		const std::string cause_of_death = GetCauseOfDeathString(*_player_death_info);
		DrawStringHelper::DrawStringC(CAUSE_OF_DEATH_FONT_SIZE, x, cause_of_death_y, to_tstring(cause_of_death).c_str(), GetColor(255, 255, 255));

		// RETRY, RETURN TO STAGE SELECT, RETURN TO TITLE
		constexpr int BUTTON_FONT_SIZE = 32;
		const int buttons_middle_y = canvas_info.height * 0.6;
		constexpr int OFFSET = 64;
		const int selected_color = GetColor(255, 255, 255);
		const int unselected_color = GetColor(128, 128, 128);

		for (int i = 0; i < 3; ++i) 
		{
			const int color = (i == _selected_button) ? selected_color : unselected_color;

			switch (i)
			{
			case BUTTON_RETRY:
				DrawStringHelper::DrawStringC(BUTTON_FONT_SIZE, x, buttons_middle_y - OFFSET, _T("RETRY"), color);
				break;
			case BUTTON_RETURN_TO_STAGE_SELECT:
				DrawStringHelper::DrawStringC(BUTTON_FONT_SIZE, x, buttons_middle_y, _T("RETURN TO STAGE SELECT"), color);
				break;
			case BUTTON_RETURN_TO_TITLE:
				DrawStringHelper::DrawStringC(BUTTON_FONT_SIZE, x, buttons_middle_y + OFFSET, _T("RETURN TO TITLE"), color);
				break;
			}
		}
	}
}

void InGameSceneState_GameOver::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (_timer - FADE_DURATION < _fade_start_time)
	{
		return;
	}

	if (DeviceInput::IsPressed(KEY_INPUT_DOWN))
	{
		_selected_button = static_cast<BUTTON>((_selected_button + 1) % 3);
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_UP))
	{
		_selected_button = static_cast<BUTTON>((_selected_button + 2) % 3);
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_RETURN))
	{
		switch (_selected_button)
		{
		case BUTTON_RETRY:
			parent_scene.RetryStage();
			break;
		case BUTTON_RETURN_TO_STAGE_SELECT:
			parent_scene._destination_scene = SceneType::SELECT_SCENE;
			parent_scene.EndInGameScene();
			break;
		case BUTTON_RETURN_TO_TITLE:
			parent_scene._destination_scene = SceneType::TITLE_SCENE;
			parent_scene.EndInGameScene();
			break;
		}
	}
}

void InGameSceneState_GameOver::SetCauseOfDeath(const CharacterDeathInfo* death_info)
{
	_player_death_info = std::make_unique<const CharacterDeathInfo>(*death_info);
}
