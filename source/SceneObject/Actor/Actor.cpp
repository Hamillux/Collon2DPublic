#include "Actor.h"
#include "Scene/SceneBase.h"
#include "Input/DeviceInput.h"
#include "SceneObject/Component/Collider/HitResult.h"
#include "SceneObject/Component/ComponentBase.h"
#include "SceneObject/Component/SceneComponent.h"
#include "SceneObject/Component/MovementComponent.h"
#include "SceneObject/Component/Collider/ColliderBase.h"

std::unique_ptr<std::ofstream> debug_actor_log_body = nullptr;
std::ofstream& debug_actor_log = *debug_actor_log_body;
bool debug_show_log = false;
bool debug_out_file = false;

Actor::Actor()
	: _is_initialized(false)
	, _is_hidden(false)
	, _root_component(nullptr)
	, _movement_component(nullptr)
	, coef_of_velocity(Vector2D())
	, _is_physics_simulation_enabled(true)
	, _draw_priority(0)
	, _bounding_circle_radius(0.f)
	, _should_call_tick_actor(true)
	, _should_call_draw(true)
	, _should_draw_always(false)
	, _should_sort_components(false)
	, _should_destroy(false)
	, _is_velocity_update_disabled(false)
	, _parent_actor(nullptr)
{
	if (debug_out_file && !debug_actor_log_body)
	{
		debug_actor_log_body = std::make_unique<std::ofstream>("ActorLog.txt");
	}
}

Actor::~Actor()
{}

void Actor::Initialize(const ActorInitialParams* actor_params)
{
	assert(actor_params);

	if (!_root_component)
	{
		_root_component = CreateComponent<SceneComponent>(this);
	}

	SetActorWorldPosition(actor_params->transform.position);
	SetActorWorldRotation(actor_params->transform.rotation);
	SetDrawPriority(actor_params->_draw_priority);

	gravity_scale = actor_params->physics.gravity_scale;
	mass = actor_params->physics.mass;
	if (mass <= 0.f)
	{
		mass = 1.f;
	}
	k_air_resistance = actor_params->physics.k_air_friction;

	_bounding_circle_radius = UNIT_TILE_SIZE * 0.5f;

	_is_initialized = true;
}

void Actor::Finalize()
{
	for (auto& component : _components)
	{
		component->Finalize();
		GameObjectManager::GetInstance().DestroyObject(component);
	}
	_components.clear();

	DetachFromParentActor();
	_is_initialized = false;
	_is_hidden = false;
	_root_component = nullptr;
	_movement_component = nullptr;
	coef_of_velocity = Vector2D();
	_is_physics_simulation_enabled = true;
	_draw_priority = 0;
	_bounding_circle_radius = 0.f;
	_should_call_tick_actor = true;
	_should_call_draw = true;
	_should_draw_always = false;
	_should_sort_components = false;
	_should_destroy = false;
	_is_velocity_update_disabled = false;
	_parent_actor = nullptr;
}											  

bool Actor::IsDespawnable() const
{
	return false;
}

void Actor::TickActor(float delta_seconds)
{
	if(_is_physics_simulation_enabled)
	{
		UpdatePhysicsSimulation(delta_seconds);
	}
	else
	{
		AddWorldPosition(_movement_component->GetVelocity() * delta_seconds);
	}

	// コンポーネントの更新
	for (auto& component : _components)
	{
		if (component->ShouldCallTickActor())
		{
			component->Tick(delta_seconds);
		}
	}

	// Y座標が一定値を超えたら強制的に破棄
	constexpr float KILL_Y = UNIT_TILE_SIZE * 1000;
	if (GetActorWorldPosition().y > KILL_Y)
	{
		MarkAsShouldDestroy();
	}
}
void Actor::Draw(const ScreenParams& screen_params)
{
	if (_should_sort_components)
	{
		SortComponentsByDrawPriority();
		_should_sort_components = false;
	}

	// コンポーネントのDrawを呼ぶ
	for (auto& component : _components)
	{

		if (component->ShouldCallDraw() || true)
		{
			component->Draw(screen_params);
		}
	}
}

