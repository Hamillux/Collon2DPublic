#pragma once
#include "GameObject.h"

class SceneBase;

class SceneObject : public GameObject
{
public:
	SceneObject();
	virtual ~SceneObject();

	void SetOwnerScene(SceneBase* const in_owner_scene);
	SceneBase* GetScene() const;

protected:
	static bool IsValid(SceneObject* game_object);

private:
	// このオブジェクトを保有するシーン
	SceneBase* _owner_scene;
};