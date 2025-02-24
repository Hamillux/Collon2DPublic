#include "ParticleShaderInclude.hlsli"

// パーティクルバッファ
StructuredBuffer<Particle> particles : register(t0);

// テクスチャ配列, サンプラー
Texture2DArray g_textures : register(t1);

// スプライトテクスチャ情報
StructuredBuffer<SpriteTextureInfo> g_texinfos : register(t2);

// サンプラー
sampler g_sampler : register(s0);

float4 main(VSOutput input) : SV_TARGET
{    
    // スプライトのサイズを計算
    const float sprite_columns = g_texinfos[input.i_texture].sprite_columns;
    const float sprite_rows = g_texinfos[input.i_texture].sprite_rows;
    const float2 spriteSize = float2(1.f / sprite_columns, 1.f / sprite_rows);

    // スプライトフレーム番号を計算
    // i_sprite = i_frame % num_anim_frames + anim_first_frame;
    const uint i_frame = particles[input.i_particle].i_frame;
    const uint num= particles[input.i_particle].num_anim_frames;
    const uint start = particles[input.i_particle].anim_first_frame;
    const uint offset = particles[input.i_particle].loop_start_offset;
    uint i_sprite = 0;
    if(i_frame < num)
    {
        i_sprite = start + i_frame;
    }
    else
    {
        i_sprite = start + offset + (i_frame - num) % (num - offset);
    }
    
    // スプライトの行, 列を計算
    const uint row = i_sprite / sprite_columns;
    const uint column = i_sprite % sprite_columns;

    // スプライトの左上のUVオフセットを計算
    const float2 uv_offset = spriteSize * float2(column, row);

    // サンプリング
    const uint i_texture = GetValue(particles[input.i_particle].packed_data, i_texture_mask, i_texture_shift);
    float4 texColor = g_textures.Sample(g_sampler, float3(uv_offset + input.uv * spriteSize, i_texture));
    
    // TODO: アルファ値の計算(particle.ageなどから)
    texColor.a *= particles[input.i_particle].alpha;
    
    return texColor;
}