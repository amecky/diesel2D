#pragma once
#include "..\..\math\Vector.h"
#include "..\lib\DataArray.h"
#include "..\dxstdafx.h"
#include <vector>
#include <map>
#include "..\utils\Log.h"
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\data\DataTranslator.h"
#include "..\data\DynamicSettings.h"
#include "..\utils\PlainTextReader.h"
#include "ActorArray.h"

const uint32 MAX_ITEMS = 1024;

namespace ds {

struct Actor {

	ID id;
	Vector2f position;
	Vector2f velocity;
	float angle;
	// remove this
	Rect textureRect;
	Vector4f uv;
	Color color;
	Vector2f scale;
	ID colliderID;
	uint32 type;
	float timer;
	bool alive;
	uint32 layer;
	uint32 index[16];
	uint32 numBehaviors;
	ID additionalID;
	bool active;
	int script;

	Actor() : id(UINT_MAX) ,script(-1) {
		clear();
	}
	
	void clear() {
		position = Vector2f(0,0);
		velocity = Vector2f(0,0);
		angle = 0.0f;
		scale = Vector2f(1,1);
		color = Color::WHITE;
		textureRect = Rect(0,0,32,32);
		colliderID = UINT_MAX;
		type = UINT_MAX;
		timer = 0.0f; 
		alive = true;
		numBehaviors = 0;
		layer = 0;
		additionalID = UINT_MAX;
		active = true;
		script = -1;
	}
};

// -------------------------------------------------------
// Behavior 
// -------------------------------------------------------
class Behavior {

public:
	Behavior() {}
	virtual ~Behavior() {}
	virtual void update(Actor& actor,float elapsed) = 0;
	virtual const char* getName() const = 0;
	virtual void prepareData(const Actor& actor) {}
	virtual void init(const Actor& actor,ID definitionID) {}
	virtual void onCreate(Actor& actor) {}
	virtual void remove(const Actor& actor) {}
	virtual ID createDefinition(BinaryLoader* loader) {
		return UINT_MAX;
	}
	virtual void convert(Category* category,BinaryWriter& writer) {}
	virtual void createTranslator() {}
};


// -------------------------------------------------------
// Behavior 
// -------------------------------------------------------
template<class DATA>
class AbstractBehavior : public Behavior {

public:
	AbstractBehavior() : Behavior() {}
	virtual ~AbstractBehavior() {}

	virtual void prepareData(const Actor& actor) {
		ID id = m_Data.add();
		uint32 idx = actor.id * 2;
		m_Mapping[idx+1] = id;
	}
	
	virtual void remove(const Actor& actor) {
		uint32 idx = actor.id * 2;
		m_Data.remove(m_Mapping[idx+1]);
		m_Mapping[idx+1] = UINT_MAX;
	}

	DATA& getData(const Actor& actor) {
		uint32 idx = actor.id * 2;
		return m_Data.get(m_Mapping[idx+1]);
	}

	DataTranslator<DATA>& getTranslator() {
		return m_Translator;
	}

	void convert(Category* category,BinaryWriter& writer) {
		DATA data;
		DataTranslator<DATA>& translator = getTranslator();
		translator.read(category,&data);
		translator.saveChunk(writer,1,&data,true);
	}


	virtual ID createDefinition(BinaryLoader* loader) {
		ID id = m_Definitions.add();
		DATA& data = m_Definitions.get(id);
		DataTranslator<DATA>& translator = getTranslator();
		translator.readChunk(*loader,&data);
		return id;
	}
protected:
	DataTranslator<DATA> m_Translator;
	ID m_Mapping[2 * MAX_ITEMS];
	DataArray<DATA,MAX_ITEMS> m_Data;
	DataArray<DATA,128> m_Definitions;

};

// -------------------------------------------------------
// Actor callback
// -------------------------------------------------------
class ActorCallback {

public:
	void onUpdate(Actor* actor) {}
	virtual void onDeactivation(const Actor& actor) = 0;
};


class Modifier {

public:
	Modifier() {}
	virtual ~Modifier() {}
	virtual void update(ActorArray& array,float dt) = 0;
};

class PositionModifier : public Modifier {

public:
	PositionModifier() : Modifier() , m_Velcoity(100,0) {}
	virtual ~PositionModifier() {}
	void update(ActorArray& array,float dt) {
		for ( uint32 i = 0; i < array.numObjects;++i ) {
			Vector2f& p = array.positions[i];
			p += m_Velcoity * dt;
		}
	}
private:
	Vector2f m_Velcoity;
};
}