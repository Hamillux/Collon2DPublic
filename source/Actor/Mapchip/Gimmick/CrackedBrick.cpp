#include "CrackedBrick.h"
#include "Component/Renderer/RendererComponent.h"
#include "Component/Collider/BoxCollider.h"
#include "GameSystems/ParticleManager/ParticleManager.h"
#include "GameSystems/Sound/SoundManager.h"

CrackedBrick::CrackedBrick()
{
}

CrackedBrick::~CrackedBrick()
{
}

void CrackedBrick::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	auto renderer = CreateComponent<InAnimateRenderer>(this);
	renderer->SetIcon(MasterDataID(357));
	
	auto collider = CreateComponent<BoxCollider>(this);
	collider->SetBoxColliderParams(
		CollisionType::BLOCK,
		CollisionObjectType::GROUND,
		{ CollisionObjectType::WILDCARD },
		false,
		Vector2D{ UNIT_TILE_SIZE, UNIT_TILE_SIZE }
	);

	_sound_instance_destroyed = SoundManager::GetInstance().MakeSoundInstance("resources/sounds/se/ingame_scene/se_cracked_block_destroyed.ogg");
	_sound_instance_destroyed->SetVolume(50);
	_sound_instance_destroyed->SetPlayToEndWhenDestroyed(true);
}

void CrackedBrick::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);
}

void CrackedBrick::TakeDamage(const DamageInfo& damage_info)
{
	__super::TakeDamage(damage_info);

	if (damage_info.damage_type == DamageType::Crush)
	{
		MarkAsShouldDestroy();

		ParticleSpawnDesc desc{};
		
		desc.FromJsonObject(MasterHelper::GetParticleJson(MasterDataID(2)));
		desc.SetSpawnTransform(GetActorWorldPosition(), Vector2D{ 0,-1 }, Transform{ Vector2D{}, 0.f });
		ParticleManager::GetInstance().Spawn(desc, 20);

		_sound_instance_destroyed->Play();
	}
}
