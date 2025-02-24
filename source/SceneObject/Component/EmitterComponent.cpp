#include "EmitterComponent.h"
#include "SceneObject/Actor/Actor.h"
#include "GameSystems/ParticleManager/ParticleManager.h"

EmitterComponent::EmitterComponent()
{
}

EmitterComponent::~EmitterComponent()
{
}

void EmitterComponent::Tick(float delta_seconds)
{
	ParticleManager& manager = ParticleManager::GetInstance();
	
	const float spawn_interval = 1.f / _emitter_params.spawn_rate;
	_spawn_timer += delta_seconds;

	int spawn_count = 0;
	while (_spawn_timer > spawn_interval)
	{
		_spawn_timer -= spawn_interval;
		++spawn_count;
	}

	if (spawn_count > 0)
	{
		_emitter_params.spawn_desc.world_transform = GetWorldTransform();
		manager.Spawn(_emitter_params.spawn_desc, spawn_count);
	}
}

void EmitterComponent::SetEmitterParams(const EmitterComponentParams& emitter_params)
{
	_emitter_params = emitter_params;
}
