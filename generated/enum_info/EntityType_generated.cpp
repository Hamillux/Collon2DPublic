#include "..\..\source\SceneObject\Actor\EntityType.h"

std::vector<EEntityType> EnumInfo<EEntityType>::enumerators =
{
    EEntityType::None,
    EEntityType::Actor,
    EEntityType::WalkingEnemy,
    EEntityType::FlyingEnemy,
    EEntityType::TacklingEnemy,
    EEntityType::ThrowingEnemy,
    EEntityType::Player,
    EEntityType::RectangleBlock,
    EEntityType::SlopeBlock,
    EEntityType::GoalFlag,
    EEntityType::Coin,
    EEntityType::ItemActor,
    EEntityType::CrackedBrick,
};

std::unordered_map<std::string, EEntityType> EnumInfo<EEntityType>::name_to_enum_map =
{
    {"None", EEntityType::None},
    {"Actor", EEntityType::Actor},
    {"WalkingEnemy", EEntityType::WalkingEnemy},
    {"FlyingEnemy", EEntityType::FlyingEnemy},
    {"TacklingEnemy", EEntityType::TacklingEnemy},
    {"ThrowingEnemy", EEntityType::ThrowingEnemy},
    {"Player", EEntityType::Player},
    {"RectangleBlock", EEntityType::RectangleBlock},
    {"SlopeBlock", EEntityType::SlopeBlock},
    {"GoalFlag", EEntityType::GoalFlag},
    {"Coin", EEntityType::Coin},
    {"ItemActor", EEntityType::ItemActor},
    {"CrackedBrick", EEntityType::CrackedBrick},
};

std::vector<EEntityCategory> EnumInfo<EEntityCategory>::enumerators =
{
    EEntityCategory::None,
    EEntityCategory::Character,
    EEntityCategory::Block,
    EEntityCategory::Gimmick,
    EEntityCategory::Item,
};

std::unordered_map<std::string, EEntityCategory> EnumInfo<EEntityCategory>::name_to_enum_map =
{
    {"None", EEntityCategory::None},
    {"Character", EEntityCategory::Character},
    {"Block", EEntityCategory::Block},
    {"Gimmick", EEntityCategory::Gimmick},
    {"Item", EEntityCategory::Item},
};

