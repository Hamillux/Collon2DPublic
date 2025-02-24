#include "StageEditorSceneState_ThumbnailShooting.h"
#include "Input/DeviceInput.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"


namespace {
	constexpr float SHUTTER_TIME = 0.1f;

	constexpr float PREVIEW_WIDTH = WINDOW_SIZE_X * 0.6f; // 0.4
	constexpr float LEFT_PREVIEW_X = WINDOW_SIZE_X * 0.25f;
	constexpr float RIGHT_PREVIEW_X = WINDOW_SIZE_X * 0.5f; // 0.75
	constexpr float PREVIEW_Y = WINDOW_SIZE_Y * 0.5f;

	constexpr MasterDataID ICON_ID_ESC = 158;
	constexpr MasterDataID ICON_ID_R = 240;
	constexpr MasterDataID ICON_ID_ENTER = 242;
}

StageEditorSceneState_ThumbnailShooting::StageEditorSceneState_ThumbnailShooting()
	: _current_substate(ThunmailShootingSubState::Default)
	, _shuttering_phase(0.f)
	, _shuttering_speed(0.f)
	, _should_show_grid(true)
{
}

StageEditorSceneState_ThumbnailShooting::~StageEditorSceneState_ThumbnailShooting()
{
}

void StageEditorSceneState_ThumbnailShooting::OnEnterState(ParentSceneClass& parent_scene)
{
	__super::OnEnterState(parent_scene);

	// スクリーンの拡大率を1.0に設定
	parent_scene._camera_params.ChangeScale(1.f, GetWindowHalfSize());
}

std::shared_ptr<SceneState<StageEditorSceneState_ThumbnailShooting::ParentSceneClass>>
	StageEditorSceneState_ThumbnailShooting::Tick(ParentSceneClass& parent_scene, float delta_seconds)
{
	std::shared_ptr<SceneState<ParentSceneClass>> ret = __super::Tick(parent_scene, delta_seconds);
	
	switch (_current_substate)
	{
	case ThunmailShootingSubState::Default:
	{}
	break;

	case ThunmailShootingSubState::ReleasingShutter:
	{
		_shuttering_phase += _shuttering_speed * delta_seconds;
		if (_shuttering_phase >= 1.f && _shuttering_speed > 0.f)
		{
			_shuttering_phase = 1.f;
			_shuttering_speed *= -1.f;
			_capture_result = parent_scene.CaptureScene();
		}
		else if (_shuttering_phase <= 0.f && _shuttering_speed < 0.f)
		{
			parent_scene._should_show_controls = false;
			ChangeSubState(ThunmailShootingSubState::Confirm);
		}
	}
	break;

	case ThunmailShootingSubState::Confirm:
	{
	}
	break;

	}

	return ret;
}

void StageEditorSceneState_ThumbnailShooting::Draw(ParentSceneClass& parent_scene)
{
	__super::Draw(parent_scene);
}

