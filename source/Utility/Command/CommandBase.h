#pragma once
#include <vector>
#include <memory>
#include "Utility/Core/Event.h"

class CommandBase
{
public:
	CommandBase() {}
	virtual~CommandBase();

	struct CommandEvents
	{
		Event<> OnDo;
		Event<> OnUndo;
	};
	CommandEvents command_events;

	//~ Begin CommandBase interface
public:
	virtual void OnErasedFromHistory() {}
protected:
	virtual void Undo() = 0;
	virtual void Do() = 0;
	//~ End CommandBase interface

public:
	void CallDo();

	void CallUndo();

	void AddSubsequentCommand(const std::shared_ptr<CommandBase>& subsequent_command);

	bool IsSubsequentCommand(const CommandBase* const command) const;

private:
	std::vector<std::shared_ptr<CommandBase>> _subsequent_commands;
};

class CmdCommandList : public CommandBase
{
public:
	CmdCommandList(const std::vector<std::shared_ptr<CommandBase>>& in_command_list)
		: command_list(in_command_list)
	{}

	//~ Begin CommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End CommandBase interface

private:
	std::vector<std::shared_ptr<CommandBase>> command_list;
};


/// <summary>
/// state0 -cmd0-> state1 -cmd1-> state2 ...
/// </summary>
class CommandHistory
{
public:
	struct CommandEvents 
	{
		Event<> OnStateChanged;
		Event<const std::shared_ptr<CommandBase>&> OnNewCommandPushed;
	};
	CommandEvents command_history_events;

	CommandHistory()
		: _current_state(0)
	{}

	void ExecuteAndPush(const std::shared_ptr<CommandBase>& new_command)
	{
		Push(new_command, true);
	}

	void Push(const std::shared_ptr<CommandBase>& new_command, const bool should_execute = false)
	{
		auto it_erase_first = history.begin() + _current_state;
		auto it_erase_last = history.end();
		for (auto it = it_erase_first; it != it_erase_last; it++)
		{
			(*it)->OnErasedFromHistory();
		}
		history.erase(it_erase_first, it_erase_last);

		new_command->CallDo();

		history.push_back(new_command);
		_current_state++;

		command_history_events.OnNewCommandPushed.Dispatch(new_command);
		command_history_events.OnStateChanged.Dispatch();
	}

	void Undo()
	{
		if (_current_state > 0)
		{
			history.at(_current_state - 1)->CallUndo();
			--_current_state;
			command_history_events.OnStateChanged.Dispatch();
		}
	}

	void Redo()
	{
		if (_current_state < history.size())
		{
			history.at(_current_state)->CallDo();
			++_current_state;
			command_history_events.OnStateChanged.Dispatch();
		}
	}

	size_t GetCurrentState() const { return _current_state; }
	size_t GetNumHistory() const { return history.size(); }

private:
	std::vector<std::shared_ptr<CommandBase>> history;
	size_t _current_state;
};