#include "MovementComponent.h"
#include "SceneObject/Actor/Actor.h"

MovementComponent::MovementComponent()
{
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Initialize()
{
	__super::Initialize();
	GetOwnerActor()->SetPhysicsSimulationEnabled(false);
}
