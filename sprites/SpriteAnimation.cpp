#include "SpriteAnimation.h"
#include "Sprite.h"

namespace ds {

SpriteAnimation::SpriteAnimation(void) {
}


SpriteAnimation::~SpriteAnimation(void) {
}

void ColorAnimation::update(float t,Sprite* object) {
	data.get(t,object->getColorPtr());		
}

void ScaleAnimation::update(float t,Sprite* object) {
	Vector2f size(1.0f);
	data.get(t,&size);		
	object->setScale(size);
}

void RotationAnimation::update(float t,Sprite* object) {
	float angle = 0.0f;
	data.get(t,&angle);		
	object->setAngle(DEGTORAD(angle));		
}

}