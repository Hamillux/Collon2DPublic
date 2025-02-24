#pragma once
#include "SceneObject/Actor/ActorInitialParams.h"
#include "Utility/Core/Math/Vector2D.h"

struct ProjectileInitialParams : public ActorInitialParams
{
	Vector2D initial_velocity;
};