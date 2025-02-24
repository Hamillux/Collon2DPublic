#pragma once
#include "SceneObject/Actor/Actor.h"
#include "SceneObject/Actor/Mapchip/Block/BlockInitialParams.h"
#include "BlockTexturing.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

/// <summary>
/// VSInput
/// </summary>
struct Vertex
{
	Vertex(float x, float y, float u, float v)
		: pos(x, y, 1, 1)
		, uv(u, v)
	{
	}
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT2 uv;
};

class BlockBase : public Actor
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	BlockBase()
		: is_horizontal_flip_enabled(false)
	{
	}
	virtual ~BlockBase() {}

	//~ Begin Actor interface
public:
	/// <summary>
	/// 内部でCreateVertices()を呼び出すので, 派生クラスからBlockBase::Initialize()を呼び出す場合,
	/// CreateVertices()やGetVerticesOffset()で使用する値の初期化を行った後にBlockBase::Initialize()を呼び出すこと.
	/// </summary>
	virtual void Initialize(const ActorInitialParams* actor_params) override;
	virtual void TickActor(float delta_seconds) override;
	virtual void Draw(const ScreenParams& screen_params) override;
	//~ End Actor interface

	//~ Begin BlockBase interface
public:
	/// <summary>
	/// BlockBase::Init()内部で呼び出される.
	/// </summary>
	/// <param name="out_vertices">頂点リスト</param>
	/// <param name="out_indices">インデックスリスト</param>
	virtual void CreateVertices(std::vector<Vertex>& out_vertices, std::vector<UINT>& out_indices) = 0;
private:
	virtual void GetVerticesOffset(float& x_offset, float& y_offset) const;
	//~ End BlockBase interface

protected:
	bool IsHorizontalFlipEnabled() const { return is_horizontal_flip_enabled; }

	static constexpr LPCSTR VS_ENTRY_POINT = "main";
	static constexpr LPCSTR VS_SHADER_MODEL = "vs_5_0";
	static constexpr LPCSTR PS_ENTRY_POINT = "main";
	static constexpr LPCSTR PS_SHADER_MODEL = "ps_5_0";

	// D3D11周り
	ComPtr<ID3D11Buffer> _p_VertexBuffer;
	ComPtr<ID3D11Buffer> _p_IndexBuffer;
	ComPtr<ID3D11InputLayout> _p_InputLayout;
	ComPtr<ID3D11VertexShader> _p_VertexShader;
	ComPtr<ID3D11PixelShader> _p_PixelShader;
	ComPtr<ID3D11Resource> _p_block_skin_texture;
	ComPtr<ID3D11ShaderResourceView> _p_srv_block_skin_texture_srv;
	ComPtr<ID3D11SamplerState> _p_SamplerState;
	ComPtr<ID3D11BlendState> _p_BlendState;
	ComPtr<ID3D11Buffer> _p_ConstantBufferVS;

	MasterDataID block_id;

private:
	std::vector<Vertex> _vertices;
	std::vector<UINT> _indices;
	bool is_horizontal_flip_enabled;
};