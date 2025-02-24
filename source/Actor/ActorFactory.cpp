#include "ActorFactory.h"
#include "AllActorsInclude_generated.h"
#include "EntityTraits.h"
#include <stdexcept>


#define ACTOR_SWITCH_CASE(AT) case AT: return CreateAndInitializeActor<matching_actor_of_entitytype_t<AT>>(initial_params, owner_scene)
Actor* ActorFactory::CreateAndInitializeActorByEntityType(const EEntityType entity_type, const ActorInitialParams* const initial_params, SceneBase* const owner_scene)
{
    switch (entity_type)
    {
        ACTOR_SWITCH_CASE(EEntityType::Actor);
        ACTOR_SWITCH_CASE(EEntityType::WalkingEnemy);
        ACTOR_SWITCH_CASE(EEntityType::FlyingEnemy);
        ACTOR_SWITCH_CASE(EEntityType::TacklingEnemy);
        ACTOR_SWITCH_CASE(EEntityType::ThrowingEnemy);
        ACTOR_SWITCH_CASE(EEntityType::Player);
        ACTOR_SWITCH_CASE(EEntityType::RectangleBlock);
        ACTOR_SWITCH_CASE(EEntityType::SlopeBlock);
        ACTOR_SWITCH_CASE(EEntityType::SlopeBlock2);
        ACTOR_SWITCH_CASE(EEntityType::GoalFlag);
		ACTOR_SWITCH_CASE(EEntityType::Coin);
		ACTOR_SWITCH_CASE(EEntityType::ItemActor);
		ACTOR_SWITCH_CASE(EEntityType::CrackedBrick);
        // TODO: 有効な列挙子全てをここに
    }

    throw std::runtime_error("entity_type is unknown or unregisterd to ActorFactory::CreateAndInitializeActorByEntityType()");
    return nullptr;
}

#define PARAMS_SWITCH_CASE(ET) case ET: return std::make_shared<initial_params_of_entitytype_t<ET>>()
std::shared_ptr<ActorInitialParams> ActorFactory::CreateInitialParamsByEntityType(const EEntityType entity_type)
{
    switch (entity_type)
    {
        PARAMS_SWITCH_CASE(EEntityType::Actor);
        PARAMS_SWITCH_CASE(EEntityType::WalkingEnemy);
        PARAMS_SWITCH_CASE(EEntityType::FlyingEnemy);
        PARAMS_SWITCH_CASE(EEntityType::TacklingEnemy);
		PARAMS_SWITCH_CASE(EEntityType::ThrowingEnemy);
        PARAMS_SWITCH_CASE(EEntityType::Player);
        PARAMS_SWITCH_CASE(EEntityType::RectangleBlock);
        PARAMS_SWITCH_CASE(EEntityType::SlopeBlock);
		PARAMS_SWITCH_CASE(EEntityType::SlopeBlock2);
        PARAMS_SWITCH_CASE(EEntityType::GoalFlag);
		PARAMS_SWITCH_CASE(EEntityType::Coin);
		PARAMS_SWITCH_CASE(EEntityType::ItemActor);
		PARAMS_SWITCH_CASE(EEntityType::CrackedBrick);
        // TODO: 有効な列挙子全てをここに
    }

    throw std::runtime_error("entity_type is unknown or unregisterd to ActorFactory::CreateInitialParamsByEntityType()");
    return nullptr;
}

void ActorFactory::DestroyActor(Actor*& actor)
{
	GameObjectManager::GetInstance().DestroyObject(actor);
	return;
}

void ActorFactory::InitializeActor(Actor* actor, const ActorInitialParams* const params, SceneBase* owner_scene)
{
    actor->SetScene(owner_scene);
    actor->Initialize(params);
    return;
}