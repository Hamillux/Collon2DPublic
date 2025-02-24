
#pragma once
#include "ParamEditComponent.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/Validator/EditableParamValidator.h"
#include <memory>
#include <functional>

/// <summary>
/// パラメータ編集パネルでパラメータ名と値編集UIを1組だけ表示するクラス
/// </summary>
/// <typeparam name="EPT"></typeparam>
template<EditParamType EPT>
class ParamEditNode : public ParamEditComponent
{
public:
	using ValueType = typename edit_param_value_type<EPT>::type;
	using CallbackArgType = const ValueType&;
	using CallbackType = std::function<void(CallbackArgType)>;
	using Validator = EditParamValidator<EPT>;

	/// <param name="in_name">ラベル名</param>
	/// <param name="initial_value">値欄に表示する値の初期値</param>
	/// <param name="process_validate_value">変更, 第4引数のバリデータで検証された値を受け取る関数: (ex)値を元の変数に書き込む</param>
	/// <param name="in_validator">バリデータ. EditParamValidator.hを参照</param>
	ParamEditNode<EPT>
		(
			const std::string& in_name, 
			const ValueType& initial_value, 
			const CallbackType& in_process_validate_value, 
			const std::shared_ptr<Validator>& in_validator = nullptr
		)
		: name(in_name)
		, value(initial_value)
		, process_validate_value(in_process_validate_value)
		, validator(in_validator)
	{}

private:
	/// <summary>
	/// ImGuiを使用してツリー構造のUI要素を表示する.
	/// パラメータ名と値編集UIをImGuiのツリー要素として表示する
	/// </summary>
	/// <param name="id">各ImGui要素に割り当てるID. ShowAsImguiTreeElement_Implを呼び出すたびにインクリメントされる.</param>
	virtual void ShowAsImguiTreeElement_Impl(int& id) override
	{
		constexpr int PADDING_BETWEEN_LABEL_INPUT = 200;

		ImGui::Indent();
		ImGui::Text("%s", name.c_str());
		ImGui::Unindent();

		ImGui::SameLine(PADDING_BETWEEN_LABEL_INPUT);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1, 1, 1, 1));
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize);
		ImGui::PushID(id++);

		if (!_last_value)
		{
			_last_value = std::make_unique<ValueType>(value);
		}

		if (ShowValueUI()) 
		{
			if (validator != nullptr)
			{
				validator->Validate(value);
			}

			// 値が変わっていたらイベントを発行
			if (value != *_last_value)
			{
				process_validate_value(value);
				events.OnValueChanged.Dispatch();
				*_last_value = value;
			}
		}

		ImGui::PopStyleColor();
		ImGui::PopID();
	}

	/// <summary>
	/// 値編集用のUIを表示する
	/// </summary>
	/// <returns>値が確定したか</returns>
	bool ShowValueUI()
	{
		static_assert(false, "ShowValueUI is not implemented for this type");
	}

	std::string name;
	ValueType value;
	CallbackType process_validate_value;
	std::shared_ptr<Validator> validator;

	std::unique_ptr<ValueType> _last_value;
};

namespace
{
template<typename T>
inline ImGuiDataType GetImGuiDataType() { static_assert("T is invalid type for 'GetImGuiDataType'"); return 0; }
template<> inline ImGuiDataType GetImGuiDataType<int>() { return ImGuiDataType_S32; }
template<> inline ImGuiDataType GetImGuiDataType<float>() { return ImGuiDataType_Float; }

} // end anonymous namespace

//~ Begin ParamEditNode specializations
template<>
inline bool ParamEditNode<EditParamType::INT>::ShowValueUI()
{
	ImGui::InputInt("##", &value);
	return ImGui::IsItemDeactivatedAfterEdit();
}

template<>
inline bool ParamEditNode<EditParamType::FLOAT>::ShowValueUI()
{
	ImGui::InputFloat("##", &value);
	return ImGui::IsItemDeactivatedAfterEdit();
}

namespace
{
	template<typename T, int N>
	bool ShowArrayValueUI(std::array<T, N>& arr)
	{
		const size_t array_size = arr.size();
		ImGui::InputScalarN("##", GetImGuiDataType<T>(), arr.data(), array_size);
		return ImGui::IsItemDeactivatedAfterEdit();
	}
} // end anonymous namespace

template<> inline bool ParamEditNode<EditParamType::INT2>::ShowValueUI() { return ShowArrayValueUI(value);}
template<> inline bool ParamEditNode<EditParamType::INT3>::ShowValueUI() { return ShowArrayValueUI(value);}
template<> inline bool ParamEditNode<EditParamType::INT4>::ShowValueUI() { return ShowArrayValueUI(value);}
template<> inline bool ParamEditNode<EditParamType::FLOAT2>::ShowValueUI() { return ShowArrayValueUI(value);}
template<> inline bool ParamEditNode<EditParamType::FLOAT3>::ShowValueUI() { return ShowArrayValueUI(value);}
template<> inline bool ParamEditNode<EditParamType::FLOAT4>::ShowValueUI() { return ShowArrayValueUI(value);}

