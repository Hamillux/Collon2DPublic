#pragma once

#include "SceneObject/Component/ComponentBase.h"
#include "Utility/Core/Math/Transform.h"

/// <summary>
/// WorldTransform (world_position, world_rotation)と, LocalTransform (local_position, local_rotation)を持つクラス.
/// <para>SceneComponent同士で親子付けできる</para>
/// <para>親なしのSceneComponentではWorldTransformとLocalTransformが等しい</para>
/// </summary>
class SceneComponent : public ComponentBase
{
public:
	SceneComponent();
	virtual ~SceneComponent();

	//~ Begin ComponentBase interface
public:
	virtual void Finalize() override;
	//~ End ComponentBase interface

	//~ Begin SceneComponent interface
protected:
	virtual void OnThisWorldTransformChanged();
	//~ End SceneComponent interface

public:
	Transform GetWorldTransform() const;
	void SetWorldTransform(const Vector2D& new_world_position, const float new_world_rotation);
	void SetWorldTransform(const Transform& new_world_transform);
	void SetLocalTransform(const Vector2D& new_local_position, const float new_local_rotation);

	Vector2D GetWorldPosition() const;
	void SetWorldPosition(const Vector2D& new_world_position);
	Vector2D GetLocalPosition() const { return local_position; }
	void AddWorldPosition(const Vector2D& delta_position);
	void SetLocalPosition(const Vector2D& new_local_position);
	void AddActorLocalPosition(const Vector2D& delta_position);

	float GetWorldRotation() const;
	void SetWorldRotation(const float new_world_rotation);
	float GetLocalRotation() const { return local_rotation; }
	void SetLocalRotation(const float new_local_rotation);
	void AddRotation(const float delta_local_rotation);

	void EnableParentRotationInheritance();
	void DisableParentRotationInheritance();	
	

	bool HasParentSceneComponent() const { return parent_scene_component; }
	SceneComponent* GetParentSceneComponent() const { return parent_scene_component; }
	bool HasChildSceneComponent() const { return children_scene_component.size() != 0; }

	/// <summary>
	/// 別のSceneComponentにアタッチ
	/// </summary>
	/// <param name="new_parent"></param>
	void AttachToSceneComponent(SceneComponent* new_parent, const bool keep_world_transform = false);
	void DetachFromParentSceneComponent();

private:
	/// <summary>
	/// world_rotation, local_rotationを[0, 2*PI)に正規化
	/// </summary>
	void NormalizeRotation();

	void AddChild(SceneComponent* child_to_add);
	void RemoveChild(SceneComponent* child_to_remove);

	SceneComponent* parent_scene_component;
	std::vector<SceneComponent*> children_scene_component;

	// LocalTransform
	Vector2D local_position;
	float local_rotation;
	
	/// <summary>
	/// world_rotationの計算方法を決めるフラグ
	/// <para>trueの場合:</para>
	/// <para>world_rotation(子) = world_rotation(親) + local_rotation(子)</para>
	/// <para>falseの場合:</para>
	/// <para>world_rotation(子) = local_rotation(子)</para>
	/// </summary>
	bool inherit_parent_rotation;
};