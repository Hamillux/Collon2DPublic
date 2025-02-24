#include "ProjectileMovementComponent.h"
#include "Scene/SceneBase.h"

ProjectileMovementComponent::ProjectileMovementComponent()
    : max_speed(500.f)
    , is_homing_enabled(false)
    , homing_target(nullptr)
    , homing_acceleration(0.f)
    , coef_restitution(1.f)
    , directional_acceleration(0.f)
    , gravity_scale(0.f)
{
}

ProjectileMovementComponent::~ProjectileMovementComponent()
{
}

void ProjectileMovementComponent::Tick(const float delta_seconds)
{
    __super::Tick(delta_seconds);

    UpdateVelocity(delta_seconds);

    GetOwnerActor()->AddWorldPosition(GetVelocity() * delta_seconds);
}

Vector2D ProjectileMovementComponent::GetVelocity() const
{
    return _current_velocity;
}

void ProjectileMovementComponent::SetVelocity(const Vector2D& new_velocity)
{
    _current_velocity = new_velocity;
}

void ProjectileMovementComponent::SetHomingTarget(const Actor* const in_homing_target, const bool enable_homing)
{
	homing_target = in_homing_target;

	is_homing_enabled = enable_homing;
}

void ProjectileMovementComponent::SetHomingEnabled()
{
    is_homing_enabled = true;
}

void ProjectileMovementComponent::UpdateVelocity(const float delta_seconds)
{
    Vector2D new_velocity = _current_velocity;
    if(gravity_scale != 0.f)
    {
        new_velocity += GetScene()->GetGravityForce() * delta_seconds;
    }

    if (is_homing_enabled && IsValid(homing_target))
    {
        const Vector2D owner_to_target = homing_target->GetActorWorldPosition() - GetOwnerActor()->GetActorWorldPosition();
        const Vector2D homing_force_direction = (owner_to_target - new_velocity.Normalize() * (Vector2D::Dot(new_velocity.Normalize(), owner_to_target))).Normalize();
        new_velocity += owner_to_target.Normalize() * homing_acceleration * delta_seconds;
    }

    const float delta_directional_speed = directional_acceleration * delta_seconds;
    const float new_directional_speed = std::min(new_velocity.Length() + delta_directional_speed, max_speed);

    new_velocity = new_velocity.Normalize() * new_directional_speed;

    _current_velocity = new_velocity;
}
