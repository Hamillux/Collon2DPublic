#include "ParticleManagerImpl.h"
#include <d3dcompiler.h>
#include "SystemTypes.h"
#include "DirectXTex.h"
#include <comdef.h>
#include "GameSystems/MasterData/MasterDataInclude.h"

#pragma comment(lib, "d3dcompiler.lib")

/////////////////////////////////
//// 定数
/////////////////////////////////
const Vector2D gravity_force = Vector2D(0.f, 1.f) * 9.8 * 32;	// 32px == 1 meter

////////////////////////////////
//// 構造体定義
////////////////////////////////
// VS用定数バッファ(b0)
struct alignas(16) CB0_VS
{
	DirectX::XMMATRIX TransformMatrix;
};

// VS用定数バッファ(b1)
struct alignas(16) UINT_WRAPPER
{
	UINT value;
};

typedef UINT_WRAPPER CB1_VS;
typedef UINT_WRAPPER CB0_CS_SPAWN;

// VS用定数バッファサイズリスト
constexpr size_t CBuffSizeArray[] = {
	sizeof(CB0_VS),
	sizeof(CB1_VS),
};
constexpr unsigned short NumCBuffs = sizeof(CBuffSizeArray)/sizeof(size_t);

// ピクセルシェーダー用定数バッファ
struct alignas(16) CB_PS
{
	unsigned int sprite_rows;
	unsigned int sprite_columns;
};

// コンピュートシェーダー用定数バッファ
struct alignas(16) CB_CS
{
	float delta_seconds;
	DirectX::XMFLOAT2 gravity_force;
};

// シェーダーファイルパス
#ifdef _DEBUG
LPCWSTR VERTEX_SHADER_PATH = L"Shaders/ParticleVS.hlsl";
LPCWSTR PIXEL_SHADER_PATH = L"Shaders/ParticlePS.hlsl";
LPCWSTR CS_Update_PATH = L"Shaders/ParticleCS.hlsl";
LPCWSTR CS_Spawn_PATH = L"Shaders/CSSpawnParticle.hlsl";
LPCWSTR CS_DeactivateAll_PATH = L"Shaders/CSDeactivateAllParticles.hlsl";
#else
LPCWSTR VS_CSO_PATH = L"shaders/ParticleVS.cso";
LPCWSTR PS_CSO_PATH = L"shaders/ParticlePS.cso";
LPCWSTR CS_Update_CSO_PATH = L"shaders/ParticleCS.cso";
LPCWSTR CS_Spawn_CSO_PATH = L"shaders/CSSpawnParticle.cso";
LPCWSTR CS_DeactivateAll_CSO_PATH = L"shaders/CSDeactivateAllParticles.cso";
#endif

bool CompileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobOut)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) | defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		fileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, shaderModel, 0,
		0, blobOut, &errorBlob);
	
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();
		return false;
	}

	if (errorBlob)
	{
		errorBlob->Release();
	}

	return true;
}

