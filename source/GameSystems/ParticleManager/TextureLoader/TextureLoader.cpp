#include "TextureLoader.h"
#include "DirectXTex.h"
#include "SpriteTextureInfo.h"

TextureLoader::TextureLoader()
{}

// TDOO: LoadTextureArrayを繰り返し呼び出すとメモリリークが発生する.
// ロード済みのテクスチャがアンロードされていない?
bool TextureLoader::LoadTextureArray(ID3D11Device* pDev, const SpriteTextureLoadInfo* load_infos, const size_t num_texs, ComPtr<ID3D11ShaderResourceView>& out_srv)
{
	if (num_texs > MaxTexturesNum)
	{
		return false;
	}
	else if (num_texs == 0)
	{
		return true;
	}

	const TextureIndex NumTextures = num_texs;

	HRESULT hr;
	DirectX::ScratchImage scratch_image_array = {};

	// テクスチャのロード
	{
		std::array<DirectX::ScratchImage, MAX_TEXTURES_NUM> scratch_images;

		for (TextureIndex i = 0; i < NumTextures; i++)
		{
			hr = DirectX::LoadFromDDSFile(load_infos[i].path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, scratch_images.at(i));
			if (FAILED(hr))
			{
				return false;
			}
		}

		// ロードしたテクスチャをscratch_image_arrayにまとめる
		std::array<DirectX::Image, MAX_TEXTURES_NUM> images;
		for (TextureIndex i = 0; i < NumTextures; i++)
		{
			images.at(i) = *(scratch_images.at(i).GetImages());
		}

		hr = scratch_image_array.InitializeArrayFromImages(images.data(), NumTextures);
		if (FAILED(hr))
		{
			return false;
		}

		// テクスチャ生成の設定
		D3D11_TEXTURE2D_DESC tex2d_desc = {};
		const DirectX::TexMetadata& metadata = scratch_images.at(0).GetMetadata();	// 最初にロードしたテクスチャのメタデータを使用する
		tex2d_desc.Format = metadata.format;
		tex2d_desc.ArraySize = NumTextures;
		tex2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2d_desc.CPUAccessFlags = 0;
		tex2d_desc.Height = metadata.height;
		tex2d_desc.Width = metadata.width;
		tex2d_desc.MipLevels = 0;
		tex2d_desc.MiscFlags = 0;
		tex2d_desc.SampleDesc.Count = 1;
		tex2d_desc.SampleDesc.Quality = 0;
		tex2d_desc.Usage = D3D11_USAGE_DEFAULT;

		hr = DirectX::CreateTexture(pDev, scratch_image_array.GetImages(), scratch_image_array.GetImageCount(), scratch_image_array.GetMetadata(), m_pResource_Textures.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// Texture2DArray SRV の生成
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format			= scratch_image_array.GetMetadata().format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = NumTextures;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.MostDetailedMip = 0;
		desc.Texture2DArray.MipLevels = 1;

		hr = pDev->CreateShaderResourceView(m_pResource_Textures.Get(), &desc, m_pSRV_Textures.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// スプライトシート情報のリソース, ビュー生成
	{
		SpriteTextureInfo texinfos[MAX_TEXTURES_NUM];
		for (TextureIndex i = 0; i < NumTextures; i++)
		{
			texinfos[i].sprite_rows = load_infos[i].sprite_rows;
			texinfos[i].sprite_columns = load_infos[i].sprite_columns;
		}

		// リソース生成
		D3D11_BUFFER_DESC buffDesc = {};
		buffDesc.ByteWidth				= sizeof(SpriteTextureInfo) * NumTextures;
		buffDesc.Usage					= D3D11_USAGE_DEFAULT;
		buffDesc.BindFlags				= D3D11_BIND_SHADER_RESOURCE;
		buffDesc.CPUAccessFlags			= 0;
		buffDesc.MiscFlags				= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		buffDesc.StructureByteStride	= sizeof(SpriteTextureInfo);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = texinfos;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;
		ID3D11Buffer* buffTemp = nullptr;
		hr = pDev->CreateBuffer(&buffDesc, &initData, &buffTemp);
		if (FAILED(hr))
		{
			return false;
		}
		m_pResource_TexInfos.Attach(buffTemp);

		// ビュー生成
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
		viewDesc.Format = DXGI_FORMAT_UNKNOWN;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		viewDesc.Buffer.FirstElement = 0;
		viewDesc.Buffer.NumElements = NumTextures;
		hr = pDev->CreateShaderResourceView(m_pResource_TexInfos.Get(), &viewDesc, m_pSRV_TexInfos.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	out_srv = m_pSRV_Textures;
	return true;
}

ID3D11ShaderResourceView** TextureLoader::GetTextureArraySRV()
{
	return m_pSRV_Textures.GetAddressOf();
}

ID3D11ShaderResourceView** TextureLoader::GetTextureInfosSRV()
{
	return m_pSRV_TexInfos.GetAddressOf();
}
