#include "SystemTimer.h"
#include "GameObject.h"
#include <DxLib.h>

void SystemTimer::Init()
{
	_current_time = DxLib::GetNowCount();

	_delayed_events.reserve(100);
	_repeating_events.reserve(100);
}

void SystemTimer::Update()
{
	_current_time = DxLib::GetNowCount();

	for (auto it = _delayed_events.begin(); it != _delayed_events.end();)
	{
		if ((*it)->dispatch_time <= static_cast<float>(_current_time))
		{
			(*it)->process();
			it = _delayed_events.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto it = _repeating_events.begin(); it != _repeating_events.end();)
	{
		if ((*it)->next_dispatch_time <= static_cast<float>(_current_time))
		{
			if (!(*it)->process())
			{
				it = _repeating_events.erase(it);
			}
			else
			{
				(*it)->next_dispatch_time += (*it)->interval;
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

void SystemTimer::MakeDelayedEventSystem(const GameObject* const listener, const float delay_time, const std::function<void()>& process)
{
	_delayed_events.push_back(std::make_unique<DelayedEvent>(listener, static_cast<float>(_current_time) + delay_time, process));
}

void SystemTimer::MakeRepeatingEventSystem(const GameObject* const listener, const float interval, const std::function<bool()>& process)
{
	_repeating_events.push_back(std::make_unique<RepeatingEvent>(listener, static_cast<float>(_current_time), interval, process));
}

SystemTimer::SystemTimer()
	: _current_time(0)
{
}
