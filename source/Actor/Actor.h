#pragma once

#include "GameObject.h"
#include "EntityType.h"
#include "ActorTraits.h"
#include "ActorParamTypes.h"
#include "ActorInitialParams.h"
#include "DamageInfo.h"
#include <vector>
#include "Component/SceneComponent.h"
#include "Component/MovementComponent.h"
#include "GameSystems/GameObjectManager.h"
#include "GameSystems/Sound/SoundInstance.h"
#include "Component/Collider/HitResult.h"

class SceneBase;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// シーンが直接所有するオブジェクト
/// </summary>
class Actor : public GameObject
{
public:
	Actor();
	virtual ~Actor();

	struct ActorEvents {
		Event<> on_draw_priority_changed;
	};
	ActorEvents actor_events;

	//~ Begin Actor interface
public:
	/// <summary>
	/// Actorの初期化
	/// </summary>
	/// <param name="actor_params">初期化構造体. Actor派生クラスと初期化構造体の動的型の対応はinitial_params_of_actor&lt;ActorDerived&gt;で定義される</param>
	virtual void Initialize(const ActorInitialParams* actor_params);

	/// <summary>
	/// Actorの更新
	/// </summary>
	/// <param name="delta_seconds">前フレームからの経過時間</param>
	virtual void TickActor(float delta_seconds);

