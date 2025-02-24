#include "Transform.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"
#include <DxLib.h>

namespace {
	constexpr const char* JKEY_POSITION = "position";
	constexpr const char* JKEY_ROTATION = "rotation";

	constexpr const char* LABEL_POSITION = "Position";
	constexpr const char* LABEL_ROTATION = "Rotation";
}


void Transform::ToJsonObject(nlohmann::json& transform_json) const
{
	position.ToJsonValue(transform_json[JKEY_POSITION]);
	transform_json[JKEY_ROTATION] = rotation;
}

void Transform::FromJsonObject(const nlohmann::json& transform_json)
{
	position.FromJsonValue(transform_json.at(JKEY_POSITION));
	transform_json.at(JKEY_ROTATION).get_to(rotation);
}

void Transform::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	auto transform_group = std::make_shared<ParamEditGroup>("Transform", 2);

	AddChildParamEditNodeToGroup<EditParamType::FLOAT>(
		transform_group,
		command_history,
		LABEL_ROTATION,
		rotation
	);

	AddChildParamEditNodeToGroup<EditParamType::FLOAT2>(
		transform_group,
		command_history,
		LABEL_POSITION,
		position
	);

	parent->AddChild(transform_group);
}

Transform Transform::operator*(const Transform& rhs) const
{
	return Transform();
}

Matrix3x3 Transform::ToMatrix3x3() const
{
	return Matrix3x3(
		cosf(rotation), -sinf(rotation), position.x,
		sinf(rotation), cosf(rotation), position.y,
		0, 0, 1
	);
}

Matrix3x3 Transform::ToMatrix3x3Inverse() const
{
	return Matrix3x3(
		cosf(rotation), sinf(rotation), -position.x * cosf(rotation) - position.y * sinf(rotation),
		-sinf(rotation), cosf(rotation), position.x * sinf(rotation) - position.y * cosf(rotation),
		0, 0, 1
	);
}

Vector2D Transform::TransformLocation(const Vector2D& in_vec) const
{
	const Matrix3x3 mat = ToMatrix3x3();
	Vector2D out_vec;
	mat.TransformVector(in_vec, out_vec);
	return out_vec;
}

Vector2D Transform::TransformDirection(const Vector2D& in_vec) const
{
	Matrix3x3 mat(
		cosf(rotation), -sinf(rotation), 0,
		sinf(rotation), cosf(rotation), 0,
		0, 0, 1
	);
	Vector2D out_vec;
	mat.TransformVector(in_vec, out_vec);
	return out_vec;
}

float Transform::TransformRotation(const float in_rot) const
{
	return in_rot + rotation;
}

Vector2D Transform::InverseTransformLocaltion(const Vector2D& in_vec) const
{
	Matrix3x3 mat = ToMatrix3x3Inverse();
	Vector2D out_vec;
	mat.TransformVector(in_vec, out_vec);
	return out_vec;
}

float Transform::InverseTransformRotation(const float in_rot) const
{
	return in_rot - rotation;
}
