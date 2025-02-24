
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

	// members
	std::string name;
	ValueType value;
	CallbackType process_validate_value;
	std::shared_ptr<Validator> validator;

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

protected:
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

		ShowValueUI();

		ImGui::PopStyleColor();
		ImGui::PopID();
	}

	/// <summary>
	/// 値編集用のUIを表示する
	/// <para>値変更開始・終了のタイミングでOnBegin/EndValueChange()を呼ぶ処理を書く必要がある</para>
	/// </summary>
	void ShowValueUI()
	{
		static_assert(false, "ShowValueUI is not implemented for this type");
	}

	void CheckValueChangeByItemFocus()
	{
		if (ImGui::IsItemActivated())
		{
			OnBeginValueChange();
		}
		else if (ImGui::IsItemDeactivated())
		{
			OnEndValueChange();
		}
	}

	void OnBeginValueChange()
	{ 
		// 値変更開始
		if (pLastValue == nullptr)
		{
			pLastValue = std::make_unique<ValueType>(value);
		}
		else
		{
			// 値変更開始のタイミングにもかかわらず, 変更前の値が残っている場合はエラー
			throw std::runtime_error("OnBeginValueChange() called twice without calling OnEndValueChange()");
		}
	}

	void OnEndValueChange()
	{
		if (pLastValue == nullptr)
		{
			// 値変更終了のタイミングにもかかわらず変更前の値が残っていない場合はエラー
			throw std::runtime_error("OnEndValueChange() called without calling OnBeginValueChange()");
		}

		if (validator != nullptr)
		{
			validator->Validate(value);
		}

		// 値が変わっていたらイベントを発行
		if (value != *pLastValue)
		{
			process_validate_value(value);
			events.OnValueChanged.Dispatch();
		}

		// 変更前の値を破棄
		pLastValue.reset();
	}

	/// <summary>
	/// OnBeginValueChange()で確保され, OnEndValueChange()で解放される
	/// </summary>
	std::unique_ptr<ValueType> pLastValue;

	friend void ShowValueUIImpl_ItemId(ParamEditNode<EditParamType::ITEM_ID>& node);
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
inline void ParamEditNode<EditParamType::INT>::ShowValueUI()
{
	ImGui::InputInt("##", &value);

	CheckValueChangeByItemFocus();
}

template<>
inline void ParamEditNode<EditParamType::FLOAT>::ShowValueUI()
{
	ImGui::InputFloat("##", &value);

	CheckValueChangeByItemFocus();
}

namespace
{
	template<typename T, int N>
	void ShowArrayValueUI(std::array<T, N>& arr)
	{
		const size_t array_size = arr.size();
		ImGui::InputScalarN("##", GetImGuiDataType<T>(), arr.data(), array_size);
	}
} // end anonymous namespace
template<> inline void ParamEditNode<EditParamType::INT2>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}
template<> inline void ParamEditNode<EditParamType::INT3>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}
template<> inline void ParamEditNode<EditParamType::INT4>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}
template<> inline void ParamEditNode<EditParamType::FLOAT2>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}
template<> inline void ParamEditNode<EditParamType::FLOAT3>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}
template<> inline void ParamEditNode<EditParamType::FLOAT4>::ShowValueUI() { ShowArrayValueUI(value); CheckValueChangeByItemFocus();}

template<>
inline void ParamEditNode<EditParamType::STRING>::ShowValueUI()
{
	char buffer[256];
	constexpr size_t BUFFER_SIZE = sizeof(buffer) / sizeof(char);
	strncpy_s(buffer, value.c_str(), BUFFER_SIZE);
	if (ImGui::InputText("##", buffer, BUFFER_SIZE))
	{
		value = std::string(buffer);
	}

	CheckValueChangeByItemFocus();
}
template<>
inline void ParamEditNode<EditParamType::BOOL>::ShowValueUI()
{
	const ValueType lastValue = value;
	ImGui::Checkbox("##", &value);
	if(lastValue != value)
	{
		ImGui::SetWindowFocus(NULL);
	}

	CheckValueChangeByItemFocus();
}

enum class ValueEditModalResult
{
	CONFIRMED,
	CANCELED,
	CONTINUE
};

/// <summary>
/// ブロックスキン選択ポップアップを表示する
/// </summary>
/// <returns>値が確定したか</returns>
ValueEditModalResult ShowBlockSkinSelectionDialog(MasterDataID& block_skin_id, const char* popup_str_id, const bool is_popup_open_timing);

template<>
inline void ParamEditNode<EditParamType::BLOCK_SKIN>::ShowValueUI()
{
	ImGui::Text("%d", value);
	ImGui::SameLine();

	constexpr const char* popup_str_id = "SelectBlockSkin";

	bool is_popup_open_timing = false;
	const ImVec2 button_size = ImGui::CalcTextSize(" Select ");
	if (ImGui::Button("Select", button_size))
	{
		// 値編集開始
		OnBeginValueChange();
		ImGui::OpenPopup(popup_str_id);
		is_popup_open_timing = true;
	}
	
	ValueEditModalResult result = ShowBlockSkinSelectionDialog(value, popup_str_id, is_popup_open_timing);
	switch (result)
	{
	case ValueEditModalResult::CONFIRMED:
		OnEndValueChange();
		break;
	case ValueEditModalResult::CANCELED:
		value = *pLastValue;
		OnEndValueChange();
		break;
	default:
		break;
	}
}

// TODO: クリックしたらアイテムID選択ダイアログを表示するUIにする
ValueEditModalResult ShowItemSelectionDialog(MasterDataID& block_skin_id, const char* popup_str_id, const bool is_popup_open_timing);
void ShowValueUIImpl_ItemId(ParamEditNode<EditParamType::ITEM_ID>& node);
template<>
inline void ParamEditNode<EditParamType::ITEM_ID>::ShowValueUI()
{
	ShowValueUIImpl_ItemId(*this);
}

template<>
inline void ParamEditNode<EditParamType::COLOR3>::ShowValueUI()
{
	ImGui::ColorEdit3("##", value.data());

	CheckValueChangeByItemFocus();
}

template<>
inline void ParamEditNode<EditParamType::COLOR4>::ShowValueUI()
{
	ImGui::ColorEdit4("##", value.data());

	CheckValueChangeByItemFocus();
}
//~ End ParamEditNode specializations 