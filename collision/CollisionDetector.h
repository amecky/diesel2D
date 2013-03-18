#pragma once
#include "CharBuffer.h"
#include <vector>
#include "..\math\math_types.h"

namespace ds {


typedef unsigned int CDHandle;

const uint32 UNUSED_HANDLE = 0xffffff;
// -------------------------------------------------------
// Collision event
// -------------------------------------------------------
struct CollisionEvent {
    CDHandle firstHandle;
    CDHandle secondHandle;
    uint32 firstID;
    uint32 secondID;
	Vec2 firstPos;
	Vec2 secondPos;
};

const uint32 COL_BUFFER_SIZE = 32;
// --------------------------------------------
// CollisionBuffer
//
// This class contains all detected collisions
// --------------------------------------------
class CollisionBuffer {
    
public:
    CollisionBuffer();
    ~CollisionBuffer();
    void reset();
	uint32 getCount() {
		return m_Counter;
	}
    void add(CDHandle firstHandle,uint32 firstID,const Vec2& firstPos,CDHandle secondHandle,uint32 secondID,const Vec2& secondPos);
    void get(uint32 index,CollisionEvent* collision);
	bool containsId(uint32 firstID,uint32 secondID);
private:
	char* m_Buffer;
	uint32 m_Counter;
	uint32 m_Size;
};

// ---------------------------------------------
// Virtual class defining a collision handler
// ---------------------------------------------
class CollisionHandler {
    
public:
    virtual void processBuffer(CollisionBuffer& buffer) = 0;
};

// ------------------------------------------
//
// ------------------------------------------

const uint32 COL_OBJ_BUFFER_SIZE = 64 * 1024;
const uint32 COL_OBJ_SIZE = 40;
// -------------------------------------------------------
// Collision object type
// -------------------------------------------------------
enum CDType {
	CDT_BOX,
	CDT_CIRCLE,
	CDT_LINE
};

// -------------------------------------------------------
// Collision object
// -------------------------------------------------------
struct CollisionObject {
	CDType type;
	Vec2 position;
	Vec2 offset;
	Vec2 extent;
	bool state;
	uint32 id;
	uint32 index;
};

// -------------------------------------------------------
// CollisionDetector
// -------------------------------------------------------
class CollisionDetector {

struct IgnoredCollision {
	CDHandle firstID;
	CDHandle secondID;
};
    
struct BoundingSphere {
	float radius;
	Vec3 position;
	uint32 index;
};

typedef std::vector<CollisionHandler*> Handlers;    
typedef std::vector<CDHandle> FreeHandles;
typedef std::vector<BoundingSphere> BoundingSpheres;

public:
    CollisionDetector();
    virtual ~CollisionDetector();
    CDHandle addBox(const Vec2& pos,const Vec2& extent,uint32 id);
	CDHandle addBox(const Vec2& pos,const Vec2& offset,const Vec2& extent,uint32 id);
	CDHandle addCircle(const Vec2& pos,float radius,uint32 id);
	CDHandle addCircle(const Vec2& pos,const Vec2& offset,float radius,uint32 id);
	CDHandle addLine(const Vec2& startPoint,const Vec2& endPoint,uint32 id);
	CDHandle addSphere(const Vec3& pos,float radius,uint32 id);
	void updateSphere(CDHandle handle,const Vec3& pos,float radius);
	void updateLine(CDHandle handle,const Vec2& startPoint,const Vec2& endPoint) {
		char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
		(*(CollisionObject*)buffer).position = startPoint;
		(*(CollisionObject*)buffer).offset = endPoint;		
	}
    void setActive(CDHandle handle,bool active) {
		char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
		(*(CollisionObject*)buffer).state = active;
    }
    void setPosition(CDHandle handle,const Vec2& pos) {
		char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
		(*(CollisionObject*)buffer).position = pos;
    }
	void setOffset(CDHandle handle,const Vec2& offset) {
		char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
		(*(CollisionObject*)buffer).offset = offset;
	}
    void setExtent(CDHandle handle,const Vec2& extent) {
		char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
		(*(CollisionObject*)buffer).extent = extent;
    }
    void update();    
    void registerHandler(CollisionHandler* handler) {
        m_Handlers.push_back(handler);
    }        
    void remove(CDHandle handle);
	void reset() {
		m_CB->reset();
	}
	bool getCollisionObject(CDHandle handle,CollisionObject& colObj);
	void processBuffer();
	void addIgnore(CDHandle first,CDHandle second) {}
	void debug();
	CollisionBuffer* getBuffer() {
		return m_CB;
	}
	
	bool checkIntersection(CDHandle handle,const Vec2& newPos,uint32 id);
private:
    //
	CDHandle add(const Vec2& pos,const Vec2& offset,const Vec2& extent,uint32 id,CDType type);
    void intersects(const Vec2& pos,const Vec2& offset,const Vec2& extent,uint32 handle,uint32 id,CDType type) const;
    bool checkBoxIntersection(const Vec2& p1,float w1,float h1,const Vec2& p2,float w2,float h2) const;
	bool checkCircle(const Vec2& p1, const Vec2& o1, float r1, const Vec2& p2, const Vec2& o2, float r2) const;
	bool checkLineCircle(const Vec2& location, float radius,const Vec2& lineFrom,const Vec2& lineTo) const;
	bool checkBoxCircle(const Vec2& p1,float w,float h,const Vec2& pos,float radius) const;
    //
	char* m_ColObjBuffer;
	uint32 m_Size;
	uint32 m_Counter;
	/*
	uint32* m_Types;
    Vec2* m_Positions;
	Vec2* m_Offsets;
    Vec2* m_Extents;
    bool* m_States;
    uint32* m_Identifiers;
	*/
    uint32 m_Index;
    Handlers m_Handlers;
    CollisionBuffer* m_CB;
	FreeHandles m_FreeHandles;
	BoundingSpheres m_BoundingSpheres;
	bool m_DrawMode;
};

namespace collision {

inline bool containsID(const CollisionEvent& collision,uint32 id) {
	if ( collision.firstID == id || collision.secondID == id ) {
		return true;
	}
	return false;
}

inline CDHandle getHandleByID(const CollisionEvent& collision,uint32 id) {
	if ( collision.firstID == id ) {
		return collision.firstHandle;
	}
	return collision.secondHandle;
}

inline const Vec2& getPositionByID(const CollisionEvent& collision,uint32 id) {
	if ( collision.firstID == id ) {
		return collision.firstPos;
	}	
	return collision.secondPos;
}

inline bool isBetween(const CollisionEvent& collision,uint32 firstID,uint32 secondID) {
	if ( containsID(collision,firstID) && containsID(collision,secondID) ) {
		return true;
	}	
	return false;
}

}

}


