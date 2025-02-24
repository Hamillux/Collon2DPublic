#include "BlockBase.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include <DxLib.h>
#include <DirectXTex.h>
#include <d3dcompiler.h>
#include <stdexcept>

#pragma comment(lib, "d3dcompiler.lib")

namespace {
	struct alignas(16) CB0_VS
	{
		DirectX::XMMATRIX T_Local_to_World;
		DirectX::XMMATRIX T_World_to_NDC;
	};


	bool CompileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) | defined( _DEBUG )
		shaderFlags |= D3DCOMPILE_DEBUG;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(
			fileName,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, shaderModel, 0,
			0, blobOut, &errorBlob);

		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			return false;
		}

		return true;
	}

#ifdef _DEBUG
	constexpr LPCWSTR VS_HLSL_PATH = L"Shaders/BlockVS.hlsl";
	constexpr LPCWSTR PS_HLSL_PATH = L"Shaders/BlockPS.hlsl";
#else
	constexpr LPCWSTR VS_CSO_PATH = L"shaders/BlockVS.cso";
	constexpr LPCWSTR PS_CSO_PATH = L"shaders/BlockPS.cso";
#endif
}

void BlockBase::Initialize(const ActorInitialParams* actor_params)
{
	__super::Initialize(actor_params);

	SetDrawAreaCheckIgnored(true);

	const BlockInitialParams* block_params = dynamic_cast<const BlockInitialParams*>(actor_params);
	block_id = block_params->block_id;
	_is_horizontal_flip_enabled = block_params->_is_horizontal_flip_enabled;

	// 頂点とインデックスの作成
	_vertices = std::vector<Vertex>();
	_indices = std::vector<UINT>();
	CreateDrawVertices(_vertices, _indices);

	// 全頂点位置をオフセット
	{
		float x_offset = 0, y_offset = 0;
		GetDrawVerticesOffset(x_offset, y_offset);
		for (auto& vertex : _vertices)
		{
			vertex.pos.x += x_offset;
			vertex.pos.y += y_offset;
		}
	}

	// デバイスの取得
	ID3D11Device* p_Device =
		const_cast<ID3D11Device*>
		(
			reinterpret_cast<const ID3D11Device*>
			(
				DxLib::GetUseDirect3D11Device()
				)
			);

	// 頂点バッファの作成
	{
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = sizeof(Vertex) * _vertices.size();
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = _vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		p_Device->CreateBuffer(&buffer_desc, &subresource_data, _p_VertexBuffer.GetAddressOf());
	}


	// インデックスバッファの作成
	{
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = sizeof(int) * _indices.size();
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = _indices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		p_Device->CreateBuffer(&buffer_desc, &subresource_data, _p_IndexBuffer.GetAddressOf());
	}

	// テクスチャの読み込み
	{
		const MasterDataID block_skin_image_id = MdBlockSkin::Get(block_id).image_id;
		const MdImageFile& block_skin_image = MdImageFile::Get(block_skin_image_id);
		const std::wstring block_skin_image_path_wstr = StringToWString(block_skin_image.path);
		EFileExtension extension = GetFileExtension(block_skin_image_path_wstr);

		DirectX::ScratchImage image;
		HRESULT hr = E_FAIL;
		switch (extension)
		{
			// WIC
		case EFileExtension::BMP:
		case EFileExtension::PNG:
			hr = DirectX::LoadFromWICFile(block_skin_image_path_wstr.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
			break;

			// DDS
		case EFileExtension::DDS:
			hr = DirectX::LoadFromDDSFile(block_skin_image_path_wstr.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
			break;

			// 未対応
		default:
			throw std::runtime_error("Unsupported image format. Supported formats: BMP, PNG, DDS");
		}

		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to load texture.");
		}

		hr = DirectX::CreateTexture(p_Device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), _p_block_skin_texture.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create texture.");
		}

		hr = DirectX::CreateShaderResourceView(p_Device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), _p_srv_block_skin_texture_srv.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create shader resource view.");
		}
	}

	// サンプラーの作成
	{
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		HRESULT  hr = p_Device->CreateSamplerState(&desc, _p_SamplerState.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create sampler state.");
		}

	}

	// ブレンドステートの作成
	{
		D3D11_BLEND_DESC blend_desc = {};
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = p_Device->CreateBlendState(&blend_desc, _p_BlendState.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create blend state.");
		}
	}

	// シェーダの読み込み
	// Debugでは毎回コンパイルし、Releaseではコンパイル済みのものを使う
	ComPtr<ID3DBlob> p_VertexShaderBlob = nullptr;
	{
		HRESULT hr;

#if defined(DEBUG) || defined(_DEBUG)
		// 頂点シェーダの読み込み
		if (!CompileShader(VS_HLSL_PATH, VS_ENTRY_POINT, VS_SHADER_MODEL, p_VertexShaderBlob.GetAddressOf()))
		{
			throw std::runtime_error("Failed to compile vertex shader.");
		}
#else
		hr = D3DReadFileToBlob(VS_CSO_PATH, p_VertexShaderBlob.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to load precompiled vertex shader.");
		}
#endif
		hr = p_Device->CreateVertexShader(p_VertexShaderBlob->GetBufferPointer(), p_VertexShaderBlob->GetBufferSize(), nullptr, _p_VertexShader.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create vertex shader.");
		}

		// ピクセルシェーダの読み込み
		ComPtr<ID3DBlob> p_PixelShaderBlob = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
		if (!CompileShader(PS_HLSL_PATH, PS_ENTRY_POINT, PS_SHADER_MODEL, &p_PixelShaderBlob))
		{
			throw std::runtime_error("Failed to compile pixel shader.");
		}
#else
		hr = D3DReadFileToBlob(PS_CSO_PATH, p_PixelShaderBlob.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to load precompiled pixel shader.");
		}
#endif
		hr = p_Device->CreatePixelShader(p_PixelShaderBlob->GetBufferPointer(), p_PixelShaderBlob->GetBufferSize(), nullptr, _p_PixelShader.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create pixel shader.");
		}
	}

	// 入力レイアウトの作成
	{
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		HRESULT hr = p_Device->CreateInputLayout(desc, ARRAYSIZE(desc), p_VertexShaderBlob->GetBufferPointer(), p_VertexShaderBlob->GetBufferSize(), _p_InputLayout.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create input layout.");
		}

	}

	// 頂点シェーダー用定数バッファの作成
	{
		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.ByteWidth = sizeof(CB0_VS);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		HRESULT hr = p_Device->CreateBuffer(&buffer_desc, nullptr, _p_ConstantBufferVS.GetAddressOf());
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create constant buffer for vertex shader.");
		}

	}

}


