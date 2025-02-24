#include "SoundInstance.h"
#include "GameSystems/Sound/SoundManager.h"

#undef PlaySound

SoundInstance::SoundInstance(const int handle)
	: _handle(handle)
	, _loop_flag(false)
	, _should_play_to_end_when_destroyed(false)
{
}

SoundInstance::~SoundInstance()
{
	SoundManager::GetInstance().UnloadSound(_handle, _should_play_to_end_when_destroyed);
}

int SoundInstance::GetVolume() const
{
	return SoundManager::GetInstance().GetVolume(_handle);
}

void SoundInstance::SetVolume(int volume) const
{
	SoundManager::GetInstance().SetVolume(_handle, volume);
}

void SoundInstance::Play(const bool top_position_flag) const
{
	SoundManager::GetInstance().PlaySound(_handle, _loop_flag, top_position_flag);
}

void SoundInstance::Stop() const
{
	SoundManager::GetInstance().StopSound(_handle);
}

void SoundInstance::SetPlaySpeed(const float play_speed)
{
	DxLib::ResetFrequencySoundMem(_handle);
	DxLib::SetFrequencySoundMem(static_cast<int>(DxLib::GetFrequencySoundMem(_handle) * play_speed), _handle);
}
