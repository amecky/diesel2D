#pragma once
#include <vector>
#include "Sprite.h"

const int COLL_SIZE = 32;
const int COLL_BUFFER_MAX = 1000;
const int COLL_BUFFER_SIZE = COLL_SIZE * COLL_BUFFER_MAX;

namespace ds {
// -------------------------------------------------------
// Collision
// -------------------------------------------------------
struct SpriteCollision {
	Vector2f firstPosition;
	Vector2f secondPosition;
	int firstId;
	int secondId;
	int firstType;
	int secondType;
};


// -------------------------------------------------------
// CollisionObject
// -------------------------------------------------------
struct SpriteCollisionObject {
	Vector2f prevPosition;
	Vector2f extent;
	int id;
	int type;
	Sprite* sprite;
};

// -------------------------------------------------------
// CollisionManager
// -------------------------------------------------------
class SpriteCollisionManager {

typedef std::vector<SpriteCollisionObject*> SpriteCollisionObjects;

struct IgnoreCollision {

	int firstType;
	int secondType;

	IgnoreCollision() : firstType(-1) , secondType(-1) {}

	bool isUsed() {
		return firstType != -1 || secondType != -1;
	}

	bool matches(int ft,int st) {
		if ( firstType == ft && secondType == st ) {
			return true;
		}
		if ( firstType == st && secondType == ft ) {
			return true;
		}
		return false;
	}
};

typedef std::vector<IgnoreCollision> Ignores;

public:
	SpriteCollisionManager();	
	virtual ~SpriteCollisionManager();
	int add(Sprite* sprite,int type);
	void remove(Sprite* sprite,int type);
	int checkIntersections();
	void reset();
	bool getCollision(int index,SpriteCollision& collision) const;
	const int getCollisionCounter() const {
		return m_BufferCounter;
	}
	bool bufferContains(int firstID,int secondID);
	void addIgnore(int firstType,int secondType) {
		IgnoreCollision ic;
		ic.firstType = firstType;
		ic.secondType = secondType;
		m_Ignores.push_back(ic);
	}
	bool shouldIgnore(int firstType,int secondType) {
		for ( size_t i = 0; i < m_Ignores.size(); ++i ) {
			if ( m_Ignores[i].matches(firstType,secondType)) {
				return true;
			}
		}
		return false;
	}
	void debug();
private:
	bool equals(const SpriteCollisionObject* first,const SpriteCollisionObject* second);
	SpriteCollisionManager(const SpriteCollisionManager& orig);
	SpriteCollisionObject* findObject(int id);
	bool intersectCircleCircle(const SpriteCollisionObject* first,const SpriteCollisionObject* second);
	bool addCollision(SpriteCollisionObject* first,SpriteCollisionObject* second);
	SpriteCollisionObjects m_Objects;
	Ignores m_Ignores;
	int m_Counter;
	char* m_ColBuffer;
	int m_Size;
	int m_BufferCounter;    
};

namespace coll {

	bool containsType(const SpriteCollision& collision,int type);

	bool matches(const SpriteCollision& collision,int firstType,int secondType);

	int getIDByType(const SpriteCollision& collision,int type);

	Vector2f getPositionByType(const SpriteCollision& collision,int type);
}

}
