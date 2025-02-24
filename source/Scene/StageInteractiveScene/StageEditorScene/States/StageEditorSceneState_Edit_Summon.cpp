#include "StageEditorSceneState_Edit_Summon.h"
#include "GameSystems/FontManager.h"
#include "Input/DeviceInput.h"
#include "Actor/AllActorsInclude_generated.h"

namespace
{
	constexpr MasterDataID CATEGORY_TEXT_FONT_ID = 2;

}

StageEditorSceneState_Edit_Summon::StageEditorSceneState_Edit_Summon()
	: _idx_summon_icon(-1)
{
}

StageEditorSceneState_Edit_Summon::~StageEditorSceneState_Edit_Summon()
{
}

void StageEditorSceneState_Edit_Summon::ShowRightbarBody(StageEditorScene& parent_editor_scene)
{
	constexpr int SUMMON_ENTITY_ICON_SIZE = 48;
	constexpr ImVec2 BUTTON_SIZE = ImVec2(SUMMON_ENTITY_ICON_SIZE, SUMMON_ENTITY_ICON_SIZE);

	// エンティティのアイコンを表示する領域の背景色
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));

	// エンティティのアイコンに振る連番は全カテゴリで共有する.
	int idx_summon_icon_temp = 0;

	// 1カテゴリのアイコン群を表示する
	auto ShowSummonButtonsWindow = [this, &parent_editor_scene, BUTTON_SIZE, &idx_summon_icon_temp](const char* strId, const char* text, const std::vector<std::shared_ptr<StageEditorScene::SummonEntityInfo>>& summon_infos_of_category)
		{
			ImGui::PushFont(FontManager::GetInstance().GetFont(CATEGORY_TEXT_FONT_ID));
			ImGui::Text(text);
			ImGui::PopFont();
			const ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, 128);
			ImGui::BeginChild(strId, child_size);
			constexpr int NUM_COLUMNS = 6;	// 1行に表示するアイコン数
			constexpr int UPPER_PADDING = 8;
			constexpr int LEFT_PADDING = 4;
			ImGui::SetCursorPos(ImVec2(LEFT_PADDING, UPPER_PADDING));

			ImGui::PushStyleColors_Button(ImGui::GetStyleColorVec4(ImGuiCol_Button) * ImVec3(0.5, 0.5, 0.5), 1, 1);
			for (size_t i = 0; i < summon_infos_of_category.size(); i++)
			{
				const int x = i % NUM_COLUMNS;
				const std::shared_ptr<StageEditorScene::SummonEntityInfo> info = summon_infos_of_category.at(i);

				ImGui::PushID(i);
				static int idx_selected_entity = 0;
				constexpr float PADDING = 2;
				constexpr ImVec4 PADDING_COLOR_PRESSED = ImVec4(1, 0, 0, 1);
				constexpr ImVec4 PADDING_COLOR_RELEASED = ImVec4(0, 0, 0, 1);
				ImGui::Texture icon_texture{};
				MasterHelper::GetGameIconImguiImage(MdEntity::Get(info->entity_id).icon_id, icon_texture);
				if (ImGui::ImageRadioButton(
					"##",
					_idx_summon_icon,
					idx_summon_icon_temp++,
					icon_texture.im_tex_id,
					BUTTON_SIZE,
					PADDING,
					PADDING_COLOR_PRESSED,
					PADDING_COLOR_RELEASED,
					icon_texture.uv0,
					icon_texture.uv1
				))
				{
					OnSummonIconClicked(parent_editor_scene, info);
				}
				if (x < NUM_COLUMNS - 1)
				{
					ImGui::SameLine();
				}
				else if (x == NUM_COLUMNS - 1)
				{
					ImGui::SetCursorPosX(LEFT_PADDING);
				}
				ImGui::PopID();
			}
			ImGui::PopStyleColors_Button();

			ImGui::EndChild();
		};
	ShowSummonButtonsWindow("SummonButtons_Character", "Character", parent_editor_scene._summon_infos.at(EEntityCategory::Character));
	ShowSummonButtonsWindow("SummonButtons_Block", "Block", parent_editor_scene._summon_infos.at(EEntityCategory::Block));
	ShowSummonButtonsWindow("SummonButtons_Gimmick", "Gimmick", parent_editor_scene._summon_infos.at(EEntityCategory::Gimmick));

	{
		ImGui::PushFont(FontManager::GetInstance().GetFont(CATEGORY_TEXT_FONT_ID));
		ImGui::Text("Item");
		ImGui::PopFont();
		const ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, 128);
		ImGui::BeginChild("SummonButtons_Item", child_size);
		constexpr int NUM_COLUMNS = 6;	// 1行に表示するアイコン数
		constexpr int UPPER_PADDING = 8;
		constexpr int LEFT_PADDING = 4;
		ImGui::SetCursorPos(ImVec2(LEFT_PADDING, UPPER_PADDING));

		ImGui::PushStyleColors_Button(ImGui::GetStyleColorVec4(ImGuiCol_Button) * ImVec3(0.5, 0.5, 0.5), 1, 1);
		const std::vector<MdItem>& all_items = MdItem::GetData();
		for (size_t i = 0; i < all_items.size(); i++)
		{
			const MdItem& current_item = all_items.at(i);
			const int x = i % NUM_COLUMNS;
			ImGui::Texture icon_texture{};
			MasterHelper::GetGameIconImguiImage(current_item.icon_id, icon_texture);

			ImGui::PushID(i);
			static int idx_selected_entity = 0;
			constexpr float PADDING = 2;
			constexpr ImVec4 PADDING_COLOR_PRESSED = ImVec4(1, 0, 0, 1);
			constexpr ImVec4 PADDING_COLOR_RELEASED = ImVec4(0, 0, 0, 1);
			if (ImGui::ImageRadioButton(
				"##",
				_idx_summon_icon,
				idx_summon_icon_temp++,
				icon_texture.im_tex_id,
				BUTTON_SIZE,
				PADDING,
				PADDING_COLOR_PRESSED,
				PADDING_COLOR_RELEASED,
				icon_texture.uv0,
				icon_texture.uv1
			))
			{
				std::shared_ptr<StageEditorScene::SummonEntityInfo> info = std::make_shared<StageEditorScene::SummonEntityInfo>();
				initial_params_of_actor_t<ItemActor> initial_params_of_item;
				initial_params_of_item.item_id = current_item.item_id;
				info->entity_id = 0;
				initial_params_of_item.ToJsonObject(info->initial_params_json);
				info->is_item_actor = true;
				OnSummonIconClicked(parent_editor_scene, info);
			}
			if (x < NUM_COLUMNS - 1)
			{
				ImGui::SameLine();
			}
			else if (x == NUM_COLUMNS - 1)
			{
				ImGui::SetCursorPosX(LEFT_PADDING);
			}
			ImGui::PopID();
		}
		ImGui::PopStyleColors_Button();
		ImGui::EndChild();
	}


	ImGui::PopStyleColor();
}