void Actor::GetWorldConvexPolygonVertices(std::vector<Vector2D>& out_vertices)
{
	out_vertices.reserve(4);
	const Vector2D actor_position = GetActorWorldPosition();
	int tiles_x, tiles_y;
	Vector2D snap_pos_to_actor_pos;
	GetOccupyingTileCounts(tiles_x, tiles_y, snap_pos_to_actor_pos);

	const float to_tiles_center_x = tiles_x % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0.f;
	const float to_tiles_center_y = tiles_y % 2 == 0 ? UNIT_TILE_SIZE * 0.5f : 0.f;
	const Vector2D tiles_center = 
		(GetActorWorldPosition() - snap_pos_to_actor_pos) + Vector2D(to_tiles_center_x, to_tiles_center_y);

	const float half_tiles_extent_x = UNIT_TILE_SIZE * 0.5f * tiles_x;
	const float half_tiles_extent_y = UNIT_TILE_SIZE * 0.5f * tiles_y;

	out_vertices.push_back(tiles_center + Vector2D{ -half_tiles_extent_x, -half_tiles_extent_y });
	out_vertices.push_back(tiles_center + Vector2D{ -half_tiles_extent_x, +half_tiles_extent_y });
	out_vertices.push_back(tiles_center + Vector2D{ +half_tiles_extent_x, +half_tiles_extent_y });
	out_vertices.push_back(tiles_center + Vector2D{ +half_tiles_extent_x, -half_tiles_extent_y });
}

void Actor::GetOccupyingTileCounts(int& out_tile_x, int& out_tile_y, Vector2D& out_snap_position_to_actor_position) const
{
	out_tile_x = 1;
	out_tile_y = 1;
	out_snap_position_to_actor_position = Vector2D{};
}

Vector2D Actor::GetSnapPosition() const
{
	return GetActorWorldPosition() - GetSnapPositionToActorPosition();
}

Vector2D Actor::GetSnapPositionToActorPosition() const
{
	int dummy_x, dummy_y;
	Vector2D snap_pos_to_actor_pos;
	GetOccupyingTileCounts(dummy_x, dummy_y, snap_pos_to_actor_pos);
	return snap_pos_to_actor_pos;
}

void Actor::DrawForeground(const CanvasInfo& canvas_info)
{
	// コンポーネントのUI描画
	for (const auto& component : _components)
	{
		component->DrawForeground(canvas_info);
	}
}

void Actor::OnHitCollision(const HitResult& hit_result)
{
	if (hit_result.self_collider->GetOwnerActor() != this)
	{
		throw std::runtime_error("self_collider's owner should be this");
		return;
	}
	if (hit_result.other_collider->GetOwnerActor() == this)
	{
		throw std::runtime_error("other_collider's owner should not be this");
		return;
	}
}

void Actor::RequestToSetActorHidden(const bool new_hidden)
{
	const bool is_changed = _is_hidden != new_hidden;
	_is_hidden = new_hidden;

	if (is_changed)
	{
		for (auto& component : _components)
		{
			component->OnParentActorHiddenChanged(new_hidden);
		}
	}
}

void Actor::ApplyDamage(const DamageInfo& damage_info)
{
	TakeDamage(damage_info);
}

void Actor::TakeDamage(const DamageInfo& damage_info)
{
}

bool Actor::ShouldCallTickActor() const
{
	return _should_call_tick_actor;
}

bool Actor::ShouldCallDraw() const
{
	return _should_call_draw;
}

bool Actor::IsHidden() const
{
	return _is_hidden;
}

bool Actor::IsInitialized() const
{
	return _is_initialized;
}

void Actor::SetShouldCallTickActor(const bool should_call)
{
	_should_call_tick_actor = should_call;
}

void Actor::SetShouldCallDraw(const bool shoud_call)
{
	_should_call_draw = shoud_call;
}

