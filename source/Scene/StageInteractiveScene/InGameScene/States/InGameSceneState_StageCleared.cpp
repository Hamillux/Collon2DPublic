#include "InGameSceneState_StageCleared.h"
#include "Actor/Character/Player/Player.h"
#include "GameSystems/Sound/SoundManager.h"

namespace
{
	constexpr float JINGLE_START_TIME = 0.75f;
}

InGameSceneState_StageCleared::InGameSceneState_StageCleared()
	: _is_player_goal_sequence_finished(false)
	, _selected_button(0)
	, _font_handle_buttons(INVALID_MASTER_ID)
	, _font_handle_score(INVALID_MASTER_ID)
	, _font_handle_stage_clear(INVALID_MASTER_ID)
{
}

void InGameSceneState_StageCleared::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	_is_player_goal_sequence_finished = false;

	_font_handle_stage_clear = DxLib::CreateFontToHandle(NULL, 48, -1, DX_FONTTYPE_NORMAL);
	_font_handle_score = DxLib::CreateFontToHandle(NULL, 32, -1, DX_FONTTYPE_NORMAL);
	_font_handle_buttons = DxLib::CreateFontToHandle(NULL, 24, -1, DX_FONTTYPE_NORMAL);

	parent_scene._sound_instance_bgm->Stop();

	_sound_instance_jingle = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/jingle/ji_goaled.ogg");
	_sound_instance_jingle->SetVolume(50);
	parent_scene.MakeDelayedEventWorld(&parent_scene, JINGLE_START_TIME, [this]()
		{
			_sound_instance_jingle->Play();
		});

	parent_scene.GetPlayerRef()->player_events.OnPlayerGoalSequenceFinished.Bind(
		[this, &parent_scene]()
		{
			OnPlayerGoalSequenceFinished(parent_scene);
		}
	);
}

void InGameSceneState_StageCleared::OnLeaveState(ParentSceneClass& parent_scene)
{
	__super::OnLeaveState(parent_scene);
}

