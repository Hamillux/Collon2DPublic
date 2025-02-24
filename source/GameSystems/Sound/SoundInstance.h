#pragma once

#include <string>

class SoundInstance
{
public:
	SoundInstance(const int handle);
	~SoundInstance();

	int GetVolume() const;
	void SetVolume(const int volume) const;

	void Play(const bool top_position_flag = true) const;
	void Stop() const;

	void SetPlaySpeed(const float play_speed);
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