#include "SceneComponent.h"
#include "Actor/Actor.h"
#include "Scene/SceneBase.h"

SceneComponent::SceneComponent()
	: parent_scene_component(nullptr)
	, inherit_parent_rotation(true)
{}

SceneComponent::~SceneComponent()
{}

void SceneComponent::Finalize()
{
	if (HasParentSceneComponent())
	{
		DetachFromParentSceneComponent();
	}
	if (HasChildSceneComponent())
	{
		for (int i = children_scene_component.size() - 1; i >= 0; --i)
		{
			children_scene_component.at(i)->DetachFromParentSceneComponent();
		}
	}

	__super::Finalize();
}


void SceneComponent::OnThisWorldTransformChanged()
{
	if (HasChildSceneComponent())
	{
		for (auto& child : children_scene_component)
		{
			child->OnThisWorldTransformChanged();
		}
	}
}

Transform SceneComponent::GetWorldTransform() const
{
	if (!HasParentSceneComponent())
	{
		return Transform{ local_position, local_rotation };
	}

	const Vector2D world_position = GetWorldPosition();
	const float world_rotation = GetWorldRotation();
	return Transform{ world_position, world_rotation };
}

void SceneComponent::SetWorldTransform(const Vector2D& new_world_position, const float new_world_rotation)
{
	SetWorldPosition(new_world_position);
	SetWorldRotation(new_world_rotation);
}

void SceneComponent::SetWorldTransform(const Transform& new_world_transform)
{
	SetWorldTransform(new_world_transform.position, new_world_transform.rotation);
}

void SceneComponent::SetLocalTransform(const Vector2D& new_local_position, const float new_local_rotation)
{
	local_position = new_local_position;
	local_rotation = new_local_rotation;
	NormalizeRotation();
	OnThisWorldTransformChanged();
}

Vector2D SceneComponent::GetWorldPosition() const
{
	if (HasParentSceneComponent())
	{
		return parent_scene_component->GetWorldTransform().TransformLocation(local_position);
	}

	return local_position;
}

void SceneComponent::SetWorldPosition(const Vector2D& new_world_position)
{
	if (HasParentSceneComponent())
	{
		Transform parent_to_world = parent_scene_component->GetWorldTransform();
		local_position = parent_to_world.InverseTransformLocaltion(new_world_position);
	}
	else
	{
		local_position = new_world_position;
	}

	OnThisWorldTransformChanged();
}

void SceneComponent::AddWorldPosition(const Vector2D& delta_position)
{
	SetWorldPosition(GetWorldPosition() + delta_position);
}

void SceneComponent::SetLocalPosition(const Vector2D& new_local_position)
{
	local_position = new_local_position;
}

void SceneComponent::AddActorLocalPosition(const Vector2D& delta_position)
{
	SetLocalPosition(GetLocalPosition() + delta_position);
}

float SceneComponent::GetWorldRotation() const
{
	if (HasParentSceneComponent() && inherit_parent_rotation)
	{
		return parent_scene_component->GetWorldRotation() + local_rotation;
	}

	return local_rotation;
}

void SceneComponent::SetWorldRotation(const float new_world_rotation)
{
	if (HasParentSceneComponent() && inherit_parent_rotation)
	{
		local_rotation = new_world_rotation - parent_scene_component->GetWorldRotation();
	}
	else
	{
		local_rotation = new_world_rotation;
	}
	NormalizeRotation();
	OnThisWorldTransformChanged();
}

void SceneComponent::SetLocalRotation(const float new_local_rotation)
{
	local_rotation = new_local_rotation;
	NormalizeRotation();
	OnThisWorldTransformChanged();
}

void SceneComponent::AddRotation(const float delta_local_rotation)
{
	SetLocalRotation(GetLocalRotation() + delta_local_rotation);
}

void SceneComponent::AttachToSceneComponent(SceneComponent* new_parent, const bool keep_world_transform)
{
	if (!GetScene()->IsValid(new_parent))
	{
		throw std::runtime_error("Attempted to attach to invalid SceneComponent.");
	}

	const Transform world_transform = GetWorldTransform();

	if (parent_scene_component != nullptr)
	{
		// 既に別のSceneComponentにアタッチ済み
		DetachFromParentSceneComponent();
	}
	assert(!parent_scene_component);
	parent_scene_component = new_parent;
	new_parent->AddChild(this);

	if(keep_world_transform)
	{
		SetWorldTransform(world_transform);
	}
	else
	{
		SetLocalTransform(Vector2D(), 0.f);
	}

}

void SceneComponent::DetachFromParentSceneComponent()
{
	if (!parent_scene_component)
	{
		return;
	}

	parent_scene_component->RemoveChild(this);
	parent_scene_component = nullptr;
}

void SceneComponent::NormalizeRotation()
{
	constexpr float TWICE_PI = 2.f * DX_PI_F;

	float normalized_local_rotation = fmodf(local_rotation, CLN2D_TWO_PI);
	if (normalized_local_rotation < 0.f)
	{
		normalized_local_rotation += CLN2D_TWO_PI;
	}

	local_rotation = normalized_local_rotation;
}

void SceneComponent::AddChild(SceneComponent* child_to_add)
{
	std::vector<SceneComponent*>& children = children_scene_component;
	auto it = std::find(children.begin(), children.end(), child_to_add);
	if (it == children.end())
	{
		children_scene_component.push_back(child_to_add);
	}
}

void SceneComponent::RemoveChild(SceneComponent* child_to_remove)
{
	std::vector<SceneComponent*>& children = children_scene_component;
	auto it = std::find(children.begin(), children.end(), child_to_remove);
	if (it != children.end())
	{
		children.erase(it);
	}
}