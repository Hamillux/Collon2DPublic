#pragma once

#include "Utility/SingletonBase.h"
#include "internal/GameConfigItemsInclude.h"
#include <unordered_map>
#include <string>
#include <memory>

class GameConfig : public Singleton<GameConfig>
{
	friend class Singleton<GameConfig>;
public:
	virtual ~GameConfig() {}

	// コンフィグファイルが存在する場合は読み込み、存在しない場合はデフォルト設定を作成
	void Init();
	void SaveToFile();

	template<typename ConfigItem>
	std::shared_ptr<ConfigItem> GetConfigItem();

private:
	GameConfig();
	std::unordered_map<std::string, std::shared_ptr<IJsonObject>> _config_items;
};

template<typename ConfigItem>
inline std::shared_ptr<ConfigItem> GameConfig::GetConfigItem()
{
	const std::string key = ConfigItem::GetKey();
	return std::dynamic_pointer_cast<ConfigItem>(_config_items.at(key));
}