void Actor::UpdatePhysicsSimulation(const float delta_seconds)
{
	if (_is_velocity_update_disabled)
	{
		_is_velocity_update_disabled = false;
	}
	else
	{
		// 空気抵抗
		ApplyForce(_physics_velocity * coef_of_velocity);

		if (!move_force.IsZeroVector())
		{
			Vector2D move_dir = move_force.Normalize();
			Vector2D F_mvdir = move_dir * Vector2D::Dot(move_dir, GetAppliedForce());
			Vector2D V_mvdir = move_dir * Vector2D::Dot(move_dir, GetVelocity());
			ApplyForce(F_mvdir * (-1.f));
			ApplyForce(move_force);
			ApplyForce(V_mvdir * k_air_resistance * (-1.f));
		}

		const float N_dot_F = Vector2D::Dot(N_dir, GetAppliedForce());
		if (N_dir.Length() > EPSIRON && N_dot_F < 0.f)	// 垂直抗力の向き と 他の力の総和 が同じ向きになることは無い
		{
			//// 接触力を加える
			const Vector2D Fn = N_dir * (N_dot_F);
			const Vector2D Ft = GetAppliedForce() - Fn;

			// 垂直抗力
			ApplyForce(Fn * (-1.f));

			// 摩擦力
			const float fs = 4.f;	// 静止摩擦係数
			const float fd = 2.f;	// 動摩擦係数
			Vector2D velocity_n = N_dir * Vector2D::Dot(N_dir, _physics_velocity);
			Vector2D velocity_t_dir = (_physics_velocity - velocity_n).Normalize();
			Vector2D friction = velocity_t_dir * (-1.f) * (-N_dot_F) * fd;

			const float current_speed = _physics_velocity.Length();
			if (move_force.IsZeroVector() && current_speed < 20)
			{
				// 接線方向の成分を打ち消す
				ApplyForce(Ft * (-1.f));
				if (current_speed > 0.f)
				{
					SetVelocity(Vector2D());
				}
			}
			else if (move_force.IsZeroVector())
			{
				// 動摩擦
				constexpr float u_kinetic = 1.f;
				ApplyForce(velocity_t_dir * (-1.f) * (-N_dot_F) * u_kinetic);
			}
		}
		//// 力をもとに速度を更新
		Vector2D delta_velocity = (_applied_force / mass) * delta_seconds;
		_physics_velocity += delta_velocity;
	}

	// 速度をもとに位置を更新
	AddWorldPosition(_physics_velocity * delta_seconds);

	// 力をリセット
	CancelAppliedForce();
	SetNormalForceDirection(Vector2D());

	// 重力など
	ApplyConstantForce();
}

void Actor::AddChildActor(Actor* new_child)
{
	if (new_child)
	{
		_child_actors.push_back(new_child);
	}
}

void Actor::RemoveChildActor(Actor* child_to_remove)
{
	if (child_to_remove)
	{
		auto it = std::find(_child_actors.begin(), _child_actors.end(), child_to_remove);
		if (it != _child_actors.end())
		{
			_child_actors.erase(it);
		}
	}
}

void Actor::MarkAsShouldDestroy()
{
	if (_should_destroy)
	{
		return;
	}
	_should_destroy = true;

	for (auto& child_actor : _child_actors)
	{
		if (child_actor)
		{
			child_actor->DetachFromParentActor();
			child_actor->MarkAsShouldDestroy();
		}
	}
}

bool Actor::ShouldDestroy() const
{
	return _should_destroy;
}

SceneComponent* Actor::GetRootComponent() const
{
	return _root_component;
}

void Actor::AttachToOtherActor(Actor* attach_target, const bool keep_world_transform)
{
	_root_component->AttachToSceneComponent(attach_target->GetRootComponent(), keep_world_transform);
	_parent_actor = attach_target;
	_parent_actor->AddChildActor(this);
}

void Actor::DetachFromParentActor()
{
	if (!_parent_actor)
	{
		return;
	}
	_root_component->DetachFromParentSceneComponent();
	_parent_actor->RemoveChildActor(this);
	_parent_actor = nullptr;
}


Transform Actor::GetActorWorldTransform() const
{
	return Transform(GetActorWorldPosition(), GetActorWorldRotation());
}

Vector2D Actor::GetActorWorldPosition() const
{
	return _root_component->GetWorldPosition();
}

void Actor::SetActorWorldPosition(const Vector2D& new_position)
{
	_root_component->SetWorldPosition(new_position);
}

void Actor::AddWorldPosition(const Vector2D& delta_position)
{
	if (delta_position.IsZeroVector())
	{
		return;
	}
	SetActorWorldPosition(GetActorWorldPosition() + delta_position);
}

Vector2D Actor::GetActorLocalPosition() const
{
	return _root_component->GetLocalPosition();
}

void Actor::SetActorLocalPosition(const Vector2D& new_position)
{
	_root_component->SetLocalPosition(new_position);
}

void Actor::AddActorLocalPosition(const Vector2D& delta_position)
{
	SetActorLocalPosition(GetActorLocalPosition() + delta_position);
}

Vector2D Actor::GetVelocity() const
{
	if (_is_physics_simulation_enabled)
	{
		return _physics_velocity;
	}
	else if (_movement_component)
	{
		return _movement_component->GetVelocity();
	}

	return Vector2D();
}

