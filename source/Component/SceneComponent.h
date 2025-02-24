#pragma once

#include "Component/ComponentBase.h"
#include "Utility/Core/Math/Transform.h"

/// <summary>
/// 位置情報を持つコンポーネント
/// <para>SceneComponent同士で親子付けできる</para>
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
	
	void SetInheritParentRotation(const bool inherit) { inherit_parent_rotation = inherit; }

	bool HasParentSceneComponent() const { return parent_scene_component; }
	SceneComponent* GetParentSceneComponent() const { return parent_scene_component; }
	bool HasChildSceneComponent() const { return children_scene_component.size() != 0; }

	/// <summary>
	/// 別のSceneComponentにアタッチ
	/// </summary>
	void AttachToSceneComponent(SceneComponent* new_parent, const bool keep_world_transform = false);

	/// <summary>
	/// 親から切り離す
	/// </summary>
	void DetachFromParentSceneComponent();

private:
	/// <summary>
	/// 回転を[0, 2*PI)に正規化
	/// </summary>
	void NormalizeRotation();

	void AddChild(SceneComponent* child_to_add);
	void RemoveChild(SceneComponent* child_to_remove);

	SceneComponent* parent_scene_component;
	std::vector<SceneComponent*> children_scene_component;

	// 親のローカル空間での位置. 親がいない場合はワールド空間での位置
	Vector2D local_position;

	// 親のローカル空間での回転. 親がいない場合はワールド空間での回転
	float local_rotation;
	
	/// <summary>
	/// trueの場合, ワールド回転 = 親のワールド回転 + ローカル回転
	/// falseの場合, ワールド回転 = ローカル回転
	/// </summary>
	bool inherit_parent_rotation;
};