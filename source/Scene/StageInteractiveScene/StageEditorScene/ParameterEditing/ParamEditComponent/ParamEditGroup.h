#pragma once
#include "Utility/ImGui/ImGuiInclude.h"
#include "ParamEditComponent.h"
#include "Actor/EntityType.h"
#include <memory>

struct ParamEditGroupStyle
{
	ImVec4 bar_color;
	ImVec4 bar_color_hovered;
	ImVec4 bar_color_active;

	ParamEditGroupStyle()
		: bar_color(ImGui::GetStyleColorVec4(ImGuiCol_Header))
		, bar_color_hovered(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered))
		, bar_color_active(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive))
	{}

	ParamEditGroupStyle(const ImVec4& base_color, const float hovered_brightness, const float active_brightness)
		: bar_color(base_color)
		, bar_color_hovered(ImVec4(base_color.x* hovered_brightness, base_color.y* hovered_brightness, base_color.z* hovered_brightness, base_color.w))
		, bar_color_active(ImVec4(base_color.x* active_brightness, base_color.y* active_brightness, base_color.z* active_brightness, base_color.w))
	{}

	ParamEditGroupStyle(const ImU32 base_color_rbga, const float hovered_brightness, const float active_brightness)
	{
		const ImVec4 base_color = ImGui::ColorConvertU32ToFloat4(ImGui::SwizzleImU32_rgba_abgr(base_color_rbga));
		bar_color = base_color;
		bar_color_hovered = ImVec4(base_color.x * hovered_brightness, base_color.y * hovered_brightness, base_color.z * hovered_brightness, base_color.w);
		bar_color_active = ImVec4(base_color.x * active_brightness, base_color.y * active_brightness, base_color.z * active_brightness, base_color.w);
	}
};

/// <summary>
/// パラメータ編集のためのグループを表現する編集コンポーネント.
/// 複数の編集コンポーネントをグループ化し, ImGui を使用してツリー構造のUI要素として表示する機能を提供する。
/// </summary>
class ParamEditGroup : public ParamEditComponent
{
	//~ Begin ParamEditComponent interface
protected:
	/// <summary>
	/// ImGuiを使用してツリー構造のUI要素を表示する.
	/// グループ内の各子要素を再帰的に表示し, styleが有効な場合はスタイルを適用する.
	/// </summary>
	/// <param name="id">各ImGui要素に割り当てるID. ShowAsImguiTreeElement_Implを呼び出すたびにインクリメントされる.</param>
	virtual void ShowAsImguiTreeElement_Impl(int& id) override;
	//~ End ParamEditComponent interface

public:
    /// <param name="in_name">グループ名. 子グループの場合, 展開/折り畳み部分のラベルとして表示される</param>
    /// <param name="in_children">グループに含まれる編集コンポーネントのリスト.</param>
    /// <param name="inStyle">グループのスタイル設定. nullptr出ない場合に適用される.</param>
    ParamEditGroup(const std::string in_name, std::vector<std::shared_ptr<ParamEditComponent>>&& in_children, const std::shared_ptr<ParamEditGroupStyle>& in_style = nullptr);

	ParamEditGroup(const std::string in_name, const std::vector<std::shared_ptr<ParamEditComponent>>& in_children, const std::shared_ptr<ParamEditGroupStyle>& in_style = nullptr);

	/// <param name="in_name">グループ名. 子グループの場合, 展開/折り畳み部分のラベルとして表示される</param>
	/// <param name="children_size">グループに含まれる編集コンポーネントの数. メモリを事前に確保するための引数</param>
	/// <param name="inStyle">グループのスタイル設定. nullptr出ない場合に適用される.</param>
	ParamEditGroup(const std::string& in_name, const size_t children_size, const std::shared_ptr<ParamEditGroupStyle>& in_style = nullptr);

	/// <summary>
	/// デフォルトコンストラクタ. 名前なし, 子要素なしのグループを作成する.
	/// </summary>
	ParamEditGroup();

	/// <summary>
	/// 子編集コンポーネントを追加する.
	/// </summary>
	void AddChild(std::shared_ptr<ParamEditComponent> newChild);

	static void Test();

private:
	void BindEventsOnChild(std::shared_ptr<ParamEditComponent> child);
	bool ShouldPushStyle() const { return style != nullptr; }
	void PushGroupStyles();
	void PopGroupStyles();

	std::string name;
	std::vector<std::shared_ptr<ParamEditComponent>> children;
	std::shared_ptr<ParamEditGroupStyle> style;
};