#pragma once

#include <memory>

class ParamEditGroup;
class CommandHistory;

/// <summary>
/// ステージエディタで編集可能なパラメータのインターフェース
/// </summary>
class IEditableParameter
{
public:
	virtual ~IEditableParameter() {}
	virtual void AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history) = 0;
};