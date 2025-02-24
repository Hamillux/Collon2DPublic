#include "Particle.h"

Particle::Particle()
    : life_time(1.f)
    , gravity_scale(0.f)
    , frame_duration(0.1f)
    , world_transform_pos(0.f, 0.f)
    , world_transform_rot(0.f)
    , pos(0.f, 0.f)
    , vel(0.f, 0.f)
    , age(0.f)
    , size(16.f)
    , alpha(1.f)
    , i_frame(0)
    , anim_first_frame(0)
    , loop_start_offset(0)
    , num_anim_frames(1)
    , packed_data(0x0000'0100)   // is_active(0), blend_mode(0), max_loop(1), i_texture(0)
{}

void Particle::SetIsActive(const uint in_is_active)
{
    SetValueInPackedData(in_is_active, is_active_mask, is_active_shift);
}

void Particle::SetBlendMode(const uint in_blend_mode)
{
    SetValueInPackedData(in_blend_mode, blend_mode_mask, blend_mode_shift);
}

void Particle::SetMaxLoop(const uint in_max_loop)
{
    SetValueInPackedData(in_max_loop, max_loop_mask, max_loop_shift);
}

void Particle::SetTextureIndex(const uint in_i_texture)
{
    SetValueInPackedData(in_i_texture, i_texture_mask, i_texture_shift);
}

bool Particle::Test()
{
    Particle particle;

    // Test SetIsActive
    particle.SetIsActive(1);
    assert((particle.packed_data & Particle::is_active_mask) >> Particle::is_active_shift == 1);

    // Test SetBlendMode
    particle.SetBlendMode(2);
    assert((particle.packed_data & Particle::blend_mode_mask) >> Particle::blend_mode_shift == 2);

    // Test SetMaxLoop
    particle.SetMaxLoop(3);
    assert((particle.packed_data & Particle::max_loop_mask) >> Particle::max_loop_shift == 3);

    // Test SetTextureIndex
    particle.SetTextureIndex(4);
    assert((particle.packed_data & Particle::i_texture_mask) >> Particle::i_texture_shift == 4);

    return true;
}

void Particle::SetValueInPackedData(const uint in_value, const uint mask, const uint shift)
{
    // 指定されたビット範囲をクリア
    packed_data &= ~mask;

    // 指定されたビット範囲に新しい値を設定
    packed_data |= (in_value << shift) & mask;
}
