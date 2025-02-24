#pragma once
#include "Utility/Core/MathCore.h"
#include <DirectXMath.h>

/// <summary>
/// カメラ情報
/// <para>ワールド座標: Z=1平面上の2次元座標. カメラはZ&lt;1からワールド平面を映す</para>
/// <para>スクリーン座標:	画面中心が原点. 左端:X=-1, 右端:X=1, 上端:Y=-1, 下端:Y=1</para>
/// <para>ビューポート座標:	画面中心が原点. 左端:X=0, 右端:X=VIEWPORT_WIDTH, 上端:Y=0, 下端:Y=VIEWPORT_HEIGHT</para>
/// <para>スクリーンサイズ: カメラとの距離が1の平面上でカメラに映る領域の大きさ. </para>
/// <para>画角拡大率: (カメラのZ座標) - 1 で定義される. カメラに映るワールド領域の大きさは (スクリーンサイズ)*(画角拡大率) に等しい</para>
/// </summary>
struct CameraParams
{
	CameraParams();

	/// <summary>
	/// ワールド座標 -> スクリーン座標
	/// <param name="world_z">ワールド平面(z=1)より奥の点を変換する場合は1より大きい値を指定</param>
	/// </summary>
	Matrix3x3 GetMatrixWorldToScreen(const float world_z = 1.f) const;

	/// <summary>
	/// スクリーン座標 -> ビューポート座標
	/// </summary>
	Matrix3x3 GetMatrixScreenToViewport() const;

	/// <summary>
	/// ビューポート座標 -> スクリーン座標
	/// </summary>
	Matrix3x3 GetMatrixViewportToScreen() const;

	/// <summary>
	/// ビューポート座標 -> 正規化デバイス座標
	/// </summary>
	/// <param name="world_z">ワールド平面(z=1)より奥の平面上に変換する場合は1より大きい値を指定</param>
	Matrix3x3 GetMatrixScreenToWorld(const float world_z = 1.f) const;

	/// <summary>
	/// ワールド座標 -> ビューポート座標
	/// </summary>
	Matrix3x3 GetMatrixWorldToViewport(const float world_z = 1.f) const;

	/// <summary>
	/// ビューポート座標 -> ワールド座標
	/// </summary>
	Matrix3x3 GetMatrixViewportToWorld(const float world_z = 1.f) const;

	/// <summary>
	/// ワールド座標 -> 正規化デバイス座標
	/// </summary>
	Matrix3x3 GetMatrixWorldToNormalizedDevice(const float world_z = 1.f) const;

	/// <summary>
	/// ワールド座標 -> 正規化デバイス座標
	/// </summary>
	void      GetMatrixWorldToNormalizedDevice(DirectX::XMMATRIX& out_xmmat, const float world_z = 1.f) const;

	/// <summary>
	/// スクリーン座標 -> 正規化デバイス座標
	/// </summary>
	static Matrix3x3 GetMatrixScreenToNormalizedDevice();

	/// <summary>
	/// スクリーン座標 -> 正規化デバイス座標
	/// </summary>
	/// <param name="out_xmmat"></param>
	void GetMatrixScreenToNormalizedDevice(DirectX::XMMATRIX& out_xmmat) const;

	/// <summary>
	/// ビューポート座標 -> 正規化デバイス座標
	/// </summary>
	Matrix3x3 GetMatrixViewportToNormalizedDevice() const;

	// ワールド位置をスクリーン位置に変換
	Vector2D TransformPosition_WorldToScreen(const Vector2D& world_position, const float world_z = 1.f) const;

	// スクリーン位置をビューポート位置に変換
	Vector2D TransformPosition_ScreenToViewport(const Vector2D& screen_position) const;

	// ワールド位置をビューポート位置に変換
	Vector2D TransformPosition_WorldToViewport(const Vector2D& world_position, const float world_z = 1.f) const;

	// ビューポート位置をスクリーン位置に変換
	Vector2D TransformPosition_ViewportToScreen(const Vector2D& viewport_position) const;

	// スクリーン位置をワールド位置に変換
	Vector2D TransformPosition_ScreenToWorld(const Vector2D& screen_position, const float world_z = 1.f) const;

	// ビューポート位置をワールド位置に変換
	Vector2D TransformPosition_ViewportToWorld(const Vector2D& viewport_position, const float world_z = 1.f) const;

	// TODO: 方向ベクトルの変換も追加

	/// <summary>
	/// 画角拡大率を変更
	/// </summary>
	/// <param name="new_scale">画角拡大率</param>
	/// <param name="pivot_position_viewport">ピボット位置のビューポート座標</param>
	void ChangeScale(const float new_scale, const Vector2D& pivot_position_viewport);

	/// <summary>
	/// ワールド座標とスクリーン座標の組を指定してカメラ位置を変更
	/// </summary>
	void ChangeWorldOffset_World_Screen(const Vector2D world, const Vector2D screen);

	/// <summary>
	/// カメラに写っているワールド領域の大きさの半分を取得
	/// </summary>
	Vector2D GetWorldViewHalfExtent() const;	

	/// <summary>
	/// カメラに写っているワールド領域の大きさを取得
	/// </summary>
	Vector2D GetWorldViewExtent() const;

	/// <summary>
	/// スクリーンサイズの半分を取得
	/// </summary>
	/// <returns></returns>
	Vector2D GetScreenHalfExtent() const;

	/// <summary>
	/// スクリーンサイズを取得
	/// </summary>
	/// <returns></returns>
	Vector2D GetScreenExtent() const;

	/// <summary>
	/// ワールド平面のZ座標を1として, カメラのZ座標を取得
	/// </summary>
	/// <returns></returns>
	float GetCameraZ() const;

	// ワールド外を映さない最大の画角拡大率を取得
	float GetMaxScale(const Vector2D world_size) const;

	/// <summary>
	/// 指定の領域外を映さないようにワールド位置をクランプ
	/// </summary>
	/// <param name="area_left_top">領域左上のワールド座標</param>
	/// <param name="area_right_bottom">領域右下のワールド座標</param>
	/// <param name="area_plane_z">指定領域が存在する平面のZ座標</param>
	void ClampWorldOffset(const Vector2D area_left_top, const Vector2D area_right_bottom, const float area_plane_z = 1.f);

	// カメラのワールド位置
	Vector2D world_offset;

	// 画角拡大率
	float screen_scale;
};