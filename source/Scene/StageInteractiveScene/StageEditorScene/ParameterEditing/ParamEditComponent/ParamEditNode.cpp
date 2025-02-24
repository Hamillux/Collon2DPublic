#include "ParamEditNode.h"
#include "SystemTypes.h"
#include "Actor/Mapchip/Block/BlockTexturing.h"
#include "GameSystems/GraphicResourceManager/GraphResourceManager.h"
#include "Input/DeviceInput.h"
#include <DxLib.h>

namespace
{
	constexpr ImGuiWindowFlags MODAL_FLAGS = 
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoNavFocus | 
		ImGuiWindowFlags_NoScrollbar | 
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse;
	constexpr ImVec2 MODAL_SIZE = ImVec2(800, 600);
	constexpr ImVec2 MODAL_POS = ImVec2(WINDOW_SIZE_X - MODAL_SIZE.x, WINDOW_SIZE_Y - MODAL_SIZE.y) * 0.5f;

	namespace BlockSkinDialog
	{
		// ページ変更セクション
		constexpr ImVec2 CHILD_WND_PIVOT_SWITCH_PAGE = ImVec2(0, 1);
		constexpr ImVec2 CHILD_WND_SIZE_SWITCH_PAGE = ImVec2(400, 100);

		// スキン選択セクション
		constexpr ImVec2 CHILD_WND_PIVOT_SKIN_BUTTONS = ImVec2(0, 0);
		constexpr ImVec2 CHILD_WND_SIZE_SKIN_BUTTONS = ImVec2(800,500);
		constexpr int NUM_SKINS_PER_PAGE_X = 6;
		constexpr int NUM_SKINS_PER_PAGE_Y = 4;
		constexpr int NUM_SKINS_PER_PAGE = NUM_SKINS_PER_PAGE_X * NUM_SKINS_PER_PAGE_Y;
		constexpr ImVec2 OFFSET_BUTTONS_OUTER = ImVec2(50,20);  // ボタンとウィンドウの間隔
		constexpr ImVec2 SKIN_BUTTON_SIZE = ImVec2(100, 100); // ボタンのサイズ
		constexpr ImVec2 OFFSET_BUTTONS_INNER = 
			(CHILD_WND_SIZE_SKIN_BUTTONS - OFFSET_BUTTONS_OUTER * 2.f - SKIN_BUTTON_SIZE * ImVec2(NUM_SKINS_PER_PAGE_X, NUM_SKINS_PER_PAGE_Y))
			/ ImVec2(NUM_SKINS_PER_PAGE_X - 1, NUM_SKINS_PER_PAGE_Y - 1);
		static_assert(OFFSET_BUTTONS_INNER.x > 0 && OFFSET_BUTTONS_INNER.y > 0, "OFFSET_BUTTONS_INNER must be positive");
		constexpr ImVec2 SKIN_BUTTON_FRAME_PADDING = ImVec2(1.f, 1.f) * 5.f;
		constexpr ImVec2 SKIN_BUTTON_IMAGE_TILE_SIZE = (SKIN_BUTTON_SIZE - SKIN_BUTTON_FRAME_PADDING) / 5.f;

		// OK/Cancelセクション
		constexpr ImVec2 CHILD_WND_PIVOT_OK_CANCEL = ImVec2(1,1);
		constexpr ImVec2 CHILD_WND_SIZE_OK_CANCEL = ImVec2(400, 100);

		constexpr float SEPARATOR_THICKNESS = 2.f;
		constexpr ImVec2 VERTICAL_SEPARATOR_FROM = ImVec2(MODAL_POS.x + 400, MODAL_POS.y + 500);
		constexpr ImVec2 VERTICAL_SEPARATOR_TO = ImVec2(MODAL_POS.x + 400, MODAL_POS.y + MODAL_SIZE.y);
		constexpr ImVec2 HORIZONTAL_SEPARATOR_FROM = ImVec2(MODAL_POS.x, MODAL_POS.y + 500);
		constexpr ImVec2 HORIZONTAL_SEPARATOR_TO = ImVec2(MODAL_POS.x + MODAL_SIZE.x, MODAL_POS.y + 500);

	}

	namespace ItemDialog 
	{
		// ページ変更セクション
		constexpr ImVec2 CHILD_WND_PIVOT_SWITCH_PAGE = ImVec2(0, 1);
		constexpr ImVec2 CHILD_WND_SIZE_SWITCH_PAGE = ImVec2(400, 100);

