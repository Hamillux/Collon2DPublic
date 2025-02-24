#include "SlopeBlockInitialParams.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"
#include "SystemTypes.h"
#include "Utility/Core/MathCore.h"

namespace
{
	constexpr const char* JKEY_SCALE = "scale";
	constexpr const char* LABEL_SCALE = "Scale";

	constexpr const char* JKEY_WIDTH_PER_HEIGHT = "widthPerHeight";

	// 縦横比1:2でステージサイズが最小でも, スロープブロックがステージからはみ出ないようなscale
	constexpr int MAX_SCALE = std::min(MIN_STAGE_LENGTH / 2, SHORT_STAGE_HEIGHT_TILES);
	
	const std::shared_ptr<EditParamValidator<EditParamType::INT>> scaleValidator = std::make_shared<EditParamValidator_Clamp<EditParamType::INT>>(1, MAX_SCALE);
}

void SlopeBlockInitialParams::ToJsonObject(nlohmann::json& initial_params_json) const
{
	__super::ToJsonObject(initial_params_json);
	initial_params_json[JKEY_SCALE] = scale;
	initial_params_json[JKEY_WIDTH_PER_HEIGHT] = width_per_height;
}

void SlopeBlockInitialParams::FromJsonObject(const nlohmann::json& initial_params_json)
{
	__super::FromJsonObject(initial_params_json);
	initial_params_json.at(JKEY_SCALE).get_to(scale);
	initial_params_json.at(JKEY_WIDTH_PER_HEIGHT).get_to(width_per_height);
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