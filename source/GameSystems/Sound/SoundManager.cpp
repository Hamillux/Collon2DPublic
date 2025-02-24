#include "SoundManager.h"
#include "Core.h"
#include <nlohmann/json.hpp>

void SoundManager::Finalize()
{
	UnloadAllSounds();
}

int SoundManager::LoadSoundResource(const std::string& file_name, const int buffer_num)
{
	const int new_handle = DxLib::LoadSoundMem(to_tstring(file_name).c_str(), buffer_num);

	_sound_handles.insert(new_handle);

	return new_handle;
}

void SoundManager::UnloadSound(const int sound_handle, const bool wait_until_the_end)
{
	_sound_handles.erase(sound_handle);

	if (wait_until_the_end && DxLib::CheckSoundMem(sound_handle) != 0)
	{
		DxLib::SetPlayFinishDeleteSoundMem(TRUE, sound_handle);
	}
	else
	{
		DxLib::DeleteSoundMem(sound_handle);
	}
}

void SoundManager::UnloadAllSounds()
{
    DxLib::InitSoundMem();
}

void SoundManager::PlaySound(const int sound_handle, const bool should_loop, const bool top_position_flag)
{
	int play_type = should_loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	DxLib::PlaySoundMem(sound_handle, play_type, top_position_flag);
}

void SoundManager::StopSound(const int sound_handle)
{
	DxLib::StopSoundMem(sound_handle);
}

void SoundManager::SetVolume(const int sound_handle, const int volume)
{
	const int clamped_volume = clamp(volume, 0, 100);
	DxLib::ChangeVolumeSoundMem(255 * clamped_volume / 100, sound_handle);
}

int SoundManager::GetVolume(const int sound_handle)
{
	int volume_value = DxLib::GetVolumeSoundMem2(sound_handle);
	return 100 * volume_value / 255;
}

std::shared_ptr<SoundInstance> SoundManager::MakeSoundInstance(const std::string& file_path, const int buffer_num)
{
	const int sound_handle = LoadSoundResource(file_path, buffer_num);
	return std::make_shared<SoundInstance>(sound_handle);
}
