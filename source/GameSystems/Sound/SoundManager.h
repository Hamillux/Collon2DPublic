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
	int LoadSoundResource(const std::string& file_name, const int buffer_num = 3);

	void UnloadSound(const int sound_handle, const bool wait_until_the_end = false);
	void UnloadAllSounds();

	void PlaySound(const int sound_handle, const bool should_loop = false, const bool top_position_flag = true);
	void StopSound(const int sound_handle);

	void SetVolume(const int sound_handle, const int volume);
	int GetVolume(const int sound_handle);

	std::shared_ptr<SoundInstance> MakeSoundInstance(const std::string& file_path, const int buffer_num = 3);

private:
	std::unordered_set<int> _sound_handles;
};