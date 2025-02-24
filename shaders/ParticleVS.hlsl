#include "ParticleShaderInclude.hlsli"

// 定数バッファ
cbuffer g_buffer_0 : register(b0)
{
    float4x4 T_world_to_ndc;   // ワールド座標->NDC 変換行列
}
cbuffer b_guffer_1 : register(b1)
{
    uint target_blend_mode;         // 描画対象ブレンドモード
}

StructuredBuffer<Particle> particles : register(t0);

VSOutput main(VSInput input)
{    
    VSOutput output = (VSOutput) 0;
    const uint id = input.instanceID;   // パーティクルID
    const uint particle_size = particles[id].size;
    
    // 描画対象外のチェック
    const uint is_active = GetValue(particles[id].packed_data, is_active_mask, is_active_shift);
    const uint blend_mode = GetValue(particles[id].packed_data, blend_mode_mask, blend_mode_shift);
    if(is_active == 0 || blend_mode != target_blend_mode)
    {
        return output;
    }
    
    // 四角形左上から反時計回りに頂点番号0, 1, 2, 3として, 
    // 四角形を構成する三角形は012, 302.
    // NOTE: vert_index == input.vertexIDとは限らない.
    uint vert_index;
    if (input.vertexID == 4)
    {
        vert_index = 0;
    }
    else if (input.vertexID == 5)
    {
        vert_index = 2;
    }
    else
    {
        vert_index = input.vertexID;
    }
    
    // 頂点のUV座標
    float2 vert_uv;
    switch (vert_index)
    {
        case 0:
            vert_uv = float2(0.f, 0.f);
            break;
        case 1:
            vert_uv = float2(0.f, 1.f);
            break;            
        case 2:
            vert_uv = float2(1.f, 1.f);
            break; 
        case 3:
            vert_uv = float2(1.f, 0.f);
            break;            
    }
        
    // 頂点のローカル座標を計算
    float2 particle_pos_local_xy = particles[id].pos;
    float2 vert_xy_local;
    switch (vert_index)
    {
        // NOTE: ワールド座標は画面下方向がY+
        case 0: // 左上
            vert_xy_local = particle_pos_local_xy + float2(-0.5, -0.5) * particle_size;
            break;
        case 1: // 左下
            vert_xy_local = particle_pos_local_xy + float2(-0.5, 0.5) * particle_size;
            break;
        case 2: // 右下
            vert_xy_local = particle_pos_local_xy + float2(0.5, 0.5) * particle_size;
            break;
        case 3: // 右上
            vert_xy_local = particle_pos_local_xy + float2(0.5, -0.5) * particle_size;
            break;
    }
    // 頂点ワールド座標をNDCに変換
    const float2 pos = particles[id].world_transform_pos;
    const float sin_theta = sin(particles[id].world_transform_rot);
    const float cos_theta = cos(particles[id].world_transform_rot);
    float4x4 world_transform = float4x4(
         cos_theta, sin_theta, 0.f, 0.f,
        -sin_theta, cos_theta, 0.f, 0.f,
             pos.x,     pos.y, 1.f, 0.f,
               0.f,       0.f, 0.f, 1.f);
    float4 vert_world = mul(float4(vert_xy_local, 1.f, 1.f), world_transform);
    float4 vert_ndc = mul(vert_world, T_world_to_ndc);
    
    output.pos  = vert_ndc;
    output.uv   = vert_uv;
    output.i_particle = id;
    output.i_texture = GetValue(particles[id].packed_data, i_texture_mask, i_texture_shift);
	return output;
}