#include "..\..\Source\Scene\StageInteractiveScene\StageEditorScene\ParameterEditing\EditParamType.h"

std::vector<EditParamType> EnumInfo<EditParamType>::enumerators =
{
    EditParamType::INT,
    EditParamType::INT2,
    EditParamType::INT3,
    EditParamType::INT4,
    EditParamType::FLOAT,
    EditParamType::FLOAT2,
    EditParamType::FLOAT3,
    EditParamType::FLOAT4,
    EditParamType::BOOL,
    EditParamType::STRING,
    EditParamType::BLOCK_SKIN,
    EditParamType::ITEM_ID,
    EditParamType::COLOR3,
    EditParamType::COLOR4,
};

std::unordered_map<std::string, EditParamType> EnumInfo<EditParamType>::name_to_enum_map =
{
    {"INT", EditParamType::INT},
    {"INT2", EditParamType::INT2},
    {"INT3", EditParamType::INT3},
    {"INT4", EditParamType::INT4},
    {"FLOAT", EditParamType::FLOAT},
    {"FLOAT2", EditParamType::FLOAT2},
    {"FLOAT3", EditParamType::FLOAT3},
    {"FLOAT4", EditParamType::FLOAT4},
    {"BOOL", EditParamType::BOOL},
    {"STRING", EditParamType::STRING},
    {"BLOCK_SKIN", EditParamType::BLOCK_SKIN},
    {"ITEM_ID", EditParamType::ITEM_ID},
    {"COLOR3", EditParamType::COLOR3},
    {"COLOR4", EditParamType::COLOR4},
};

