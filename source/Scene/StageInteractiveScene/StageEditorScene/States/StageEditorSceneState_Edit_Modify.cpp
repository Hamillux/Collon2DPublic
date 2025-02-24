#include "StageEditorSceneState_Edit_Modify.h"
#include "GameSystems/FontManager.h"
#include "Scene/StageInteractiveScene/StageEditorScene/ParameterEditing/ParameterEditingInclude.h"

namespace
{
	constexpr bool SHOULD_OPEN_RIGHTBAR_ON_TARGET_SELECTED = true;
	constexpr MasterDataID ENTITY_PARAMS_FONT_ID = 2;
}

StageEditorSceneState_Edit_Modify::StageEditorSceneState_Edit_Modify()
{
}

StageEditorSceneState_Edit_Modify::~StageEditorSceneState_Edit_Modify()
{
}

void StageEditorSceneState_Edit_Modify::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);

	if (IsModifyingActor())
	{
		parent_scene.DrawActorConvex(actor_modification_context->actor, 0xFFFF00, 200);
	}
}

void StageEditorSceneState_Edit_Modify::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	__super::HandleInput(parent_scene, delta_seconds);

	const Vector2D mouse_pos = DeviceInput::GetMousePosition();
	const bool is_mouse_on_stage = parent_scene.IsMouseOnStage(mouse_pos);

	// ステージ上を左クリック
	if (DeviceInput::IsPressed(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE) && is_mouse_on_stage)
	{
		if (Actor* hovered_actor = parent_scene.GetActorAt(mouse_pos))
		{
			BeginActorModification(parent_scene, hovered_actor);
			if (SHOULD_OPEN_RIGHTBAR_ON_TARGET_SELECTED) 
			{
				parent_scene.OpenSidebar(parent_scene.sidebar_info->right);
			}
		}
		else
		{
			EndActorModification();
		}
	}

}

void StageEditorSceneState_Edit_Modify::ShowControls(StageEditorScene& parent_editor_scene)
{
	__super::ShowControls(parent_editor_scene);
}

void StageEditorSceneState_Edit_Modify::ShowRightbarBody(StageEditorScene& parent_editor_scene)
{
	if (IsModifyingActor())
	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(ENTITY_PARAMS_FONT_ID));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 60);
		actor_modification_context->edit_param_group_root->ShowAsImguiTreeElement();
		ImGui::PopFont();
	}
}

void StageEditorSceneState_Edit_Modify::OnUndo(StageEditorScene& parent_editor_scene)
{
	__super::OnUndo(parent_editor_scene);

	if(IsModifyingActor())
	{
		BeginActorModification(parent_editor_scene, actor_modification_context->actor);
	}
}

void StageEditorSceneState_Edit_Modify::OnRedo(StageEditorScene& parent_editor_scene)
{
	__super::OnRedo(parent_editor_scene);

	if(IsModifyingActor())
	{
		BeginActorModification(parent_editor_scene, actor_modification_context->actor);
	}
}

void StageEditorSceneState_Edit_Modify::BeginActorModification(StageEditorScene& parent_scene, Actor* target_actor)
{
	const bool can_begin_modification = parent_scene.IsValid(target_actor) && parent_scene.IsActorInStage(target_actor);
	if (!can_begin_modification)
	{
		EndActorModification();
		return;
	}

	if (IsModifyingActor())
	{
		EndActorModification();
	}
	
	actor_modification_context 
		= std::make_unique<ActorModificationContext>(target_actor, parent_scene.GetSpawnActorInfo(target_actor), parent_scene._command_history);

	// パラメータ変更時のコールバック登録
	actor_modification_context->edit_param_group_root->events.OnValueChanged += [&parent_scene, target_actor]()
		{
			parent_scene.ReloadActorInStage(target_actor);
		};
}

void StageEditorSceneState_Edit_Modify::EndActorModification()
{
	actor_modification_context.reset();
}

StageEditorSceneState_Edit_Modify::ActorModificationContext::ActorModificationContext(Actor* const in_actor, const std::shared_ptr<SpawnActorInfo>& in_spawn_info, std::shared_ptr<CommandHistory> command_history)
	: actor(in_actor)
	, spawn_info(in_spawn_info)
{
	edit_param_group_root = std::make_shared<ParamEditGroup>("", 10);
	spawn_info->initial_params->AddToParamEditGroup(edit_param_group_root, command_history);
}
