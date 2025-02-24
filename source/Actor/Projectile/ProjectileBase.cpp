#include "ProjectileBase.h"
#include "Actor/Character/Character.h"
#include "Component/Collider/HitResult.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/ProjectileMovementComponent.h"
#include "Scene/SceneBase.h"

ProjectileBase::ProjectileBase()
	: _projectile_movement(nullptr)
{}

ProjectileBase::~ProjectileBase()
{}

void ProjectileBase::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	using InitialParamsType = initial_params_of_actor_t<ProjectileBase>;
	const InitialParamsType* projectile_params = dynamic_cast<const InitialParamsType*>(actor_params);

	_projectile_movement = CreateComponent<ProjectileMovementComponent>(this);
	_projectile_movement->SetVelocity(projectile_params->initial_velocity);

	// 生成から一定時間経過後に破壊される
	// constexpr float lifetime = 5.f;
	// GetScene()->MakeDelayedEventWorld(lifetime, [this]() {MarkAsShouldDestroy(); });
}

void ProjectileBase::Finalize()
{
	__super::Finalize();
}

ProjectileMovementComponent* ProjectileBase::GetProjectileMovement() const
{
	return _projectile_movement;
}
