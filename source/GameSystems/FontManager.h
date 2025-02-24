#pragma once

#include "Utility/SingletonBase.h"
#include <unordered_map>
#include "GameSystems/MasterData/MasterDataInclude.h"

/// <summary>
/// ImGuiで使用するフォントを管理するクラス.
/// </summary>
class FontManager : public Singleton<FontManager>
{
	friend class Singleton<FontManager>;
public:
	virtual ~FontManager();

	/// <summary>
	/// マスターデータロード, ImGui初期化後〜ループ開始前に呼ぶ.
	/// </summary>
	void AddFontsFromMasterData();

	ImFont* GetFont(MasterDataID id) const;
	
private:
	FontManager();

	std::unordered_map<MasterDataID, ImFont*> _fonts;
};