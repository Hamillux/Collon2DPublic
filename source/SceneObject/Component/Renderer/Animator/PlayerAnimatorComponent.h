#pragma once

#include "SceneObject/Component/Renderer/Animator/AnimatorComponent.h"
#include "SceneObject/Actor/Character/Player/Player.h"

class PlayerAnimatorComponent : public AnimatorComponent<PlayerAnimatorComponent>
{
public:
	PlayerAnimatorComponent()
		: _player_ref(nullptr)
	{}
	virtual ~PlayerAnimatorComponent() {}

	//~ Begin ComponentBase interface
public:
	virtual void Initialize() override;
	virtual void Tick(const float delta_seconds) override;
	//~ End ComponentBase interface

public:
	CharacterMovementMode _movement_mode;
	Vector2D _velocity;

private:
	Player* _player_ref;
};