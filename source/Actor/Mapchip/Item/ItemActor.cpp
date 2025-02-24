#include "ItemActor.h"

#include "GameSystems/Sound/SoundManager.h"
#include "Component/Collider/HitResult.h"
#include "Actor/Character/Player/Player.h"
#include "Component/Collider/BoxCollider.h"
#include "Component/Renderer/RendererComponent.h"


ItemActor::ItemActor()
	: body_collider(nullptr)
	, renderer(nullptr)
	, _item_id(0)
{
}

ItemActor::~ItemActor()
{
}

void ItemActor::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);
	const ItemActorInitialParams* item_params = dynamic_cast<const ItemActorInitialParams*>(actor_params);
	assert(item_params);

	_item_id = item_params->item_id;

	// コライダー
	body_collider = CreateComponent<BoxCollider>(this);
	body_collider->SetBoxColliderParams(
		CollisionType::OVERLAP,
		CollisionObjectType::ITEM,
		{ CollisionObjectType::PLAYER },
		false,
		Vector2D(UNIT_TILE_SIZE, UNIT_TILE_SIZE)
	);

	const MdItem& md_item = MdItem::Get(item_params->item_id);
	renderer = CreateComponent<InAnimateRenderer>(this);
	renderer->SetIcon(md_item.icon_id);
	renderer->SetExtent(Vector2D(UNIT_TILE_SIZE, UNIT_TILE_SIZE));
}

void ItemActor::OnHitCollision(const HitResult& hit_result)
{
	__super::OnHitCollision(hit_result);

	// 衝突相手はプレイヤーのみ
	Actor* other_owner = hit_result.other_collider->GetOwnerActor();
	Player* player = dynamic_cast<Player*>(hit_result.other_collider->GetOwnerActor());
	if (player == nullptr)
	{
		throw std::runtime_error("ItemActor should be able to collide with only Player.");
	}

	player->GetItem(_item_id);

	MarkAsShouldDestroy();
}