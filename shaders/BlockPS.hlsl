#include "BlockShaderInclude.hlsli"

Texture2D g_texture : register(t0);
sampler g_sampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	float4 texColor = g_texture.Sample(g_sampler, input.uv);    
    return texColor;
}