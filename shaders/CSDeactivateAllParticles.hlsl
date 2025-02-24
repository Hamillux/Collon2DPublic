#include "ParticleShaderInclude.hlsli"

// パーティクルバッファ
RWStructuredBuffer<Particle> particles : register(u0);

// パーティクルプール
AppendStructuredBuffer<uint> particle_pool_append : register(u1);

[numthreads(NUM_THREADS_X, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    const uint id = DTid.x;
    if(GetValue(particles[id].packed_data, is_active_mask, is_active_shift))
    {
        particles[id].packed_data = SetValue(particles[id].packed_data, 0, is_active_mask, is_active_shift);
        particle_pool_append.Append(id);

    }
}