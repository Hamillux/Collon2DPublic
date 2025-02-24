#include "ParticleSpawnDesc.h"

void ParticleSpawnDesc::ToJsonObject(nlohmann::json& jobj) const
{
	life_time.ToJsonValue(jobj["lifeTime"]);
	gravity_scale.ToJsonValue(jobj["gravityScale"]);
	size.ToJsonValue(jobj["size"]);
	jobj["animationId"] = animation_id;
	jobj["velocityAngle"] = velocity_angle_deg;
	initial_speed.ToJsonValue(jobj["initialSpeed"]);
}

void ParticleSpawnDesc::FromJsonObject(const nlohmann::json& jobj)
{
	life_time.FromJsonValue(jobj.at("lifeTime"));
	gravity_scale.FromJsonValue(jobj.at("gravityScale"));
	size.FromJsonValue(jobj.at("size"));
	jobj.at("animationId").get_to(animation_id);
	jobj.at("velocityAngle").get_to(velocity_angle_deg);
	initial_speed.FromJsonValue(jobj.at("initialSpeed"));
}

ParticleSpawnDesc::ParticleSpawnDesc()
	: life_time(1.f)
	, gravity_scale(0.f)
	, size(16.f)
	, animation_id(INVALID_MASTER_ID)
	, velocity_angle_deg(0.f)
	, initial_speed(100.f)
	, initial_position()
	, initial_velocity_normalized(1.f, 0.f)
	, world_transform()
{
}