void StageEditorSceneState_Edit_Summon::OnSummonIconClicked(StageEditorScene& parent_editor_scene, const std::shared_ptr<StageEditorScene::SummonEntityInfo> selected_info)
{
	if (selected_info == nullptr)
	{
		return;
	}

	ResetActorToSummon(parent_editor_scene, selected_info);
}

void StageEditorSceneState_Edit_Summon::ResetActorToSummon(StageEditorScene& parent_editor_scene, const std::shared_ptr<StageEditorScene::SummonEntityInfo> summon_info)
{
	// 召喚対象変更のため, 未召喚のインスタンスを破壊
	if (parent_editor_scene.IsValid(_created_actor_to_summon))
	{
		_created_actor_to_summon->Finalize();
		parent_editor_scene.DestroyActor(_created_actor_to_summon);
	}

	_summoning_entity_info = summon_info;

	if (_summoning_entity_info == nullptr)
	{
		return;
	}

	if (!_summoning_entity_info->is_item_actor)
	{
		const std::string& entity_type_str = MdEntity::Get(_summoning_entity_info->entity_id).entity_type_str;
		EEntityType entity_type = EnumInfo<EEntityType>::StringToEnum(entity_type_str);
		auto initial_params = ActorFactory::CreateInitialParamsByEntityType(entity_type);
		initial_params->FromJsonObject(_summoning_entity_info->initial_params_json);
		_created_actor_to_summon = ActorFactory::CreateAndInitializeActorByEntityType(entity_type, initial_params.get(), &parent_editor_scene);
	}
	else
	{
		initial_params_of_actor_t<ItemActor> initial_params;
		initial_params.FromJsonObject(_summoning_entity_info->initial_params_json);
		_created_actor_to_summon = ActorFactory::CreateAndInitializeActor<ItemActor>(&initial_params, &parent_editor_scene);
	}
}

