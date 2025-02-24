#pragma once
#include "Utility/Core/EnumInfo.h"
#include <array>
#include "GameSystems/MasterData/MasterDataInclude.h"

DEFINE_ENUM()
/// <summary>
/// <para>StageEditorSceneのパラメータ編集パネルで使用するパラメータの種類</para>
/// <para>値の編集方式を表す</para>
/// </summary>
enum class EditParamType
{
	INT, INT2, INT3, INT4,				// ImGui::InputIntN()で値編集
	FLOAT, FLOAT2, FLOAT3, FLOAT4,		// ImGui::InputFloatN()で値編集
	BOOL,								// ImGui::Checkbox()で値編集
	STRING,								// ImGui::InputText()で値編集
	BLOCK_SKIN,							// BlockSkinModalで値編集
	ITEM_ID,							// ItemModalで値編集
	COLOR3,								// ImGui::ColorEdit3()で値編集
	COLOR4,								// ImGui::ColorEdit4()で値編集
};


// define value-type for each enumerator in EditParamType
template<EditParamType E>
struct edit_param_value_type
{
};
template<EditParamType E> using edit_param_value_type_t = typename edit_param_value_type<E>::type;

template<> struct edit_param_value_type<EditParamType::INT2> { using type = std::array<int, 2>; };
template<> struct edit_param_value_type<EditParamType::INT> { using type = int; };
template<> struct edit_param_value_type<EditParamType::INT3> { using type = std::array<int, 3>; };
template<> struct edit_param_value_type<EditParamType::INT4> { using type = std::array<int, 4>; };
template<> struct edit_param_value_type<EditParamType::FLOAT> { using type = float; };
template<> struct edit_param_value_type<EditParamType::FLOAT2> { using type = std::array<float, 2>; };
template<> struct edit_param_value_type<EditParamType::FLOAT3> { using type = std::array<float, 3>; };
template<> struct edit_param_value_type<EditParamType::FLOAT4> { using type = std::array<float, 4>; };
template<> struct edit_param_value_type<EditParamType::BOOL> { using type = bool; };
template<> struct edit_param_value_type<EditParamType::STRING> { using type = std::string; };
template<> struct edit_param_value_type<EditParamType::BLOCK_SKIN> { using type = MasterDataID; };
template<> struct edit_param_value_type<EditParamType::ITEM_ID> { using type = MasterDataID; };
template<> struct edit_param_value_type<EditParamType::COLOR3> { using type = std::array<float, 3>; };
template<> struct edit_param_value_type<EditParamType::COLOR4> { using type = std::array<float, 4>; };
// TODO: 有効な列挙子全てをここに