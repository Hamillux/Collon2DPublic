#pragma once
#include <stdint.h>

/**
 * シーンタイプ
 */
enum class SceneType : uint8_t
{
	NONE,
	EDITOR_SCENE,
	SELECT_SCENE,
	TEST_SELECT_SCENE,
	TEST_SCENE,
	TEST_SCENE_IMPL,
	TITLE_SCENE,
	INGAME_SCENE,
	MSG_RELOAD,	
};