void StageEditorSceneState_Edit_Summon::SummonActor(StageEditorScene& parent_editor_scene)
{
	if (_summoning_entity_info == nullptr)
	{
		return;
	}

	Actor* new_actor = nullptr;

	if (!_summoning_entity_info->is_item_actor)
	{
		const std::string& entity_type_str = MdEntity::Get(_summoning_entity_info->entity_id).entity_type_str;
		EEntityType entity_type = EnumInfo<EEntityType>::StringToEnum(entity_type_str);
		auto initial_params = ActorFactory::CreateInitialParamsByEntityType(entity_type);
		initial_params->FromJsonObject(_summoning_entity_info->initial_params_json);
		new_actor = ActorFactory::CreateAndInitializeActorByEntityType(entity_type, initial_params.get(), &parent_editor_scene);
	}
	else
	{
		initial_params_of_actor_t<ItemActor> initial_params;
		initial_params.FromJsonObject(_summoning_entity_info->initial_params_json);
		new_actor = ActorFactory::CreateAndInitializeActor<ItemActor>(&initial_params, &parent_editor_scene);
	}

	// デフォルトの初期化パラメータを取得
	std::shared_ptr<SpawnActorInfo> spawn_info = std::make_shared<SpawnActorInfo>();
	if (_summoning_entity_info->is_item_actor)
	{
		spawn_info->entity_type = EEntityType::ItemActor;
		spawn_info->initial_params = ActorFactory::CreateInitialParamsByEntityType(EEntityType::ItemActor);
		spawn_info->initial_params->FromJsonObject(_summoning_entity_info->initial_params_json);
	}
	else
	{
		EEntityType entity_type = EnumInfo<EEntityType>::StringToEnum(MdEntity::Get(_summoning_entity_info->entity_id).entity_type_str);
		spawn_info->entity_type = entity_type;
		spawn_info->initial_params = ActorFactory::CreateInitialParamsByEntityType(spawn_info->entity_type);
		spawn_info->initial_params->FromJsonObject(_summoning_entity_info->initial_params_json);
	}

	spawn_info->initial_params->transform.position = _created_actor_to_summon->GetActorWorldPosition();

	parent_editor_scene.AddActorToWaitingRoom(new_actor);
	parent_editor_scene._command_history->ExecuteAndPush(
		std::make_shared<stage_editor_scene::CmdSummonActor>(&parent_editor_scene, new_actor, spawn_info)
	);

	parent_editor_scene._editor_scene_sounds->se_place_actor->Play();
}

void StageEditorSceneState_Edit_Summon::OnLeaveState(ParentSceneClass& parent_scene)
{
	if (parent_scene.IsValid(_created_actor_to_summon))
	{
		parent_scene.DestroyActor(_created_actor_to_summon);
	}

	_idx_summon_icon = -1;
	
	_summoning_entity_info.reset();

	__super::OnLeaveState(parent_scene);
}

void StageEditorSceneState_Edit_Summon::Draw(ParentSceneClass& parent_scene)
{
	__super::Draw(parent_scene);
	if (parent_scene.IsValid(_created_actor_to_summon) && parent_scene.IsMouseOnStage(DeviceInput::GetMousePosition()))
	{
		_created_actor_to_summon->Draw(parent_scene._camera_params);

		constexpr int convex_r = 255;
		constexpr int convex_g = 255;
		constexpr int convex_b = 255;
		parent_scene.DrawActorConvex(_created_actor_to_summon, DxLib::GetColor(convex_r, convex_g, convex_b));

		const Vector2D center = parent_scene._camera_params.TransformPosition_WorldToViewport(_created_actor_to_summon->GetActorWorldPosition());
		constexpr int point_radius = 3;
		constexpr int point_color = DxLib::GetInvertedColor(convex_r, convex_g, convex_b);
		DxLib::DrawCircle(center.x, center.y, point_radius, point_color, TRUE);
	}
}

void StageEditorSceneState_Edit_Summon::DrawForeground(ParentSceneClass& parent_scene, const CanvasInfo& canvas_info)
{
	__super::DrawForeground(parent_scene, canvas_info);
}

void StageEditorSceneState_Edit_Summon::HandleInput(ParentSceneClass& parent_scene, const float delta_seconds)
{
	__super::HandleInput(parent_scene, delta_seconds);

	const Vector2D& mouse_pos = DeviceInput::GetMousePosition();
	const Vector2D& mouse_pos_world = parent_scene._camera_params.TransformPosition_ViewportToWorld(mouse_pos);
	const bool is_mouse_on_stage = parent_scene.IsMouseOnStage(mouse_pos);

	// 召喚中アクターをマウス位置にスナップ
	if(is_mouse_on_stage)
	{
		if(parent_scene.IsValid(_created_actor_to_summon))
		{
			const Vector2D actor_pos =
				GetTileCenterPosition(mouse_pos_world, 0, 0) + _created_actor_to_summon->GetSnapPositionToActorPosition();

			_created_actor_to_summon->SetActorWorldPosition(actor_pos);
		}
	}

	// ステージ上を左クリック
	if (DeviceInput::IsPressed(MOUSE_INPUT_LEFT, DeviceInput::Device::MOUSE) && is_mouse_on_stage)
	{
		SummonActor(parent_scene);
	}
}
