#include "ImGuiUtil.h"

void ImGui::ShowJsonAsTreeNode(nlohmann::json& json_object)
{
	for (auto& child_property : json_object.items())
	{
		const std::string& key = child_property.key();
		nlohmann::json& value = child_property.value();

		if (value.is_object())
		{
			if (ImGui::TreeNode(key.c_str()))
			{
				ShowJsonAsTreeNode(value);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::Text("%s", key.c_str());
			ImGui::SameLine();
			ImGui::Button("value");

			auto type = value.type();
			using ValueType = nlohmann::json::value_t;
			switch (value.type())
			{
			case ValueType::array:
				break;
			case ValueType::boolean:
				break;
			case ValueType::number_float:
				break;
			case ValueType::number_integer:
				break;
			}
		}
	}
}
