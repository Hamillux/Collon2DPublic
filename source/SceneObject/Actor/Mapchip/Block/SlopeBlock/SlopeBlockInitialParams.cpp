#include "SlopeBlockInitialParams.h"

namespace
{
	constexpr const char* JKEY_SLOPE_BLOCK_ID = "slopeBlockId";
	constexpr const char* JKEY_SCALE = "scale";
	constexpr const char* JKEY_HORIZONAL_FLIP = "flipHorizontal";

	constexpr const char* LABEL_BLOCK_ID = "BlockID";
	constexpr const char* LABEL_SCALE = "Scale";
	constexpr const char* LABEL_FLIPPED = "Flipped";
}

void SlopeBlockInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JKEY_SCALE] = scale;
}

void SlopeBlockInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
	initial_params_json.at(JKEY_SCALE).get_to(scale);
}

void SlopeBlockInitialParams::AddToParamEditGroup(const std::shared_ptr<ParamEditGroup>& parent, const std::shared_ptr<CommandHistory>& command_history)
{
	__super::AddToParamEditGroup(parent, command_history);

	AddChildParamEditNodeToGroup<EditParamType::INT>(
		parent,
		command_history,
		LABEL_SCALE,
		scale,
		scaleValidator);
}

// scaleÇÕ1à»è„ÇÃêÆêî
const std::shared_ptr<EditParamValidator<EditParamType::INT>> SlopeBlockInitialParams::scaleValidator = std::make_shared<EditParamValidator_ClampMin<EditParamType::INT>>(1);