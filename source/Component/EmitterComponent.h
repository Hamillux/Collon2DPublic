#pragma once

#include "SceneComponent.h"
#include "GameSystems/ParticleManager/Particle/ParticleSpawnDesc.h"

struct EmitterComponentParams;

/// <summary>
/// パーティクルを生成するコンポーネント
/// </summary>
class EmitterComponent : public SceneComponent
{
public:
	EmitterComponent();
	virtual ~EmitterComponent();

public:
	virtual void Tick(float delta_seconds);

public:
	void SetEmitterParams(const EmitterComponentParams& emitter_params);

private:
	std::unique_ptr<EmitterComponentParams> _emitter_params;
	float _spawn_timer;
};

/// <summary>
/// EmitterComponentの設定
/// </summary>
struct EmitterComponentParams
{
	float spawn_rate;
	ParticleSpawnDesc spawn_desc;
};