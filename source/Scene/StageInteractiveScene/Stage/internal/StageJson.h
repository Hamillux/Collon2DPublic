#pragma once

#include "Utility/IJsonSerializable.h"
#include <string>
#include <memory>
#include <stdint.h>
#include "Utility/Core/Math/Vector2D.h"
#include "Scene/StageInteractiveScene/Stage/internal/StageId.h"
#include "Scene/StageInteractiveScene/Stage/internal/StageBGInfo.h"
#include "Utility/Core/Event.h"

struct SpawnActorInfo;

/// <summary>
/// ステージ情報を保持するクラス
/// </summary>
class Stage : public IJsonObject
{
public:
	struct StageEvents 
	{
		Event<> OnStageSizeChanged;
	};
	StageEvents stage_events;

	Stage();
	virtual ~Stage();

	//~ Begin IJsonObject interface
	virtual void ToJsonObject(nlohmann::json& stage_json) const override;
	virtual void FromJsonObject(const nlohmann::json& stage_json) override;
	//~ End IJsonObject interface

	/// <summary>
	/// 指定されたパスのファイルにステージJSONを保存する
	/// </summary>
	/// <param name="save_destination_file_path">セーブが成功したか</param>
	/// <returns></returns>
	bool SaveToFile(const std::string& save_destination_file_path) const;

	static Stage MakeFromTemplate(const MasterDataID stage_template_id);

	StageId GetStageId() const;
	std::string GetStageName() const;
	std::string GetDescription() const;
	std::string GetThumbnailFileName() const;
	uint16_t GetTimeLimit() const;
	int GetStageLength() const;
	static void GetStageLengthRange(int& out_min, int& out_max);
	Vector2D GetStageLeftTop() const;
	Vector2D GetStageRightBottom() const;
	Vector2D GetStageSize() const;
	MasterDataID GetBgLayerId() const;
	MasterDataID GetBgmId() const;
	std::vector<std::shared_ptr<SpawnActorInfo>>& GetSpawnActorInfosRef()
	{
		return spawn_actor_infos;
	}

	void SetStageId(const StageId& stage_id);
	void SetStageName(const std::string& stage_name);
	void SetDescription(const std::string& description);
	void SetTimeLimit(const uint16_t time_limit);

	/// <summary>
	/// ステージの長さを設定する. 
	/// <param name="tiles_count">ステージの長さ(タイル数). ステージ長の最大/最小値よりも大きい/小さい場合は何もしない</param>
	/// </summary>
	void SetStageLength(const int tiles_count);
	void SetBgLayerId(const MasterDataID bg_layer_id);
	void SetBgmId(const MasterDataID bgm_id);
	void SetSpawnActors(const std::vector<std::shared_ptr<SpawnActorInfo>>& spawn_actor_infos);
	void AddSpawnActor(const std::shared_ptr<SpawnActorInfo> spawn_actor_info);
	void RemoveSpawnActorInfo(const std::shared_ptr<SpawnActorInfo> spawn_actor_info);

	std::string& GetStageNameRef()
	{
		return stage_name;
	}

	std::string& GetDescriptionRef()
	{
		return description;
	}

	std::uint16_t& GetTimeLimitRef()
	{
		return time_limit;
	}

private:
	enum StageHeight 
	{
		StageHeight_Short = 0,
		StageHeight_Tall = 1
	};

	StageId stage_id;
	std::string stage_name;
	std::string description;
	uint16_t time_limit;
	int _stage_length_tile_count;
	StageHeight _stage_height;
	MasterDataID _bg_layer_id;
	MasterDataID _bgm_id;
	std::vector<std::shared_ptr<SpawnActorInfo>> spawn_actor_infos;
};