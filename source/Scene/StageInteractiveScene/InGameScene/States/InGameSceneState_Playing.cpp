#include "Scene/StageInteractiveScene/InGameScene/InGameSceneStatesInclude.h"
#include "Input/DeviceInput.h"
#include "Actor/Character/Player/Player.h"

namespace
{
	constexpr float RESET_TIME = 2.f;
	constexpr MasterDataID SPINNER_ICONS_ID_BEGIN = 3;
	constexpr MasterDataID SPINNER_ICONS_ID_NUM = 9;
}

InGameSceneState_Playing::InGameSceneState_Playing()
	: _reset_timer(0.f)
{
}

void InGameSceneState_Playing::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	_spinner_icons.clear();
	for (MasterDataID i = SPINNER_ICONS_ID_BEGIN; i < SPINNER_ICONS_ID_BEGIN + SPINNER_ICONS_ID_NUM; ++i)
	{
		_spinner_icons.push_back(MasterHelper::GetGameIconHandleForDxLib(i));
	}

	parent_scene.GetPlayerRef()->character_events.OnDead.Bind(
			[&](const CharacterDeathInfo* death_info) 
			{
				OnPlayerDead(parent_scene, death_info); 
			},
			this
		);

	parent_scene.GetPlayerRef()->player_events.OnPlayerReachedGoal.Bind(
			[&]()
			{
				OnPlayerReachedGoal(parent_scene); 
			},
			this
		);
}

void InGameSceneState_Playing::OnLeaveState(ParentSceneClass& parent_scene)
{
	for (auto& handle : _spinner_icons)
	{
		DeleteGraph(handle);
	}

	Player* player = parent_scene.GetPlayerRef();
	if (parent_scene.IsValid(player))
	{
		player->character_events.OnDead.UnBind(this);
		player->player_events.OnPlayerReachedGoal.UnBind(this);
	}
	__super::OnLeaveState(parent_scene);
}

std::shared_ptr<SceneState<InGameSceneState_Playing::ParentSceneClass>> InGameSceneState_Playing::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	auto ret = __super::Tick(parent_scene, delta_seconds);

	if (!parent_scene._is_timer_stopped)
	{
		parent_scene._remaining_time -= delta_seconds;
		if (parent_scene._remaining_time <= 0.f)
		{
			parent_scene._remaining_time = 0.f;
			OnTimeUp(parent_scene);
		}
		else if (!parent_scene._has_hurried_player && parent_scene._remaining_time <= parent_scene.HURRY_TIME) 
		{
			parent_scene.HurryPlayer();
		}
	}
	
	return ret;
}

void InGameSceneState_Playing::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);

	// 残り時間
	TCHAR time_string[32];
	_stprintf_s(time_string, _T("%03.f"), parent_scene._remaining_time);
	constexpr const int clock_size = 20;
	constexpr const int clock_left = WINDOW_SIZE_X - 60;
	constexpr const int clock_top = 10;
	DrawString(clock_left + clock_size + 3, clock_top, time_string, 0xFFFFFF);

	// ステージ名
	const tstring stage_name = to_tstring(parent_scene.GetStageRef().GetStageName());
	const int stage_name_width = GetDrawStringWidth(stage_name.c_str(), strlenDx(stage_name.c_str()));
	const int stage_name_left = clock_left - (stage_name_width + 10);
	constexpr const int stage_name_top = clock_top;
	DrawString(stage_name_left, stage_name_top, stage_name.c_str(), 0xFFFFFF);

	//スコア
	constexpr const int score_left = WINDOW_SIZE_X - 213;
	constexpr const int score_top = 32;
	SetFontSize(24);
	// 14桁で0埋め
	DrawFormatString(score_left, score_top, 0xFFFFFF, _T("%014d"), parent_scene._total_score);
	SetFontSize(DEFAULT_FONT_SIZE);

	if (_reset_timer > 0.f)
	{
		// 画面全体を暗くする
		BlendDrawHelper::DrawBox(DrawBlendInfo(DX_BLENDMODE_ALPHA, 128), 0, 0, canvas_info.width, canvas_info.height, GetColor(0, 0, 0), TRUE);

		// 「このステージをやり直します...」
		const tstring text = to_tstring(u8"このステージをやり直します...");
		const int text_width = GetDrawStringWidth(text.c_str(), strlenDx(text.c_str()));
		const int text_left = canvas_info.width / 2 - text_width / 2;
		const int text_top = canvas_info.height/ 2 - 50;
		constexpr const int text_color = 0xFFFFFF;
		DrawString(text_left, text_top, text.c_str(), text_color);

		// スピナーアイコン
		const float progress = _reset_timer / RESET_TIME;
		const int spinner_icon_index = static_cast<int>(progress * (_spinner_icons.size() - 1));
		const int spinner_icon_handle = _spinner_icons.at(spinner_icon_index);
		const int spinner_icon_size = 64;
		DrawRotaGraph(canvas_info.width / 2, canvas_info.height/ 2, 1.0, 0.0, spinner_icon_handle, TRUE);
	}
}

