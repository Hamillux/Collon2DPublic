#include "ColliderHolder.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/Collider/SegmentCollider.h"

ColliderHolder::ColliderHolder()
	: _collider(nullptr)
{
}

ColliderHolder::~ColliderHolder()
{
}

void ColliderHolder::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	using ParamsType = ColliderHolderInitialParams;
	auto params = dynamic_cast<const ParamsType*>(actor_params);
	switch (params->collider_type)
	{
	case ParamsType::COLLIDER_SHAPE_BOX:
		_collider = CreateComponent<BoxCollider>(this);
		static_cast<BoxCollider*>(_collider)->SetBoxColliderParams(
			CollisionType::BLOCK,
			CollisionObjectType::GROUND,
			{},
			false,
			Vector2D(64, 64)
		);
		break;
	case ParamsType::COLLIDER_SHAPE_SEGMENT:
		_collider = CreateComponent<SegmentCollider>(this);
		break;
	}


}

void ColliderHolder::TickActor(float delta_seconds)
{
	__super::TickActor(delta_seconds);
}

void ColliderHolder::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);
	_collider->Draw(camera_params);
}
