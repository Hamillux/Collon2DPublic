#include "ParamEditGroup.h"
#include "ParamEditNode.h"
#include "Utility/ImGui/ImGuiInclude.h"

ParamEditGroup::ParamEditGroup(const std::string in_name, std::vector<std::shared_ptr<ParamEditComponent>>&& in_children, const std::shared_ptr<ParamEditGroupStyle>& in_style)
	: name(in_name)
	, children(in_children)
	, style(in_style)
{
	for (auto& child : children)
	{
		BindEventsOnChild(child);
	}
}

ParamEditGroup::ParamEditGroup(const std::string in_name, const std::vector<std::shared_ptr<ParamEditComponent>>& in_children, const std::shared_ptr<ParamEditGroupStyle>& in_style)
	: name(in_name)
	, children(in_children)
	, style(in_style)
{
	for (auto& child : children)
	{
		BindEventsOnChild(child);
	}
}

ParamEditGroup::ParamEditGroup(const std::string& in_name, const size_t children_size, const std::shared_ptr<ParamEditGroupStyle>& in_style)
	: name(in_name)
	, children()
	, style(in_style)
{
	children.reserve(children_size);
}

ParamEditGroup::ParamEditGroup()
{
}

void ParamEditGroup::AddChild(std::shared_ptr<ParamEditComponent> newChild)
{
	BindEventsOnChild(newChild);
	children.push_back(newChild);
}

void ParamEditGroup::ShowAsImguiTreeElement_Impl(int& id)
{
	if (id < 0)
	{
		throw std::runtime_error("id must be greater than or equal to 0");
	}

	if (id++ == 0)
	{
		if(ShouldPushStyle())
		{
			PushGroupStyles();
		}

		for (auto& child : children)
		{
			call_ShowAsImguiTreeElement_Impl(child, id);
		}

		if(ShouldPushStyle())
		{
			PopGroupStyles();
		}
		return;
	}

	// ImGuiCol_HeaderをPushしても指定色になるのはラベルの文字部分だけ.
	// ChildWindowの右端まで色を塗りたい場合は以下のように自前で描画する必要がある
	{
		const ImVec4 bar_color = ImGui::GetStyleColorVec4(ImGuiCol_Header);
		const ImVec2 rmin = ImGui::GetCursorScreenPos();
		const float avail = ImGui::GetContentRegionAvail().x;
		const ImVec2 rmax = rmin + ImVec2(avail, ImGui::GetFontSize());
		ImGui::GetWindowDrawList()->AddRectFilled(rmin, rmax, ImGui::GetColorU32(bar_color));
	}

	if(ShouldPushStyle())
	{
		PushGroupStyles();
	}

	if (ImGui::TreeNode(name.c_str()))
	{
		for (auto& child : children)
		{
			call_ShowAsImguiTreeElement_Impl(child, id);
		}
	
		ImGui::TreePop();
	}

	if (ShouldPushStyle())
	{
		PopGroupStyles();
	}
}

void ParamEditGroup::Test()
{
	static int param0 = 0;
	using E = EditParamType;
	auto p_param0 = std::make_shared<ParamEditNode<E::INT>>(
		"param0",
		param0,
		[](const int& x)
	{
		param0 = x;
	},
		std::make_shared<EditParamValidator_Clamp<E::INT>>(-10, 10)
	);

	static std::string param1 = u8"文字列";
	auto p_param1 = std::make_shared<ParamEditNode<E::STRING>>(
		"param1",
		param1,
		[](const std::string& s)
	{
		param1 = s;
	},
		std::make_shared<EditParamValidator_MaxStringLength>(10)
	);

	static bool param2 = false;
	auto p_param2 = std::make_shared<ParamEditNode<E::BOOL>>(
		"param2",
		param2,
		[](const bool& b)
	{
		param2 = b;
	});

	const auto group_style = std::make_shared<ParamEditGroupStyle>();
	group_style->bar_color = ImVec4(1, 1, 1, 1);
	group_style->bar_color_hovered = ImVec4(1, 0, 0, 1);
	group_style->bar_color_active = ImVec4(0, 1, 0, 1);

	auto p_subgroup = std::make_shared<ParamEditGroup>("SubGroup", std::vector<std::shared_ptr<ParamEditComponent>>{p_param2}, group_style);

	int local_id = 0;
	ParamEditGroup group("Root", { p_param0, p_subgroup, p_param1 });
	group.ShowAsImguiTreeElement_Impl(local_id);

}

void ParamEditGroup::BindEventsOnChild(std::shared_ptr<ParamEditComponent> child)
{
	child->events.OnValueChanged += [this]()
	{
		this->events.OnValueChanged.Dispatch();
	};
}

namespace
{
	constexpr int GROUP_STYLES_NUM = 3;
}// end anonymous namespace
void ParamEditGroup::PushGroupStyles()
{
	if (style == nullptr)
	{
		return;
	}
	ImGui::PushStyleColor(ImGuiCol_Header, style->bar_color);
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style->bar_color_hovered);
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, style->bar_color_active);
}

void ParamEditGroup::PopGroupStyles()
{
	if (style == nullptr)
	{
		return;
	}
	ImGui::PopStyleColor(GROUP_STYLES_NUM);
}
