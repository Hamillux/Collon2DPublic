#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <type_traits>

/// <summary>
/// <para>
/// " DEFINE_ENUM() enum class MyEnum { ... }; "
/// </para><para>
/// の形式で定義された列挙型のヘルパークラス.
/// </para><para>
/// カスタムビルドで_generated.cppファイルを生成する
/// </para><para>
/// _generated.cppにはEnumInfo静的メンバの定義が書かれる
/// </para><para>
/// ※ EnumInfoを利用するenum classを定義するファイルは項目テンプレート'EnumDefinition'で追加する
/// </para>
/// </summary>
template<typename EnumType>
class EnumInfo
{
public:
    using UnderlyingType = typename std::underlying_type_t<EnumType>;
    static UnderlyingType CastToUnderlyingType(const EnumType e)
    {
        return static_cast<UnderlyingType>(e);
    }

    static const char* EnumToString(const EnumType e)
    {
        for (auto& pair : name_to_enum_map)
        {
            if (e == pair.second)
            {
                return pair.first.c_str();
            }
        }

        throw std::runtime_error("Unknown enumerator");
    }

    static EnumType StringToEnum(const std::string& str)
    {
        return name_to_enum_map.at(str);
    }

    static const std::vector<EnumType> List() { return enumerators; }

private:
    static std::unordered_map<std::string, EnumType> name_to_enum_map;
    static std::vector<EnumType> enumerators;
};

// EnumInfoのテンプレートパラメータとして利用するenum classの定義の前に書く
#define DEFINE_ENUM()
