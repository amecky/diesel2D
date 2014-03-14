#pragma once
#include "..\io\Serializer.h"
#include "..\utils\PlainTextReader.h"
#include "..\data\Gizmo.h"
#include "..\io\FileWatcher.h"
#include "SpriteAnimation.h"

namespace ds {

struct SpriteDescription : public Gizmo {

	ds::Rect textureRect;

	SpriteDescription() : Gizmo("settings") {
		add("texture_rect",&textureRect);
	}

};

class SpriteObject;

class SpriteObjectDescription : public Serializer {

public:
	SpriteObjectDescription() : m_RotationPath(0) , m_ScalePath(0) , m_ColorPath(0) , m_SpriteDescription(0) {}
	~SpriteObjectDescription();
	void load(const char* name);
	void reload(const char* name);
	void setAnimations(SpriteObject* object);
	void prepare(ds::SpriteObject* object);
private:
	SpriteDescription* m_SpriteDescription;
	RotationAnimation* m_RotationPath;
	ScaleAnimation* m_ScalePath;
	ColorAnimation* m_ColorPath;
};

}
