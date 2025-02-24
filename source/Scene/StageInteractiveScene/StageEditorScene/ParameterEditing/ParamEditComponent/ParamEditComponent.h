#pragma once
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/EditParamType.h"
#include "Utility/Command/CommandBase.h"
#include <imgui.h>
#include "Utility/ImGui/ImGuiInclude.h"
#include "Utility/Core/Event.h"
#include <string>
#include <memory>
#include <vector>

/// <summary>
/// StageEditorSceneのパラメータ編集パネルでパラメータをImGuiでツリー表示し, パラメータを編集するための基底クラス
/// </summary>
class ParamEditComponent
{
public:
	ParamEditComponent()
		: _parent(nullptr)
	{}
	virtual ~ParamEditComponent() {}

	struct EventList
	{
		/// <summary>
		/// <para>EditParamNode: パラメータの値が変更されたときに呼ばれる</para>
		/// <para>ParamEditGroup: いずれかの子孫ノードのパラメータの値が変更されたときに呼ばれる</para>
		/// </summary>
		Event<> OnValueChanged;
	};
	EventList events;

	void ShowAsImguiTreeElement()
	{
		int id = 0;
		ShowAsImguiTreeElement_Impl(id);
	}

	void SetParent(ParamEditComponent* parent)
	{
		_parent = parent;
	}

	ParamEditComponent* GetRootComponent()
	{
		if (_parent == nullptr)
		{
			return this;
		}
		else
		{
			return _parent->GetRootComponent();
		}
	}


	//~ Begin ParamEditComponent interface
protected:
	/// <summary>
	/// ImGuiを使用してツリー構造のUI要素を表示する.
	/// </summary>
	/// <param name="id">各ImGui要素に割り当てるID. ShowAsImguiTreeElement_Implを呼び出すたびにインクリメントされる.</param>
	virtual void ShowAsImguiTreeElement_Impl(int& id) = 0;
	//~ End ParamEditComponent interface

	static void call_ShowAsImguiTreeElement_Impl(ParamEditComponent& edit_component, int& id)
	{
		edit_component.ShowAsImguiTreeElement_Impl(id);
	}
	static void call_ShowAsImguiTreeElement_Impl(std::shared_ptr<ParamEditComponent> edit_component, int& id)
	{
		edit_component->ShowAsImguiTreeElement_Impl(id);
	}

	ParamEditComponent* _parent;
};

// typename of std::vector<std::shared_ptr<ParamEditComponent>>
using EditComponentPtrVector = typename std::vector<std::shared_ptr<ParamEditComponent>>;