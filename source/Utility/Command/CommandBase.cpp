#include "CommandBase.h"
#include <stdexcept>

CommandBase::~CommandBase()
{
}

void CommandBase::CallDo()
{
	Do();

	for (auto it = _subsequent_commands.begin(); it != _subsequent_commands.end(); it++)
	{
		(*it)->CallDo();
	}

	command_events.OnDo.Dispatch();
}

void CommandBase::CallUndo()
{
	for (auto it = _subsequent_commands.rbegin(); it != _subsequent_commands.rend(); it++)
	{
		(*it)->CallUndo();
	}

	Undo();

	command_events.OnUndo.Dispatch();
}

void CommandBase::AddSubsequentCommand(const std::shared_ptr<CommandBase>& subsequent_command)
{
	if (subsequent_command->IsSubsequentCommand(this))
	{
		throw std::runtime_error("Circular reference detected");
	}

	_subsequent_commands.push_back(subsequent_command);
}

bool CommandBase::IsSubsequentCommand(const CommandBase* const command) const
{
	for (auto& sub_command : _subsequent_commands)
	{
		if (sub_command.get() == command)
		{
			return true;
		}

		if (sub_command->IsSubsequentCommand(command))
		{
			return true;
		}
	}

	return false;
}

void CmdCommandList::Undo()
{
	for (int i = command_list.size() - 1; i >= 0; i--)
	{
		command_list.at(i)->CallUndo();
	}
}

void CmdCommandList::Do()
{
	for (auto& command : command_list)
	{
		command->CallDo();
	}
}