void Actor::SetVelocity(const Vector2D& new_velocity, const bool dont_update_at_next_frame)
{
	if(_is_physics_simulation_enabled)
	{
		_physics_velocity = new_velocity;
		_is_velocity_update_disabled = dont_update_at_next_frame;
	}
	else if (_movement_component)
	{
		_movement_component->SetVelocity(new_velocity);
	}
}

void Actor::SetVelocity(const float new_x, const float new_y, const bool dont_update_at_next_frame)
{
	_physics_velocity.x = new_x;
	_physics_velocity.y = new_y;
	_is_velocity_update_disabled = dont_update_at_next_frame;
}

void Actor::AddVelocity(const Vector2D& delta_velocity)
{
	if(_is_physics_simulation_enabled)
	{
		SetVelocity(_physics_velocity + delta_velocity);
	}
	else if (_movement_component != nullptr)
	{
		_movement_component->SetVelocity(_movement_component->GetVelocity() + delta_velocity);
	}
}

void Actor::ApplyForce(const Vector2D& force)
{
	_applied_force += force;
}


void Actor::ApplyForcePropotionalToVelocity(const Vector2D& k)
{
	coef_of_velocity += k;
}

void Actor::ApplyMoveForce(const Vector2D& move_dir, const float max_move_speed)
{
	if (max_move_speed == 0.f || move_dir.IsZeroVector())
	{
		return;
	}

	// dV/dt = F/m - kV/m
	// V = F/k + Cexp(-kt/m)  (C:定数)
	// Vmax = F/k
	// F = kVmax
	move_force = move_dir * k_air_resistance * max_move_speed;
	return;
}

void Actor::SetNormalForceDirection(const Vector2D& new_N_dir)
{
	N_dir = new_N_dir;
}


void Actor::ApplyConstantForce()
{
	// 重力
	if (gravity_scale != 0.f)
	{
		ApplyForce(GetScene()->GetGravityForce() * gravity_scale * mass);
	}

	// 空気抵抗
	if (k_air_resistance != 0.f)
	{
		ApplyForcePropotionalToVelocity(Vector2D(1.f, 1.f) * (-k_air_resistance));
	}
}

void Actor::CancelAppliedForce()
{
	_applied_force *= 0.f;
	coef_of_velocity *= 0.f;
	move_force *= 0.f;
}

void Actor::SetPhysicsSimulationEnabled(const bool new_enabled)
{
	_is_physics_simulation_enabled = new_enabled;
}

float Actor::GetActorWorldRotation() const
{
	return _root_component->GetWorldRotation();
}

void Actor::SetActorWorldRotation(float new_rotation)
{
	_root_component->SetWorldRotation(new_rotation);
}

void Actor::AddActorRotation(const float delta_rotation)
{
	_root_component->AddRotation(delta_rotation);
}

float Actor::GetActorLocalRotation() const
{
	return _root_component->GetLocalRotation();
}

void Actor::SetActorLocalRotation(const float new_rotation)
{
	_root_component->SetLocalRotation(new_rotation);
}

void Actor::SetDrawPriority(const int new_priority)
{
	_draw_priority = new_priority;
	GetScene()->OnActorDrawPriorityChanged();
}

bool Actor::GetShouldDrawAlways() const
{
	return _should_draw_always;
}

void Actor::SetShouldDrawAlways(const bool new_should_draw_always)
{
	_should_draw_always = new_should_draw_always;
}

bool Actor::IsInDrawArea(const ScreenParams& screen_params) const
{
	const float d = (screen_params.world_offset - GetActorWorldPosition()).Length();
	const float r0 = _bounding_circle_radius;
	const float r1 = screen_params.GetScreenHalfExtentScaled().Length();
	return d <= r0 + r1;
}

float Actor::GetBoundingCircleRadius() const
{
	return _bounding_circle_radius;
}

FCircle Actor::GetBoundingCircle() const
{
	return FCircle{ GetActorWorldPosition(), GetBoundingCircleRadius()};
}

void Actor::OnComponentDrawPriorityChanged()
{
	_should_sort_components = true;
}

void Actor::SetBoundingCircleRadius(const float new_radius)
{
	_bounding_circle_radius = new_radius;
}

void Actor::SortComponentsByDrawPriority()
{
	std::sort(_components.begin(), _components.end(), [](const ComponentBase* comp1, const ComponentBase* comp2)
	{
		const int priority1 = comp1->GetComponentDrawPriority();
		const int priority2 = comp2->GetComponentDrawPriority();

		return comp1->GetComponentDrawPriority() < comp2->GetComponentDrawPriority();
	});

}