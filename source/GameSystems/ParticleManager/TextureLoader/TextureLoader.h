#pragma once
#include "GameSystems/ParticleManager/ParticleSystemSettings.h"
#include <unordered_map>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <array>
#include <vector>

struct SpriteTextureLoadInfo;

class TextureLoader
{
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	typedef unsigned short TextureIndex;
	static constexpr int MaxTexturesNum = MAX_TEXTURES_NUM;

	TextureLoader();

	/// <summary>
	/// テクスチャをロードし、Texture2DArrayのSRVを作成する.
	/// すでにロードされているテクスチャはアンロードされる
	/// </summary>
	/// <param name="pDev">D3D11デバイス</param>
	/// <param name="texture_paths">
	/// <para>テクスチャパス配列. 要素数はMaxTexturesNum以下</para>
	/// <para>texture_pathsの並びとTexture2DArray内のリソースの並びは同じ</para>
	/// </param>
	/// <param name="out_srv">シェーダーリソースビュー</param>
	/// <returns></returns>
	bool LoadTextureArray(ID3D11Device* pDev, const SpriteTextureLoadInfo* texture_paths, const size_t num_texs, ComPtr<ID3D11ShaderResourceView>& out_srv);

	ID3D11ShaderResourceView** GetTextureArraySRV();
	ID3D11ShaderResourceView** GetTextureInfosSRV();

private:
	ComPtr<ID3D11Resource> m_pResource_Textures;
	ComPtr<ID3D11ShaderResourceView> m_pSRV_Textures;

	ComPtr<ID3D11Resource> m_pResource_TexInfos;
	ComPtr<ID3D11ShaderResourceView> m_pSRV_TexInfos;
};

struct SpriteTextureLoadInfo
{
	std::wstring path;
	unsigned int sprite_rows;
	unsigned int sprite_columns;
};