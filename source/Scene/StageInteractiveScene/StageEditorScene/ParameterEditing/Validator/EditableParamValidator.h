#pragma once
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/EditParamType.h"
#include "Utility/Core/MathCore.h"
#include "Utility/Core/StringUtils.h"
#include <algorithm>
#include <cmath>
#include "GameSystems/MasterData/MasterDataInclude.h"
#include <imgui.h>
#include <array>
#include <string>

#undef max
#undef min

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


/// <summary>
/// 
/// </summary>
template<EditParamType EPT>
class EditParamValidator
{
public:
	using ValueType = typename edit_param_value_type<EPT>::type;
	EditParamValidator() {}

	/// <summary>
	/// 値の検証と, 場合によっては加工も行われる
	/// </summary>
	/// <param name="unvalidated">未検証の値</param>
	/// <returns>unvalidatedが有効であった, もしくは加工されて有効になったらtrue</returns>
	virtual bool Validate(ValueType& unvalidated) { return true; }
};

////////////////////////////////////////////////////
// Basic validators
////////////////////////////////////////////////////

/// <summary>
/// 子Validatorを保有するValidator.
/// </summary>
template<EditParamType EPT>
class EditParamValidatorComposite : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidatorComposite(std::vector<std::shared_ptr<EditParamValidator<EPT>>>&& in_validators)
		: validators(std::move(in_validators))
	{}
	bool Validate(ValueType& unvalidated)
	{
		bool is_valid = true;
		for (auto& validator : validators)
		{
			is_valid &= validator->Validate(unvalidated);
			if (!is_valid)
			{
				break;
			}
		}
		return is_valid;
	}

private:
	std::vector< std::shared_ptr<EditParamValidator<EPT>>> validators;
};


namespace
{
template<typename T>
inline T ClampMin(const T& x, const T& x_min)
{
	return std::max(x_min, x);
}
} // end anonymous namespace

// X以上であることを保証: INT, FLOAT
template<EditParamType EPT>
class EditParamValidator_ClampMin : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidator_ClampMin(const ValueType& in_clamp_min)
		: clamp_min(in_clamp_min)
	{}

	virtual bool Validate(ValueType& unvalidated) override
	{
		unvalidated = ClampMin(unvalidated, clamp_min);
		return true;
	}

private:
	const ValueType clamp_min;
};

// array要素がX以上であることを保証: INT2, INT3, INT4, FLOAT2, FLOAT3, FLOAT4
template<EditParamType EPT>
class EditParamValidator_ClampMinArray : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
	using ElementType = typename ValueType::value_type;
	static constexpr size_t ArraySize = std::tuple_size<ValueType>::value;
public:

	EditParamValidator_ClampMinArray(const ElementType& in_clamp_min) : clamp_min(in_clamp_min) {}
	virtual bool Validate(ValueType& unvalidated) override
	{
		for (ElementType& x : unvalidated)
		{
			x = ClampMin(x, clamp_min);
		}
		return true;
	}

private:
	const ElementType clamp_min;
};

namespace {
template<typename T>
inline T ClampMax(const T& x, const T& x_max)
{
	return std::min(x_max, x);
}
} // end anonymous namespace

// X以下であることを保証: INT, FLOAT
template<EditParamType EPT>
class EditParamValidator_ClampMax : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidator_ClampMax(const ValueType& in_clamp_max)
		: clamp_max(in_clamp_max)
	{}

	virtual bool Validate(ValueType& unvalidated) override
	{
		unvalidated = ClampMax(unvalidated, clamp_max);
		return true;
	}

private:
	const ValueType clamp_max;
};

// array要素がX以下であることを保証: INT2, INT3, INT4, FLOAT2, FLOAT3, FLOAT4
template<EditParamType EPT>
class EditParamValidator_ClampMaxArray : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
	using ElementType = typename ValueType::value_type;
	static constexpr size_t ArraySize = std::tuple_size<ValueType>::value;
public:

	EditParamValidator_ClampMaxArray(const ElementType& in_clamp_max) : clamp_max(in_clamp_max) {}
	virtual bool Validate(ValueType& unvalidated) override
	{
		for (ElementType& x : unvalidated)
		{
			x = ClampMax(x, clamp_max);
		}
		return true;
	}

private:
	const ElementType clamp_max;
};


