#pragma once

#include "Core.h"
#include "Utility/SingletonBase.h"

class GameObject;

/// <summary>
/// システム時間での遅延処理や定期実行処理を管理するクラス
/// </summary>
class SystemTimer : public Singleton<SystemTimer>
{
	friend class Singleton<SystemTimer>;

public:
	virtual ~SystemTimer() {}

public:
	void Init();

	void Update();

	/// <summary>
	/// システム時間での遅延処理を作成する.
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理はキャンセルされる</param>
	/// <param name="delay_time">この関数呼び出しから処理実行までのワールド時間</param>
	/// <param name="process">実行する処理</param>
	void MakeDelayedEventSystem(const GameObject* const object, const float delay_time , const std::function<void()>& process);

	/// <summary>
	/// システム時間での定期実行処理を作成する. 初回実行はこの関数の呼び出しからintervalだけ経過した時に行われる
	/// </summary>
	/// <param name="object">処理を紐づけるGameObject. 実行タイミングでこれが無効の場合, 処理は破棄される</param>
	/// <param name="interval">実行間隔</param>
	/// <param name="process">実行する処理. 戻り値がfalseの場合, その処理は破棄される</param>
	void MakeRepeatingEventSystem(const GameObject* const object, const float interval, const std::function<bool()>& process);


private:
	SystemTimer();

	int _current_time;

	struct DelayedEvent
	{
		const GameObject* const listener;
		std::function<void()> process;
		float dispatch_time;

		DelayedEvent(const GameObject* const in_listener, const float in_dispatch_time, const std::function<void()>& in_process)
			: listener(in_listener)
			, dispatch_time(in_dispatch_time)
			, process(in_process)
		{}

		DelayedEvent()
			: listener(nullptr)
			, dispatch_time(0)
		{}
	};
	std::vector<std::unique_ptr<DelayedEvent>> _delayed_events;

	struct RepeatingEvent
	{
		const GameObject* const listener;
		std::function<bool()> process;
		float interval;
		float next_dispatch_time;

		RepeatingEvent(const GameObject* const in_listener, const float current_time, const float in_interval, const std::function<bool()>& in_process)
			: listener(in_listener)
			, interval(in_interval)
			, process(in_process)
			, next_dispatch_time(current_time + in_interval)
		{
		}

		RepeatingEvent()
			: listener(nullptr)
			, interval(0)
			, next_dispatch_time(0)
		{}
	};
	std::vector<std::unique_ptr<RepeatingEvent>> _repeating_events;
};