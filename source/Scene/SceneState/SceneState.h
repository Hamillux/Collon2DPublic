#pragma once

#include "Input/DeviceInput.h"
#include "Utility/ImGui/ImGuiInclude.h"
#include "Utility/Core/Rendering/CanvasInfo.h"
#include "Utility/Core/Event.h"
#include <stack>
#include <stdexcept>
#include <type_traits>
#include <memory>

class SceneBase;
template<class ParentScene> class StackableSceneState;
template<class ParentScene> class SceneStateStack;

/// <summary>
/// SceneBaseインターフェースを備えた状態クラスの基底クラス
/// </summary>
/// <typeparam name="T">親シーンクラス</typeparam>
template<class ParentScene>
class SceneState : public EventListener
{
	static_assert(std::is_base_of_v<SceneBase, ParentScene>, "ParentSceneClass must be derived from SceneBase");
public:
	SceneState() {}
	virtual ~SceneState() {};
	using ParentSceneClass = ParentScene;

	//~ Begin SceneState interface
public:
	// 何もしない
	virtual void OnEnterState(ParentSceneClass& parent_scene) {}

	// 何もしない
	virtual void OnLeaveState(ParentSceneClass& parent_scene) {}

	/// <summary>
	/// ステートの更新
	/// </summary>
	/// <returns>遷移先ステート. nullptrの場合は現在のステートを継続</returns>
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) { return nullptr; }

	// 何もしない
	virtual void Draw(ParentSceneClass& parent_scene) {}

	// 何もしない
	virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) {}

	// 何もしない
	virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) {}

	// 何もしない
	virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) {}
	//~ End SceneState interface

protected:
	/// <summary>
	/// スタックのトップを0番目としてn番目の要素を取り出す
	/// </summary>
	/// <typeparam name="StackContent">スタックの要素の型</typeparam>
	/// <param name="stack">スタック</param>
	/// <param name="n">取り出す要素の番号. トップを0とする</param>
	/// <returns></returns>
	template<class StackContent>
	StackContent PopNthElement(std::stack<StackContent>& stack, const size_t n)
	{
		std::stack<StackContent> temp_stack;
		for (size_t i = 0; i < n; i++)
		{
			temp_stack.push(stack.top());
			stack.pop();
		}
		const StackContent ret = stack.top();
		stack.pop();
		while (!temp_stack.empty())
		{
			stack.push(temp_stack.top());
			temp_stack.pop();
		}

		return ret;
	}
};


#include <memory>
#include <stack>
/// <summary>
/// ステートスタック.
/// <para>NOTE:ステートのHandleInputは ImGuiのポップアップが非表示 && どのImGuiアイテムもアクティブでない 場合にのみ呼ばれる</para>
/// </summary>
template<class ParentSceneClass>
class SceneStateStack
{
	static_assert(std::is_base_of_v<SceneBase, ParentSceneClass>, "ParentSceneClass must be derived from SceneBase");
public:
	SceneStateStack() {}
	~SceneStateStack() {}

	std::shared_ptr<StackableSceneState<ParentSceneClass>>& top()
	{
		return _state_stack.top();
	}

	size_t size() const
	{
		return _state_stack.size();
	}

	bool empty() const
	{
		return _state_stack.empty();
	}

	void ChangeState(ParentSceneClass& parent_scene, std::shared_ptr<StackableSceneState<ParentSceneClass>> state)
	{
		if (!_state_stack.empty())
		{
			GetCurrentState()->OnLeaveState(parent_scene);
		}

		push(state);
		state->OnEnterState(parent_scene);
	}

	void Tick(ParentSceneClass& parent_scene, float delta_seconds)
	{
		const bool should_call_handle_input = !ImGui::IsAnyPopupOpen() && !ImGui::IsAnyItemActive();
		if (should_call_handle_input)
		{
			GetCurrentState()->HandleInput(parent_scene, delta_seconds);
		}

		auto next_state = GetCurrentState()->Tick(parent_scene, delta_seconds);
		if (next_state != nullptr)
		{
			std::shared_ptr<StackableSceneState<ParentSceneClass>> next_stackable_state 
				= std::dynamic_pointer_cast<StackableSceneState<ParentSceneClass>>(next_state);
			if (next_stackable_state == nullptr)
			{
				throw std::runtime_error("SceneStateStack can only handle StackableSceneState");
			}
			ChangeState(parent_scene, next_stackable_state);
		}

	}

	void Draw(ParentSceneClass& parent_scene)
	{
		GetCurrentState()->Draw(parent_scene);
	}

	void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
	{
		GetCurrentState()->DrawForeground(parent_scene, canvas_info);
	}

