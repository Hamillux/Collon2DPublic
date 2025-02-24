#pragma once
#include "MasterDataBase.h"
#include "Utility/Core/Rendering/BlendMode.h"

/// <summary>
/// スプライトアニメーションのマスターデータ
/// </summary>
struct MdAnimation : public MasterData<MdAnimation>
{
	MasterDataID   id;					// アニメーションID
	MasterDataID   sprite_id;			// スプライトID
	uint32_t       first_frame;			// 初期フレームのスプライトフレーム番号
	uint32_t       num_frames;			// アニメーションのフレーム数
	uint32_t       loop_start_offset;	// 初期フレームからループ部分の開始フレームまでのオフセット
	uint32_t        max_loop;			// 最大ループ数. 0の場合は無限ループ
	float default_frame_duration;		// フレームのデフォルト表示時間

	// メンバ変数への参照をタプルで返す関数
	auto GetMembers()
	{
		return std::tie(
			id,
			sprite_id,
			first_frame,
			num_frames,
			loop_start_offset,
			max_loop,
			default_frame_duration
		);
	}

	// マップキーを返す関数
	MasterDataID GetMapKey() const
	{
		return id;
	}

	// コンストラクタ
	MdAnimation()
		: id(0)
		, sprite_id(0)
		, first_frame(0)
		, num_frames(0)
		, loop_start_offset(0)
		, max_loop(0)
		, default_frame_duration(0)
	{}
};