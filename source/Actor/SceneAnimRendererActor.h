#pragma once

#include "Actor/Actor.h"
#include <array>

class AnimRendererComponent;

CLN2D_GEN_DEFINE_ACTOR()
/// <summary>
/// シーン上でアニメーションを再生するアクター. 
/// <para>主な用途はエフェクト</para>
/// <para>同時に再生できる数はそれほど多くないので, パーティクルには向かない</para>
/// </summary>
class SceneAnimRendererActor : public Actor
{
	static constexpr int NUM_ANIM_COMPONENTS = 100;
public:
	SceneAnimRendererActor();
	virtual ~SceneAnimRendererActor();

	//~ Begin Actor interface
public:
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) override;
	virtual void RequestToSetActorHidden(const bool new_hidden) override;
	//~ End Actor interface

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	/// <param name="anim_play_info">アニメーション情報</param>
	/// <param name="transform">再生位置・回転</param>
	/// <param name="attach_to">
	///		アニメーションをアタッチするアクター. 
	///		第2引数の位置・回転はこのアクターのローカル座標系で解釈される.
	///		nullptrの場合は第2引数がワールド座標系で解釈される
	/// </param>
	/// <returns>再生したアニメーションのインデックス</returns>
	size_t PlayAnimation(const AnimPlayInfo& anim_play_info, const Transform& transform, Actor* const attach_to = nullptr);

	/// <summary>
	/// アニメーションの再生を停止する
	/// </summary>
	/// <param name="scene_anim_index">PlayAnimtionで取得したインデックス</param>
	void StopAnimation(const size_t scene_anim_index);

private:
	size_t GetAvailableAnimComponentIndex() const;
	std::array<AnimRendererComponent*, NUM_ANIM_COMPONENTS> _anim_components;
};

template<>
struct initial_params_of_actor<SceneAnimRendererActor> { using type = initial_params_of_actor_t<Actor>; };