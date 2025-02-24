#pragma once

#include "SceneObject/Component/SceneComponent.h"
#include "GameSystems/MasterData/MasterDataInclude.h"

class RendererBase : public SceneComponent
{
public:
	RendererBase()
		: _is_visible(true)
	{}
	virtual ~RendererBase() {}

	//~ Begin ComponentBase interface
public:
	virtual void Draw(const ScreenParams& screen_params) override final
	{
		if (_is_visible) 
		{
			__super::Draw(screen_params);
			RendererDraw(screen_params);
		}
	}
	//~ End ComponentBase interface

	//~ Begin RendererBase interface
protected:
	virtual void RendererDraw(const ScreenParams& screen_params) = 0;
	//~ End RendererBase interface

public:
	void SetVisibility(const bool is_visible) { _is_visible = is_visible; }

private:
	bool _is_visible;
};

class AnimRendererComponent : public RendererBase
{
public:
	AnimRendererComponent();
	virtual ~AnimRendererComponent();

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Tick(const float delta_seconds) override;
	//~ End ComponentBase interface

	//~ Begin RendererBase interface
protected:
	virtual void RendererDraw(const ScreenParams& screen_params) override;
	//~ End RendererBase interface

public:
	/// <summary>
	/// 再生するアニメーションを設定する
	/// </summary>
	/// <param name="anim_info">アニメーション情報</param>
	/// <param name="should_play_immediately">アニメーション設定後すぐに再生するか</param>
	void SetAnimation(const AnimPlayInfo& anim_info, const bool should_play_immediately = true);

	/// <summary>
	/// 優先的に再生するアニメーションを設定する
	/// </summary>
	/// <param name="anim_info">アニメーション情報</param>
	/// <param name="override_time">
	/// オーバーライドする時間.override_time==0かつ無限ループするアニメーションでオーバーライドする場合, ClearOverrideAnimation()を呼ぶまでオーバーライド状態が続く
	/// </param>
	void SetOverrideAnimation(const AnimPlayInfo& anim_info, const float override_time = 0.f)
	{
		_override_anim_state = std::make_unique<AnimationState>(anim_info); 
		_anim_override_time = override_time;
	}

	void ClearOverrideAnimation()
	{
		_override_anim_state.reset(); 
		_anim_override_time = 0.f;
	}

	void Play() { _is_playing = true; }
	void Stop() { _is_playing = false; }

	bool IsPlaying() const 
	{
		return _is_playing && _anim_state != nullptr;
	}

	/// <summary>
	/// 再生中のアニメーションの再生速度を変更する
	/// </summary>
	/// <param name="play_speed">再生速度</param>
	void SetPlaySpeed(const float play_speed)
	{
		if (_anim_state != nullptr)
		{
			_anim_state->anim_info.play_speed = play_speed;
		}

		if (_override_anim_state != nullptr)
		{
			_override_anim_state->anim_info.play_speed = play_speed;
		}
	}

	/// <summary>
	/// 再生中のアニメーションの拡大率を変更する
	/// </summary>
	/// <param name="ex_rate">拡大率</param>
	void SetExRate(const float ex_rate) 
	{
		if (_anim_state != nullptr)
		{
			_anim_state->anim_info.ex_rate = ex_rate;
		}

		if (_override_anim_state != nullptr)
		{
			_override_anim_state->anim_info.ex_rate = ex_rate;
		}
	}

	/// <summary>
	/// 再生中のアニメーションの反転状態を変更する
	/// </summary>
	/// <param name="reverse_x">X方向反転</param>
	void SetReverseX(const int reverse_x) 
	{
		if (_anim_state != nullptr)
		{
			_anim_state->anim_info.reverse_x = reverse_x;
		}

		if (_override_anim_state != nullptr)
		{
			_override_anim_state->anim_info.reverse_x = reverse_x;
		}
	}

	/// <summary>
	/// 再生中のアニメーションの反転状態を変更する
	/// </summary>
	/// <param name="reverse_y">Y方向反転</param>
	void SetReverseY(const int reverse_y) 
	{
		if (_anim_state != nullptr)
		{
			_anim_state->anim_info.reverse_y = reverse_y;
		}

		if (_override_anim_state != nullptr)
		{
			_override_anim_state->anim_info.reverse_y = reverse_y;
		}
	}

	void SetBlendValue(const int blend_value)
	{
		_blend_value = blend_value; 
	}

private:
	bool _is_playing;

	struct AnimationState 
	{
		AnimationState(const AnimPlayInfo& anim_info)
			: anim_info(anim_info)
			, curernt_frame(0)
			, time(0.f)
		{}
		AnimationState()
			: anim_info()
			, curernt_frame(0)
			, time(0.f)
		{
		}
		AnimPlayInfo anim_info;
		int curernt_frame;;
		float time;
	};

	std::unique_ptr<AnimationState> _anim_state;
	std::unique_ptr<AnimationState> _override_anim_state;
	float _anim_override_time;
	int _blend_value;
};

// 静止画レンダラ
class InAnimateRenderer : public RendererBase
{
public:
	InAnimateRenderer();
	virtual ~InAnimateRenderer();

	//~ Begin RendererBase interface
protected:
	virtual void RendererDraw(const ScreenParams& screen_params) override;
	//~ End RendererBase interface

public:
	void SetIcon(const MasterDataID icon_id);
	void SetExtent(const Vector2D& extent) { _extent = extent; }
	void SetExRate(const float ex_rate) { _ex_rate = ex_rate; }

private:
	MasterDataID _icon_id;
	int _handle;
	Vector2D _extent;
	float _ex_rate;
};