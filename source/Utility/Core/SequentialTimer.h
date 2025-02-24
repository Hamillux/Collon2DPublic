#pragma once

#include "Event.h"

class SequentialTimer
{
public:
	SequentialTimer(const std::vector<float> rest_time_list, const float initial_rest_time = 0.f)
		: _time(0.f)
		, _time_offset(-initial_rest_time)
		, _rest_time_index(0)
		, _next_dispatch_time(0.f)
		, _rest_time_list()
	{
	}

	void Bind(const std::function<void()>& f, const EventListener* const listener)
	{
		_event.Bind(f, listener);
	}

	void UnBind(const EventListener* const listener)
	{
		_event.UnBind(listener);
	}

	void Update(const float delta_seconds)
	{
		if (_time + _time_offset >= _next_dispatch_time)
		{
			_event.Dispatch();
			_next_dispatch_time = _time + _rest_time_list.at(_rest_time_index);
			if (_rest_time_index < _rest_time_list.size() - 1)
			{
				_rest_time_index++;
			}
		}

		_time += delta_seconds;
	}

	void Reset()
	{
		_time = 0.f;
		_rest_time_index = 0;
		_next_dispatch_time = 0.f;
	}

private:
	float _time;
	float _next_dispatch_time;
	size_t _rest_time_index;
	const float _time_offset;
	const std::vector<float> _rest_time_list;
	Event<> _event;
};