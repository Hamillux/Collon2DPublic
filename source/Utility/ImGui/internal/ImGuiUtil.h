#pragma once
#include <nlohmann/json.hpp>
#include <imgui.h>

namespace ImGui
{

void ShowJsonAsTreeNode(nlohmann::json& json_object);

} // end namespace ImGui
