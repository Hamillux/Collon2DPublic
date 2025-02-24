#pragma once

#include "Utility/IJsonSerializable.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/IEditableParameter.h"
#include "Vector2D.h"
#include "Matrix3X3.h"

struct Transform : public IJsonObject, public IEditableParameter
{
	Transform()
		: position(Vector2D())
		, rotation(0.f)
	{
	}
	Transform(const Vector2D& in_pos, const float in_rot)
		: position(in_pos)
		, rotation(in_rot)
	{
	}

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& transform_json) const override;
	virtual void FromJsonObject(const nlohmann::json& transform_json) override;
	//~ End IJsonObject interface

	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	Transform operator*(const Transform& rhs) const;
	Matrix3x3 ToMatrix3x3() const;
	Matrix3x3 ToMatrix3x3Inverse() const;
	Vector2D TransformLocation(const Vector2D& in_vec) const;
	Vector2D TransformDirection(const Vector2D& in_vec) const;
	float TransformRotation(const float in_rot) const;
	Vector2D InverseTransformLocaltion(const Vector2D& in_vec) const;
	float InverseTransformRotation(const float in_rot) const;

	Vector2D position;
	float rotation;
};