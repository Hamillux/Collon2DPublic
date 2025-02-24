#include "GameConfig.h"
#include <fstream>
#include "internal/GameConfigItemsInclude.h"

namespace
{
	constexpr const char* CONFIG_FILE_NAME = "cln2d_config.json";

	template<typename ConfigItem>
	void LoadConfigItem_Impl(std::unordered_map<std::string, std::shared_ptr<IJsonObject>>& config_items, nlohmann::json& config_json)
	{
		const std::string key = ConfigItem::GetConfigKey();

		if (config_items.find(key) != config_items.end())
		{
			throw std::runtime_error("detect duplicate key: " + key);
		}

		config_items[key] = std::make_shared<ConfigItem>();
		if (config_json.contains(key))
		{
			config_items.at(key)->FromJsonObject(config_json.at(key));
		}
	}

	template<typename Head, typename... Tails>
	void LoadConfigItem(std::unordered_map<std::string, std::shared_ptr<IJsonObject>>& config_items, nlohmann::json& config_json)
	{
		LoadConfigItem_Impl<Head>(config_items, config_json);

		if constexpr (sizeof...(Tails) > 0)
		{
			LoadConfigItem<Tails...>(config_items, config_json);
		}
	}

}

void GameConfig::Init()
{
	nlohmann::json config_json{};

	std::ifstream config_file(CONFIG_FILE_NAME);
	if (config_file.is_open()) 
	{
		config_json = nlohmann::json::parse(config_file);
		config_file.close();
	}

	LoadConfigItem<
		StageEditorConfig
		// TODO: Add more config items here
	>(_config_items, config_json);
}

void GameConfig::SaveToFile()
{
	nlohmann::json config_json{};

	for (auto& item : _config_items)
	{
		config_json[item.first] = nlohmann::json::object({});
		item.second->ToJsonObject(config_json[item.first]);
	}

	std::ofstream config_file(CONFIG_FILE_NAME);
	config_file << config_json.dump(4);

	config_file.close();
}

GameConfig::GameConfig()
{}
