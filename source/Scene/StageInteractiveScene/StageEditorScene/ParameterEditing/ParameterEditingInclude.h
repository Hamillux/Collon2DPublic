#pragma once

#include "EditParamType.h"
#include "IEditableParameter.h"
#include "Validator/EditableParamValidator.h"
#include "ParamEditComponent/ParamEditGroup.h"
#include "ParamEditComponent/ParamEditNode.h"
#include "Utility/Command/BasicCommands.h"

/// <summary>
/// ParamEditGroupにParamEditNodeを子として追加する
/// </summary>
/// <typeparam name="TargetValueType">値変更対象変数の型</typeparam>
/// <typeparam name="EPT">EditParamType列挙子</typeparam>
/// <param name="parent">親グループ</param>
/// <param name="command_history">コマンド履歴</param>
/// <param name="label">ラベル</param>
/// <param name="target">編集対象変数</param>
/// <param name="in_validator">バリデータ</param>
template<EditParamType EPT, typename TargetValueType>
std::shared_ptr<ParamEditNode<EPT>> AddChildParamEditNodeToGroup(
	const std::shared_ptr<ParamEditGroup>& parent,
	const std::shared_ptr<CommandHistory>& command_history,
	const std::string& label,
	TargetValueType& target,
	const std::shared_ptr<typename ParamEditNode<EPT>::Validator>& in_validator = nullptr
)
{
	using EditValueType = typename ParamEditNode<EPT>::ValueType;

	std::shared_ptr<ParamEditNode<EPT>> ret = std::make_shared<ParamEditNode<EPT>>
		(
			label,
			static_cast<EditValueType>(target),
			[=, &target](const EditValueType& validated)
			{
				std::shared_ptr<CmdChangeValue<TargetValueType>> command = ChangeValueAndAddHistory(target, static_cast<TargetValueType>(validated), command_history);
				command->command_events.OnDo += [&parent]() {parent->events.OnValueChanged.Dispatch(); };
				command->command_events.OnUndo += [&parent]() {parent->events.OnValueChanged.Dispatch(); };
			},
			in_validator
		);

	parent->AddChild(ret);

	return ret;
}