#pragma once

#include "SceneObject/Component/ComponentBase.h"
#include "MovementMode.h"

class MovementComponent : public ComponentBase
{
public:
	MovementComponent();
	virtual ~MovementComponent();

	//~ Begin ComponentBase interface
	virtual void Initialize() override;
	//~ End ComponentBase interface

	//~ Begin MovementComponent interface
	virtual Vector2D GetVelocity() const = 0;
	virtual void SetVelocity(const Vector2D& new_velocity) = 0;
	//~ End MovementComponent interface
};