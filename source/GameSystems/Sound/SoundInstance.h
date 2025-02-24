#pragma once

#include <string>

/// <summary>
/// サウンドインスタンス. SoundManagerで作成される.
/// </summary>
class SoundInstance
{
public:
	SoundInstance(const int handle);
	~SoundInstance();

	/// <summary>
	/// 音量を取得. (0-100)
	/// </summary>
	/// <returns></returns>
	int GetVolume() const;

	/// <summary>
	/// 音量を設定. (0-100)
	/// </summary>
	void SetVolume(const int volume) const;

	/// <summary>
	/// サウンドを再生
	/// </summary>
	/// <param name="top_position_flag">一時停止中の場合, 先頭から再生するかどうか</param>
	void Play(const bool top_position_flag = true) const;

	/// <summary>
	/// サウンドを停止
	/// </summary>
	void Stop() const;

	/// <summary>
	/// 再生速度を設定
	/// </summary>
	void SetPlaySpeed(const float play_speed);

	/// <summary>
	/// ループ再生フラグを設定
	/// </summary>
	/// <param name="loop_flag"></param>
	void SetLoopEnabled(const bool loop_flag) { _loop_flag = loop_flag; }

	/// <summary>
	/// サウンドインスタンスを破棄する際に、再生中であれば最後まで再生するかどうかを設定
	/// </summary>
	void SetPlayToEndWhenDestroyed(const bool should_play_to_end_when_destroyed) { _should_play_to_end_when_destroyed = should_play_to_end_when_destroyed; }

private:
	int _handle;
	bool _loop_flag;
	bool _should_play_to_end_when_destroyed;
};