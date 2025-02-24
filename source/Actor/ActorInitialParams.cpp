#include "ActorInitialParams.h"
#include "AllActorsInclude_generated.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace {
	constexpr const char* JKEY_DRAW_PRIORITY = "drawPriority";
	constexpr const char* JKEY_PHYSICS = "physics";
	constexpr const char* JKEY_TRANSFORM = "transform";

	constexpr const char* LABEL_DRAW_PRIORITY = "DrawPriority";
	constexpr const char* LABEL_PHYSICS = "Physics";
	constexpr const char* LABEL_TRANSFORM = "Transform";
}

void ActorInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	initial_params_json[JKEY_DRAW_PRIORITY] = _draw_priority;
	physics.ToJsonObject(initial_params_json[JKEY_PHYSICS]);
	transform.ToJsonObject(initial_params_json[JKEY_TRANSFORM]);
}

void ActorInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	initial_params_json.at(JKEY_DRAW_PRIORITY).get_to(_draw_priority);
	physics.FromJsonObject(initial_params_json.at(JKEY_PHYSICS));
	transform.FromJsonObject(initial_params_json.at(JKEY_TRANSFORM));
}

void ActorInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	/*AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_DRAW_PRIORITY,
		_draw_priority);*/

	/*constexpr size_t NUM_TRANSFORM_MEMBERS = 2;
	const std::shared_ptr<ParamEditGroup> transform_group = std::make_shared<ParamEditGroup>(LABEL_TRANSFORM, NUM_TRANSFORM_MEMBERS);
	transform.AddToParamEditGroup(transform_group, command_history);
	parent->AddChild(transform_group);*/

	/*constexpr size_t NUM_PHYSICS_MEMBERS = 3;
	const std::shared_ptr<ParamEditGroup> physics_group = std::make_shared<ParamEditGroup>(LABEL_PHYSICS, NUM_PHYSICS_MEMBERS);
	physics.AddToParamEditGroup(physics_group, command_history);
	parent->AddChild(physics_group);*/
}

void ActorInitialParams::AddToParamEditGroup_Transform(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	transform.AddToParamEditGroup(parent, command_history);
}

void ActorInitialParams::AddToParamEditGroup_Physics(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	physics.AddToParamEditGroup(parent, command_history);
}

void ActorInitialParams::AddToParamEditGroup_DrawPriority(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_DRAW_PRIORITY,
		_draw_priority
	);
}
