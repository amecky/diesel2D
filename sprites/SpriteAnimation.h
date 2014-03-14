#pragma once
#include "..\data\Gizmo.h"

namespace ds {

class SpriteObject;
// -------------------------------------------------------
// SpriteAnimation
// -------------------------------------------------------
class SpriteAnimation {

public:
	SpriteAnimation(void);
	~SpriteAnimation(void);
	virtual void update(float t,SpriteObject* object) = 0;
};

// -------------------------------------------------------
// ColorAnimation
// -------------------------------------------------------
class ColorAnimation : public Gizmo , public SpriteAnimation {

public:
	ColorAnimation() : Gizmo("color") {
		add("color",&data);
	}
	virtual ~ColorAnimation() {}
	void update(float t,SpriteObject* object);
protected:
	IPath<20,Color> data;
};

// -------------------------------------------------------
// ScaleAnimation
// -------------------------------------------------------
class ScaleAnimation : public Gizmo , public SpriteAnimation {

public:
	ScaleAnimation() : Gizmo("scale") {
		add("scale",&data);
	}
	virtual ~ScaleAnimation() {}
	void update(float t,SpriteObject* object);
protected:
	IPath<20,Vector2f> data;
};

// -------------------------------------------------------
// RotationAnimation
// -------------------------------------------------------
class RotationAnimation : public Gizmo , public SpriteAnimation {

public:
	RotationAnimation() : Gizmo("rotation") {
		add("rotation",&data);
	}
	virtual ~RotationAnimation() {}
	void update(float t,SpriteObject* object);
protected:
	IPath<20,float> data;
};

}

