#pragma once

#include "Scene/SceneBase.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"
#include "Scene/StageInteractiveScene/SpawnActorInfo.h"
#include <memory>

class Player;

class StageInteractiveScene : public SceneBase
{
public:
	StageInteractiveScene();
	virtual ~StageInteractiveScene();

	//~ Begin SceneBase interface
public:
	virtual void Initialize(const SceneBaseInitialParams* const scene_params) override;
	virtual SceneType Tick(const float delta_seconds) override;
	virtual void Finalize() override;
	virtual std::unique_ptr<const SceneBaseInitialParams> GetInitialParamsForNextScene(const SceneType next_scene) const override;
protected:
	virtual void PreDestroyActor(Actor* destroyee) override;
	//~ End SceneBase interface

	//~ Begin StageInteractiveScene interface
protected:
	virtual void BuildStage(const Stage& stage);
	//~ End StageInteractiveScene interface

public:
	/// <summary>
	/// アクターをステージに追加する
	/// </summary>
	void AddActorToStage(Actor* new_actor, std::shared_ptr<SpawnActorInfo> spawn_info);

	/// <summary>
	/// アクターをステージから削除する.
	/// <para>NOTE: アクターを破棄するわけではないので注意.</para>
	/// </summary>
	/// <returns>削除対象アクターに紐づけられていたスポーン情報. 見つからなかった場合はnullptr</returns>
	std::shared_ptr<SpawnActorInfo> RemoveActorFromStage(Actor* actor_to_be_removed);

	/// <summary>
	/// ステージの下端Y座標を取得
	/// </summary>
	float GetStageBottomY() const;

	Player* GetPlayerRef() const;

	/// <summary>
	/// ステージのタイル範囲を取得
	/// <para>NOTE: タイルインデックス(0,0)のタイルは左上頂点の座標がワールド原点に一致する</para>;
	/// </summary>
	/// <param name="left">ステージ左端のタイルインデックス(X)</param>
	/// <param name="top">ステージ上端のタイルインデックス(Y)</param>
	/// <param name="right">ステージ右端のタイルインデックス(X)</param>
	/// <param name="bottom">ステージ下端のタイルインデックス(Y)</param>
	void GetStageTileIndex(int& left, int& top, int& right, int& bottom) const;

	/// <summary>
	/// 点Qを含むタイルの中心座標を取得
	/// </summary>
	/// <param name="q_world">クエリ座標</param>
	/// <param name="offset_tiles_x">Qを含むタイルから中心座標を取得するタイルまでのタイル数オフセット(X)</param>
	/// <param name="offset_tiles_y">Qを含むタイルから中心座標を取得するタイルまでのタイル数オフセット(Y)</param>
	static Vector2D GetTileCenterPosition
	(
		const Vector2D& q_world,
		const int offset_tiles_x,
		const int offset_tiles_y
	);

	/// <summary>
	/// インデックス指定されたタイルの中心座標を取得
	/// </summary>
	/// <para>NOTE: タイルインデックス(0,0)のタイルは左上頂点の座標がワールド原点に一致する</para>;
	/// <param name="tile_index_x">タイルインデックス(X)</param>
	/// <param name="tile_index_y">タイルインデックス(Y)</param>
	/// <returns></returns>
	static Vector2D GetTileCenterPosition(const int tile_index_x, const int tile_index_y);

	/// <summary>
	/// ワールド座標q_worldが含まれるタイルのインデックスを取得.
	/// <para>NOTE: タイルインデックス(0,0)のタイルは左上頂点の座標がワールド原点に一致する</para>;
	/// </summary>
	/// <param name="q_world">クエリ座標</param>
	/// <param name="out_tile_index_x"></param>
	/// <param name="out_tile_index_y"></param>
	static void GetTileIndex
	(
		const Vector2D& q_world,
		int& out_tile_index_x,
		int& out_tile_index_y
	);

	/// <summary>
	/// actorが占有するタイル範囲の境界を取得
	/// タイルインデックス(ti_x, ti_y)が ti_x∈[left_index_left, tile_index_right], ti_y∈[tile_index_top, tile_index_bottom]
	/// であるようなタイルはactorによって占有されている
	/// </summary>
	static void GetActorOccupyingArea(
		Actor* const actor,
		int& tile_index_left,
		int& tile_index_top,
		int& tile_index_right,
		int& tile_index_bottom
	);


protected:
	/// <summary>
	/// ステージの参照を取得
	/// </summary>
	/// <returns></returns>
	Stage& GetStageRef() const;

	/// <summary>
	/// 指定されたアクターのスポーン情報が登録されているか
	/// </summary>
	bool IsActorInStage(Actor* actor) const;

	/// <summary>
	/// 指定されたアクターのスポーン情報を取得. スポーン情報が登録されていない場合は例外を投げる
	/// </summary>
	std::shared_ptr<SpawnActorInfo> GetSpawnActorInfo(Actor* actor) const;

	/// <summary>
	/// カメラがステージ外を映さないようにクランプする
	/// </summary>
	void ClampCameraPositionInStage(CameraParams& camera_params) const;

	/// <summary>
	/// ステージ外を映さない最大の画角拡大率を取得
	/// </summary>
	/// <returns></returns>
	float GetMaxScreenScale() const;

	/// <summary>
	/// スポーン情報が登録されているアクター全てをFinalize()=>Initialize()する
	/// </summary>
	void ReloadAllActorsInStage();

	/// <summary>
	/// スポーン情報が登録されている場合, アクターをFinalize()=>Initialize()する
	/// スポーン情報が登録されていない場合, 例外を投げる
	/// </summary>
	/// <param name="actor"></param>
	void ReloadActorInStage(Actor* actor);

	/// <summary>
	/// 背景を設定する
	/// </summary>
	void SetStageBackground(const StageBGLayer& stage_bg_layer);

private:
	void CreateActorsInStage();
	void CreateStagePerimeterColliders();
	void SetupBackgrounds();

	std::unique_ptr<Stage> _stage;

	// アクターのスポーン情報
	// 要素の追加・削除はAddActorToStage()とRemoveActorFromStage()で
	// このマップのキーとして登録されているアクターはステージに存在するアクターである
	std::unordered_map<Actor*, std::shared_ptr<SpawnActorInfo>> actor_spawn_info_map;

	Player* _player_ref;
};

struct StageInteractiveSceneInitialParams: public SceneBaseInitialParams
{
	StageInteractiveSceneInitialParams()
		: stage_id(StageId::NONE)
	{}
	StageInteractiveSceneInitialParams(const SceneType prev_scene_in, const StageId stage_id_in)
		: SceneBaseInitialParams(prev_scene_in)
		, stage_id(stage_id_in)
	{}
	virtual ~StageInteractiveSceneInitialParams() {}

	StageId stage_id;
};


template<>
struct SceneBase::traits<StageInteractiveScene>
{
	typedef StageInteractiveSceneInitialParams initial_params_type;
};