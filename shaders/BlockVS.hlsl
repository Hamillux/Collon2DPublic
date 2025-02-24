#include "BlockShaderInclude.hlsli"

struct VSInput
{
	float4 pos: POSITION;
	float2 uv : TEXCOORD;
};

// 定数バッファ
cbuffer g_buffer_0 : register(b0)
{
	float4x4 T_local_to_world;
    float4x4 T_world_to_ndc; // ワールド座標->NDC 変換行列
}

VSOutput main(VSInput input)
{
	VSOutput ret = (VSOutput)0;
	
	float4 pos_world = mul(input.pos, T_local_to_world);
    ret.position = mul(pos_world, T_world_to_ndc);
	
	
	ret.uv = input.uv;
	
    return ret;
}