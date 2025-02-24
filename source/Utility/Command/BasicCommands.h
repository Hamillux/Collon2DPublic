#pragma once
#include "CommandBase.h"
#include <DxLib.h>
#include <string>

template<typename T>
class CmdChangeValue : public CommandBase
{
public:
	CmdChangeValue(T& inTarget, const T& inAfterValue)
		: target(inTarget)
		, before(inTarget)
		, after(inAfterValue)
	{}
	//~ Begin CommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End CommandBase interface

private:
	T& target;
	const T before;
	const T after;
};

template<typename T>
inline void CmdChangeValue<T>::Undo()
{
	target = before;
}

template<typename T>
inline void CmdChangeValue<T>::Do()
{
	target = after;
}

/// <summary>
/// 値を変更してCommandHistoryにCmdChangeValueを追加する
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="target">値変更対象の変数</param>
/// <param name="new_value">変更後の値</param>
/// <param name="command_history"></param>
/// <returns>追加したCmdChangeValue</returns>
template<typename T>
std::shared_ptr<CmdChangeValue<T>> ChangeValueAndAddHistory(T& target, const T& new_value, const std::shared_ptr<CommandHistory>& command_history)
{
	std::shared_ptr<CmdChangeValue<T>> command = std::make_shared<CmdChangeValue<T>>(target, new_value);
	command_history->ExecuteAndPush(command);
	return command;
}