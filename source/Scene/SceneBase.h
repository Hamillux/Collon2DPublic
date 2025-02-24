#pragma once

#include "Core.h"
#include "GameObject.h"
#include "SceneBaseInitialParams.h"
#include "GameSystems/Sound/SoundInstance.h"
#include "SceneObject/Actor/ActorParamTypes.h"
#include "SceneObject/Actor/Actor.h"
#include "SceneObject/Actor/ActorTraits.h"
#include "SceneObject/Actor/ActorFactory.h"
#include "SceneObject/Component/Collider/HitResult.h"
#include <type_traits>
#include <memory>
#include <vector>
#include <string>
#include <map>

class BoxCollision;
enum class CollisionObjectType : uint32_t;
struct ActorInitialParams;
class SceneAnimRendererActor;

/**
 * シーンの基底クラス
 */
class SceneBase : public GameObject
{
public:
	SceneBase();
	virtual ~SceneBase();

	template<class T>
	struct traits
	{
		static_assert(std::is_base_of<SceneBase, T>::value, "T is not a Scene");
		typedef SceneBaseInitialParams initial_params_type;
	};

	template<class SceneClass>
	using initial_params_of_scene_t = typename traits<SceneClass>::initial_params_type;

	//~ Begin SceneBase interface
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="scene_params">シーン初期化パラメータ</param>
	virtual void Initialize(const SceneBaseInitialParams* const scene_params);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="delta_seconds"></param>
	/// <returns>遷移先シーンタイプ. GetSceneType()の戻り値と異なる値を返すと遷移する.</returns>
	virtual SceneType Tick(float delta_seconds);

	/// <summary>
	/// ワールドの描画処理
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 前景描画処理
	/// </summary>
	virtual void DrawForeground(const CanvasInfo& canvas_info);

	/// <summary>
	/// 解放
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// スクリーンパラメータの更新. 全アクターのTickActor()を呼んだ直後に呼ばれる.
	/// </summary>
	/// <param name="delta_seconds"></param>
	virtual void UpdateScreenParams(const float delta_seconds);

	/// <summary>
	/// シーンタイプの取得. 
	/// </summary>
	/// <returns></returns>
	virtual SceneType GetSceneType() const = 0;

	/// <summary>
	/// 遷移先シーンの初期化パラメータを取得
	/// </summary>
	/// <param name="next_scene">遷移先シーン</param>
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const = 0;

	virtual Vector2D GetGravityForce() const;

protected:
	/// <summary>
	/// Actorの破棄直前に実行する処理
	/// </summary>
	/// <param name="destroyactor">破棄対象アクター</param>
	virtual void PreDestroyActor(Actor* destroyee);

	/// <summary>
	/// Actorの破壊直後に実行する処理
	/// </summary>
	/// <param name="destroyed"></param>
	virtual void OnDestroyedActor(Actor* destroyed);

private:
	virtual bool ShouldSpawnSceneAnimRendererActor() const;
	//~ End SceneBase interface

public:
	SceneType ExecuteTick(const float delta_seconds);

	/// <summary>
	/// 必要であればアクターの描画順をソートし,
	/// 背景->ワールド->前景の順で描画処理を実行する
	/// </summary>
	void ExecuteDrawProcess();

	/// <summary>
	/// ゲーム進行速度の割合を取得. デフォルト値は1.0
	/// <para>SceneManagerがSceneBase::Tickに渡すのは,この値に実際の経過時間を掛けた値</para>
	/// </summary>
	/// <returns></returns>
	float GetGameSpeed() const { return _game_speed_rate; }
	void SetGameSpeed(const float new_speed_rate);

	float GetWorldTime() const;

	std::shared_ptr<DxLibScreenCapture> CaptureScene();

	/// <summary>
	/// アクターが自身の描画優先度を変更した際に呼ぶ関数
	/// </summary>
	void OnActorDrawPriorityChanged();

	/// <summary>
	/// ワールド時間での遅延処理を作成する.
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理はキャンセルされる</param>
	/// <param name="delay_time">この関数呼び出しから処理実行までのワールド時間</param>
	/// <param name="process">実行する処理</param>
	void MakeDelayedEventWorld(GameObject* const object, const float delay_time, const std::function<void()> process);

