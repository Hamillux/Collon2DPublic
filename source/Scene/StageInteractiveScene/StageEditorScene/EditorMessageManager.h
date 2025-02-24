#pragma once

#include "Core.h"
#include "Utility/ImGui/ImGuiInclude.h"
#include <deque>

struct EditorMessageManagerDesc 
{
	int max_message_count;
	float display_time;
	ImVec2 message_card_size;
	ImVec4 card_back_color;
	ImVec4 card_front_color;
	ImVec4 card_border_color;
	ImVec2 window_pos;
	ImVec2 window_pivot;
	ImVec2 message_type_icon_size;
	ImVec2 GetWindowSize() const
	{
		return ImVec2
		(
			message_card_size.x,
			message_card_size.y * max_message_count
		);
	}
};

class EditorMessageManager 
{
public:
	EditorMessageManager();
	~EditorMessageManager();

	void Initialize(const EditorMessageManagerDesc& desc);
	void Tick(const float delta_seconds);
	void Finalize();
	void EnqueueMessage(const std::string& message);
private:
	void ShowMessages();

	struct Message 
	{
		std::string message;
		float remain_time;
	};

	std::deque<Message> _messages; // std::queueからstd::dequeに変更
	EditorMessageManagerDesc _desc;
};