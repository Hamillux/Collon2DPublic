#pragma once

#include <list>
#include <functional>
#include <unordered_map>

using EventKey = size_t;
constexpr EventKey INVALID_EVENT_KEY = 0;

class EventListener 
{
public:
	EventListener() {}
	virtual ~EventListener() {}
};

/// <summary>
/// 可変長テンプレート引数は, バインドする関数の引数リスト
/// <para>
/// (ex)バインドする関数がvoid(int,float)であればEvent&lt;int,float&gt;
/// </para>
/// </summary>
/// <typeparam name="...Args"></typeparam>
template<typename... Args>
class Event
{
public:
	using Key = EventKey;

	Event()
		: _next_event_key(1)
	{}

	Key operator+=(const std::function<void(Args...)>& f);

	/// <summary>
	/// イベントに関数オブジェクトをバインドする
	/// </summary>
	/// <param name="f">コールバック</param>
	/// <returns>イベントのアンバインドに利用するキー</returns>
	Key Bind(const std::function<void(Args...)>& f);
	Key Bind(const std::function<void(Args...)>& f, const EventListener* const listener);

	/// <summary>
	/// イベントから関数オブジェクトをアンバインドする
	/// </summary>
	/// <param name="unbound_event_key">バインド時に取得したキー</param>
	void UnBind(const Key unbound_event_key);

	/// <summary>
	/// リスナーにバインドされた関数オブジェクトをすべてアンバインドする
	/// </summary>
	/// <param name="listener"></param>
	void UnBind(const EventListener* const listener);

	/// <summary>
	/// バインドされた関数オブジェクトをすべて実行
	/// </summary>
	/// <param name="...args"></param>
	void Dispatch(Args... args);
private:
	Key _next_event_key;
	std::unordered_map<Key, std::function<void(Args...)>> _key_to_callback_map;
	std::unordered_map<const EventListener*, std::vector<Key>> _listener_to_key_list_map;
};

template<typename ...Args>
inline typename Event<Args...>::Key Event<Args...>::operator+=(const std::function<void(Args...)>& f)
{
	return Bind(f);
}

template<typename ...Args>
inline typename Event<Args...>::Key Event<Args...>::Bind(const std::function<void(Args...)>& f)
{
	const Key this_event_key = _next_event_key++;
	_key_to_callback_map[this_event_key] = f;
	return this_event_key;
}

template<typename ...Args>
inline typename Event<Args...>::Key Event<Args...>::Bind(const std::function<void(Args...)>& f, const EventListener* const listener)
{
	Key ret = Bind(f);
	_listener_to_key_list_map[listener].push_back(ret);
	return ret;
}

template<typename ...Args>
inline void Event<Args...>::UnBind(const Key unbound_event_key)
{
	_key_to_callback_map.erase(unbound_event_key);
}

template<typename ...Args>
inline void Event<Args...>::UnBind(const EventListener* const listener)
{
	auto it_key_list = _listener_to_key_list_map.find(listener);
	if (it_key_list == _listener_to_key_list_map.end())
	{
		return;
	}

	for (auto& key : it_key_list->second)
	{
		_key_to_callback_map.erase(key);
	}
}

template<typename ...Args>
inline void Event<Args...>::Dispatch(Args ...args)
{
	for (auto& key_callback : _key_to_callback_map)
	{
		key_callback.second(args...);
	}
}
