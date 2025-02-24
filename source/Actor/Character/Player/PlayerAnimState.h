#pragma once

#include <stdint.h>
#include "Utility/Core/Rendering/AnimPlayInfo.h"

class PlayerAnimatorComponent;

enum PlayerAnimStateID : uint8_t
{
	PlayerAnimStateID_Idle,
	PlayerAnimStateID_Run,
	PlayerAnimStateID_Jump,
	PlayerAnimStateID_Fall,
	PlayerAnimStateID_Magic,
};

uint8_t PlayerAnimStateUpdate_Idle(PlayerAnimatorComponent* player_animator);
uint8_t PlayerAnimStateUpdate_Run(PlayerAnimatorComponent* player_animator);
uint8_t PlayerAnimStateUpdate_Jump(PlayerAnimatorComponent* player_animator);
uint8_t PlayerAnimStateUpdate_Fall(PlayerAnimatorComponent* player_animator);
uint8_t PlayerAnimStateUpdate_Magic(PlayerAnimatorComponent* player_animator);

struct PlayerAnimPlayInfos
{
	constexpr PlayerAnimPlayInfos(
		const AnimPlayInfo& idle_,
		const AnimPlayInfo& run_,
		const AnimPlayInfo& jump_,
		const AnimPlayInfo& fall_,
		const AnimPlayInfo& magic_,
		const AnimPlayInfo& damaged_,
		const AnimPlayInfo& attack_right_,
		const AnimPlayInfo& attack_upper_
	)
	: idle(idle_)
	, run(run_)
	, jump(jump_)
	, fall(fall_)
	, magic(magic_)
	, damaged(damaged_)
	, attack_right(attack_right_)
	, attack_upper(attack_upper_)
	{}

	AnimPlayInfo idle;
	AnimPlayInfo run;
	AnimPlayInfo jump;
	AnimPlayInfo fall;
	AnimPlayInfo magic;
	AnimPlayInfo damaged;
	AnimPlayInfo attack_right;
	AnimPlayInfo attack_upper;
};