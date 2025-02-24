#pragma once
#include "Utility/Core/MathCore.h"
#include <unordered_map>

/// <summary>
/// タイルの形状
/// </summary>
enum class TileShape : uint8_t
{
    None = 0,
    Rectangle = 1,
    Slope_100 = 2,	// 傾斜100%
    Slope_50_1 = 3,	// 傾斜50%の左半分
    Slope_50_2 = 4,	// 傾斜50%の右半分
};

/// <summary>
/// タイルの種類を表す列挙型.
/// <para>NOTE: タイルの種類はそのタイルの形状と周囲のタイルの形状から一意に決まる.</para>
/// </summary>
enum class TileType : int
{
    None,
    A, B, C, D, E, F, G, H, I,
    J1, K1, L1, M1, E1_1, F1, H1_1, I1,
    J2, K2, L2, M2, E1_2, E2_2, F2, H1_2, H2_2, I2,
};

class BlockTextureMapping
{
public:
    /// <summary>
    /// テクスチャのタイルサイズ[px]を取得する.
    /// </summary>
    /// <param name="texture_size">テクスチャサイズ[px]</param>
    static constexpr int GetTileSize(const int texture_size)
    {
        // NOTE: ブロックスキンフォーマットを参照
        return texture_size / 10;
    }

    /// <summary>
    /// タイルの形状からタイルの種類を取得する.
    /// </summary>
    /// <param name="target_shape">対象のタイル形状</param>
    /// <param name="upper_shape">上側のタイル形状</param>
    /// <param name="left_shape">左側のタイル形状</param>
    /// /// <param name="lower_shape">下側のタイル形状</param>
    /// <param name="right_shape">右側のタイル形状</param>
    static constexpr TileType GetTileType(
        const TileShape target_shape,
        const TileShape upper_shape,
        const TileShape left_shape,
        const TileShape lower_shape,
        const TileShape right_shape
    );

    /// <summary>
    /// タイルの描画矩形のテクスチャ座標を取得する.
    /// </summary>
    /// <param name="out_u0">描画矩形左上のテクスチャ座標</param>
    /// <param name="out_v0">描画矩形左上のテクスチャ座標</param>
    /// <param name="out_u1">描画矩形右下のテクスチャ座標</param>
    /// <param name="out_v1">描画矩形右下のテクスチャ座標</param>
    /// <param name="target_shape">対象のタイル形状</param>
    static void GetTileTextureRegion(
        float& out_u0,
        float& out_v0,
        float& out_u1,
        float& out_v1,
        const TileType tile_type
    );

    /// <summary>
    /// タイル左上から描画矩形の頂点位置へのオフセットをタイル数単位で取得する.
    /// NOTE: タイル数が負の場合, オフセット方向は左上
    /// </summary>
    /// <param name="out_tiles_to_left">描画矩形の左端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_top">描画矩形の上端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_right">描画矩形の右端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_bottom">描画矩形の下端へのオフセットタイル数</param>
    /// <param name="tile_type">対象のタイル種類</param>
    static void GetVertexPositionOffsetsFromTile(
        int& out_tiles_to_left,
        int& out_tiles_to_top,
        int& out_tiles_to_right,
        int& out_tiles_to_bottom,
        const TileType tile_type
    );

private:
    /// <summary>
    /// タイルの描画矩形のテクスチャ座標を取得する.
    /// </summary>
    /// <param name="out_uv_left_top">描画矩形左上のテクスチャ座標</param>
    /// <param name="out_uv_right_bottom">描画矩形右下のテクスチャ座標</param>
    /// <param name="target_shape">対象のタイル形状</param>
    /// <param name="upper_shape">上側のタイル形状</param>
    /// <param name="left_shape">左側のタイル形状</param>
    /// <param name="right_shape">右側のタイル形状</param>
    /// <param name="lower_shape">下側のタイル形状</param>
    /// <returns>対象タイルに描画矩形が割り当てられているか否か.
    /// <para>NOTE: 傾斜100%でないSlopeの場合, 右端のタイルにのみ描画矩形が割り当てられる.</para>
    /// </returns>
    static void GetTileTextureRegion(
        float& out_u0,
        float& out_v0,
        float& out_u1,
        float& out_v1,
        const TileShape target_shape,
        const TileShape upper_shape,
        const TileShape left_shape,
        const TileShape lower_shape,
        const TileShape right_shape
    );

    /// <summary>
    /// タイル左上から描画矩形の頂点位置へのオフセットをタイル数単位で取得する.
    /// NOTE: タイル数が負の場合, オフセット方向は左上
    /// </summary>
    /// <param name="out_tiles_to_left">描画矩形の左端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_top">描画矩形の上端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_right">描画矩形の右端へのオフセットタイル数</param>
    /// <param name="out_tiles_to_bottom">描画矩形の下端へのオフセットタイル数</param>
    /// <param name="target_shape">対象のタイル形状</param>
    /// <param name="upper_shape">上側のタイル形状</param>
    /// <param name="left_shape">左側のタイル形状</param>
    /// <param name="lower_shape">下側のタイル形状</param>
    /// <param name="right_shape">右側のタイル形状</param>
    /// <returns>
    /// 対象タイルに描画矩形が割り当てられているか否か.
    /// <para>NOTE: 傾斜100%でないSlopeの場合, 右端のタイルにのみ描画矩形が割り当てられる.</para>
    /// </returns>
    static void GetVertexPositionOffsetsFromTile(
        int& out_tiles_to_left,
        int& out_tiles_to_top,
        int& out_tiles_to_right,
        int& out_tiles_to_bottom,
        const TileShape target_shape,
        const TileShape upper_shape,
        const TileShape left_shape,
        const TileShape lower_shape,
        const TileShape right_shape
    );

    /// <summary>
    /// テクスチャのタイル領域を表す構造体.
    /// </summary>
    struct TextureRegion
    {
        TextureRegion(double u0, double v0, double u1, double v1)
            : u0(u0), v0(v0), u1(u1), v1(v1)
        {
        }
        TextureRegion()
            : u0(0.0), v0(0.0), u1(1.0), v1(1.0)
        {
        }
        double u0;
        double v0;
        double u1;
        double v1;
    };
    static const std::unordered_map<TileType, TextureRegion> _tile_texture_region_map;
};