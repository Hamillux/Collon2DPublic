#pragma once

#include "Utility/IJsonSerializable.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/IEditableParameter.h"
#include "Utility/Core/MathCore.h"

struct Physics : public IJsonObject, public IEditableParameter
{
	Physics()
		: gravity_scale(0.f)
		, mass(1.f)
		, k_air_friction(0.f)
	{}
	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& physics_json) const override;
	virtual void FromJsonObject(const nlohmann::json& physics_json) override;
	//~ End IJsonObject interface

	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	// 重力スケール
	float gravity_scale;

	// 質量
	float mass;

	// 空気抵抗係数 F = -kv
	float k_air_friction;
};