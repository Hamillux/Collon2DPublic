#pragma once
#include "Core.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "GameSystems/ParticleManager/Particle/Particle.h"
#include "GameSystems/ParticleManager/Particle/ParticleSpawnDesc.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Textureloader/TextureLoader.h"

/////////////////////
// プロトタイプ宣言
/////////////////////
class ParticleManager;

/// <summary>
/// パーティクルシステムの本体.
/// ParticleManagerからのみインスタンス生成可能.
/// DxLib_End()よりも前にDirectX関係のリソースを解放しなければならないが、
/// このクラスをシングルトンにすると, メンバ全てに対してComPtr::Release()を呼ぶ
/// 処理を書く必要がある
/// </summary>
class ParticleManagerImpl
{
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	friend class ParticleManager;

private:
	ParticleManagerImpl();
	~ParticleManagerImpl();

	bool Init();
	void End();
	bool Spawn(const ParticleSpawnDesc& spawn_desc, const int num_spawn);
	void DeactivateAllParticles();
	void Tick(const float delta_seconds);
	void Draw(const CameraParams& camera_params);
	UINT GetPoolCount() const;

	bool InitBlendStateMap(ID3D11Device* pDev);
	bool InitParticleBuffer(ID3D11Device* pDev);

	ID3D11Device* GetDevice() const;
	void MakeSpriteTextureLoadInfos(std::vector<SpriteTextureLoadInfo>& out_infos, const std::vector<MasterDataID> sprite_ids);

	ComPtr<ID3D11InputLayout>			m_pInputLayout;			// 入力レイアウト
	ComPtr<ID3D11Buffer>				m_pVertexBuffer;		// 頂点バッファ
	ComPtr<ID3D11Buffer>				m_pCB_VS[14];			// 定数バッファ(頂点シェーダー)
	ComPtr<ID3D11Buffer>				m_pCB_PS;				// 定数バッファ(ピクセルシェーダー)
	ComPtr<ID3D11Buffer>				m_pIndexBuffer;			// インデックスバッファ
	ComPtr<ID3D11Resource>				m_pTexture2D;			// テクスチャ
	ComPtr<ID3D11ShaderResourceView>	m_pSRV_TextureArray;			// テクスチャ用シェーダーリソースビュー
	ComPtr<ID3D11SamplerState>			m_pSamplerState;		// サンプラーステート

	ComPtr<ID3D11Buffer>				m_pParticleBuffer;		// パーティクルバッファ
	ComPtr<ID3D11ShaderResourceView>	m_pSRV_PtBuff;			// パーティクルバッファ用SRV (for VS, PS)
	ComPtr<ID3D11UnorderedAccessView>	m_pUAV_PtBuff;			// パーティクルバッファUAV (for CS)

	ComPtr<ID3D11Buffer>				m_pSpawnInfoBuffer;		// パーティクル生成情報バッファ
	ComPtr<ID3D11ShaderResourceView>	m_pSRV_SpnInfoBuff;		// パーティクル生成情報バッファSRV (for CS)
	ComPtr<ID3D11Buffer>				m_pCB_CS_SpawnNum;		// パーティクル生成数を格納する定数バッファ

	ComPtr<ID3D11Buffer>				m_pParticleIDBuffer;	// パーティクルIDバッファ
	ComPtr<ID3D11UnorderedAccessView>	m_pUAV_PtIDBuff;		// パーティクルIDバッファUAV (for CS)

	ComPtr<ID3D11Buffer>				m_pPoolCounterBuffer;	// パーティクルプールカウンタバッファ

	ComPtr<ID3D11Buffer>				m_pCB_CS_Update;				// 定数バッファ(コンピュートシェーダー)

	// シェーダー
	ComPtr<ID3D11VertexShader>			m_pVertexShader;
	ComPtr<ID3D11PixelShader>			m_pPixelShader;
	ComPtr<ID3D11ComputeShader>			m_pComputeShader;
	ComPtr<ID3D11ComputeShader>			m_pCS_Update;
	ComPtr<ID3D11ComputeShader>			m_pCS_Spawn;
	ComPtr<ID3D11ComputeShader>			m_pCS_DeactivateAll;

	ID3D11DeviceContext* m_pContextRef = nullptr;

	std::unique_ptr<class TextureLoader> m_textureLoader;

	// ブレンドステート
	std::unordered_map<EBlendMode, ComPtr<ID3D11BlendState>> m_pBlendState;

	// シェーダーリソースにコピーするためのバッファ
	std::vector<Particle> m_particles;

	UINT m_pool_count = 0;

	CameraParams m_lastCameraParams;

	std::unordered_map<MasterDataID, int> _sprite_id_to_texture_index_map;
};