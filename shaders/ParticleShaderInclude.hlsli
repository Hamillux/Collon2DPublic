#include "../Source/GameSystems/ParticleManager/Particle/Particle.h"
#include "../Source/GameSystems/ParticleManager/ParticleSystemSettings.h"
#include "../Source/GameSystems/ParticleManager/TextureLoader/SpriteTextureInfo.h"

// 頂点シェーダー入力
struct VSInput
{
    uint instanceID : SV_InstanceID;
    uint vertexID : SV_VertexID;
};

// 頂点シェーダー出力
struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    uint i_texture : TexIndex;
    uint i_particle : ParticleIndex;
};

uint SetValue(uint packed_data, uint value, uint mask, uint shift)
{
    // 指定されたビット範囲をクリア
    packed_data &= ~mask;

    // 指定されたビット範囲に新しい値を設定
    packed_data |= (value << shift) & mask;
    
    return packed_data;
}

uint GetValue(uint packed_data, uint mask, uint shift)
{
    return (packed_data & mask) >> shift;
}