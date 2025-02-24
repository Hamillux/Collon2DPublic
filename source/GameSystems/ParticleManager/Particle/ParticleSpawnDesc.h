#pragma once
#include "Utility/IJsonSerializable.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include "Utility/Core/MathCore.h"
#include "Utility/Core/Math/Transform.h"

struct ParticleSpawnDesc : public IJsonObject
{
	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& json_object) const override;
	virtual void FromJsonObject(const nlohmann::json& json_object) override;
	//~ End IJsonObject interface

	// JSONとの相互変換に対応
	FloatRange life_time;
	FloatRange gravity_scale;
	FloatRange size;
	MasterDataID animation_id;
	float velocity_angle_deg;
	FloatRange initial_speed;

	// ソースコードからのみ変更可能
	Vector2D initial_position;
	Vector2D initial_velocity_normalized;
	Transform world_transform;	// initial_position, initial_velocity_normalizedをワールド変換する
	void SetSpawnTransform(const Vector2D& position, const Vector2D& velocity_normalized, const Transform& world_transform_)
	{
		initial_position = position;
		initial_velocity_normalized = velocity_normalized;
		world_transform = world_transform_;
	}

	ParticleSpawnDesc();
};