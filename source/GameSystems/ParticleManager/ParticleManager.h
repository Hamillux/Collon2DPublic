#pragma once
#include "ParticleManagerImpl.h"

// TODO: ParticleManagerをSingleton<ParticleManager>派生クラスにして, Implから実装を移動する

/// <summary>
/// パーティクルシステムのインターフェース. 本体はParticleManagerImpl
/// </summary>
class ParticleManager
{
public:
	static ParticleManager& GetInstance();

	/// <summary>
	/// 初期化
	/// <para>NOTE: DXライブラリ初期化処理後に呼ぶ</para>
	/// </summary>
	bool Init();

	/// <summary>
	/// 終了処理
	/// <para>NOTE: DXライブラリ終了処理前に呼ぶ</para>
	/// </summary>
	void End();

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <param name="spawn_desc">パーティクル生成情報</param>
	/// <param name="num_spawn">生成数</param>
	/// <returns></returns>
	bool Spawn(const ParticleSpawnDesc& spawn_desc, const int num_spawn = 1);

	/// <summary>
	/// 全てのパーティクルを非アクティブ化
	/// </summary>
	void DeactivateAllParticles();

	/// <summary>
	/// パーティクルの更新
	/// </summary>
	void Tick(const float delta_seconds);

	/// <summary>
	/// パーティクルの描画
	/// </summary>
	void Draw(const CameraParams& camera_params);

	/// <summary>
	/// プールのパーティクル数を取得
	/// </summary>
	/// <returns></returns>
	UINT GetPoolCount() const;

private:
	ParticleManager();
	~ParticleManager();

	// コピー,ムーブの禁止
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
	ParticleManager(ParticleManager&&) = delete;
	ParticleManager& operator=(ParticleManager&&) = delete;

	template<class T>
	void SafeRelease(T*& p)
	{
		if (p)
		{
			delete p;
			p = nullptr;
		}
	}

	// ParticleManagerImplインスタンス
	ParticleManagerImpl* p_pm;
};