		// アイテム選択セクション
		constexpr ImVec2 CHILD_WND_PIVOT_ITEM_BUTTONS = ImVec2(0, 0);
		constexpr ImVec2 CHILD_WND_SIZE_ITEM_BUTTONS = ImVec2(800, 500);
		constexpr int NUM_ITEMS_PER_PAGE_X = 10;
		constexpr int NUM_ITEMS_PER_PAGE_Y = 5;
		constexpr int NUM_ITEMS_PER_PAGE = NUM_ITEMS_PER_PAGE_X * NUM_ITEMS_PER_PAGE_Y;
		constexpr ImVec2 OFFSET_BUTTONS_OUTER = ImVec2(50, 20);  // ボタンとウィンドウの間隔
		constexpr ImVec2 ITEM_BUTTON_SIZE = ImVec2(50, 50); // ボタンのサイズ
		constexpr ImVec2 OFFSET_BUTTONS_INNER =
			(CHILD_WND_SIZE_ITEM_BUTTONS - OFFSET_BUTTONS_OUTER * 2.f - ITEM_BUTTON_SIZE * ImVec2(NUM_ITEMS_PER_PAGE_X, NUM_ITEMS_PER_PAGE_Y))
			/ ImVec2(NUM_ITEMS_PER_PAGE_X - 1, NUM_ITEMS_PER_PAGE_Y - 1);
		static_assert(OFFSET_BUTTONS_INNER.x > 0 && OFFSET_BUTTONS_INNER.y > 0, "OFFSET_BUTTONS_INNER must be positive");
		constexpr ImVec2 ITEM_BUTTON_FRAME_PADDING = ImVec2(1.f, 1.f) * 5.f;
		constexpr ImVec2 ITEM_BUTTON_IMAGE_TILE_SIZE = (ITEM_BUTTON_SIZE - ITEM_BUTTON_FRAME_PADDING) / 5.f;

		// OK/Cancelセクション
		constexpr ImVec2 CHILD_WND_PIVOT_OK_CANCEL = ImVec2(1, 1);
		constexpr ImVec2 CHILD_WND_SIZE_OK_CANCEL = ImVec2(400, 100);

		constexpr float SEPARATOR_THICKNESS = 2.f;
		constexpr ImVec2 VERTICAL_SEPARATOR_FROM = ImVec2(MODAL_POS.x + 400, MODAL_POS.y + 500);
		constexpr ImVec2 VERTICAL_SEPARATOR_TO = ImVec2(MODAL_POS.x + 400, MODAL_POS.y + MODAL_SIZE.y);
		constexpr ImVec2 HORIZONTAL_SEPARATOR_FROM = ImVec2(MODAL_POS.x, MODAL_POS.y + 500);
		constexpr ImVec2 HORIZONTAL_SEPARATOR_TO = ImVec2(MODAL_POS.x + MODAL_SIZE.x, MODAL_POS.y + 500);
	}

	int CalculateTotalPages(const int items_per_page, const int total_items) {
		return (total_items + items_per_page - 1) / items_per_page;
	}

}

