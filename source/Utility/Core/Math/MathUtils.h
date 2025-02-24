#pragma once

#include "Utility/IJsonSerializable.h"
#include <nlohmann/json.hpp>
#include "RandomNumberGenerator.h"
#include <algorithm>
#include <limits>

#undef max
#undef min

/// <summary>
/// 常に正の値を返す剰余演算を行う。
/// <para>例: (-1,3)->2 / (-4,3)->2 / (4,3)->1</para>
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="dividend"></param>
/// <param name="divisor"></param>
/// <returns></returns>
template<typename T>
inline constexpr T positive_modulo(const T dividend, const T divisor)
{
	return (dividend % divisor + divisor) % divisor;
}

template<typename T>
inline constexpr T clamp(const T x, const T x_min, const T x_max)
{
	return (std::min)(x_max, (std::max)(x, x_min));
}

// 型Tの最大値
template<typename T>
inline constexpr T GetMaxValueOfType()
{
	return std::numeric_limits<T>::max();
}

// 型Tの最小値
template<typename T>
inline constexpr T GetMinValueOfType()
{
	return std::numeric_limits<T>::min();
}

/// <summary>
/// 指定された型 T の値の範囲を表すテンプレート構造体。
/// この構造体は、最小値 (min) と最大値 (max) を保持し、JSONとの相互変換機能を提供する。
/// また、範囲内のランダムな値を生成する機能も持つ。
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
struct ValueRange : public IJsonValue
{
	static_assert(std::is_arithmetic_v<T>, "T is NOT a arithmetic type");
	T min;
	T max;
	ValueRange(T in_min, T in_max)
		: min(in_min)
		, max(in_max)
	{}

	// min == max == in_value
	ValueRange(T in_value)
		: min(in_value)
		, max(in_value)
	{}

	virtual void ToJsonValue(nlohmann::json& value_json) const override;
	virtual void FromJsonValue(const nlohmann::json& value_json) override;

	/// <summary>
	/// 範囲内のランダムな値を取得する。
	/// </summary>
	/// <returns>[min, max]のランダム値</returns>
	T GetRandomValueInRange() const
	{
		return RandomNumberGenerator::GetRandomFloat(this->min, this->max);
	}
};
using FloatRange = ValueRange<float>;
using IntRange = ValueRange<int>;

template<typename T>
inline void ValueRange<T>::ToJsonValue(nlohmann::json& value_json) const
{
	value_json = nlohmann::json::array({ this->min, this->max });
}

template<typename T>
inline void ValueRange<T>::FromJsonValue(const nlohmann::json& value_json)
{
	if (!value_json.is_array())
	{
		throw std::runtime_error("Attempted to parse non-array value to ValueRange");
	}

	value_json.at(0).get_to(this->min);
	value_json.at(1).get_to(this->max);
}
