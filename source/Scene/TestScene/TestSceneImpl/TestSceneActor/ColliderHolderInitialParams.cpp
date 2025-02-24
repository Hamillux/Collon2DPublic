#include "ColliderHolderInitialParams.h"

namespace
{
	constexpr const char* JKEY_colliderType = "colliderType";
}


void ColliderHolderInitialParams::ToJsonObject(nlohmann::json& j) const
{
	__super::ToJsonObject(j);

	j[JKEY_colliderType] = collider_type;
}

void ColliderHolderInitialParams::FromJsonObject(const nlohmann::json& j)
{
	__super::FromJsonObject(j);
	j.at(JKEY_colliderType).get_to(collider_type);
}