void InGameSceneState_StageCleared::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	if (!_is_player_goal_sequence_finished)
	{
		return;
	}

	__super::DrawForeground(parent_scene, canvas_info);
	const float result_overlay_width = canvas_info.width * 0.9f;
	const float result_overlay_height = canvas_info.height * 0.9f;
	const float overlay_color = GetColor(0, 0, 0);
	constexpr float overlay_alpha = 192;

	DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, overlay_alpha);
	DxLib::DrawBox(
		(canvas_info.width - result_overlay_width) / 2,
		(canvas_info.height - result_overlay_height) / 2,
		(canvas_info.width + result_overlay_width) / 2,
		(canvas_info.height + result_overlay_height) / 2,
		overlay_color,
		TRUE
	);
	DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// "STAGE CLEAR"
	{
		const tstring text_stage_clear = to_tstring(u8"STAGE CLEAR");
		const int str_width_stage_clear 
			= DxLib::GetDrawStringWidthToHandle(text_stage_clear.c_str(), strlenDx(text_stage_clear.c_str()), _font_handle_stage_clear);
		const float stage_clear_y = canvas_info.height * 0.5f - result_overlay_height * 0.3f;
		DxLib::DrawStringToHandle(
			canvas_info.width / 2 - str_width_stage_clear / 2,
			stage_clear_y,
			text_stage_clear.c_str(),
			GetColor(255, 255, 255),
			_font_handle_stage_clear
		);
	}

	// "SCORE: XXX...(14桁)"
	// NOTE: 表示は14桁だが, スコアはintなので実際の最大桁数は10桁
	const float score_y = canvas_info.height * 0.5f - result_overlay_height * 0.1f;
	{
		const int score = parent_scene.GetScore();
		const int additional_score = CalculateAdditionalScore(parent_scene);
		const int total_score = score + additional_score;

		const tstring text_score = tstring(_T("SCORE: ")) + to_tstring(total_score, _T("%014d"));
		const int score_text_width = GetDrawStringWidthToHandle(text_score.c_str(), strlenDx(text_score.c_str()), _font_handle_score);

		DrawFormatStringToHandle(
			canvas_info.width / 2 - score_text_width / 2,
			score_y,
			GetColor(255, 255, 255),
			_font_handle_score,
			text_score.c_str(),
			total_score);
	}

	// "CREAR TIME: XX:XX.XX"
	{
		const float clear_time = parent_scene.GetStageRef().GetTimeLimit() - parent_scene._remaining_time;
		const int clear_time_minutes = static_cast<int>(clear_time) / 60;
		const int clear_time_seconds = static_cast<int>(clear_time) % 60;
		const int clear_time_milliseconds = static_cast<int>((clear_time - static_cast<int>(clear_time)) * 100);  // 小数点以下2桁まで

		const tstring text_clear_time = 
			tstring(_T("CLEAR TIME: ")) + 
			to_tstring(clear_time_minutes, _T("%02d")) +
			_T(":") +
			to_tstring(clear_time_seconds, _T("%02d")) +
			_T(".") +
			to_tstring(clear_time_milliseconds, _T("%02d"));
		const float clear_time_y = score_y + 60;
		const int clear_time_text_width
			= DxLib::GetDrawStringWidthToHandle(text_clear_time.c_str(), strlenDx(text_clear_time.c_str()), _font_handle_score);

		DrawFormatStringToHandle(
			canvas_info.width / 2 - clear_time_text_width / 2,
			clear_time_y,
			GetColor(255, 255, 255),
			_font_handle_score,
			text_clear_time.c_str(),
			clear_time_minutes, clear_time_seconds, clear_time_milliseconds
		);
	}
	
	// "RETRY", "RETURN TO STAGE SELECT", "RETURN TO TITLE"
	{
		constexpr const TCHAR* button_texts[] 
			= { _T("RETRY"), _T("RETURN TO STAGE SELECT"), _T("RETURN TO TITLE") };
		constexpr int font_size_buttons = 24;
		const float buttons_y = canvas_info.height * 0.5f + result_overlay_height * 0.3f;
		const int button_color_selected = GetColor(255, 255, 255);
		const int button_color_unselected = GetColor(128, 128, 128);
		for (size_t i = 0; i < 3; i++)
		{
			const int font_color = (i == _selected_button) ? button_color_selected : button_color_unselected;
			const int cx = canvas_info.width / 2;
			const int cy = buttons_y + i * 40;
			const int text_width = DxLib::GetDrawStringWidthToHandle(button_texts[i], strlenDx(button_texts[i]), _font_handle_buttons);
			DxLib::DrawStringToHandle(
				cx - text_width / 2,
				cy,
				button_texts[i],
				font_color,
				_font_handle_buttons
			);
		}
	}		
}

void InGameSceneState_StageCleared::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	if (!_is_player_goal_sequence_finished)
	{
		return;
	}

	if (DeviceInput::IsPressed(KEY_INPUT_DOWN))
	{
		_selected_button = (_selected_button + 1) % 3;
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_UP)) 
	{
		_selected_button = (_selected_button - 1 + 3) % 3;
	}
	else if (DeviceInput::IsPressed(KEY_INPUT_RETURN))
	{
		switch (_selected_button)
		{
		case 0:
			parent_scene.RetryStage();
			break;
		case 1:
			parent_scene._destination_scene = SceneType::SELECT_SCENE;
			parent_scene.EndInGameScene();
			break;
		case 2:
			parent_scene._destination_scene = SceneType::TITLE_SCENE;
			parent_scene.EndInGameScene();
			break;
		}
	}
}

void InGameSceneState_StageCleared::OnPlayerGoalSequenceFinished(ParentSceneClass& parent_scene)
{
	_is_player_goal_sequence_finished = true;
}

int InGameSceneState_StageCleared::CalculateAdditionalScore(ParentSceneClass& parent_scene) const
{
	constexpr int SCORE_PER_SECOND = 100;

	int additional_score = 0;

	additional_score += SCORE_PER_SECOND * static_cast<int>(parent_scene._remaining_time);

	return additional_score;
}