	/// <summary>
	/// Actorの描画
	/// </summary>
	/// <param name="camera_params"></param>
	virtual void Draw(const CameraParams& camera_params);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="out_vertexes">時計回り or 反時計回りに 頂点を格納</param>
	virtual void GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertexes);

	/// <summary>
	/// エディターでこのアクターが占有するタイル数を取得.
	/// <para>NOTE: スナップ位置はタイル数をNx,Nyとして、0-basedで(N-1)/2番目の占有タイルの中心</para>
	/// </summary>
	/// <param name="out_tile_x">横方向のタイル数</param>
	/// <param name="out_tile_y">縦方向のタイル数</param>
	/// <param name="out_snap_position_to_actor_position">アクターの位置とスナップ位置の差分</param>
	virtual void GetOccupyingTiles(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const;

	/// <summary>
	/// Actorによる前景への描画
	/// </summary>
	virtual void DrawForeground(const CanvasInfo& canvas_info);

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// デスポーン可能か
	/// <para>NOTE:画面外に出る等でデスポーンされたアクターは, スポーン条件を満たしたときに初期位置にリスポーンする</para>
	/// </summary>
	/// <returns></returns>
	virtual bool IsDespawnable() const;

	// 衝突時処理
	virtual void OnHitCollision(const HitResult& hit_result);

	/// <summary>
	/// アクターの表示状態の変更をリクエストする. 非表示化されたアクターは更新, 描画されない
	/// <para>Actor::RequestToSetActorHidden()は, Actor::IsHidden()の値を変更し, その変更をコンポーネントに通知する</para>
	/// </summary>
	virtual void RequestToSetActorHidden(const bool new_hidden);

	/// <summary>
	/// TakeDamageを呼ぶ
	/// </summary>
	virtual void ApplyDamage(const DamageInfo& damage_info);

protected:
	/// <summary>
	/// ダメージを受ける処理.
	/// <para>Actor::TakeDamage()では何もしない</para>
	/// </summary>
	virtual void TakeDamage(const DamageInfo& damage_info);

	//~ End Actor interface

public:
	void SetScene(SceneBase* const in_owner_scene);
	SceneBase* GetScene() const;

	bool IsInitialized() const;
	bool ShouldCallTickActor() const;
	bool ShouldCallDraw() const;
	bool IsHidden() const;

	/// <summary>
	/// このアクターと, 子アクターに破壊フラグを立てる.
	/// <para>フラグが立っているアクターはSceneBaseによって破壊される</para>
	/// </summary>
	void MarkAsShouldDestroy();
	bool ShouldDestroy() const;

	/// <summary>
	/// アクターのルートコンポーネントを取得
	/// </summary>
	SceneComponent* GetRootComponent() const;

	/// <summary>
	/// ルートコンポーネントを他アクターのルートコンポーネントにアタッチする
	/// </summary>
	void AttachToOtherActor(Actor* attach_target, const bool keep_world_transform = false);

	/// <summary>
	/// アタッチされているアクターからデタッチする
	/// </summary>
	void DetachFromParentActor();

	/// <summary>
	/// ルートコンポーネントのワールド位置・回転を取得
	/// </summary>
	Transform GetActorWorldTransform() const;

	/// <summary>
	/// ルートコンポーネントのワールド位置を取得
	/// </summary>
	Vector2D GetActorWorldPosition() const;

	/// <summary>
	/// ルートコンポーネントのワールド位置を設定
	/// </summary>
	void SetActorWorldPosition(const Vector2D& new_position);

	/// <summary>
	/// ルートコンポーネントのワールド位置を移動
	/// </summary>
	void AddWorldPosition(const Vector2D& delta_position);

	/// <summary>
	/// ルートコンポーネントのローカル位置を取得
	/// </summary>
	Vector2D GetActorLocalPosition() const;

	/// <summary>
	/// ルートコンポーネントのローカル位置を設定
	/// </summary>
	void SetActorLocalPosition(const Vector2D& new_position);

	/// <summary>
	/// ルートコンポーネントのローカル位置を移動
	/// </summary>
	void AddActorLocalPosition(const Vector2D& delta_position);

	/// <summary>
	/// ルートコンポーネントのワールド回転を取得
	/// </summary>
	float GetActorWorldRotation() const;

	/// <summary>
	/// ルートコンポーネントのワールド回転を設定
	/// </summary>
	void SetActorWorldRotation(float new_rotation);

	/// <summary>
	/// ルートコンポーネントのローカル回転を取得
	/// </summary>
	float GetActorLocalRotation() const;

	/// <summary>
	/// ルートコンポーネントのローカル回転を設定
	/// </summary>
	void SetActorLocalRotation(const float new_rotation);

	/// <summary>
	/// ルートコンポーネントの回転を追加
	/// </summary>
	void AddActorRotation(const float delta_rotation);

	/// <summary>
	/// アクターの速度を取得
	/// <para>物理シミュレーションが有効な場合はシミュレーション速度を返す</para>
	/// <para>_movement_componentが有効な場合は_movement_component->GetVelocity()を返す</para>
	/// </summary>
	Vector2D GetVelocity() const;
	void SetVelocity(const Vector2D& new_velocity, const bool dont_update_at_next_frame = true);
	void AddVelocity(const Vector2D& delta_velocity);

	// 描画優先度の取得,設定 DrawPriority大ほど手前に描画
	int GetDrawPriority() const { return _draw_priority; }
	void SetDrawPriority(const int new_priority);

	// 画面外にあっても描画するか
	bool IsDrawAreaCheckIgnored() const;
	void SetDrawAreaCheckIgnored(const bool new_draw_area_check_ignored);

	/// <summary>
	/// 描画判定用の外接円半径を取得
	/// </summary>
	float GetBoundingCircleRadius() const;

	/// <summary>
	/// アクターの境界円. 中心はルートコンポーネントのワールド位置, 半径はGetBoundingCircleRadius()の値
	/// </summary>
	FCircle GetBoundingCircle() const;

	/// <summary>
	/// スナップ位置を取得. 
	/// <para>スナップ位置は占有タイル数をNx,Nyとして、0-basedで(N-1)/2番目の占有タイルの中心</para>
	/// </summary>
	Vector2D GetSnapPosition() const;

	/// <summary>
	/// スナップ位置からルート位置へのベクトルを取得
	/// </summary>
	Vector2D GetSnapPositionToActorPosition() const;

	template <class T>
	T* CreateComponent(Actor* owner_actor)
	{
		static_assert(std::is_base_of_v<ComponentBase, T>, "T must be derived from ComponentBase");

		// Note: GameObjectManager::CreateObject()の戻り値は常に有効で,
		//       static_assertによりTがComponentBase派生であることが
		//       保証されているので, new_componentは常に有効
		T* new_component = GameObjectManager::GetInstance().CreateObject<T>();

		// コンポーネントの初期化
		new_component->SetOwnerActor(owner_actor);
		new_component->Initialize();

		// SceneComponent派生クラスの場合, デフォルトではルートコンポーネントにアタッチ
		SceneComponent* new_component_as_scene_component = dynamic_cast<SceneComponent*>(new_component);
		if (_root_component && new_component_as_scene_component != nullptr)
		{
			new_component_as_scene_component->AttachToSceneComponent(_root_component);
		}

		// MovementComponent派生クラスの場合
		MovementComponent* new_component_as_movement_component = dynamic_cast<MovementComponent*>(new_component);
		if (_movement_component == nullptr && new_component_as_movement_component != nullptr)
		{
			_movement_component = new_component_as_movement_component;
		}

		new_component->component_events.on_draw_priority_changed.Bind([this]() { OnComponentDrawPriorityChanged(); }, this);

		_components.push_back(new_component);
		return new_component;
	}

protected:
	/// <summary>
	/// 境界円半径を設定
	/// </summary>
	void SetBoundingCircleRadius(const float new_radius);

	/// <summary>
	/// 描画優先度が昇順になるようにコンポーネントをソート
	/// </summary>
	void SortComponentsByDrawPriority();

	void SetShouldCallTickActor(const bool should_call);
	void SetShouldCallDraw(const bool should_call);

private:
	void UpdatePhysicsSimulation(const float delta_seconds);

	void AddChildActor(Actor* new_child);
	void RemoveChildActor(Actor* child_to_remove);

	SceneBase* _owner_scene;

	bool _is_initialized;
	bool _should_call_tick_actor;
	bool _should_call_draw;
	bool _is_hidden;

	// アクターの位置・回転を表現するためのSceneComponent
	SceneComponent* _root_component;

	// 親アクター 
	Actor* _parent_actor;

	// 子アクターリスト
	std::vector<Actor*> _child_actors;

	// 所有コンポーネント
	std::vector<ComponentBase*> _components;

	// 最初に追加されたMovementComponent. 速度取得等に利用する
	MovementComponent* _movement_component;

	// 描画順。数値が小さい順から描画を行う
	int _draw_priority;
	void OnComponentDrawPriorityChanged();

	// hiddenでない限り常に描画するか
	bool _is_draw_area_check_ignored;

	// 描画対象判定円の半径.
	float _bounding_circle_radius;

	// コンポーネントの描画優先度が変更されたループの描画処理直前に1回だけコンポーネントをソートするためのフラグ
	bool _should_sort_components;

	// 破壊フラグ
	bool _should_destroy;
};

template<> struct initial_params_of_actor<Actor> { using type = ActorInitialParams; };
