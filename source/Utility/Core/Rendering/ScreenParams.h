#pragma once
#include "Utility/Core/MathCore.h"
#include <DirectXMath.h>

struct ScreenParams
{
	///////////////////////////
	// 変換行列取得 (Matrix3x3)
	///////////////////////////
	Matrix3x3 GetMatrixWorldToScreen() const;										// ワールド座標		-> スクリーン座標
	Matrix3x3 GetMatrixScreenToViewport() const;									// スクリーン座標	-> ビューポート座標
	Matrix3x3 GetMatrixViewportToScreen() const;									// ビューポート座標	-> スクリーン座標
	Matrix3x3 GetMatrixScreenToWorld() const;										// スクリーン座標	-> ワールド座標
	Matrix3x3 GetMatrixWorldToViewport() const;										// ワールド座標		-> ビューポート座標
	Matrix3x3 GetMatrixViewportToWorld() const;										// ビューポート座標	-> ワールド座標
	Matrix3x3 GetMatrixViewportToNormalizedDevice() const;							// ビューポート座標	-> 正規化デバイス座標
	Matrix3x3 GetMatrixWorldToNormalizedDevice() const;								// ワールド座標		-> 正規化デバイス座標
	void      GetMatrixWorldToNormalizedDevice(DirectX::XMMATRIX& out_xmmat) const;	// ワールド座標		-> 正規化デバイス座標

	/// <summary>
	/// 画角拡大率を変更
	/// </summary>
	/// <param name="new_scale">画角拡大率</param>
	/// <param name="pivot_position_viewport">ピボット位置のビューポート座標</param>
	void ChangeScale(const float new_scale, const Vector2D& pivot_position_viewport);

	Vector2D TransformPosition_WorldToScreen(const Vector2D& world_position) const;
	Vector2D TransformPosition_ScreenToViewport(const Vector2D& screen_position) const;
	Vector2D TransformPosition_WorldToViewport(const Vector2D& world_position) const;
	Vector2D TransformPosition_ViewportToScreen(const Vector2D& viewport_position) const;
	Vector2D TransformPosition_ScreenToWorld(const Vector2D& screen_position) const;
	Vector2D TransformPosition_ViewportToWorld(const Vector2D& viewport_position) const;

	/// <summary>
	/// ワールド座標とスクリーン座標の組を指定してスクリーン位置を変更
	/// </summary>
	/// <param name="world"></param>
	/// <param name="screen"></param>
	void ChangeWorldOffset_World_Screen(const Vector2D world, const Vector2D screen);

	/// <summary>
	/// 拡大率も考慮した半スクリーンサイズを取得
	/// </summary>
	/// <returns></returns>
	Vector2D GetScreenHalfExtentScaled() const;

	Vector2D GetScreenExtentUnscaled() const;

	// 比率 ビューポートサイズ/スクリーンサイズ を取得
	// NOTE: ビューポートとスクリーンのアスペクト比が同じであることを前提とする
	// TODO: アスペクト比が異なる場合の対応
	float GetViewportPerScreen() const;

	float GetCameraZ() const;

	// ワールド外を映さない最大の画角拡大率を取得
	float GetMaxScale(const Vector2D world_size) const;

	// スクリーン左上のワールド座標
	Vector2D world_offset;

	// 画角拡大率( = 1 - CameraZ). この値が大きいほど画角が広くなる
	float screen_scale;
};