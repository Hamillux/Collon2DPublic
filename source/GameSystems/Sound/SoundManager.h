#pragma once

#include "Core.h"
#include "Utility/SingletonBase.h"
#include "SoundInstance.h"
#include <unordered_map>
#include <unordered_set>

#undef PlaySound

struct Sound 
{
	int handle;
	int volume;	// [0, 100]
};

class SoundManager : public Singleton<SoundManager>
{
private:
	friend class Singleton<SoundManager>;

public:
	virtual ~SoundManager() {}

	//~ Begin Singleton interface
public:
	virtual void Finalize() override;
	//~ End Singleton interface

public:
	/// <summary>
	/// サウンドをロード
	/// </summary>
	/// <param name="file_name">音声ファイルパス</param>
	/// <param name="buffer_num">同時に再生できる数</param>
	int LoadSoundResource(const std::string& file_name, const int buffer_num = 3);

	/// <summary>
	/// サウンドを解放
	/// </summary>
	/// <param name="sound_handle">サウンドハンドル</param>
	/// <param name="wait_until_the_end">trueなら最後まで再生して解放する</param>
	void UnloadSound(const int sound_handle, const bool wait_until_the_end = false);

	/// <summary>
	/// すべてのサウンドを解放
	/// </summary>
	void UnloadAllSounds();

	/// <summary>
	/// サウンドを再生開始
	/// </summary>
	/// <param name="sound_handle">サウンドハンドル</param>
	/// <param name="should_loop">ループ再生</param>
	/// <param name="top_position_flag">再生するサウンドが再生途中の場合, 先頭から再生するかどうか</param>
	void PlaySound(const int sound_handle, const bool should_loop = false, const bool top_position_flag = true);

	/// <summary>
	/// サウンドを停止
	/// </summary>
	/// <param name="sound_handle">サウンドハンドル</param>
	void StopSound(const int sound_handle);

	/// <summary>
	/// 音量を設定. (0-100)
	/// </summary>
	/// <param name="sound_handle">サウンドハンドル</param>
	/// <param name="volume">音量(0-100)</param>
	void SetVolume(const int sound_handle, const int volume);

	/// <summary>
	/// 音量を取得. (0-100)
	/// </summary>
	/// <param name="sound_handle">サウンドハンドル</param>
	int GetVolume(const int sound_handle);

	/// <summary>
	/// サウンドインスタンスを生成
	/// </summary>
	/// <param name="file_path">音声ファイルパス</param>
	/// <param name="buffer_num">同時に再生できる数</param>
	/// <returns>サウンドインスタンス</returns>
	std::shared_ptr<SoundInstance> MakeSoundInstance(const std::string& file_path, const int buffer_num = 3);

private:
	std::unordered_set<int> _sound_handles;
};