ValueEditModalResult ShowBlockSkinSelectionDialog(MasterDataID& block_skin_id, const char* popup_str_id, const bool is_popup_open_timing)
{
	ValueEditModalResult result = ValueEditModalResult::CONTINUE;

	const std::vector<MdBlockSkin>& block_skin_list = MdBlockSkin::GetData();
	const size_t num_pages = CalculateTotalPages(BlockSkinDialog::NUM_SKINS_PER_PAGE, block_skin_list.size());
	static size_t current_page = 0;
	if (is_popup_open_timing)
	{
		current_page = MdBlockSkin::GetIndex(block_skin_id) / BlockSkinDialog::NUM_SKINS_PER_PAGE;
	}

	auto goto_next_page = [num_pages]()
		{
			current_page = std::min<size_t>(num_pages - 1, current_page + 1);
		};
	auto goto_prev_page = []()
		{
			current_page = std::max<int>(0, current_page - 1);
		};

	// モーダル表示中の入力処理
	if(ImGui::IsPopupOpen(popup_str_id))
	{
		if (DeviceInput::WheelIsDown())
		{
			goto_next_page();
		}
		else if (DeviceInput::WheelIsUp())
		{
			goto_prev_page();
		}

		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			result = ValueEditModalResult::CANCELED;
		}
	}

	ImGui::SetNextWindowPos(MODAL_POS);
	ImGui::SetNextWindowSize(MODAL_SIZE);
	constexpr int POP_COUNT = 1;
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.75));
	bool open = true;
	if (ImGui::BeginPopupModal(popup_str_id, &open, MODAL_FLAGS))
	{
		if (result == ValueEditModalResult::CANCELED)
		{
			ImGui::CloseCurrentPopup();
		}

		// ページ切り替えボタンを表示するウィンドウ
		ImGui::BeginAlignedChild("child_wnd_switch_page", BlockSkinDialog::CHILD_WND_PIVOT_SWITCH_PAGE, BlockSkinDialog::CHILD_WND_PIVOT_SWITCH_PAGE, BlockSkinDialog::CHILD_WND_SIZE_SWITCH_PAGE);
		{
			// ページ切り替えボタン
			ImGui::Texture tex_prev, tex_next;
			MasterHelper::GetGameIconImguiImage(306, tex_prev);
			MasterHelper::GetGameIconImguiImage(307, tex_next);
			constexpr ImVec2 PREV_NEXT_BUTTON_SIZE = ImVec2(64, 64);

			constexpr int HORIZONTAL_PADDING = 100;
			constexpr int UPPER_PADDING = 10;
			ImGui::SetCursorPosX(HORIZONTAL_PADDING);
			ImGui::SetCursorPosY(UPPER_PADDING);
			if (ImGui::ImageButton("Prev", tex_prev.im_tex_id, PREV_NEXT_BUTTON_SIZE, tex_prev.uv0, tex_prev.uv1))
			{
				goto_prev_page();
			}

			// TODO: フォントサイズを調整する
			const std::string label_text = std::to_string(current_page + 1) + std::string(" / ") + std::to_string(num_pages);
			const ImVec2 label_size = ImGui::CalcTextSize(label_text.c_str());
			ImGui::SetCursorPosX((BlockSkinDialog::CHILD_WND_SIZE_SWITCH_PAGE.x - label_size.x) * 0.5f);
			ImGui::SetCursorPosY((PREV_NEXT_BUTTON_SIZE.y - label_size.y) * 0.5f + UPPER_PADDING);
			ImGui::Text(label_text.c_str());

			ImGui::SetCursorPosX(BlockSkinDialog::CHILD_WND_SIZE_SWITCH_PAGE.x - PREV_NEXT_BUTTON_SIZE.x - HORIZONTAL_PADDING);
			ImGui::SetCursorPosY(UPPER_PADDING);
			if (ImGui::ImageButton("Next", tex_next.im_tex_id, PREV_NEXT_BUTTON_SIZE, tex_next.uv0, tex_next.uv1))
			{
				goto_next_page();
			}
		}
		ImGui::EndAlignedChild();

		// ブロックスキン選択ボタンを表示するウィンドウ
		ImGui::BeginAlignedChild("child_wnd_skin_buttons", BlockSkinDialog::CHILD_WND_PIVOT_SKIN_BUTTONS, BlockSkinDialog::CHILD_WND_PIVOT_SKIN_BUTTONS, BlockSkinDialog::CHILD_WND_SIZE_SKIN_BUTTONS);
		{
			const size_t idx_skins_in_page_start = current_page * BlockSkinDialog::NUM_SKINS_PER_PAGE;
			const size_t idx_skins_in_page_end = std::min(idx_skins_in_page_start + BlockSkinDialog::NUM_SKINS_PER_PAGE, block_skin_list.size());

			// ページ内のブロックスキン選択ボタンを表示
			for (size_t i = 0; i < BlockSkinDialog::NUM_SKINS_PER_PAGE; i ++)
			{
				const size_t x = i % BlockSkinDialog::NUM_SKINS_PER_PAGE_X;
				const size_t y = i / BlockSkinDialog::NUM_SKINS_PER_PAGE_X;

				const size_t idx_skin = idx_skins_in_page_start + i;
				if (idx_skin >= idx_skins_in_page_end)
				{
					break;
				}
				const MdBlockSkin& current_skin = block_skin_list.at(idx_skin);
				const bool is_current_skin_selected = block_skin_id == current_skin.id;

				const float button_left = BlockSkinDialog::OFFSET_BUTTONS_OUTER.x + (BlockSkinDialog::SKIN_BUTTON_SIZE.x + BlockSkinDialog::OFFSET_BUTTONS_INNER.x) * x;
				const float button_top = BlockSkinDialog::OFFSET_BUTTONS_OUTER.y + (BlockSkinDialog::SKIN_BUTTON_SIZE.y + BlockSkinDialog::OFFSET_BUTTONS_INNER.y) * y;

				// ボタン本体
				ImGui::SetCursorPos(ImVec2(button_left, button_top));
				ImGui::PushID(i);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // ボタンの背景色を透明に設定
				ImGui::PushStyleColor(ImGuiCol_Border, is_current_skin_selected ? ImVec4(1, 0, 0, 1) : ImVec4(0.5, 0.5, 0.5, 1)); // 枠線の色を白に設定
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f); // 枠線の太さを設定
				if (ImGui::Button("##Button", BlockSkinDialog::SKIN_BUTTON_SIZE))
				{
					block_skin_id = block_skin_list.at(idx_skin).id;
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(2);
				ImGui::PopID();

				// ボタンをダブルクリック => 選択中のスキンで確定してモーダルを閉じる
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					result = ValueEditModalResult::CONFIRMED;
					ImGui::CloseCurrentPopup();
				}


				// ボタンの上にブロックスキンの画像を表示
				const MdImageFile& skin_image_file = MdImageFile::Get(current_skin.image_id);
				ID3D11ShaderResourceView* srv_texture = nullptr;
				GraphicResourceManager::GetInstance().GetTexture(skin_image_file.id, nullptr, &srv_texture);
				if (srv_texture == nullptr)
				{
					throw std::runtime_error("Failed to load block skin texture");
				}

				constexpr TileType TILE_TYPES[9] = 
				{
					TileType::A, TileType::B, TileType::C, TileType::D, TileType::E, TileType::F, TileType::G, TileType::H, TileType::I
				};

				ImVec2 child_pos = ImGui::GetWindowPos();

				for (int tile_y = 0; tile_y < 3; tile_y++)
				{
					for (int tile_x = 0; tile_x < 3; tile_x++)
					{
						const size_t i_tile_type = tile_y * 3 + tile_x;
						const TileType tile_type = TILE_TYPES[i_tile_type];
						const float tile_left = button_left + BlockSkinDialog::SKIN_BUTTON_FRAME_PADDING.x*0.5f + BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.x * (tile_x + 1);
						const float tile_top = button_top + BlockSkinDialog::SKIN_BUTTON_FRAME_PADDING.y*0.5f + BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.y * (tile_y + 1);

						float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
						BlockTextureMapping::GetTextureRegion(u0, v0, u1, v1, tile_type);
						int to_left = 0, to_top = 0, to_right = 0, to_bottom = 0;
						BlockTextureMapping::GetVertexPositionOffsetsFromTile(to_left, to_top, to_right, to_bottom, tile_type);

						ImGui::GetWindowDrawList()->AddImage(
							(ImTextureID)srv_texture,
							child_pos + ImVec2(tile_left + to_left * BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.x, tile_top + to_top * BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.y),
							child_pos + ImVec2(tile_left + to_right * BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.x, tile_top + to_bottom * BlockSkinDialog::SKIN_BUTTON_IMAGE_TILE_SIZE.y),
							ImVec2(u0, v0),
							ImVec2(u1, v1)
						);

					}
				}

			}
		}
		ImGui::EndAlignedChild();

		// OK/Cancelボタン
		ImGui::BeginAlignedChild("ok_and_cancel_buttons", BlockSkinDialog::CHILD_WND_PIVOT_OK_CANCEL, BlockSkinDialog::CHILD_WND_PIVOT_OK_CANCEL, BlockSkinDialog::CHILD_WND_SIZE_OK_CANCEL);
		{
			ImGui::BeginAlignedChild("buttons", ImVec2(0.5, 0.5), ImVec2(0.5, 0.5), ImVec2(300, 80));
			{
				ImGui::SetCursorPos(ImVec2(0, 0));
				if (ImGui::Button("OK", ImVec2(130, 80)))
				{
					result = ValueEditModalResult::CONFIRMED;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SetCursorPos(ImVec2(170, 0));
				if (ImGui::Button("Cancel", ImVec2(130, 80)))
				{
					result = ValueEditModalResult::CANCELED;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndAlignedChild();
		}
		ImGui::EndAlignedChild();

		// 区切り線
		{
			ImGui::GetWindowDrawList()->AddLine(
				BlockSkinDialog::VERTICAL_SEPARATOR_FROM,
				BlockSkinDialog::VERTICAL_SEPARATOR_TO,
				IM_COL32(0, 0, 0, 255),
				BlockSkinDialog::SEPARATOR_THICKNESS
			);

			ImGui::GetWindowDrawList()->AddLine(
				BlockSkinDialog::HORIZONTAL_SEPARATOR_FROM,
				BlockSkinDialog::HORIZONTAL_SEPARATOR_TO,
				IM_COL32(0, 0, 0, 255),
				BlockSkinDialog::SEPARATOR_THICKNESS
			);
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(POP_COUNT);

	return result;
}

ValueEditModalResult ShowItemSelectionDialog(MasterDataID& item_id, const char* popup_str_id, const bool is_popup_open_timing)
{
	ValueEditModalResult result = ValueEditModalResult::CONTINUE;

	const std::vector<MdItem>& item_list = MdItem::GetData();
	const size_t num_pages = CalculateTotalPages(ItemDialog::NUM_ITEMS_PER_PAGE, item_list.size());
	static size_t current_page = 0;
	if (is_popup_open_timing)
	{
		current_page = MdItem::GetIndex(item_id) / ItemDialog::NUM_ITEMS_PER_PAGE;
	}

	auto goto_next_page = [num_pages]()
		{
			current_page = std::min<size_t>(num_pages - 1, current_page + 1);
		};
	auto goto_prev_page = []()
		{
			current_page = std::max<int>(0, current_page - 1);
		};

	// モーダル表示中の入力処理
	if (ImGui::IsPopupOpen(popup_str_id))
	{
		if (DeviceInput::WheelIsDown())
		{
			goto_next_page();
		}
		else if (DeviceInput::WheelIsUp())
		{
			goto_prev_page();
		}

		if (DeviceInput::IsPressed(KEY_INPUT_ESCAPE))
		{
			result = ValueEditModalResult::CANCELED;
		}
	}

	ImGui::SetNextWindowPos(MODAL_POS);
	ImGui::SetNextWindowSize(MODAL_SIZE);
	constexpr int POP_COUNT = 1;
	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0.75));
	bool open = true;
	if (ImGui::BeginPopupModal(popup_str_id, &open, MODAL_FLAGS))
	{
		if (result == ValueEditModalResult::CANCELED)
		{
			ImGui::CloseCurrentPopup();
		}

		// ページ切り替えボタンを表示するウィンドウ
		ImGui::BeginAlignedChild
		(
			"child_wnd_switch_page",
			ItemDialog::CHILD_WND_PIVOT_SWITCH_PAGE,
			ItemDialog::CHILD_WND_PIVOT_SWITCH_PAGE,
			ItemDialog::CHILD_WND_SIZE_SWITCH_PAGE
		);
		{
			// ページ切り替えボタン
			ImGui::Texture tex_prev, tex_next;
			MasterHelper::GetGameIconImguiImage(306, tex_prev);
			MasterHelper::GetGameIconImguiImage(307, tex_next);
			constexpr ImVec2 PREV_NEXT_BUTTON_SIZE = ImVec2(64, 64);

			constexpr int HORIZONTAL_PADDING = 100;
			constexpr int UPPER_PADDING = 10;
			ImGui::SetCursorPosX(HORIZONTAL_PADDING);
			ImGui::SetCursorPosY(UPPER_PADDING);
			if (ImGui::ImageButton("Prev", tex_prev.im_tex_id, PREV_NEXT_BUTTON_SIZE, tex_prev.uv0, tex_prev.uv1))
			{
				goto_prev_page();
			}

			// TODO: フォントサイズを調整する
			const std::string label_text = std::to_string(current_page + 1) + std::string(" / ") + std::to_string(num_pages);
			const ImVec2 label_size = ImGui::CalcTextSize(label_text.c_str());
			ImGui::SetCursorPosX((ItemDialog::CHILD_WND_SIZE_SWITCH_PAGE.x - label_size.x) * 0.5f);
			ImGui::SetCursorPosY((PREV_NEXT_BUTTON_SIZE.y - label_size.y) * 0.5f + UPPER_PADDING);
			ImGui::Text(label_text.c_str());

			ImGui::SetCursorPosX(ItemDialog::CHILD_WND_SIZE_SWITCH_PAGE.x - PREV_NEXT_BUTTON_SIZE.x - HORIZONTAL_PADDING);
			ImGui::SetCursorPosY(UPPER_PADDING);
			if (ImGui::ImageButton("Next", tex_next.im_tex_id, PREV_NEXT_BUTTON_SIZE, tex_next.uv0, tex_next.uv1))
			{
				goto_next_page();
			}
		}
		ImGui::EndAlignedChild();

		// ブロックスキン選択ボタンを表示するウィンドウ
		ImGui::BeginAlignedChild
		(
			"child_wnd_skin_buttons",
			ItemDialog::CHILD_WND_PIVOT_ITEM_BUTTONS,
			ItemDialog::CHILD_WND_PIVOT_ITEM_BUTTONS,
			ItemDialog::CHILD_WND_SIZE_ITEM_BUTTONS
		);
		{
			const size_t idx_skins_in_page_start = current_page * ItemDialog::NUM_ITEMS_PER_PAGE;
			const size_t idx_skins_in_page_end = std::min(idx_skins_in_page_start + ItemDialog::NUM_ITEMS_PER_PAGE, item_list.size());

			// ページ内のブロックスキン選択ボタンを表示
			for (size_t i = 0; i < ItemDialog::NUM_ITEMS_PER_PAGE; i++)
			{
				const size_t x = i % ItemDialog::NUM_ITEMS_PER_PAGE_X;
				const size_t y = i / ItemDialog::NUM_ITEMS_PER_PAGE_X;

				const size_t idx_skin = idx_skins_in_page_start + i;
				if (idx_skin >= idx_skins_in_page_end)
				{
					break;
				}
				const MdItem& current_item = item_list.at(idx_skin);
				const bool is_current_item_selected = item_id == current_item.item_id;

				const float button_left = ItemDialog::OFFSET_BUTTONS_OUTER.x + (ItemDialog::ITEM_BUTTON_SIZE.x + ItemDialog::OFFSET_BUTTONS_INNER.x) * x;
				const float button_top = ItemDialog::OFFSET_BUTTONS_OUTER.y + (ItemDialog::ITEM_BUTTON_SIZE.y + ItemDialog::OFFSET_BUTTONS_INNER.y) * y;

				ImGui::Texture icon_texture;
				MasterHelper::GetGameIconImguiImage(current_item.icon_id, icon_texture);

				// ボタン本体
				ImGui::SetCursorPos(ImVec2(button_left, button_top));
				ImGui::PushID(i);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
				
				ImGui::PushStyleColor(ImGuiCol_Border, is_current_item_selected ? ImVec4(1, 0, 0, 1) : ImVec4(0.5, 0.5, 0.5, 1)); // 枠線の色を白に設定
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 5.0f); // 枠線の太さを設定
				if (ImGui::ImageButton("##", icon_texture, ItemDialog::ITEM_BUTTON_SIZE))
				{
					item_id = item_list.at(idx_skin).item_id;
				}
				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(1);
				ImGui::PopID();

				// ボタンをダブルクリック => 選択中のスキンで確定してモーダルを閉じる
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					result = ValueEditModalResult::CONFIRMED;
					ImGui::CloseCurrentPopup();
				}
			}
		}
		ImGui::EndAlignedChild();

		// OK/Cancelボタン
		ImGui::BeginAlignedChild("ok_and_cancel_buttons", BlockSkinDialog::CHILD_WND_PIVOT_OK_CANCEL, BlockSkinDialog::CHILD_WND_PIVOT_OK_CANCEL, BlockSkinDialog::CHILD_WND_SIZE_OK_CANCEL);
		{
			ImGui::BeginAlignedChild("buttons", ImVec2(0.5, 0.5), ImVec2(0.5, 0.5), ImVec2(300, 80));
			{
				ImGui::SetCursorPos(ImVec2(0, 0));
				if (ImGui::Button("OK", ImVec2(130, 80)))
				{
					result = ValueEditModalResult::CONFIRMED;
					ImGui::CloseCurrentPopup();
				}

				ImGui::SetCursorPos(ImVec2(170, 0));
				if (ImGui::Button("Cancel", ImVec2(130, 80)))
				{
					result = ValueEditModalResult::CANCELED;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndAlignedChild();
		}
		ImGui::EndAlignedChild();

		// 区切り線
		{
			ImGui::GetWindowDrawList()->AddLine(
				BlockSkinDialog::VERTICAL_SEPARATOR_FROM,
				BlockSkinDialog::VERTICAL_SEPARATOR_TO,
				IM_COL32(0, 0, 0, 255),
				BlockSkinDialog::SEPARATOR_THICKNESS
			);

			ImGui::GetWindowDrawList()->AddLine(
				BlockSkinDialog::HORIZONTAL_SEPARATOR_FROM,
				BlockSkinDialog::HORIZONTAL_SEPARATOR_TO,
				IM_COL32(0, 0, 0, 255),
				BlockSkinDialog::SEPARATOR_THICKNESS
			);
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(POP_COUNT);

	return result;
}

