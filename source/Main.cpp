#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "SystemTypes.h"
#include "Scene/SceneManager.h"
#include "Input/DeviceInput.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "GameSystems/FontManager.h"
#include "GameSystems/GameConfig/GameConfig.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {

		SetUseHookWinProcReturnValue(TRUE);
		return true;
	}

	//IME関連はOSに任せる
	switch (msg)
	{
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_REQUEST:
		SetUseHookWinProcReturnValue(TRUE);
		return DefWindowProc(hWnd, msg, wParam, lParam);

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) { // Disable ALT application menu
			SetUseHookWinProcReturnValue(TRUE);
			return 0;
		}
		break;
	}

	return 0;
}



// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//マスターデータのロード
	LoadAllMasterData();

	// ＤＸライブラリ初期化処理
	{
#ifndef _DEBUG
		DxLib::SetOutApplicationLogValidFlag(FALSE);
#endif

		DxLib::SetMainWindowText(_T("Collon's Action"));
		DxLib::SetAlwaysRunFlag(true);
		DxLib::SetGraphMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, SCREEN_COLOR_BIT_NUM);
		DxLib::ChangeWindowMode(true);
		DxLib::SetUseDirect3DVersion(DX_DIRECT3D_11);
		DxLib::SetUseIMEFlag(TRUE);
		DxLib::SetHookWinProc(WndProc);
		DxLib::SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

		if (DxLib_Init() == -1)
		{
			return -1;
		}
	}

	// コンフィグのロード
	GameConfig::GetInstance().Init();

	// パーティクルシステム初期化
	if (!ParticleManager::GetInstance().Init())
	{
		return -1;
	}

	// imgui初期化
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifndef _DEBUG
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
#endif
		
		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(DxLib::GetMainWindowHandle());
		ID3D11Device* pDev = const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(DxLib::GetUseDirect3D11Device()));
		ID3D11DeviceContext* pContext= const_cast<ID3D11DeviceContext*>(reinterpret_cast<const ID3D11DeviceContext*>(DxLib::GetUseDirect3D11DeviceContext()));
		ImGui_ImplDX11_Init(pDev, pContext);
	}

	FontManager::GetInstance().AddFontsFromMasterData();

	// 透過色を設定
	SetTransColor(TRANS_COLOR_R, TRANS_COLOR_G, TRANS_COLOR_B);

	// 文字列描画に使用するフォントタイプを変更
	ChangeFontType(DX_FONTTYPE_EDGE);

	// SceneManagerの生成
	SceneManager* scene_manager = new SceneManager();
	scene_manager->Initialize();

	// ゲームループの周期
	constexpr const int loop_time = 1000 / FRAME_RATE;

	// メインループ
	int prev_frame_start = -1;
	while (true)
	{
		const int dxlib_message = DxLib::ProcessMessage();
		if (dxlib_message == -1)
		{
			break;
		}

		if (prev_frame_start == -1)
		{
			prev_frame_start = GetNowCount();
			Sleep(loop_time - (prev_frame_start-GetNowCount()));
		}

		// DeltaSecondsの計算
		const int current_frame_start = GetNowCount();
		const float delta_seconds = (float)(current_frame_start - prev_frame_start) / 1000.0f;
		prev_frame_start = current_frame_start;

		// キーボード入力情報の更新
		DeviceInput::Tick();

		// SceneManagerの更新
		SceneManagerMessage msg = scene_manager->Tick(delta_seconds);

		if (msg == SceneManagerMessage::CHANGED_SCENE)
		{
			prev_frame_start = -1;
			continue;
		}
		else if (msg == SceneManagerMessage::QUITTED_GAME)
		{
			break;
		}

#ifdef _DEBUG
		// デバッグ出力をクリア
		if (DeviceInput::IsActive(KEY_INPUT_BACK))
		{
			clsDx();
		}
#endif

		// スリープ
		if ((GetNowCount() - current_frame_start) < loop_time)
		{
			Sleep( loop_time - (GetNowCount() - current_frame_start) );
		}
	}

	// SceneManagerの破棄
	scene_manager->Finalize();
	delete scene_manager;
	scene_manager = nullptr;

	// DirectX依存
	{
		ImGui::GetIO().Fonts->ClearFonts();
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		GraphicResourceManager::GetInstance().Destroy();
		ParticleManager::GetInstance().End();
	}

	GameConfig::GetInstance().SaveToFile();

	// NOTE: デバッグビルド時にDxLib_End()を呼び出すと例外が発生する.
	// 明示的な終了処理がなくても特に問題は無いようなので(https://dxlib.xsrv.jp/cgi/patiobbs/patio.cgi?mode=past&no=2600),
	// ひとまずデバッグ時には呼び出さないようにすることで対処.
	// TODO: 原因調査
#ifndef _DEBUG
	DxLib_End();
#endif

	return 0;
}