void BlockBase::TickActor(const float delta_seconds)
{
	__super::TickActor(delta_seconds);
}

void BlockBase::Draw(const CameraParams& camera_params)
{
	__super::Draw(camera_params);

	DxLib::RefreshDxLibDirect3DSetting();

	// 描画
	ID3D11DeviceContext* p_Context =
		const_cast<ID3D11DeviceContext*>
		(
			reinterpret_cast<const ID3D11DeviceContext*>
			(
				DxLib::GetUseDirect3D11DeviceContext()
				)
			);

	// IAステージ
	{
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;
		p_Context->IASetInputLayout(_p_InputLayout.Get());
		p_Context->IASetVertexBuffers(0, 1, _p_VertexBuffer.GetAddressOf(), &stride, &offset);
		p_Context->IASetIndexBuffer(_p_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		p_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// ブレンドステートの設定
	{
		float blendFactor[4] = { 0, 0, 0, 0 };
		UINT sampleMask = 0xffffffff;
		p_Context->OMSetBlendState(_p_BlendState.Get(), blendFactor, sampleMask);
	}

	// VSの設定
	{
		// ローカルXを反転させる行列
		Matrix3x3 flip_matrix = Matrix3x3::Identity;
		flip_matrix._00 = _is_horizontal_flip_enabled ? -1.f : 1.f;

		CB0_VS cb0_vs = {};

		cb0_vs.T_Local_to_World = (GetActorWorldTransform().ToMatrix3x3() * flip_matrix).ToXMMATRIX();
		camera_params.GetMatrixWorldToNormalizedDevice(cb0_vs.T_World_to_NDC);

		p_Context->UpdateSubresource(_p_ConstantBufferVS.Get(), 0, nullptr, &cb0_vs, 0, 0);

		p_Context->VSSetShader(_p_VertexShader.Get(), nullptr, 0);
		p_Context->VSSetConstantBuffers(0, 1, _p_ConstantBufferVS.GetAddressOf());
	}

	// PSの設定
	{
		p_Context->PSSetShader(_p_PixelShader.Get(), nullptr, 0);
		p_Context->PSSetShaderResources(0, 1, _p_srv_block_skin_texture_srv.GetAddressOf());
		p_Context->PSSetSamplers(0, 1, _p_SamplerState.GetAddressOf());
	}

	// 描画
	{
		p_Context->DrawIndexed(_indices.size(), 0, 0);
	}

	DxLib::RefreshDxLibDirect3DSetting();
}

void BlockBase::GetDrawVerticesOffset(float& x_offset, float& y_offset) const
{
	x_offset = 0.f;
	y_offset = 0.f;
}