template<>
inline bool ParamEditNode<EditParamType::STRING>::ShowValueUI()
{
	char buffer[256];
	constexpr size_t BUFFER_SIZE = sizeof(buffer) / sizeof(char);
	strncpy_s(buffer, value.c_str(), BUFFER_SIZE);
	if (ImGui::InputText("##", buffer, BUFFER_SIZE))
	{
		value = std::string(buffer);
	}

	return ImGui::IsItemDeactivatedAfterEdit();
}
template<>
inline bool ParamEditNode<EditParamType::BOOL>::ShowValueUI()
{
	ImGui::Checkbox("##", &value);
	return ImGui::IsItemDeactivatedAfterEdit();
}

enum class ValueEditModalResult
{
	CONFIRMED,	// OKボタンが押された
	CANCELED,	// キャンセルボタンが押された
	CONTINUE	// まだ確定していない
};

/// <summary>
/// ブロックスキン選択ポップアップを表示する
/// </summary>
/// <param name="block_skin_id">選択されたブロックスキンのID</param>
/// <param name="popup_str_id">ポップアップのID</param>
/// <param name="is_popup_open_timing">ポップアップが開かれたタイミングか. モーダルを開いた時に, 変更前のスキンが表示されるページを開くために使用される</param>
ValueEditModalResult ShowBlockSkinSelectionDialog(MasterDataID& block_skin_id, const char* popup_str_id, const bool is_popup_open_timing);

template<>
inline bool ParamEditNode<EditParamType::BLOCK_SKIN>::ShowValueUI()
{
	constexpr const char* popup_str_id = "SelectBlockSkin";
	bool is_popup_open_timing = false;

	ImGui::Text("%d", value);
	ImGui::SameLine();

	const ImVec2 button_size = ImGui::CalcTextSize(" Select ");
	if (ImGui::Button("Select", button_size))
	{
		ImGui::OpenPopup(popup_str_id);
		is_popup_open_timing = true;
	}
	
	// ポップアップウィンドウを表示
	{
		const MasterDataID last_value = value;

		ValueEditModalResult result = ShowBlockSkinSelectionDialog(value, popup_str_id, is_popup_open_timing);
		switch (result)
		{
		case ValueEditModalResult::CONFIRMED:
			return true;

		case ValueEditModalResult::CANCELED:
			value = last_value;
			return true;

		default:
			return false;
		}
	}

	return false;
}

/// <summary>
/// アイテム選択ポップアップを表示する
/// </summary>
/// <param name="item_id">アイテムID</param>
/// <param name="popup_str_id">ポップアップのID</param>
/// <param name="is_popup_open_timing">ポップアップが開かれたタイミングか. モーダルを開いた時に, 変更前のスキンが表示されるページを開くために使用される</param>
/// <returns></returns>
ValueEditModalResult ShowItemSelectionDialog(MasterDataID& item_id, const char* popup_str_id, const bool is_popup_open_timing);

template<>
inline bool ParamEditNode<EditParamType::ITEM_ID>::ShowValueUI()
{
	ImGui::Texture icon_texture;
	MasterHelper::GetGameIconImguiImage(MdItem::Get(value).icon_id, icon_texture);
	constexpr const char* popup_str_id = "str_id_select_item_modal";

	bool is_popup_open_timing = ImGui::ImageButton("##open_modal_button", icon_texture, ImVec2{ 32,32 });

	if (ImGui::BeginItemTooltip())
	{
		// ツールチップにアイテム名を表示
		const char* item_name = MdItem::Get(value).item_name.c_str();
		ImGui::Text(item_name);
		ImGui::EndTooltip();
	}

	if (is_popup_open_timing)
	{
		// 値編集開始
		ImGui::OpenPopup(popup_str_id);
	}

	const MasterDataID last_value = value;

	ValueEditModalResult result = ShowItemSelectionDialog(value, popup_str_id, is_popup_open_timing);
	switch (result)
	{
	case ValueEditModalResult::CONFIRMED:
		return true;

	case ValueEditModalResult::CANCELED:
		value = last_value;
		return true;

	default:
		return false;
	}
}

template<>
inline bool ParamEditNode<EditParamType::COLOR3>::ShowValueUI()
{
	ImGui::ColorEdit3("##", value.data());

	return ImGui::IsItemDeactivatedAfterEdit();
}

template<>
inline bool ParamEditNode<EditParamType::COLOR4>::ShowValueUI()
{
	ImGui::ColorEdit4("##", value.data());

	return ImGui::IsItemDeactivatedAfterEdit();
}
//~ End ParamEditNode specializations 