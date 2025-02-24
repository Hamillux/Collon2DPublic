#pragma once

#include "ActorParamTypes.h"
#include "Utility/IJsonSerializable.h"
#include <memory>

/// <summary>
/// アクターの初期化パラメータ
/// </summary>
struct ActorInitialParams : public IJsonObject, public IEditableParameter
{
	ActorInitialParams()
		: transform(Transform())
		, physics(Physics())
		, _draw_priority(0)
	{}
	virtual ~ActorInitialParams() {}

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& initial_params_json) const override;
	virtual void FromJsonObject(const nlohmann::json& initial_params_json) override;
	//~ End IJsonObject interface
	
	//~ Begin IEditableParameter interface
	virtual void AddToParamEditGroup
	(
		const std::shared_ptr<ParamEditGroup>& parent,
		const std::shared_ptr<CommandHistory>& command_history
	) override;
	//~ End IEditableParameter interface

	Transform transform;
	Physics physics;
	int _draw_priority;
};