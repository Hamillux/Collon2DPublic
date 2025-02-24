#pragma once
#include "Actor/ActorInitialParams.h"

/// <summary>
/// テスト用アクターColliderHolderの初期化パラメータ
/// </summary>
struct ColliderHolderInitialParams : public ActorInitialParams
{
	ColliderHolderInitialParams()
		: collider_type(COLLIDER_SHAPE_BOX)
	{
	}
	virtual ~ColliderHolderInitialParams() {}

public:
	virtual void ToJsonObject(nlohmann::json& j) const override;
	virtual void FromJsonObject(const nlohmann::json& j) override;

	static constexpr int COLLIDER_SHAPE_BOX = 0;
	static constexpr int COLLIDER_SHAPE_SEGMENT = 1;

	int collider_type;
};