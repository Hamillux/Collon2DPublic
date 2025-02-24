#pragma once
#include "GameSystems/MasterData/internal/MasterDataBase.h"

struct AnimPlayInfo
{
	MasterDataID animation_id;
	float play_speed; // 再生速度 [default]1.f
	float ex_rate;	// 拡大率 [default]1.f
	int reverse_x;	// X方向反転 [default]FALSE
	int reverse_y;	// Y方向反転 [default]FALSE

	constexpr AnimPlayInfo()
		: animation_id(0)
		, play_speed(1.f)
		, ex_rate(1.f)
		, reverse_x(0)
		, reverse_y(0)
	{
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="animation_id_">アニメーションID</param>
	/// <param name="play_speed_">再生速度</param>
	/// <param name="ex_rate_">拡大率</param>
	/// <param name="reverse_x_">X方向反転</param>
	/// <param name="reverse_y_">Y方向反転</param>
	constexpr AnimPlayInfo(
		const MasterDataID animation_id_,
		const float play_speed_,
		const float ex_rate_,
		const int reverse_x_,
		const int reverse_y_
	)
		: animation_id(animation_id_)
		, play_speed(play_speed_)
		, ex_rate(ex_rate_)
		, reverse_x(reverse_x_)
		, reverse_y(reverse_y_)
	{
	}
};
