#pragma once
#include "Utility/Core/EnumInfo.h"

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
