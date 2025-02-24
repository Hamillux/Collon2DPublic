#pragma once
#include "Utility/Command/CommandBase.h"
#include "Utility/Core/Math/Vector2D.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"

class Actor;
class StageEditorScene;

// StageEditorSceneで使用するコマンド

/// <summary>
/// StageEditorSceneのメンバにアクセスする必要がある場合は
/// コマンドクラスをStageEditorSceneのfriendにする
/// </summary>
namespace stage_editor_scene
{

class CmdManipulateActorWaitingRoom : public CommandBase
{
public:
	CmdManipulateActorWaitingRoom(StageEditorScene* const scene, const std::vector<Actor*>& actor_list);

	//~ Begin ICommandBase interface
public:
	virtual void OnErasedFromHistory() override;
	//~ End ICommandBase interface

	//~ Begin CmdManipulateActorWaitingRoom interface
protected:
	virtual std::shared_ptr<SpawnActorInfo> GetSpawnInfo(Actor* const actor) = 0;
	//~ End CmdManipulateActorWaitingRoom interface

protected:
	void ToCemetery();
	void FromCemetery();
	static void MoveActorToCemetery(StageEditorScene& scene, Actor* actor);
private:
	StageEditorScene* _scene;
	const std::vector<Actor*> _actor_list;
};

class CmdRemoveActors : public CmdManipulateActorWaitingRoom
{
public:
	CmdRemoveActors(StageEditorScene* const scene, const std::vector<Actor*>& remove_list);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

	//~ Begin CmdManipulateActorWaitingRoom interface
protected:
	virtual std::shared_ptr<SpawnActorInfo> GetSpawnInfo(Actor* const actor) override;
	//~ End CmdManipulateActorWaitingRoom interface

private:
	std::unordered_map<Actor*, std::shared_ptr<SpawnActorInfo>> _spawn_info_map;
};

class CmdSummonActor : public CmdManipulateActorWaitingRoom
{
public:
	CmdSummonActor(StageEditorScene* const scene, Actor* const actor_in_waiting_room, const std::shared_ptr<SpawnActorInfo>& spawn_info);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

	//~ Begin CmdManipulateActorWaitingRoom interface
protected:
	virtual std::shared_ptr<SpawnActorInfo> GetSpawnInfo(Actor* const actor) override;
	//~ End CmdManipulateActorWaitingRoom interface

private:
	std::shared_ptr<SpawnActorInfo> _spawn_info;
};

class CmdChangeActorPosition : public CommandBase
{
public:
	CmdChangeActorPosition
	(
		StageEditorScene* const scene,
		Actor* target_actor,
		const Vector2D& from_position,
		const Vector2D& to_position
	);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

private:
	StageEditorScene* _scene;
	Actor* _target;
	Vector2D _from;
	Vector2D _to;
};

class CmdChangeStageBackground : public CommandBase
{
public:
	CmdChangeStageBackground(StageEditorScene* const scene, const StageBGLayer& from_bg_layer_id, const StageBGLayer& to_bg_layer_id);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

private:
	StageEditorScene* _scene;
	StageBGLayer _from;
	StageBGLayer _to;
};

class CmdChangeStageBGM : public CommandBase
{
public:
	CmdChangeStageBGM(StageEditorScene* const scene, const MasterDataID from_bgm_id, const MasterDataID to_bgm_id);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

private:
	StageEditorScene* _scene;
	MasterDataID _from;
	MasterDataID _to;
};

class CmdChangeStageLength : public CommandBase
{
public:
	CmdChangeStageLength(StageEditorScene* const scene, const int from_length, const int to_length);

	//~ Begin ICommandBase interface
	virtual void Undo() override;
	virtual void Do() override;
	//~ End ICommandBase interface

private:
	StageEditorScene* _scene;
	int _from;
	int _to;
};

}