bool ParticleManagerImpl::Init()
{
	if (DxLib_IsInit() == FALSE)
	{
		if (DxLib_Init() == -1)
		{
			return false;
		}
	}

	// D3D11デバイス, コンテキストの取得
	auto pDev = GetDevice();
	m_pContextRef = const_cast<ID3D11DeviceContext*>(reinterpret_cast<const ID3D11DeviceContext*>(DxLib::GetUseDirect3D11DeviceContext()));

	if (!(pDev && m_pContextRef))
	{
		return false;
	}

	// ブレンドステートの生成
	if (!InitBlendStateMap(pDev))
	{
		return false;
	}

	// パーティクルバッファの生成
	if (!InitParticleBuffer(pDev))
	{
		return false;
	}

	HRESULT hr;
	ComPtr<ID3DBlob> blob = nullptr;
	// シェーダーのコンパイルと生成
	{
		// 頂点シェーダー
		m_pVertexShader.Reset();
#ifdef _DEBUG
		if (!CompileShader(VERTEX_SHADER_PATH, "main", "vs_5_0", blob.GetAddressOf()))
		{
			return false;
		}
#else
		hr = D3DReadFileToBlob(VS_CSO_PATH, blob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
#endif

		hr = pDev->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pVertexShader.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// ピクセルシェーダー
		m_pPixelShader.Reset();
#ifdef _DEBUG
		if (!CompileShader(PIXEL_SHADER_PATH, "main", "ps_5_0", blob.GetAddressOf()))
		{
			return false;
		}
#else
		hr = D3DReadFileToBlob(PS_CSO_PATH, blob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
#endif

		hr = pDev->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pPixelShader.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// コンピュートシェーダー
		m_pCS_Update.Reset();

#ifdef _DEBUG
		if (!CompileShader(CS_Update_PATH, "main", "cs_5_0", blob.GetAddressOf()))
		{
			return false;
		}
#else
		hr = D3DReadFileToBlob(CS_Update_CSO_PATH, blob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
#endif

		hr = pDev->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pCS_Update.GetAddressOf()
		);
		if (FAILED(hr))
		{
			return false;
		}

		m_pCS_Spawn.Reset();
#ifdef _DEBUG
		if (!CompileShader(CS_Spawn_PATH, "main", "cs_5_0", blob.GetAddressOf()))
		{
			return false;
		}
#else
		hr = D3DReadFileToBlob(CS_Spawn_CSO_PATH, blob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
#endif

		hr = pDev->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pCS_Spawn.GetAddressOf()
		);
		if (FAILED(hr))
		{
			return false;
		}

		m_pCS_DeactivateAll.Reset();
#ifdef _DEBUG
		if (!CompileShader(CS_DeactivateAll_PATH, "main", "cs_5_0", blob.GetAddressOf()))
		{
			return false;
		}
#else
		hr = D3DReadFileToBlob(CS_DeactivateAll_CSO_PATH, blob.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
#endif

		hr = pDev->CreateComputeShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_pCS_DeactivateAll.GetAddressOf()
		);
		if (FAILED(hr))
		{
			return false;
		}

	}

	// 頂点シェーダー用定数バッファーの作成
	{
		for (int i = 0; i < sizeof(CBuffSizeArray)/sizeof(size_t); i++)
		{
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.ByteWidth = CBuffSizeArray[i];
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			hr = pDev->CreateBuffer(&desc, nullptr, m_pCB_VS[i].GetAddressOf());
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	// ピクセルシェーダー用定数バッファの作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(CB_PS);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		hr = pDev->CreateBuffer(&desc, nullptr, m_pCB_PS.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// コンピュートシェーダー用定数バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(CB_CS);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		CB_CS cb = {};
		cb.delta_seconds = 1.f / static_cast<float>(FRAME_RATE);
		cb.gravity_force.x = gravity_force.x;
		cb.gravity_force.y = gravity_force.y;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = &cb;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		ID3D11Buffer* temp = nullptr;
		hr = pDev->CreateBuffer(&desc, &initData, &temp);
		if (FAILED(hr))
		{
			return false;
		}
		m_pCB_CS_Update = temp;
	}

	// サンプラーステートの作成
	{
		// サンプラーステートの設定
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = pDev->CreateSamplerState(&samplerDesc, m_pSamplerState.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// 初期化終了
	DxLib::RefreshDxLibDirect3DSetting();
	return true;
}

void ParticleManagerImpl::End()
{}

ParticleManagerImpl::ParticleManagerImpl()
	: m_pBlendState(
		{
			{EBlendMode::Alpha, nullptr},
			{EBlendMode::Add, nullptr},
			{EBlendMode::Mula, nullptr},
		}
		)
	, m_textureLoader(std::make_unique<TextureLoader>())
	, m_particles(MAX_PARTICLES_NUM)
	, m_pool_count(MAX_PARTICLES_NUM)
{
}

ParticleManagerImpl::~ParticleManagerImpl()
{
}

void ParticleManagerImpl::DeactivateAllParticles()
{
	m_pContextRef->CSSetShader(m_pCS_DeactivateAll.Get(), nullptr, 0);
	m_pContextRef->CSSetUnorderedAccessViews(0, 1, m_pUAV_PtBuff.GetAddressOf(), nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, m_pUAV_PtIDBuff.GetAddressOf(), nullptr);
	m_pContextRef->Dispatch(NUM_PARTICLE_CS_DISPATCH, 1, 1);

	// アンバインド
	ID3D11UnorderedAccessView* p_null = nullptr;
	m_pContextRef->CSSetUnorderedAccessViews(0, 1, &p_null, nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, &p_null, nullptr);
	return;

	DxLib::RefreshDxLibDirect3DSetting();
}

bool ParticleManagerImpl::Spawn(const ParticleSpawnDesc& spawn_desc, const int num_spawn)
{
	// プールに十分な数のパーティクルがあるかチェック
	if (m_pool_count < num_spawn)
	{
		return false;
	}

	// 生成するパーティクルが参照するスプライトテクスチャがロードされていない場合
	// ロード済みのテクスチャも合わせて再ロードする
	const MdAnimation& animation = MdAnimation::Get(spawn_desc.animation_id);
	if (_sprite_id_to_texture_index_map.find(animation.sprite_id) == _sprite_id_to_texture_index_map.end())
	{
		// ロードするテクスチャのIDをまとめる
		std::vector<MasterDataID> sprite_ids;
		sprite_ids.reserve(_sprite_id_to_texture_index_map.size() + 1);	// ロード済み+1個分の領域を確保
		for (auto& pair : _sprite_id_to_texture_index_map)
		{
			sprite_ids.push_back(pair.first);
		}
		sprite_ids.push_back(MdAnimation::Get(spawn_desc.animation_id).sprite_id);

		std::vector<SpriteTextureLoadInfo> texture_load_infos;
		MakeSpriteTextureLoadInfos(texture_load_infos, sprite_ids);

		m_textureLoader->LoadTextureArray(GetDevice(), texture_load_infos.data(), texture_load_infos.size(), m_pSRV_TextureArray);
	}

	// 1. 定数バッファ内の生成数を更新
	CB0_CS_SPAWN cb = {};
	cb.value = num_spawn;
	m_pContextRef->UpdateSubresource(m_pCB_CS_SpawnNum.Get(), 0, nullptr, &cb, 0, 0);

	// 2. バッファm_pSpawnInfoBufferの[0]から[num_spawn-1]までを更新
	const Vector2D init_pos_world = spawn_desc.world_transform.TransformLocation(spawn_desc.initial_position);

	for (int i = 0; i < num_spawn; i++)
	{
		Particle& p = m_particles.at(i);
		p = Particle();
		p.size = spawn_desc.size.GetRandomValueInRange();
		p.life_time = spawn_desc.life_time.GetRandomValueInRange();

		// 初期位置, 初速度
		p.pos.x = init_pos_world.x;
		p.pos.y = init_pos_world.y;

		const float rot_angle_deg = RandomNumberGenerator::GetRandomFloat(-spawn_desc.velocity_angle_deg * 0.5f, spawn_desc.velocity_angle_deg * 0.5f);		
		const Vector2D init_vel_local = Vector2D::Rotate(
			(spawn_desc.initial_velocity_normalized * spawn_desc.initial_speed.GetRandomValueInRange()),
			DX_PI_F * rot_angle_deg / 180.f
		);
		const Vector2D init_vel_world = spawn_desc.world_transform.TransformDirection(init_vel_local);
		p.vel.x = init_vel_world.x;
		p.vel.y = init_vel_world.y;

		p.gravity_scale = spawn_desc.gravity_scale.GetRandomValueInRange();
		
		p.anim_first_frame = animation.first_frame;
		p.num_anim_frames = animation.num_frames;
		p.loop_start_offset = animation.loop_start_offset;
		p.frame_duration = animation.default_frame_duration;

		p.SetBlendMode(static_cast<Particle::uint>(MasterHelper::GetAnimationBlendMode(animation)));
		p.SetMaxLoop(0);
		p.SetTextureIndex(_sprite_id_to_texture_index_map.at(animation.sprite_id));
	}
	m_pContextRef->UpdateSubresource(m_pSpawnInfoBuffer.Get(), 0, nullptr, m_particles.data(), 0, 0);

	// 3. リソースバインド & ディスパッチ
	const UINT NumDispatch = num_spawn / NUM_THREADS_X + 1;
	m_pContextRef->CSSetShader(m_pCS_Spawn.Get(), nullptr, 0);
	m_pContextRef->CSSetConstantBuffers(0, 1, m_pCB_CS_SpawnNum.GetAddressOf());
	m_pContextRef->CSSetUnorderedAccessViews(0, 1, m_pUAV_PtBuff.GetAddressOf(), nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, m_pUAV_PtIDBuff.GetAddressOf(), nullptr);
	m_pContextRef->CSSetShaderResources(0, 1, m_pSRV_SpnInfoBuff.GetAddressOf());
	m_pContextRef->Dispatch(NumDispatch, 1, 1);
	
	// 4. UAVアンバインド
	ID3D11UnorderedAccessView* pNull = nullptr;
	m_pContextRef->CSSetUnorderedAccessViews(0, 1, &pNull, nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, &pNull, nullptr);

	DxLib::RefreshDxLibDirect3DSetting();
	return true;
}

void ParticleManagerImpl::Tick(const float delta_seconds)
{
	CB_CS cb = {};
	cb.delta_seconds = delta_seconds;
	cb.gravity_force.x = gravity_force.x;
	cb.gravity_force.y = gravity_force.y;
	m_pContextRef->UpdateSubresource(m_pCB_CS_Update.Get(), 0, nullptr, &cb, 0, 0);

	m_pContextRef->CSSetUnorderedAccessViews(0, 1, m_pUAV_PtBuff.GetAddressOf(), nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, m_pUAV_PtIDBuff.GetAddressOf(), nullptr);
	m_pContextRef->CSSetConstantBuffers(0, 1, m_pCB_CS_Update.GetAddressOf());
	m_pContextRef->CSSetShader(m_pCS_Update.Get(), nullptr, 0);
	m_pContextRef->Dispatch(NUM_PARTICLE_CS_DISPATCH, 1, 1);

	// Append/ConsumeStructuredBufferの要素数を取得
	// カウンタバッファにプールのカウントをコピー
	m_pContextRef->CopyStructureCount(m_pPoolCounterBuffer.Get(), 0, m_pUAV_PtIDBuff.Get());

	// カウンタバッファからカウントを読み出す
	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	auto hr = m_pContextRef->Map(m_pPoolCounterBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mapped_resource);
	if (SUCCEEDED(hr))
	{
		m_pool_count = *reinterpret_cast<UINT*>(mapped_resource.pData);
		m_pContextRef->Unmap(m_pPoolCounterBuffer.Get(), 0);
	}

	// UAVを外す
	ID3D11UnorderedAccessView* pNull = nullptr;
	m_pContextRef->CSSetUnorderedAccessViews(0, 1, &pNull, nullptr);
	m_pContextRef->CSSetUnorderedAccessViews(1, 1, &pNull, nullptr);

	DxLib::RefreshDxLibDirect3DSetting();
}

void ParticleManagerImpl::Draw(const CameraParams& camera_params)
{
	DxLib::RefreshDxLibDirect3DSetting();

	m_lastCameraParams = camera_params;
	
	// パイプラインの設定
	// IA関係
	{
		m_pContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	// VS関係
	{
		CB0_VS cb0 = {};
		camera_params.GetMatrixWorldToNormalizedDevice(cb0.TransformMatrix);
		m_pContextRef->UpdateSubresource(m_pCB_VS[0].Get(), 0, nullptr, &cb0, 0, 0);
		CB1_VS cb1 = {};
		cb1.value = static_cast<unsigned int>(EBlendMode::Alpha);
		m_pContextRef->UpdateSubresource(m_pCB_VS[1].Get(), 0, nullptr, &cb1, 0, 0);

		m_pContextRef->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		// 定数バッファバインド(VS)
		for (int i = 0; i < NumCBuffs; i++)
		{
			if (m_pCB_VS[i] == nullptr)
			{
				break;
			}
			m_pContextRef->VSSetConstantBuffers(i, 1, m_pCB_VS[i].GetAddressOf());
		}
		// 他リソースバインド(VS)
		m_pContextRef->VSSetShaderResources(0, 1, m_pSRV_PtBuff.GetAddressOf());
	}
	
	// PS関係
	{
		m_pContextRef->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
		// リソースバインド(PS)
		m_pContextRef->PSSetShaderResources(0, 1, m_pSRV_PtBuff.GetAddressOf());
		m_pContextRef->PSSetShaderResources(1, 1, m_textureLoader->GetTextureArraySRV());
		m_pContextRef->PSSetShaderResources(2, 1, m_textureLoader->GetTextureInfosSRV());
		m_pContextRef->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	}

	// ブレンドモードごとに描画処理
	for (auto& blend_mode : EnumInfo<EBlendMode>::List())
	{
		ID3D11BlendState* bs = m_pBlendState.at(blend_mode).Get();
		m_pContextRef->OMSetBlendState(bs, nullptr, 0xFFFFFFFF);

		// 頂点シェーダーで描画対象パーティクルの判別に利用する定数
		CB1_VS cb1_vs{};
		cb1_vs.value = static_cast<int>(blend_mode);
		m_pContextRef->UpdateSubresource(m_pCB_VS[1].Get(), 0, nullptr, &cb1_vs, 0, 0);
		
		m_pContextRef->DrawInstanced(6, MAX_PARTICLES_NUM, 0, 0);
	}

	// DXライブラリのDirect3D設定を再度行う.
	DxLib::RefreshDxLibDirect3DSetting();
}

UINT ParticleManagerImpl::GetPoolCount() const
{
	return m_pool_count;
}

bool ParticleManagerImpl::InitBlendStateMap(ID3D11Device* pDev)
{
	HRESULT hr;

	// アルファブレンド
	D3D11_BLEND_DESC alphaBlendDesc = {};
	alphaBlendDesc.AlphaToCoverageEnable = FALSE;
	alphaBlendDesc.IndependentBlendEnable = FALSE;
	alphaBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = pDev->CreateBlendState(&alphaBlendDesc, m_pBlendState[EBlendMode::Alpha].GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 加算ブレンド
	D3D11_BLEND_DESC addBlendDesc = {};
	addBlendDesc.AlphaToCoverageEnable = FALSE;
	addBlendDesc.IndependentBlendEnable = FALSE;
	addBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	addBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	addBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	addBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	addBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	addBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = pDev->CreateBlendState(&addBlendDesc, m_pBlendState[EBlendMode::Add].GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 乗算ブレンド
	D3D11_BLEND_DESC mulaBlendDesc = {};
	mulaBlendDesc.AlphaToCoverageEnable = FALSE;
	mulaBlendDesc.IndependentBlendEnable = FALSE;
	mulaBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	mulaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	mulaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	mulaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	mulaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mulaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	mulaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	mulaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = pDev->CreateBlendState(&mulaBlendDesc, m_pBlendState[EBlendMode::Mula].GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	
	return true;
}

bool ParticleManagerImpl::InitParticleBuffer(ID3D11Device* pDev)
{
	HRESULT hr;

	// パーティクルの初期化
	for(int i = 0; i < MAX_PARTICLES_NUM; i ++)
	{
		m_particles.at(i) = Particle();
		// ライフタイムをゼロにしてアクティブにするのは
		// 全パーティクルを初期化直後にプールに移すため.
		m_particles.at(i).life_time = 0.f;
		m_particles.at(i).SetIsActive(1);
	}

	// パーティクルバッファの生成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Particle) * MAX_PARTICLES_NUM;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Particle);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_particles.data();
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		hr = pDev->CreateBuffer(&desc, &initData, m_pParticleBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// パーティクルバッファのビュー作成
	{
		// SRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = MAX_PARTICLES_NUM;
			hr = pDev->CreateShaderResourceView(m_pParticleBuffer.Get(), &desc, m_pSRV_PtBuff.GetAddressOf());
			if (FAILED(hr))
			{
				_com_error err(hr);
				OutputDebugString(err.ErrorMessage());
				return false;
			}
		}

		// UAV
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.NumElements = MAX_PARTICLES_NUM;
			hr = pDev->CreateUnorderedAccessView(m_pParticleBuffer.Get(), &desc, m_pUAV_PtBuff.GetAddressOf());
			if (FAILED(hr))
			{
				return false;
			}
		}
	}

	// パーティクル生成時のコピー元バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Particle) * MAX_PARTICLES_NUM;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(Particle);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_particles.data();
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		hr = pDev->CreateBuffer(&desc, &initData, m_pSpawnInfoBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// パーティクルコピー元バッファ用SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = MAX_PARTICLES_NUM;

		hr = pDev->CreateShaderResourceView(m_pSpawnInfoBuffer.Get(), &desc, m_pSRV_SpnInfoBuff.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// パーティクル生成数定数バッファ
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(CB0_CS_SPAWN);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		hr = pDev->CreateBuffer(&desc, nullptr, m_pCB_CS_SpawnNum.GetAddressOf());
		if(FAILED(hr))
		{
			return false;
		}
	}


	// IDバッファの生成
	{
		unsigned int* ids = new unsigned int[MAX_PARTICLES_NUM];
		for (unsigned int i = 0; i < MAX_PARTICLES_NUM; i++)
		{
			ids[i] = 0;
		}
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = sizeof(UINT) * MAX_PARTICLES_NUM;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(UINT);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = ids;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		hr = pDev->CreateBuffer(&desc, &initData, m_pParticleIDBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// IDバッファのビュー作成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = MAX_PARTICLES_NUM;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG::D3D11_BUFFER_UAV_FLAG_APPEND;

		hr = pDev->CreateUnorderedAccessView(m_pParticleIDBuffer.Get(), &desc, m_pUAV_PtIDBuff.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	// パーティクルプールのカウンタバッファ作成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_STAGING;
		desc.ByteWidth = sizeof(UINT);
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		hr = pDev->CreateBuffer(&desc, nullptr, m_pPoolCounterBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

ID3D11Device* ParticleManagerImpl::GetDevice() const
{
	return const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(DxLib::GetUseDirect3D11Device()));
}

void ParticleManagerImpl::MakeSpriteTextureLoadInfos(std::vector<SpriteTextureLoadInfo>& out_infos, const std::vector<MasterDataID> sprite_ids)
{
	_sprite_id_to_texture_index_map.clear();
	out_infos.clear();
	out_infos.reserve(sprite_ids.size());

	int texture_index = 0;
	for (auto& id : sprite_ids)
	{
		const MdSpriteSheet& sprite_data = MdSpriteSheet::Get(id);
		const MdImageFile& image_data = MasterHelper::GetImageFile(sprite_data);

		out_infos.push_back(SpriteTextureLoadInfo{});
		SpriteTextureLoadInfo& info = out_infos.at(out_infos.size() - 1);
		info.path = StringToWString(image_data.path);
		info.sprite_rows = sprite_data.num_rows;
		info.sprite_columns = sprite_data.num_columns;
		
		_sprite_id_to_texture_index_map[id] = texture_index;
		++texture_index;
	}
}
