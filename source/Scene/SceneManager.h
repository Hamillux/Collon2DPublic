#pragma once
#include <memory>
#include <Scene/SceneType.h>
#include "Utility/Core/Rendering/CanvasInfo.h"

struct SceneBaseInitialParams;

/// <summary>
/// SceneManager::Updateの戻り値
/// </summary>
enum class SceneManagerMessage
{
	CHANGED_SCENE,
	QUITTED_GAME,
	CONTINUE,
};

/**
 * 各シーンを管理するクラス
 * シーンの生成/破棄や変更を行う
 */
class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();

public:
	/**
	 * 初期化
	 */
	void Initialize();

	/**
	 * 更新
	 * @param	DeltaSeconds	前フレームとの差分時間(s)
	 */
	SceneManagerMessage Tick(float DeltaSeconds);

	/**
	 * 解放
	 */
	void Finalize();

private:
	void NewImGuiFrame();

	/**
	 * 描画
	 */
	void Draw();

	void DrawLoadingScreen() const;

	/**
	 * シーンの遷移
	 * @param	new_scene_type	遷移したいシーン
	 */
	void ChangeScene(SceneType next_scene_type, std::unique_ptr<const SceneBaseInitialParams>& scene_params);

	/**
	 * シーンの生成
	 * @param	new_scene_type	生成する新しいシーン
	 * @return	生成したシーン
	 */
	class SceneBase* CreateScene(SceneType new_scene_type);

	class SceneBase* _current_scene;
	
	int _draw_scene_screen_handle;

#if defined(_DEBUG) || defined(DEBUG)
	int _debug_screen_handle;
#endif

	// シーン遷移にかける最小時間. 遷移先シーンの初期化開始からこの時間が経過するまでシーンのUpdateは呼ばれない
	static constexpr const int MIN_LOADING_TIME_MS = 300;
};