	/// <summary>
	/// システム時間での遅延処理を作成する.
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理はキャンセルされる</param>
	/// <param name="delay_time">この関数呼び出しから処理実行までのワールド時間</param>
	/// <param name="process">実行する処理</param>
	static void MakeDelayedEventSystem(const GameObject* const object, const float delay_time, const std::function<void()> process);

	/// <summary>
	/// ワールド時間での定期実行処理を作成する. 初回実行はこの関数の呼び出しからintervalだけ経過した時に行われる
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理は破棄される</param>
	/// <param name="interval">実行間隔</param>
	/// <param name="process">実行する処理. 戻り値がfalseの場合, その処理は破棄される</param>
	void MakeRepeatingEventWorld(const GameObject* const object, const float interval, const std::function<bool()> process);

	/// <summary>
	/// システム時間での定期実行処理を作成する. 初回実行はこの関数の呼び出しからintervalだけ経過した時に行われる
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理は破棄される</param>
	/// <param name="interval">実行間隔</param>
	/// <param name="process">実行する処理. 戻り値がfalseの場合, その処理は破棄される</param>
	static void MakeRepeatingEventSystem(const GameObject* const object, const float interval, const std::function<bool()> process);

	// アクターを除外する. 破壊されるわけではないので呼び出し側でアドレスを失わないようにする
	void RemoveActor(Actor* actorToRemove);

	// 初期化済みのアクターを追加する. 追加済みの場合はスルーする
	void AddActor(Actor* actor_to_add);

	// 戻り値をStopAnimation()に渡すことでアニメーションを停止できる
	size_t PlayAnimation(const AnimPlayInfo& anim_play_info, const Transform& transform, Actor* const attach_to = nullptr);
	void StopAnimation(const size_t scene_anim_index);

	void SingleLineTrace(QueryResult_SingleLineTrace& out_query_result, const CollisionQueryParams_SingleLineTrace& query_params);
	void MultiAARectTrace(QueryResult_MultiAARectTrace& out_query_result, const CollisionQueryParams_RectAA& query_params = CollisionQueryParams_RectAA{});

	/// <summary>
	/// actorsのイテレート中に呼んでもOKなアクター生成関数
	/// </summary>
	template< class T>
	T* CreateActorDynamic(const initial_params_of_actor_t<T>* actor_params)
	{
		T* created_instance = ActorFactory::CreateAndInitializeActor<T>(actor_params, this);
		if (created_instance == nullptr)
		{
			return nullptr;
		}

		// actorsのイテレートに影響を与えないようにするため,
		// temp_actorsに追加して後からactorsへ移動させる
		AddActor(created_instance);

		return created_instance;
	}

	void DrawDebugLine(
		const Vector2D& start_world,
		const Vector2D& end_world,
		const int line_color = 0xFF0000,
		const int line_thickness = 1,
		const DrawBlendInfo& blend_info = DrawBlendInfo{}
	);

	void DrawDebugRect(
		const FRect& rect,
		const int line_color = 0xFF0000,
		const int line_thickness = 1,
		const DrawBlendInfo& blend_info = DrawBlendInfo{}
	);

protected:
	/// <summary>
	/// actorsのイテレート中に呼んではいけないアクター生成関数
	/// </summary>
	template <class T>
	T* CreateActor(const initial_params_of_actor_t<T>* const actor_params)
	{
		T* created_instance = ActorFactory::CreateAndInitializeActor<T>(actor_params, this);
		if (created_instance == nullptr)
		{
			return nullptr;
		}

		_actors.push_back(created_instance);
		return created_instance;
	}

	Actor* CreateAndInitializeActorByEntityType(const EEntityType entity_type, const ActorInitialParams* actor_params);

	void SortActorsByDrawPriority();

	/// <summary>
	/// Actorを破壊する. SceneBase::_actorsのイテレーション中に呼ばない
	/// </summary>
	/// <param name="to_destroy">破棄対象</param>
	void DestroyActor(Actor*& destroyee);

