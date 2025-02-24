#include "SceneObject.h"
#include "Scene/SceneBase.h"
#include "GameSystems/GameObjectManager.h"

SceneObject::SceneObject()
	: _owner_scene(nullptr)
{
}

SceneObject::~SceneObject()
{
}

void SceneObject::SetOwnerScene(SceneBase* const in_owner_scene)
{
	_owner_scene = in_owner_scene;
}

SceneBase* SceneObject::GetScene() const
{
	return _owner_scene;
}

bool SceneObject::IsValid(SceneObject* game_object)
{
	return GameObjectManager::GetInstance().IsValid(game_object);
}
