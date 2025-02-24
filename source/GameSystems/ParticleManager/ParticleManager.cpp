#include "ParticleManager.h"

ParticleManager& ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return instance;
}

bool ParticleManager::Init()
{
	SafeRelease(p_pm);
	p_pm = new ParticleManagerImpl();
	if (!p_pm)
	{
		return false;
	}

	if (p_pm->Init())
	{
		return true;
	}

	SafeRelease(p_pm);
	return false;
}

void ParticleManager::End()
{
	if (p_pm)
	{
		p_pm->End();
		SafeRelease(p_pm);
	}
}

bool ParticleManager::Spawn(const ParticleSpawnDesc& spawn_desc, const int num_spawn)
{
	if (!p_pm)
	{
		return false;
	}

	return p_pm->Spawn(spawn_desc, num_spawn);
}

void ParticleManager::Tick(const float delta_seconds)
{
	if (!p_pm)
	{
		return;
	}

	p_pm->Tick(delta_seconds);
}

void ParticleManager::DeactivateAllParticles()
{
	p_pm->DeactivateAllParticles();
}

void ParticleManager::Draw(const CameraParams& camera_params)
{
	if (!p_pm)
	{
		return;
	}

	p_pm->Draw(camera_params);
}

UINT ParticleManager::GetPoolCount() const
{
	return p_pm->GetPoolCount();
}

ParticleManager::ParticleManager()
	: p_pm(nullptr)
{}

ParticleManager::~ParticleManager()
{
	SafeRelease(p_pm);
}