namespace {
template<typename T>
inline T Clamp(const T& x, const T& x_min, const T& x_max)
{
	return std::min(x_max, std::max(x_min, x));
}
}

// X以上Y以下であることを保証
template<EditParamType EPT>
class EditParamValidator_Clamp : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidator_Clamp(const ValueType& in_clamp_min, const ValueType& in_clamp_max)
		: clamp_min(in_clamp_min), clamp_max(in_clamp_max)
	{}

	virtual bool Validate(ValueType& unvalidated) override
	{
		unvalidated = Clamp(unvalidated, clamp_min, clamp_max);
		return true;
	}

private:
	const ValueType clamp_min;
	const ValueType clamp_max;
};

// array要素がX以上Y以下であることを保証: INT2, INT3, INT4, FLOAT2, FLOAT3, FLOAT4
template<EditParamType EPT>
class EditParamValidator_ClampArray : public EditParamValidator<EPT>
{
	using ValueType = typename EditParamValidator<EPT>::ValueType;
	using ElementType = typename ValueType::value_type;
	static constexpr size_t ArraySize = std::tuple_size<ValueType>::value;
public:

	EditParamValidator_ClampArray(const ElementType& in_clamp_min, const ElementType& in_clamp_max)
		: clamp_min(in_clamp_min)
		, clamp_max(in_clamp_max)
	{}
	virtual bool Validate(ValueType& unvalidated) override
	{
		for (ElementType& x : unvalidated)
		{
			x = Clamp(x, clamp_min, clamp_max);
		}
		return true;
	}

private:
	const ElementType clamp_min;
	const ElementType clamp_max;
};

// 文字数を制限する: STRING
class EditParamValidator_MaxStringLength : public EditParamValidator<EditParamType::STRING>
{
	using ValueType = typename EditParamValidator<EditParamType::STRING>::ValueType;
public:
	EditParamValidator_MaxStringLength(const size_t in_max_length)
		: max_length(in_max_length)
	{}
	virtual bool Validate(ValueType& unvalidated) override
	{
		std::string validated;
		GetUtf8Substring(unvalidated, max_length, validated);
		unvalidated = validated;
		return true;
	}
private:
	const size_t max_length;
};

namespace
{
template<typename T>
inline auto Mod(const T& dividend, const T& divisor) -> typename std::enable_if_t<std::is_integral_v<T>, T>
{
	return dividend % divisor;
}

template<typename T>
inline auto Mod(const T& dividend, const T& divisor) -> typename std::enable_if_t<std::is_floating_point_v<T>, T>
{
	return std::fmod(dividend, divisor);
}

}// end anonymous namespace

// 値がdivisor未満であることを保証する. 
// 値がdivisor以上である場合, その剰余を値として設定する
template<EditParamType EPT>
class EditParamValidator_Mod : public EditParamValidator<EPT>
{
	static_assert(
		EPT == EditParamType::INT ||
		EPT == EditParamType::FLOAT,
		"Invalid EditParamType enumerator for EditParamValidator_Mod"
		);
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidator_Mod(const ValueType& in_divisor)
		: divisor(in_divisor)
	{}
	
	virtual bool Validate(ValueType& unvalidated) override
	{
		unvalidated = Mod(unvalidated, divisor);
		return true;
	}

private:
	const ValueType divisor;
};

// 配列要素の値がdivisor未満であることを保証する. 
// 値がdivisor以上である場合, その剰余を値として設定する
template<EditParamType EPT>
class EditParamValidator_ModArray : public EditParamValidator<EPT>
{
	static_assert(
		EPT == EditParamType::INT2 ||
		EPT == EditParamType::INT3 ||
		EPT == EditParamType::INT4 ||
		EPT == EditParamType::FLOAT2 ||
		EPT == EditParamType::FLOAT3 ||
		EPT == EditParamType::FLOAT4,
		"Invalid EditParamType enumerator for EditParamValidator_ModArray"
		);
	using ValueType = typename EditParamValidator<EPT>::ValueType;
public:
	EditParamValidator_ModArray(const ValueType& in_divisor)
		: divisor(in_divisor)
	{}

	virtual bool Validate(ValueType& unvalidated) override
	{
		for(auto& x : unvalidated)
		{
			x = Mod(x, divisor);
		}
		return true;
	}

private:
	const ValueType divisor;
};