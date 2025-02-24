#pragma once

#include "Component/Renderer/RendererComponent.h"
#include <stdint.h>

template<class T> class AnimatorComponent;

/// <summary>
/// アニメーションステート
/// </summary>
/// <typeparam name="AnimatorClass">AnimatorComponent派生クラス</typeparam>
template<class AnimatorClass>
struct AnimState
{
	// このステートで再生するアニメーション
	AnimPlayInfo anim_info;

	// ステートの更新処理. 引数はAnimator派生クラス. 戻り値は次のステートのID
	std::function<uint8_t(AnimatorClass*)> update_func;

	AnimState() {}
	AnimState(const AnimPlayInfo& anim_info_, const std::function<uint8_t(AnimatorClass*)>& update_func_)
		: anim_info(anim_info_)
		, update_func(update_func_)
	{}
};

// CRTP. AnimStateから参照されるので, Derivedにはステートの更新に必要な変数等を持たせる.
template<class Derived>
class AnimatorComponent : public AnimRendererComponent
{
public:
	AnimatorComponent()
		: _current_anim_state(0)
	{}
	virtual ~AnimatorComponent() {}

	//~ Begin ComponentBase interface
public:
	virtual void Tick(const float delta_seconds) override
	{
		__super::Tick(delta_seconds);
		const uint8_t next_state = _anim_states.at(_current_anim_state).update_func(static_cast<Derived*>(this));
		if (next_state != _current_anim_state)
		{
			_current_anim_state = next_state;
			SetAnimation(_anim_states.at(_current_anim_state).anim_info);
		}
	}
	//~ End ComponentBase interface

public:
	/// <summary>
	/// アニメーションステートを設定する
	/// </summary>
	/// <param name="anim_states">アニメーションステートのマップ</param>
	/// <param name="initial_state_id">初期ステート</param>
	void SetAnimStateMapping(const std::unordered_map<uint8_t, AnimState<Derived>>& anim_states, const uint8_t initial_state_id = 0)
	{
		_anim_states = anim_states;
		ChangeState(initial_state_id);
	}

	void ChangeState(const uint8_t new_anim_state_id)
	{
		_current_anim_state = new_anim_state_id;
		SetAnimation(_anim_states.at(new_anim_state_id).anim_info);
	}

private:
	uint8_t _current_anim_state;
	std::unordered_map<uint8_t, AnimState<Derived>> _anim_states;
};