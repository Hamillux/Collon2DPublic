#ifdef __cplusplus
#pragma once
#include <DirectXMath.h>
#include "GameSystems/ParticleManager/ParticleSystemSettings.h"
#endif

/// <summary>
/// パーティクル構造体.
/// </summary>
struct Particle
{
#ifdef __cplusplus
    //////////////////////////////////////////
    /// C++
    //////////////////////////////////////////
    typedef DirectX::XMFLOAT2 float2;
    typedef unsigned int uint;
#endif

    //////////////////////////////////////////
    /// C++, シェーダー
    //////////////////////////////////////////
    float size;                 // ワールド空間でのパーティクルサイズ [default]16.f
    float life_time;            // 寿命 [default]1.f
    float age;                  // 生成からの経過時間 [default]0.f
    float alpha;                // アルファ値 [default]1.f

    // 運動関係
    float gravity_scale;        // 重力スケール [default]0.f
    float2 pos;                 // ワールド空間での位置 [default](0.f,0.f)
    float2 vel;                 // ワールド空間での速度 [default](0.f,0.f)

    // アニメーション関係
    uint i_frame;               // 初期値0. frame_duration_ms経過ごとにインクリメントされる [default]0
    uint anim_first_frame;      // [default]0
    uint loop_start_offset;     // [default]0
    uint num_anim_frames;       // [default]1
    float frame_duration;    // アニメーションフレーム時間[sec] [default]0.1f
    
    /// <summary>
    /// <para>1Byteデータを4つ詰めたデータ</para>
    /// <para>(is_active)-(blend_mode)-(max_loop)-(i_texture)</para>
    /// <para>[default] 0-0-1-0</para>
    /// </summary>
    uint packed_data;

    // ワールド変換行列 [default]単位行列
    float2 world_transform_pos;
    float  world_transform_rot;

#ifdef __cplusplus
    //////////////////////////////////////////
    /// C++
    //////////////////////////////////////////
    // Particle::packed_dataのビットマスクとシフト量
    static constexpr uint is_active_mask = 0xFF000000;
    static constexpr uint blend_mode_mask = 0x00FF0000;
    static constexpr uint max_loop_mask = 0x0000FF00;
    static constexpr uint i_texture_mask = 0x000000FF;

    static constexpr uint is_active_shift = 24;
    static constexpr uint blend_mode_shift = 16;
    static constexpr uint max_loop_shift = 8;
    static constexpr uint i_texture_shift = 0;

    Particle();
    void SetIsActive(const uint in_is_active);
    void SetBlendMode(const uint in_blend_mode);
    void SetMaxLoop(const uint in_max_loop);
    void SetTextureIndex(const uint in_i_texture);
    static bool Test();

private:
    void SetValueInPackedData(const uint in_value, const uint mask, const uint shift);
#endif
};

#ifdef __cplusplus
constexpr unsigned long PARTICLES_BUFFER_SIZE = MAX_PARTICLES_NUM * sizeof(Particle);
constexpr unsigned long PARTICLES_BUFFER_SIZE_KB = PARTICLES_BUFFER_SIZE >> 10;
constexpr unsigned long PARTICLES_BUFFER_SIZE_MB = PARTICLES_BUFFER_SIZE_KB >> 10;
#endif

#ifndef __cplusplus
static const uint is_active_mask = 0xFF000000;
static const uint blend_mode_mask = 0x00FF0000;
static const uint max_loop_mask = 0x0000FF00;
static const uint i_texture_mask = 0x000000FF;

static const uint is_active_shift = 24;
static const uint blend_mode_shift = 16;
static const uint max_loop_shift = 8;
static const uint i_texture_shift = 0;

#endif