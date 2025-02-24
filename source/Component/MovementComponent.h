#pragma once

#include "Component/ComponentBase.h"
#include "MovementMode.h"

/// <summary>
/// 移動コンポーネントの基底クラス
/// </summary>
class MovementComponent : public ComponentBase
{
public:
	MovementComponent();
	virtual ~MovementComponent();

	//~ Begin MovementComponent interface
	virtual Vector2D GetVelocity() const = 0;
	virtual void SetVelocity(const Vector2D& new_velocity) = 0;
	//~ End MovementComponent interface
};