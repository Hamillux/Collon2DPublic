#pragma once


enum class DamageType
{
	None,
	Stomp,
	SideHit,
	Punch,
	Crush,
};

struct DamageInfo
{
	struct StompDamageInfo {};
	struct PunchDamageInfo
	{
		float impulse[2];
		PunchDamageInfo()
			: impulse{ 0.f, 0.f }
		{
		}
	};
	struct SideHitDamageInfo {};
	struct CrushDamageInfo 
	{
		float impulse[2];
		CrushDamageInfo()
			: impulse{ 0.f, 0.f }
		{
		}
	};

	int damage_value;
	DamageType damage_type;
	Actor* damage_causer;
	union
	{
		StompDamageInfo stomp_damage_info;
		PunchDamageInfo punch_damage_info;
		SideHitDamageInfo side_hit_damage_info;
		CrushDamageInfo crush_damage_info;
	};

	DamageInfo()
		: damage_value(0)
		, damage_type(DamageType::None)
		, damage_causer(nullptr)
		, stomp_damage_info()
	{
	}
};

