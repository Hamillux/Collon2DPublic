#include "GameObject.h"
#include "GameSystems/GameObjectManager.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

bool GameObject::IsValid(const GameObject* const game_object)
{
    return GameObjectManager::GetInstance().IsValid(game_object);
}
