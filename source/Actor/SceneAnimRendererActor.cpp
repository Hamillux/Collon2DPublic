#include "SceneAnimRendererActor.h"
#include "Component/Renderer/RendererComponent.h"

SceneAnimRendererActor::SceneAnimRendererActor()
{
}

SceneAnimRendererActor::~SceneAnimRendererActor()
{
}

void SceneAnimRendererActor::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);
	SetActorWorldPosition(Vector2D{ 0,0 });

	SetDrawAreaCheckIgnored(true);

	for (size_t i = 0; i < _anim_components.size(); ++i)
	{
		_anim_components[i] = CreateComponent<AnimRendererComponent>(this);
	}
}

void SceneAnimRendererActor::TickActor(float delta_seconds)
{
	__super::TickActor(delta_seconds);
}

void SceneAnimRendererActor::RequestToSetActorHidden(const bool new_hidden)
{
	// 非表示にしない
}

size_t SceneAnimRendererActor::PlayAnimation(const AnimPlayInfo& anim_play_info, const Transform& transform, Actor* const attach_to)
{
	size_t index = GetAvailableAnimComponentIndex();
	if (index == SIZE_MAX)
	{
		return -1;
	}

	_anim_components.at(index)->SetAnimation(anim_play_info);

	if (attach_to)
	{
		_anim_components.at(index)->AttachToSceneComponent(attach_to->GetRootComponent());
		_anim_components.at(index)->SetLocalTransform(transform.position, transform.rotation);
	}
	else
	{
		_anim_components.at(index)->DetachFromParentSceneComponent();
		_anim_components.at(index)->SetWorldTransform(transform.position, transform.rotation);
	}	

	return index;
}

void SceneAnimRendererActor::StopAnimation(const size_t scene_anim_index)
{
	if (scene_anim_index < _anim_components.size())
	{
		_anim_components.at(scene_anim_index)->Stop();
	}
}

size_t SceneAnimRendererActor::GetAvailableAnimComponentIndex() const
{
	for (size_t i = 0; i < _anim_components.size(); ++i)
	{
		if (_anim_components[i]->IsPlaying() == false)
		{
			return i;
		}
	}

	return SIZE_MAX;
}
