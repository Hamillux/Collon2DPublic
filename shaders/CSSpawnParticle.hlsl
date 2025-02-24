#include "ParticleShaderInclude.hlsli"

// パーティクルバッファ
RWStructuredBuffer<Particle> particles : register(u0);

// パーティクルプール
ConsumeStructuredBuffer<uint> particle_pool_consume : register(u1);

// スポーンパーティクル (先頭からnum_spawn個が有効)
StructuredBuffer<Particle> SpawnInfos: register(t0);

// スポーン数
cbuffer g_cbuffer : register(b0)
{
    uint num_spawn;
}

[numthreads(NUM_THREADS_X, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.x >= num_spawn)
    {
        return;
    }
    
    // パーティクルの初期化
    const uint particle_id = particle_pool_consume.Consume();
    particles[particle_id] = SpawnInfos[DTid.x];
    
    // パーティクルをアクティブに
    particles[particle_id].packed_data = SetValue(particles[particle_id].packed_data, 1, is_active_mask, is_active_shift);
}