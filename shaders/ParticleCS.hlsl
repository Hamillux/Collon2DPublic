#include "ParticleShaderInclude.hlsli"

// パーティクルバッファ
RWStructuredBuffer<Particle> particles : register(u0);

// パーティクルプール
AppendStructuredBuffer<uint> particle_pool_append : register(u1);

cbuffer g_constbuff : register(b0)
{
    float delta_seconds;
    float2 gravity_force;
}


[numthreads(NUM_THREADS_X, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    const uint id = DTid.x;
    const uint is_active =
        GetValue(particles[id].packed_data, is_active_mask, is_active_shift);
    if (is_active == 0)
    {
        return;
    }    
         
    particles[id].vel += particles[id].gravity_scale * gravity_force * delta_seconds;
    particles[id].pos += particles[id].vel * delta_seconds;
        
    
    const bool is_frame_update_timing = particles[id].age >= particles[id].frame_duration * (particles[id].i_frame + 1);
    if (is_frame_update_timing)
    {
        const uint max_loop = GetValue(particles[id].packed_data, max_loop_mask, max_loop_shift);
        // インクリメントの可否をチェックしてOKならインクリメント
        // 2行目の計算は i_frame < offset + (num_anim_frames - offset) * max_loop - 1
        if 
        (
            max_loop == 0 ||
            particles[id].i_frame < particles[id].loop_start_offset + (particles[id].num_anim_frames - particles[id].loop_start_offset) * max_loop - 1
        )
        {
            particles[id].i_frame++;
        }
    }
    
    // アルファ値更新
    // TODO: 様々な補間に対応させる
    float age = particles[id].age;
    float life = particles[id].life_time;
    float t = age / life;
    if(age / life > 0.5f)
    {
        particles[id].alpha = clamp((1.f - 2 * (t - 0.5f)), 0.f, 1.f);
    }

    particles[id].age += delta_seconds;
    if (particles[id].age >= particles[id].life_time)
    {
        particles[id].packed_data =
            SetValue(particles[id].packed_data, 0, is_active_mask, is_active_shift);
        particle_pool_append.Append(id);

    }
    
}