	// 背景情報
	struct BackgroundParams
	{
		BackgroundParams(const int handle_in = -1, const float distance_in = 1.f)
			: handle(handle_in)
			, distance(distance_in)
		{}

		// 設定必要 //
		int handle;
		float distance;
	};

	/// <summary>
	/// BackgroundParams::distance_scaleが降順になるように追加対象を適切な位置に挿入する. 同じdistance_scaleの背景が挿入済みの場合は, 追加対象が同一distance_scaleの背景で最後尾にくるように挿入する.
	/// </summary>
	/// <param name="bg_params">追加対象背景</param>
	void AddBackgroundToLayer(BackgroundParams bg_params);
	void ClearBackgroundLayer();
	void DrawBackground() const;

	void SetBGDrawArea(const Vector2D& left_top, const Vector2D& right_bottom);
	int GetBGDrawAreaLeft() const;
	int GetBGDrawAreaRight() const;

	void DrawWorldGrid(const int color, const uint8_t alpha) const;

	/// <summary>
	/// ワールドの更新が行われるか否か
	/// </summary>
	/// <returns></returns>
	bool IsWorldTimerActive() const { return _is_world_timer_active; }
	void SetWorldTimerActive(const bool new_world_timer_active);

	bool should_call_draw_foreground = true;

	ScreenParams _screen_params;
	std::vector<Actor*> _actors;
	bool IsContainedInActors(Actor* actor, std::vector<Actor*>::iterator* outIt = nullptr);

private:
	bool _is_world_timer_active = true;

	/// <summary>
	/// シーン内の全Actorを破棄
	/// </summary>
	void DestroyAllActors();

	// SceneBaseのループでactorsへの追加・削除処理が実行される
	std::vector<Actor*> _actors_to_add;
	std::vector<Actor*> _actors_to_remove;

	float _game_speed_rate;

	// distanceが降順になるように並ぶ(先頭要素が一番奥に描画される)
	std::vector<BackgroundParams> _bg_layer;	

	// 背景を描画するエリアの両端のワールド座標
	Vector2D bg_area_left_top;
	Vector2D bg_area_right_bottom;

	/// <summary>
	/// trueの場合, アクターのDrawを呼ぶ前に描画優先度によるアクターのソートを行った後, falseに設定される.
	/// </summary>
	bool should_sort_actors;

	// should_update_objectsがtrueの場合にのみ更新
	float _world_timer;

	// ディレイ処理
	struct WorldDelayedEvent
	{
		const GameObject* const object;
		float dispatch_world_time;
		std::function<void()> process;
		WorldDelayedEvent(const GameObject* const in_object, const float in_dispatch_world_time, const std::function<void()>& in_process)
			: object(in_object)
			, dispatch_world_time(in_dispatch_world_time)
			, process(in_process)
		{}
	};
	std::vector<std::unique_ptr<WorldDelayedEvent>> _world_delayed_events;
	void UpdateWorldDelayedEvents(const float delta_seconds);

	struct WorldRepeatingEvent
	{
		const GameObject* const object;
		float next_dispatch_time;
		float interval;
		std::function<bool()> process;
		WorldRepeatingEvent(const GameObject* const in_object, const float in_next_dispatch_time, const float in_interval, const std::function<bool()>& in_process)
			: object(in_object)
			, next_dispatch_time(in_next_dispatch_time)
			, interval(in_interval)
			, process(in_process)
		{}
	};
	std::vector<std::unique_ptr<WorldRepeatingEvent>> _world_repeating_events;
	void UpdateWorldRepeatingEvents(const float delta_seconds);

	SceneAnimRendererActor* _scene_anim_actor;

	struct Canvas
	{
		Canvas(const CanvasInfo& canvas_info);
		void ClearScreen();
		CanvasInfo info;
		int handle;
	};
	std::unique_ptr<Canvas> _foreground_canvas;

	// 重力ベクトル
	static const Vector2D BASIC_GRAVITY_FORCE;

	// デバッグ描画用
	std::unique_ptr<Canvas> _debug_world_canvas;
};