	void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds)
	{
		GetCurrentState()->UpdateCameraParams(parent_scene, delta_seconds);
	}

	void Finalize(ParentSceneClass& parent_scene)
	{
		GetCurrentState()->OnLeaveState(parent_scene);
		_state_stack.pop();
		_state_stack = std::stack<std::shared_ptr<StackableSceneState<ParentSceneClass>>>();
	}

	/// <summary>
	/// スタックのトップを0番目としてn番目の要素を取り出す
	/// </summary>
	/// <param name="n">取り出す要素の番号. トップを0とする</param>
	/// <returns></returns>
	std::shared_ptr<StackableSceneState<ParentSceneClass>> PopNthElement(const size_t n)
	{
		std::stack<std::shared_ptr<StackableSceneState<ParentSceneClass>>> temp_stack;
		for (size_t i = 0; i < n; i++)
		{
			temp_stack.push(_state_stack.top());
			_state_stack.pop();
		}
		const std::shared_ptr<StackableSceneState<ParentSceneClass>> ret = _state_stack.top();
		_state_stack.pop();
		while (!temp_stack.empty())
		{
			_state_stack.push(temp_stack.top());
			temp_stack.pop();
		}

		return ret;
	}
private:
	void push(std::shared_ptr<StackableSceneState<ParentSceneClass>> state)
	{
		state->SetStackRef(this);
		_state_stack.push(state);
	}

	std::shared_ptr<StackableSceneState<ParentSceneClass>> GetCurrentState() const
	{
		return _state_stack.top();
	}
	std::stack<std::shared_ptr<StackableSceneState<ParentSceneClass>>> _state_stack;
};



/// <summary>
/// ステートスタックへの参照をもつステートクラス
/// </summary>
template<class ParentSceneClass>
class StackableSceneState : public SceneState<ParentSceneClass>
{
	static_assert(std::is_base_of_v<SceneBase, ParentSceneClass>, "ParentSceneClass must be derived from SceneBase");
public:
	StackableSceneState()
		: _state_stack_ref(nullptr)
	{}
	virtual ~StackableSceneState() {}

	//~ Begin SceneState interface
public:
	/// <summary>
	/// - ShouldDestroyPreviousState()がtrueの場合, 1つ前のステートを破棄する.
	/// <para>- _next_stateをnullptrにリセットする</para>
	/// </summary>
	virtual void OnEnterState(ParentSceneClass& parent_scene) override
	{
		if (ShouldDestroyPreviousState())
		{
			PopPreviousStateFromStateStack();
		}

		_next_state.reset();
	}
	//virtual void OnLeaveState(ParentSceneClass& parent_scene) override;

	/// <summary>
	/// SetNextState()でセットされたステートを返す
	/// </summary>
	virtual std::shared_ptr<SceneState<ParentSceneClass>> Tick(ParentSceneClass& parent_scene, float delta_seconds) override
	{
		return _next_state;
	}
	//virtual void Draw(ParentSceneClass& parent_scene) override;
	//virtual void DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info) override;
	//virtual void UpdateCameraParams(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//virtual void HandleInput(ParentSceneClass& parent_scene, const float delta_seconds) override;
	//~ End SceneState interface

	//~ Begin StackableSceneState interface
private:
	/// <summary>
	/// このステートに遷移する際に前のステートを破棄するかどうか
	/// <para>NOTE: デフォルトはtrue.</para>
	/// <para>NOTE: ポーズステートのように、前のステートを戻るために残しておく必要がある場合はfalseを返すようにオーバーライドする</para>
	/// </summary>
	virtual bool ShouldDestroyPreviousState() const
	{
		return true;
	}
	//~ End StackableSceneState interface

public:
	void SetStackRef(SceneStateStack<ParentSceneClass>* state_stack_ref)
	{
		_state_stack_ref = state_stack_ref;
	}

protected:
	void SetNextState(std::shared_ptr<SceneState<ParentSceneClass>> next_state)
	{
		_next_state = next_state;
	}
	bool IsNextStateSet() const
	{
		return _next_state != nullptr;
	}

	/// <summary>
	/// 直前のステートをスタックから取り出す.
	/// <para>NOTE: 
	/// </summary>
	/// <returns></returns>
	std::shared_ptr<SceneState<ParentSceneClass>> PopPreviousStateFromStateStack()
	{
		if (_state_stack_ref == nullptr)
		{
			throw std::runtime_error("SceneStateStack reference is not set");
		}

		if (_state_stack_ref->top().get() != this)
		{
			throw std::runtime_error("PopPreviousStateFromStateStack() can only be called when this state is at the top of the stack");
		}

		// 前のステートが存在しない場合
		if (_state_stack_ref->size() == 1)
		{
			return nullptr;
		}

		return _state_stack_ref->PopNthElement(1);
	}

	/// <summary>
	/// 直前のステートに戻る
	/// <para>NOTE: 前のステートが存在しない場合は例外を投げる</para>
	/// </summary>
	/// <param name="parent_scene"></param>
	void ResumeLastState(ParentSceneClass& parent_scene)
	{
		if (IsNextStateSet())
		{
			return;
		}

		std::shared_ptr<SceneState<ParentSceneClass>> last_state = PopPreviousStateFromStateStack();
		if (last_state == nullptr)
		{
			throw std::runtime_error("No previous state to resume");
		}

		SetNextState(last_state);
	}

private:
	SceneStateStack<ParentSceneClass>* _state_stack_ref;
	std::shared_ptr<SceneState<ParentSceneClass>> _next_state;	// StackableSceneState::Tick()で返される
};