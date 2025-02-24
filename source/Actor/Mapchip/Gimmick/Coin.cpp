#include "Coin.h"
#include "Component/Renderer/RendererComponent.h"
#include "Component/Collider/BoxCollider.h"
#include "Actor/Character/Player/Player.h"

Coin::Coin()
{
}

Coin::~Coin()
{
}

void Coin::Initialize(const initial_params_of_actor_t<Actor>* const actor_params)
{
	__super::Initialize(actor_params);

	InAnimateRenderer* renderer = CreateComponent<InAnimateRenderer>(this);
	renderer->SetIcon(360);

	BoxCollider* collider = CreateComponent<BoxCollider>(this);
	collider->SetBoxColliderParams(
		CollisionType::OVERLAP,
		CollisionObjectType::GIMMICK,
		{ CollisionObjectType::PLAYER },
		false,
		Vector2D{ UNIT_TILE_SIZE, UNIT_TILE_SIZE }
	);

	
}

void Coin::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	Player* hit_player = dynamic_cast<Player*>(hit_result.other_collider->GetOwnerActor());
	if(IsValid(hit_player))
	{
		hit_player->AddCoin(1);
		MarkAsShouldDestroy();
	}
}
