#pragma once
#include "..\renderer\Renderer.h"
#include "..\sprites\SpriteCollisionManager.h"
//#include "..\nodes\SpriteBatch.h"

namespace ds {

struct ApplicationContext {

	Renderer* renderer;
	SpriteCollisionManager* collisionManager;
	//SpriteBatch* spriteBatch;

};

}