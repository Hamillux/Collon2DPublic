#include "ComponentBase.h"
#include "Scene/SceneBase.h"
#include "Actor/Actor.h"

void ComponentBase::Initialize()
{
}

void ComponentBase::Tick(const float delta_seconds)
{
}

void ComponentBase::Draw(const CameraParams& camera_params)
{}

void ComponentBase::DrawForeground(const CanvasInfo & canvas_info)
{
}

void ComponentBase::Finalize()
{
}

void ComponentBase::OnParentActorHiddenChanged(const bool new_hidden)
{
}

SceneBase* ComponentBase::GetScene() const
{
	return GetOwnerActor()->GetScene();
}

void ComponentBase::Sleep()
{
	should_call_component_tick = false;
	should_call_component_draw = false;
}

void ComponentBase::Awake()
{
	should_call_component_tick = true;
	should_call_component_draw = true;
}

void ComponentBase::SetComponentDrawPriority(const int new_draw_priority)
{
	_draw_priority = new_draw_priority;
	component_events.on_draw_priority_changed.Dispatch();
}
