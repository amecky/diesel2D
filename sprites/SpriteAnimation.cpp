#include "SpriteAnimation.h"
#include "SpriteObject.h"

namespace ds {

SpriteAnimation::SpriteAnimation(void) {
}


SpriteAnimation::~SpriteAnimation(void) {
}

void ColorAnimation::update(float t,SpriteObject* object) {
	data.get(t,object->getColorPtr());		
}

void ScaleAnimation::update(float t,SpriteObject* object) {
	Vector2f size(1.0f);
	data.get(t,&size);		
	object->setScale(size);
}

void RotationAnimation::update(float t,SpriteObject* object) {
	float angle = 0.0f;
	data.get(t,&angle);		
	object->setAngle(DEGTORAD(angle));		
}

}