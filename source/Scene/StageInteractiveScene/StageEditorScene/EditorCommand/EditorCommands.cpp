#include "EditorCommands.h"
#include "Scene/StageInteractiveScene/StageEditorScene/StageEditorScene.h"
#include "Scene/StageInteractiveScene/Stage/Stage.h"
#include "Actor/Actor.h"
#include <string>

using ActorVectorIterator = std::vector<Actor*>::iterator;

stage_editor_scene::CmdManipulateActorWaitingRoom::CmdManipulateActorWaitingRoom(StageEditorScene* const scene, const std::vector<Actor*>& actor_list)
	: _scene(scene)
	, _actor_list(actor_list)
{}

void stage_editor_scene::CmdManipulateActorWaitingRoom::OnErasedFromHistory()
{
	ActorVectorIterator it_first = std::find(_scene->_actor_waiting_room.begin(), _scene->_actor_waiting_room.end(), _actor_list.front());
	if (it_first != _scene->_actor_waiting_room.end())
	{
		ActorVectorIterator it_last = it_first + _actor_list.size();
		for (auto it = it_first; it != it_last; it++)
		{
			_scene->DestroyActor(*it);
		}

		_scene->_actor_waiting_room.erase(it_first, it_last);
	}
}

void stage_editor_scene::CmdManipulateActorWaitingRoom::ToCemetery()
{
	const size_t current_cemetery_size = _scene->_actor_waiting_room.size();
	_scene->_actor_waiting_room.reserve(current_cemetery_size + _actor_list.size());

	for (auto& actor : _actor_list)
	{
		MoveActorToCemetery(*_scene, actor);
	}
}

void stage_editor_scene::CmdManipulateActorWaitingRoom::FromCemetery()
{
	std::vector<Actor*>& cemetery = _scene->_actor_waiting_room;

	// 墓地から削除
	ActorVectorIterator it_erase_first = std::find(cemetery.begin(), cemetery.end(), _actor_list.front());
	ActorVectorIterator it_erase_last = it_erase_first + _actor_list.size();
	cemetery.erase(it_erase_first, it_erase_last);

	// シーン, ステージに追加
	for (Actor* actor : _actor_list)
	{
		_scene->AddActorToStage(actor, GetSpawnInfo(actor));
		_scene->AddActor(actor);
	}
}

void stage_editor_scene::CmdManipulateActorWaitingRoom::MoveActorToCemetery(StageEditorScene& scene, Actor* actor)
{
	scene.RemoveActorFromStage(actor);
	scene.RemoveActor(actor);
	scene.AddActorToWaitingRoom(actor);
}

stage_editor_scene::CmdRemoveActors::CmdRemoveActors(StageEditorScene* const scene, const std::vector<Actor*>& remove_list)
	: CmdManipulateActorWaitingRoom(scene, remove_list)
{
	for (Actor* actor : remove_list)
	{
		_spawn_info_map[actor] = scene->GetSpawnActorInfo(actor);
	}
}

void stage_editor_scene::CmdRemoveActors::Undo()
{
	FromCemetery();
}

void stage_editor_scene::CmdRemoveActors::Do()
{
	ToCemetery();
}

std::shared_ptr<SpawnActorInfo> stage_editor_scene::CmdRemoveActors::GetSpawnInfo(Actor* const actor)
{
	return _spawn_info_map.at(actor);
}

stage_editor_scene::CmdChangeActorPosition::CmdChangeActorPosition(StageEditorScene* const scene, Actor* target_actor, const Vector2D& from_position, const Vector2D& to_position)
	: _scene(scene)
	, _target(target_actor)
	, _from(from_position)
	, _to(to_position)
{
}

void stage_editor_scene::CmdChangeActorPosition::Undo()
{
	_target->SetActorWorldPosition(_from);
	_scene->GetSpawnActorInfo(_target)->initial_params->transform.position = _from;
}

void stage_editor_scene::CmdChangeActorPosition::Do()
{
	_target->SetActorWorldPosition(_to);
	_scene->GetSpawnActorInfo(_target)->initial_params->transform.position = _to;
}

stage_editor_scene::CmdChangeStageBackground::CmdChangeStageBackground(StageEditorScene* const scene, const StageBGLayer& from_bg_layer_id, const StageBGLayer& to_bg_layer_id)
	: _scene(scene)
	, _from(from_bg_layer_id)
	, _to(to_bg_layer_id)
{
}

void stage_editor_scene::CmdChangeStageBackground::Undo()
{
	_scene->SetStageBackground(_from);
	_scene->GetStageRef().SetBgLayerId(_from.bg_layer_id);
}

void stage_editor_scene::CmdChangeStageBackground::Do()
{
	_scene->SetStageBackground(_to);
	_scene->GetStageRef().SetBgLayerId(_to.bg_layer_id);
}

stage_editor_scene::CmdSummonActor::CmdSummonActor(StageEditorScene* const scene, Actor* const actor_in_waiting_room, const std::shared_ptr<SpawnActorInfo>& spawn_info)
	: CmdManipulateActorWaitingRoom(scene, { actor_in_waiting_room })
	, _spawn_info(spawn_info)
{
	if (std::find(scene->_actor_waiting_room.begin(), scene->_actor_waiting_room.end(), actor_in_waiting_room) == scene->_actor_waiting_room.end())
	{
		throw std::runtime_error("The actor is not in the waiting room.");
	}
}

void stage_editor_scene::CmdSummonActor::Undo()
{
	ToCemetery();
}

void stage_editor_scene::CmdSummonActor::Do()
{
	FromCemetery();
}

std::shared_ptr<SpawnActorInfo> stage_editor_scene::CmdSummonActor::GetSpawnInfo(Actor* const actor)
{
	return _spawn_info;
}

stage_editor_scene::CmdChangeStageLength::CmdChangeStageLength(StageEditorScene* const scene, const int from_length, const int to_length)
	: _scene(scene)
	, _from(from_length)
	, _to(to_length)
{
}

void stage_editor_scene::CmdChangeStageLength::Undo()
{
	_scene->GetStageRef().SetStageLength(_from);
}

void stage_editor_scene::CmdChangeStageLength::Do()
{
	_scene->GetStageRef().SetStageLength(_to);
}

stage_editor_scene::CmdChangeStageBGM::CmdChangeStageBGM(StageEditorScene* const scene, const MasterDataID from_bgm_id, const MasterDataID to_bgm_id)
	: _scene(scene)
	, _from(from_bgm_id)
	, _to(to_bgm_id)
{
}

void stage_editor_scene::CmdChangeStageBGM::Undo()
{
	_scene->GetStageRef().SetBgmId(_from);
	_scene->PushEditorMessage(u8"BGMを元に戻しました");
}

void stage_editor_scene::CmdChangeStageBGM::Do()
{
	_scene->GetStageRef().SetBgmId(_to);
	_scene->PushEditorMessage(u8"BGMを変更しました");
}
