#pragma once

#include "SceneObject/SceneObject.h"
#include "EntityType.h"
#include "ActorTraits.h"
#include "ActorParamTypes.h"
#include "ActorInitialParams.h"
#include "DamageInfo.h"
#include <vector>
#include "SceneObject/Component/SceneComponent.h"
#include "SceneObject/Component/MovementComponent.h"
#include "GameSystems/GameObjectManager.h"
#include "GameSystems/Sound/SoundInstance.h"
#include "SceneObject/Component/Collider/HitResult.h"

class SceneBase;

CLN2D_GEN_DEFINE_ACTOR()
class Actor : public SceneObject
{
public:
	Actor();
	virtual ~Actor();

	struct ActorEvents{
		
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
	/// <param name="screen_params"></param>
	virtual void Draw(const ScreenParams& screen_params);

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
	virtual void GetOccupyingTileCounts(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const;

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
	/// <param name="damage_info"></param>
	virtual void ApplyDamage(const DamageInfo& damage_info);

protected:
	/// <summary>
	/// ダメージを受ける処理.
	/// <para>Actor::TakeDamage()では何もしない</para>
	/// </summary>
	virtual void TakeDamage(const DamageInfo& damage_info);

	//~ End Actor interface

public:
	/// <summary>
	/// 初期化済みか
	/// </summary>
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

	SceneComponent* GetRootComponent() const;

	/// <summary>
	/// アクターのアタッチメント
	/// </summary>
	void AttachToOtherActor(Actor* attach_target, const bool keep_world_transform = false);
	void DetachFromParentActor();

	Transform GetActorWorldTransform() const;

	//// ワールド座標
	Vector2D GetActorWorldPosition() const;
	void SetActorWorldPosition(const Vector2D& new_position);
	void AddWorldPosition(const Vector2D& delta_position);

	//// ローカル座標
	Vector2D GetActorLocalPosition() const;
	void SetActorLocalPosition(const Vector2D& new_position);
	void AddActorLocalPosition(const Vector2D& delta_position);

	//// ワールド回転
	float GetActorWorldRotation() const;
	void SetActorWorldRotation(float new_rotation);
	void AddActorRotation(const float delta_rotation);

	//// ローカル回転
	float GetActorLocalRotation() const;
	void SetActorLocalRotation(const float new_rotation);

	//// 速度
	Vector2D GetVelocity() const;
	void SetVelocity(const Vector2D& new_velocity, const bool dont_update_at_next_frame = true);
	void SetVelocity(const float new_x, const float new_y, const bool dont_update_at_next_frame = true);
	void AddVelocity(const Vector2D& delta_velocity);

	//// 力
	void SetGravityScale(const float new_gravity_scale) { gravity_scale = new_gravity_scale; }
	void ApplyForce(const Vector2D& force);

	/// <summary>
	/// 速度に比例する力を加える.
	/// <para>速度更新の前にApplyForce(kv)が実行される</para>
	/// </summary>
	/// <param name="k_i">加える力はSum(F_i)=Sum(k_i*v)
	/// <para>ただしk = Sum(k_i)</para>
	/// </param>
	void ApplyForcePropotionalToVelocity(const Vector2D& k_i);
	Vector2D coef_of_velocity;

	float GetMass() const { return mass; }

	/// <summary>
	/// 速さがmax_move_speedに収束するようにmove_dirの向きに力を加える.
	/// <para>この力と空気抵抗(-kv)以外の, move_dirと平行な力は全て打ち消される.</para>
	/// </summary>
	/// <para name="move_dir">移動方向単位ベクトル</para>
	/// <para name="max_move_speed">最高速</para>
	void ApplyMoveForce(const Vector2D& move_dir, const float max_move_speed);
	Vector2D GetMoveForce() const { return move_force; }
	Vector2D move_force;

	/// <summary>
	/// 接触力を加えるために垂直抗力の向きを設定する. 
	/// <para>N_dirは毎ループ零ベクトルにリセットされる</para>
	/// </summary>
	/// <para name="new_N_dir">垂直抗力と同じ向きの単位ベクトル</para>
	void SetNormalForceDirection(const Vector2D& new_N_dir);
	Vector2D GetNormalForceDirection() { return N_dir; }
	Vector2D N_dir;

	/// <summary>
	/// アクターに加わっている力を取得する. Update中に呼ぶと
	/// </summary>
	/// <returns></returns>
	Vector2D GetAppliedForce() { return _applied_force; }
	void ApplyConstantForce();
	void CancelAppliedForce();

	// MovementComponentで動きを定義する場合はfalseにする
	void SetPhysicsSimulationEnabled(const bool new_enabled);

	//// 描画優先度の取得,設定 DrawPriority大ほど手前に描画
	int GetDrawPriority() const { return _draw_priority; }
	void SetDrawPriority(const int new_priority);
	void OnComponentDrawPriorityChanged();

	// 画面外にいても常に描画するか
	bool GetShouldDrawAlways() const;
	void SetShouldDrawAlways(const bool new_should_draw_always);

	/// <summary>
	/// 画面内に映り込むかを判定する
	/// </summary>
	/// <param name="screen_params"></param>
	bool IsInDrawArea(const ScreenParams& screen_params) const;

	/// <summary>
	/// 描画判定用の外接円半径を取得
	/// </summary>
	/// <returns></returns>
	float GetBoundingCircleRadius() const;

	FCircle GetBoundingCircle() const;

	Vector2D GetSnapPosition() const;
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

		_components.push_back(new_component);
		return new_component;
	}

protected:
	void SetBoundingCircleRadius(const float new_radius);
	void SortComponentsByDrawPriority();

