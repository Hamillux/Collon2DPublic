#pragma once

#include "GameObject.h"

struct CameraParams;
class SceneBase;
class Actor;

/// <summary>
/// アクターコンポーネントの基底クラス
/// </summary>
class ComponentBase : public GameObject
{
public:
	struct ComponentEvents
	{
		Event<> on_draw_priority_changed;
	};
	ComponentEvents component_events;

	ComponentBase()
		: owner_actor(nullptr)
		, should_call_component_tick(true)
		, should_call_component_draw(true)
		, _draw_priority(0)
	{}
	virtual ~ComponentBase() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize();
	virtual void Tick(const float delta_seconds);
	virtual void Draw(const CameraParams& camera_params);
	virtual void DrawForeground(const CanvasInfo& canvas_info);
	virtual void Finalize();
	virtual void OnParentActorHiddenChanged(const bool new_hidden);
	//~ End ComponentBase interface

public:
	void SetOwnerActor(Actor* owner_actor_in) { owner_actor = owner_actor_in; }
	Actor* GetOwnerActor() const { return owner_actor; }
	SceneBase* GetScene() const;
	bool ShouldCallTickActor() const { return should_call_component_tick; }
	bool ShouldCallDraw() const { return should_call_component_draw; }

	/// <summary>
	/// ComponentのTickとDrawの呼び出しを無効化する
	/// </summary>
	void Sleep();

	/// <summary>
	/// ComponentのTickとDrawの呼び出しを有効にする
	/// </summary>
	void Awake();

	// 描画優先度をセット
	void SetComponentDrawPriority(const int new_draw_priority);
	// 描画優先度を取得
	int GetComponentDrawPriority() const { return _draw_priority; }

protected:
	bool should_call_component_tick;
	bool should_call_component_draw;

private:
	Actor* owner_actor;
	int _draw_priority;
};