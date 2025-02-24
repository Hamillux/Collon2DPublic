#pragma once

#include "Utility/IJsonSerializable.h"
#include <stdint.h>

struct DrawBlendInfo : public IJsonObject
{
	static constexpr const uint8_t MAX_BLEND_VALUE = 255;

	/// <summary>
	/// デフォルトではブレンドなし
	/// </summary>
	constexpr DrawBlendInfo()
		: dx_blend_mode(BLENDMODE_NOBLEND)
		, blend_value(MAX_BLEND_VALUE)
	{}

	constexpr DrawBlendInfo(const int dx_blend_mode_in, const uint8_t blend_value_in)
		: dx_blend_mode(dx_blend_mode_in)
		, blend_value(blend_value_in)
	{}
	
	//~ Begin AbstractjsoObject interface
	virtual void FromJsonObject(const nlohmann::json& blend_info_json) override;
	//~ End AbstractjsoObject interface

	bool operator==(const DrawBlendInfo& other) const;
	bool operator!=(const DrawBlendInfo& other) const { return !(*this == other); }

	bool IsNoBlend() const;

	int dx_blend_mode;
	uint8_t blend_value;

private:
	static const int BLENDMODE_NOBLEND;
};