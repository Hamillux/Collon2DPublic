#include "..\..\Source\Utility\Core\Rendering\BlendMode.h"

std::vector<EBlendMode> EnumInfo<EBlendMode>::enumerators =
{
    EBlendMode::Alpha,
    EBlendMode::Add,
    EBlendMode::Mula,
};

std::unordered_map<std::string, EBlendMode> EnumInfo<EBlendMode>::name_to_enum_map =
{
    {"Alpha", EBlendMode::Alpha},
    {"Add", EBlendMode::Add},
    {"Mula", EBlendMode::Mula},
};