void StageEditorSceneState_ThumbnailShooting::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	switch (_current_substate)
	{

	case ThunmailShootingSubState::Default:
	{
		// 縦・横それぞれを3分割するグリッド
		if(_should_show_grid)
		{
			constexpr int LINE_THICKNESS = 3;
			constexpr int GRID_DIVISION = 3;

			const int grid_width = WINDOW_SIZE_X / GRID_DIVISION;
			const int grid_height = WINDOW_SIZE_Y / GRID_DIVISION;

			for (int i = 1; i < GRID_DIVISION; i++)
			{
				// 横線
				DrawLine(0, grid_height * i, WINDOW_SIZE_X, grid_height * i, GetColor(255, 255, 255), LINE_THICKNESS);

				// 縦線
				DrawLine(grid_width * i, 0, grid_width * i, WINDOW_SIZE_Y, GetColor(255, 255, 255), LINE_THICKNESS);
			}
		}
	}
	break;

	case ThunmailShootingSubState::ReleasingShutter:
	{
		// シャッター開閉アニメーション
		const float black_height = WINDOW_SIZE_Y * 0.5f * _shuttering_phase;
		DrawBox(0, 0, WINDOW_SIZE_X, black_height, GetColor(0, 0, 0), TRUE);
		DrawBox(0, WINDOW_SIZE_Y - black_height, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), TRUE);
	}
	break;

	case ThunmailShootingSubState::Confirm:
	{
		// 撮影結果確認画面
		const float width_per_height = _capture_result->GetWidthPerHeight();
		const float preview_height = PREVIEW_WIDTH / width_per_height;

		// 背景
		DrawBox(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, GetColor(0, 0, 0), TRUE);

		// 「ステージのサムネイルを更新しますか？」
		// 「この操作は取り消すことができません」
		{
			constexpr int FONT_SIZE = 40;
			constexpr float TEXT_X = WINDOW_SIZE_X * 0.5f;
			constexpr float TEXT_Y = WINDOW_SIZE_Y * 0.1f;
			DrawStringHelper::DrawStringC(
				FONT_SIZE,
				TEXT_X,
				TEXT_Y,
				to_tstring(u8"ステージのサムネイルを更新しますか？").c_str(),
				GetColor(255, 255, 255)
			);
			DrawStringHelper::DrawStringC(
				FONT_SIZE,
				TEXT_X,
				TEXT_Y + FONT_SIZE,
				to_tstring(u8"※更新後に元に戻すことはできません").c_str(),
				GetColor(255, 255, 255)
			);
		}

		// 撮影結果
		const int capture_result_handle = _capture_result->GetHandle();
		DrawExtendGraphF(
			RIGHT_PREVIEW_X - PREVIEW_WIDTH * 0.5f,
			PREVIEW_Y - preview_height * 0.5f,
			RIGHT_PREVIEW_X + PREVIEW_WIDTH * 0.5f,
			PREVIEW_Y + preview_height * 0.5f,
			capture_result_handle,
			FALSE
		);

		// 横並びの操作キーアイコンと説明を縦に3つ表示
		// Esc: キャンセル
		// R: 撮り直す
		// Enter: サムネイルを更新
		{
			constexpr int FONT_SIZE = 32;
			constexpr float ICON_SIZE = FONT_SIZE;
			constexpr float Y_OFFSET = ICON_SIZE;
			constexpr float X_OFFSET = 32;
			constexpr float ICON_X = WINDOW_SIZE_X * 0.5f - 150.f;
			constexpr float ICON_Y = WINDOW_SIZE_Y * 0.85f;

			const int ghandle_esc = MasterHelper::GetGameIconHandleForDxLib(ICON_ID_ESC);
			const int ghandle_r = MasterHelper::GetGameIconHandleForDxLib(ICON_ID_R);
			const int ghandle_enter = MasterHelper::GetGameIconHandleForDxLib(ICON_ID_ENTER);

			DrawExtendGraphF(
				ICON_X,
				ICON_Y,
				ICON_X + ICON_SIZE,
				ICON_Y + ICON_SIZE,
				ghandle_esc,
				TRUE
			);
			DrawStringHelper::DrawString(
				FONT_SIZE,
				ICON_X + ICON_SIZE + X_OFFSET,
				ICON_Y,
				to_tstring(u8"キャンセル").c_str(),
				GetColor(255, 255, 255)
			);

			DrawExtendGraphF(
				ICON_X,
				ICON_Y + Y_OFFSET,
				ICON_X + ICON_SIZE,
				ICON_Y + ICON_SIZE + Y_OFFSET,
				ghandle_r,
				TRUE
			);
			DrawStringHelper::DrawString(
				FONT_SIZE,
				ICON_X + ICON_SIZE + X_OFFSET,
				ICON_Y + Y_OFFSET,
				to_tstring(u8"撮り直す").c_str(),
				GetColor(255, 255, 255)
			);

			DrawExtendGraphF(
				ICON_X,
				ICON_Y + Y_OFFSET * 2,
				ICON_X + ICON_SIZE,
				ICON_Y + ICON_SIZE + Y_OFFSET * 2,
				ghandle_enter,
				TRUE
			);
			DrawStringHelper::DrawString(
				FONT_SIZE,
				ICON_X + ICON_SIZE + X_OFFSET,
				ICON_Y + Y_OFFSET * 2,
				to_tstring(u8"サムネイルを更新").c_str(),
				GetColor(255, 255, 255)
			);
		}
	}
	break;

	}
}

