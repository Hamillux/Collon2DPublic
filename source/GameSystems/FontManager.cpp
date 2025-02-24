#include "FontManager.h"

FontManager::FontManager()
{}

FontManager::~FontManager()
{
}

void FontManager::AddFontsFromMasterData()
{
	// NOTE: 最初に追加されたフォントがデフォルトフォントになると思われる
	ImGui::GetIO().Fonts->AddFontDefault();

	for (const auto& md_font : MdFont::GetData())
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* font =
			io.Fonts->AddFontFromFileTTF(md_font.ttf_path.c_str(), md_font.font_size, nullptr, io.Fonts->GetGlyphRangesJapanese());
		_fonts[md_font.id] = font;
	}
}

ImFont* FontManager::GetFont(MasterDataID id) const
{
	return _fonts.at(id);
}
