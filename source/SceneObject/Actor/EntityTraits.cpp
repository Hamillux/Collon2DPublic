#include "AllActorsInclude_generated.h"
#include "EntityTraits.h"
#include "GameSystems/MasterData/MasterDataInclude.h"
#include <unordered_map>

const char* EntityTraits::GetName(const EEntityType entity_type)
{
    return EnumInfo<EEntityType>::EnumToString(entity_type);
}

const char* EntityTraits::GetDisplayName(const EEntityType entity_type)
{
    return nullptr;
}

MasterDataID EntityTraits::GetEntityID(const EEntityType entity_type)
{
    
    return MasterDataID();
}

namespace {

#define GetDefaultConstructedParams_SWITCH_CASE(ET) case ET: return std::make_shared<initial_params_of_entitytype_t<ET>>()
/// <summary>
/// デフォルトコンストラクタでentity_typeに対応する初期化パラメータを生成
/// </summary>
/// <returns></returns>
std::shared_ptr<ActorInitialParams> GetDefaultConstructedParams(EEntityType entity_type)
{
    switch (entity_type)
    {
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::Actor);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::WalkingEnemy);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::FlyingEnemy);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::TacklingEnemy);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::ThrowingEnemy);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::Player);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::RectangleBlock);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::SlopeBlock);
        GetDefaultConstructedParams_SWITCH_CASE(EEntityType::GoalFlag);
		GetDefaultConstructedParams_SWITCH_CASE(EEntityType::Coin);
		GetDefaultConstructedParams_SWITCH_CASE(EEntityType::ItemActor);
		GetDefaultConstructedParams_SWITCH_CASE(EEntityType::CrackedBrick);
        // TODO: 有効な列挙子全てをここに
    }
    
    throw std::runtime_error("'GetDefaultConstructedParams' detected an invalid entity-type");
}

} // end anonymous namespace


std::shared_ptr<ActorInitialParams> EntityTraits::GetDefaultInitialParamsOf(const EEntityType entity_type)
{
	throw std::runtime_error("Not implemented yet");

    // TODO: エンティティ別に定義されたデフォルトパラメータJSONをロードする

    return nullptr;
}