void InGameSceneState_Playing::UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
{
	const Vector2D delta_player_position = parent_scene.GetPlayerRef()->GetDeltaPosition();
	const Vector2D player_world_pos = parent_scene.GetPlayerRef()->GetActorWorldPosition();

	CameraParams& camera_params = parent_scene._camera_params;

	// X方向
	{
		// screen_world_offsetはX方向にプレイヤーの1.3倍の速度で移動する
		camera_params.world_offset.x += delta_player_position.x * 1.5f;

		const Vector2D player_pos_screen = 
			camera_params.TransformPosition_WorldToScreen(player_world_pos);
		const float player_screen_x = player_pos_screen.x;
		const float player_screen_y = player_pos_screen.y;
		constexpr float left_limit = -1 + 2.f / 3.f;
		constexpr float right_limit = 1 - 2.f / 3.f;

		if (player_screen_x < left_limit)
		{
			camera_params.ChangeWorldOffset_World_Screen(player_world_pos, Vector2D{ left_limit, player_screen_y });
		}
		else if (player_screen_x > right_limit)
		{
			camera_params.ChangeWorldOffset_World_Screen(player_world_pos, Vector2D{ right_limit, player_screen_y });
		}
	}

	// Y方向
	{
		constexpr float TOP_LIMIT = -1 + 1.f / 3.f;
		constexpr float BOTTOM_LIMIT = 0;
		const Vector2D player_pos_screen = camera_params.TransformPosition_WorldToScreen(parent_scene.GetPlayerRef()->GetActorWorldPosition());
		const float player_screen_x = player_pos_screen.x;
		const float player_screen_y = player_pos_screen.y;
		if (player_screen_y < TOP_LIMIT)
		{
			camera_params.ChangeWorldOffset_World_Screen(player_world_pos, Vector2D{ player_screen_x, TOP_LIMIT});
		}
		else if (player_screen_y > BOTTOM_LIMIT)
		{
			camera_params.ChangeWorldOffset_World_Screen(player_world_pos, Vector2D{ player_screen_x, BOTTOM_LIMIT });
		}
	}
}

void InGameSceneState_Playing::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	// ポーズボタンが押されたらポーズ状態に遷移する
	if(DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
	{
		SetNextState(std::make_shared<InGameSceneState_Paused>());
	}

	if (DeviceInput::IsActive(KEY_INPUT_R))
	{
		_reset_timer += delta_seconds;
		if (_reset_timer >= RESET_TIME)
		{
			parent_scene.RetryStage();
		}
	}
	else
	{
		_reset_timer = 0.f;
	}
}

void InGameSceneState_Playing::OnPlayerDead(ParentSceneClass& parent_scene, const CharacterDeathInfo* death_info)
{
	auto gameover_state = std::make_shared<InGameSceneState_GameOver>();
	gameover_state->SetCauseOfDeath(death_info);
	SetNextState(gameover_state);
}

void InGameSceneState_Playing::OnPlayerReachedGoal(ParentSceneClass& parent_scene)
{
	auto stage_cleared_state = std::make_shared<InGameSceneState_StageCleared>();
	SetNextState(stage_cleared_state);
}

void InGameSceneState_Playing::OnTimeUp(ParentSceneClass& parent_scene)
{
	CharacterDeathInfo death_info{};
	death_info.cause_of_death = ECauseOfCharacterDeath::TIME_UP;
	parent_scene.GetPlayerRef()->KillCharacter(&death_info);
}
