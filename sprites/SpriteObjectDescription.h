#pragma once
#include "..\io\Serializer.h"
#include "..\utils\PlainTextReader.h"
#include "..\data\Gizmo.h"
#include "..\io\FileWatcher.h"
#include "SpriteAnimation.h"

namespace ds {

struct SpriteDescription {

	uint32 id;
	int textureID;
	Vector2f position;
	Rect textureRect;
	float angle;
	Vector2f velocity;
	float radius;
	int index;
	Color color;
	Vector2f scale;
	int userValue;

};

class SpriteDescriptionManager : public Serializer {

public:
	SpriteDescriptionManager() {}
	~SpriteDescriptionManager() {}
	void load(BinaryLoader* loader);
	const SpriteDescription& get(int id) const {
		return m_Descriptions[id];
	}
private:
	SpriteDescription m_Descriptions[64];
};

}
