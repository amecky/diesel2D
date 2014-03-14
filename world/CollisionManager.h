#pragma once
#include "..\math\math_types.h"
#include <vector>

const int COL_SIZE = 32;
const int COL_BUFFER_MAX = 1000;
const int COL_BUFFER_SIZE = COL_SIZE * COL_BUFFER_MAX;

namespace ds {

// -------------------------------------------------------
// Collision
// -------------------------------------------------------
struct Collision {
	Vector2f firstPosition;
	Vector2f secondPosition;
	int firstId;
	int secondId;
	int firstType;
	int secondType;
};

// -------------------------------------------------------
// CollisionObjectType
// -------------------------------------------------------
enum CDType {
	CDT_CIRCLE,
	CDT_BOX,
	CDT_LINE
};    

class Entity;
// -------------------------------------------------------
// CollisionObject
// -------------------------------------------------------
struct CollisionObject {
	Vector2f prevPosition;
	//Vector2f offset;
	Vector2f extent;
	CDType shape;
	int id;
	int type;
	Entity* entity;
};

// -------------------------------------------------------
// CollisionManager
// -------------------------------------------------------
class CollisionManager {

typedef std::vector<CollisionObject> CollisionObjects;

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
	CollisionManager();
	CollisionManager(const CollisionManager& orig);
	virtual ~CollisionManager();
	int addLine(const Vector2f& startPos,const Vector2f& endPos);
	int addCircle(Entity* entity,float radius,int type);
	int addBox(Entity* entity,float width,float height,int type);
	void remove(Entity* entity);
	//void updateLine(int id,const Vector2f& startPos,const Vector2f& endPos);
	//void updateCircle(int id,const Vector2f& position);
	void checkIntersections();
	void reset();
	bool getCollision(int index,Collision& collision) const;
	const int getCollisionCounter() const {
		return m_BufferCounter;
	}
	bool bufferContains(int firstID,int secondID);
	void update(float elapsed);
	bool circleSweepTest(int firstID,int secondID);
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
	bool checkBoxCircle(const Vector2f& p1,float w,float h,const Vector2f& pos,float radius) const;
	bool sweepTest(const Vector2f& a0,const Vector2f& a1,float ra,const Vector2f& b0,const Vector2f& b1,float rb,float* u0,float* u1);
	CollisionObject* findObject(int id);
	bool intersectLineCircle(const Vector2f& location, float radius, const Vector2f& lineFrom, const Vector2f& lineTo);
	bool intersectCircleCircle(const CollisionObject* first,const CollisionObject* second);
	void addCollision(CollisionObject* first,CollisionObject* second);
	bool solveQuadraticFormula(float a,float b,float c,float* r1,float* r2);
	CollisionObjects m_Objects;
	Ignores m_Ignores;
	int m_Counter;
	char* m_ColBuffer;
	int m_Size;
	int m_BufferCounter;    
};

namespace coll {

	bool containsType(const Collision& collision,int type);

	int getIDByType(const Collision& collision,int type);

	Vector2f getPositionByType(const Collision& collision,int type);
}

}