void StageEditorSceneState_ThumbnailShooting::UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
{
	switch (_current_substate)
	{
	case ThunmailShootingSubState::Default:
	{
		const Vector2D move_dir = DeviceInput::GetInputDir_WASD();
		constexpr float MOVE_SPEED_BASE = 256.0f;
		float move_speed = MOVE_SPEED_BASE;
		if (DeviceInput::IsActive(KEY_INPUT_LSHIFT))
		{
			move_speed *= 4.f;
		}
		else if (DeviceInput::IsActive(KEY_INPUT_LCONTROL))
		{
			move_speed *= 0.25f;
		}
		parent_scene._camera_params.world_offset += move_dir * move_speed * delta_seconds;

		// 上下左右キーで1ずつ移動
		if (DeviceInput::IsPressed(KEY_INPUT_UP))
		{
			parent_scene._camera_params.world_offset.y -= 1;
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_DOWN))
		{
			parent_scene._camera_params.world_offset.y += 1;
		}
		if (DeviceInput::IsPressed(KEY_INPUT_LEFT))
		{
			parent_scene._camera_params.world_offset.x -= 1;
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_RIGHT))
		{
			parent_scene._camera_params.world_offset.x += 1;
		}

		// スクリーンオフセットを整数値にする
		parent_scene._camera_params.world_offset.x = static_cast<int>(parent_scene._camera_params.world_offset.x);
		parent_scene._camera_params.world_offset.y = static_cast<int>(parent_scene._camera_params.world_offset.y);
	}
	break;
	case ThunmailShootingSubState::ReleasingShutter:
	case ThunmailShootingSubState::Confirm:
		return;
	}
}

void StageEditorSceneState_ThumbnailShooting::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	__super::HandleInput(parent_scene, delta_seconds);

	switch (_current_substate)
	{
	case ThunmailShootingSubState::Default:
	{
		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			ResumeLastState(parent_scene);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_SPACE))
		{
			// 撮影開始
			ChangeSubState(ThunmailShootingSubState::ReleasingShutter);
			parent_scene._editor_scene_sounds->se_camera_shutter->Play();
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_F2))
		{
			_should_show_grid = !_should_show_grid;
		}
	}
		break;
	
	case ThunmailShootingSubState::ReleasingShutter:
		break;
	case ThunmailShootingSubState::Confirm:
	{
		// Esc: 撮影キャンセル
		// R: 撮影やり直し
		// Enter: サムネイル更新->撮影終了

		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE) && !IsNextStateSet())
		{
			ResumeLastState(parent_scene);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_R))
		{
			ChangeSubState(ThunmailShootingSubState::Default);
		}
		else if (DeviceInput::IsPressed(KEY_INPUT_RETURN))
		{
			UpdateStageThumbnail(parent_scene.GetStageRef());
			parent_scene.PushEditorMessage(u8"ステージのサムネイルを更新しました");
			ResumeLastState(parent_scene);
		}

		const Vector2D mouse_pos = DeviceInput::GetMousePosition();

	}
		break;
	}
}

void StageEditorSceneState_ThumbnailShooting::ShowControls(StageEditorScene& parent_editor_scene)
{
	__super::ShowControls(parent_editor_scene);
	switch(_current_substate)
	{
	case ThunmailShootingSubState::Default:
		{
			ImGui::Text(u8"F2: グリッドの表示・非表示");
			ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text(u8"ESC: キャンセル");

			ImGui::Text(u8"WASD: カメラ移動");
			ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text(u8"方向キー: カメラ移動(1px)");
			
			ImGui::Text(u8"Ctrl: カメラ移動を減速");
			ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);
			ImGui::Text(u8"Shift: カメラ移動を加速");			

			ImGui::Text(u8"SPACE: 撮影");
			ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);
			
		}
		break;
	}
}

void StageEditorSceneState_ThumbnailShooting::UpdateStageThumbnail(const Stage& stage) const
{
	if (_capture_result == nullptr)
	{
		throw std::runtime_error("Capture result is not set.");
	}

	const std::string file_path = ResourcePaths::Dir::STAGES + stage.GetThumbnailFileName();
	_capture_result->SaveToPNG(to_tstring(file_path));
}

void StageEditorSceneState_ThumbnailShooting::ChangeSubState(const ThunmailShootingSubState next_substate)
{
	// OnLeave
	switch (_current_substate)
	{
	case ThunmailShootingSubState::Default:
		break;
	case ThunmailShootingSubState::ReleasingShutter:
		_shuttering_speed = 0.f;
		break;
	case ThunmailShootingSubState::Confirm:
		_capture_result.reset();
		break;
	}

	_current_substate = next_substate;

	// OnEnter
	switch (next_substate)
	{
	case ThunmailShootingSubState::Default:
		break;
	case ThunmailShootingSubState::ReleasingShutter:
		_shuttering_phase = 0.f;
		_shuttering_speed = 1.f / SHUTTER_TIME;
		break;
	case ThunmailShootingSubState::Confirm:
		break;
	}
}