	void SetShouldCallTickActor(const bool should_call);
	void SetShouldCallDraw(const bool should_call);

	// 質量. 0より大きくなければならない
	float mass = 1.0f;

	// 重力倍率
	float gravity_scale = 0.f;

	// 空気抵抗係数. F=-kv
	float k_air_resistance = 1.0f;

private:
	void UpdatePhysicsSimulation(const float delta_seconds);

	void AddChildActor(Actor* new_child);
	void RemoveChildActor(Actor* child_to_remove);

	bool _is_initialized;
	bool _should_call_tick_actor;
	bool _should_call_draw;
	bool _is_hidden;

	// アクターの位置・回転を表現するためのSceneComponent
	SceneComponent* _root_component;	

	// AttachToOtherActorでこのアクターがアタッチされたアクター
	Actor* _parent_actor;
	// AttachToOtherActorでこのアクターにアタッチされているアクター
	std::vector<Actor*> _child_actors;

	// 所有コンポーネント
	std::vector<ComponentBase*> _components;

	// 物理演算での速度
	Vector2D _physics_velocity;

	// 力ベクトル
	Vector2D _applied_force;

	// 物理シミュレーションは有効か
	// MovementComponentで動きを定義する場合はfalseにする
	bool _is_physics_simulation_enabled;

	// 最初に追加されたMovementComponent. 速度取得等に利用する
	MovementComponent* _movement_component;

	// 描画順。数値が小さい順から描画を行う
	int _draw_priority;

	// positionやdraw_radiusに関係なく常に描画するか
	bool _should_draw_always;

	// 描画対象判定円の半径. 中心はposition
	float _bounding_circle_radius;

	// コンポーネントの描画優先度が変更されたループの描画処理直前に1回だけコンポーネントをソートするためのフラグ
	bool _should_sort_components;
	
	// 破壊フラグ
	bool _should_destroy;

	// SetVelocityを呼んだループの直後のループで速度を更新しないためのフラグ
	bool _is_velocity_update_disabled;
	
};

template<> struct initial_params_of_actor<Actor> { using type = ActorInitialParams; };