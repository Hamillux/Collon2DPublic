#pragma once
#include "ParticleManagerImpl.h"

/// <summary>
/// パーティクルシステムのインターフェース. 本体はParticleManagerImpl
/// </summary>
class ParticleManager
{
public:
	static ParticleManager& GetInstance();

	template<class T>
	void SafeRelease(T*& p)
	{
		if (p)
		{
			delete p;
			p = nullptr;
		}
	}

	bool Init();
	void End();

	bool Spawn(const ParticleSpawnDesc& spawn_desc, const int num_spawn = 1);
	void Tick(const float delta_seconds);
	void DeactivateAllParticles();
	void Draw(const ScreenParams& screen_params);
	UINT GetPoolCount() const;

private:
	ParticleManager();
	~ParticleManager();

	// コピー,ムーブの禁止
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
	ParticleManager(ParticleManager&&) = delete;
	ParticleManager& operator=(ParticleManager&&) = delete;

	// ParticleManagerImplインスタンス
	ParticleManagerImpl* p_pm;
};