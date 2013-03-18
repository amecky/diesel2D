#include "CollisionDetector.h"
#include "..\utils\Log.h"
#include "..\renderer\DebugRenderer.h"
#include "..\math\vector.h"

const uint32 MAX_OBJECTS = 2048;

namespace ds {

// ----------------------------------------------------
// CollisionBuffer
// ----------------------------------------------------

CollisionBuffer::CollisionBuffer() : m_Size(0) , m_Counter(0) {
    m_Buffer = new char[COL_BUFFER_SIZE];
}

CollisionBuffer::~CollisionBuffer() {
    delete m_Buffer;            
}

bool CollisionBuffer::containsId(uint32 firstID,uint32 secondID) {
	char* buffer = m_Buffer;
	for ( uint32 s = 0; s < m_Counter; ++s ) {
		CollisionEvent event = (*(CollisionEvent*)buffer);
		if ( event.firstID == firstID && event.secondID == secondID ) {
			return true;
		}
		if ( event.firstID == secondID && event.secondID == firstID ) {
			return true;
		}
		buffer += COL_BUFFER_SIZE;
	}
	return false;
}

void CollisionBuffer::add(CDHandle firstHandle,uint32 firstID,const Vec2& firstPos,CDHandle secondHandle,uint32 secondID,const Vec2& secondPos) {
	bool found = false;
	CollisionEvent ce;
	char* buffer = m_Buffer;
	for ( uint32 s = 0; s < m_Counter; ++s ) {
		CollisionEvent event = (*(CollisionEvent*)buffer);		
		if ( containsId(event.firstID,event.secondID) ) {
			found = true;
		}
		buffer += COL_BUFFER_SIZE;
	}
	if ( !found ) {
		//LOG(logINFO) << "adding collision between " << firstID << " and " << secondID;
		if ( m_Size < COL_BUFFER_SIZE ) {
			char* buffer = m_Buffer + m_Size;
			(*(CollisionEvent*)(buffer)).firstID = firstID;
			(*(CollisionEvent*)(buffer)).secondID = secondID;
			(*(CollisionEvent*)(buffer)).firstPos = firstPos;
			(*(CollisionEvent*)(buffer)).secondPos = secondPos;
			(*(CollisionEvent*)(buffer)).firstHandle = firstHandle;
			(*(CollisionEvent*)(buffer)).secondHandle = secondHandle;
			m_Size += COL_BUFFER_SIZE;
			++m_Counter;
		}
	}
}

void CollisionBuffer::get(uint32 index, CollisionEvent* collision) {
    if ( index < m_Counter ) {
		char* buffer = m_Buffer + COL_BUFFER_SIZE * index;
		CollisionEvent event = (*(CollisionEvent*)buffer);
		collision->firstHandle = event.firstHandle;
		collision->firstID = event.firstID;
		collision->firstPos = event.firstPos;
		collision->secondID = event.secondID;
		collision->secondHandle = event.secondHandle;
		collision->secondPos = event.secondPos;
	}
}

void CollisionBuffer::reset() {
	m_Size = 0;
	m_Counter = 0;
}
// ----------------------------------------------------
//  Collision detector
// ----------------------------------------------------
CollisionDetector::CollisionDetector() : m_Index(0) , m_Counter(0) , m_Size(0) , m_DrawMode(false) {
    m_CB = new CollisionBuffer();
	m_ColObjBuffer = new char[COL_OBJ_BUFFER_SIZE];
}

CollisionDetector::~CollisionDetector() {
	delete m_ColObjBuffer;
    delete m_CB;	
}

// -------------------------------------------------------
// Remove collision object by handle
// -------------------------------------------------------
void CollisionDetector::remove(CDHandle handle) {	
	char* buffer = m_ColObjBuffer + handle * COL_OBJ_SIZE;
	bool active = (*(CollisionObject*)buffer).state;
	if ( active ) {
		(*(CollisionObject*)buffer).state = false;
		m_FreeHandles.push_back(handle);
	}
}

CDHandle CollisionDetector::addBox(const Vec2& position,const Vec2& extension,uint32 id) {
	return add(position,Vec2(0,0),extension,id,CDT_BOX);    
}

CDHandle CollisionDetector::addBox(const Vec2& position,const Vec2& offset,const Vec2& extension,uint32 id) {
	return add(position,offset,extension,id,CDT_BOX);    
}

// -------------------------------------------------------
// Add circle
// -------------------------------------------------------
CDHandle CollisionDetector::addCircle(const Vec2& position,float radius,uint32 id) {
	return add(position,Vec2(0,0),Vec2(radius,0.0f),id,CDT_CIRCLE);	
}

CDHandle CollisionDetector::addCircle(const Vec2& position,const Vec2& offset,float radius,uint32 id) {
	return add(position,offset,Vec2(radius,0.0f),id,CDT_CIRCLE);	
}

CDHandle CollisionDetector::addLine(const Vec2& startPoint,const Vec2& endPoint,uint32 id) {	
	return add(startPoint,endPoint,Vec2(0,0),id,CDT_LINE);	
}

CDHandle CollisionDetector::addSphere(const Vec3& pos,float radius,uint32 id) {
	BoundingSphere bs;
	uint32 index = m_Index;
	if ( m_FreeHandles.size() > 0 ) {		
		CDHandle handle = m_FreeHandles[0];
		m_FreeHandles.erase(m_FreeHandles.begin());
		index = handle;
	}	
	else {
		++m_Index;
	}
	bs.index = index;
	bs.position = pos;
	bs.radius = radius;
	m_BoundingSpheres.push_back(bs);
	return index;
}

void CollisionDetector::updateSphere(CDHandle handle,const Vec3& pos,float radius) {
	for ( size_t i = 0; i < m_BoundingSpheres.size();++i ) {
		BoundingSphere* bs = &m_BoundingSpheres[i];
		if (bs->index == handle) {
			bs->position = pos;
			bs->radius = radius;
		}
	}
}

// ---------------------------------------------------------------
// Creates a new collision object in the buffer returning
// the new handle
// ---------------------------------------------------------------
CDHandle CollisionDetector::add(const Vec2& position,const Vec2& offset,const Vec2& extent,uint32 id,CDType type) {
	uint32 index = m_Index;
	if ( m_FreeHandles.size() > 0 ) {		
		CDHandle handle = m_FreeHandles[0];
		m_FreeHandles.erase(m_FreeHandles.begin());
		index = handle;		
	}	
	else {
		++m_Index;
		++m_Counter;
	}
	if ( m_Size < COL_OBJ_BUFFER_SIZE ) {
		char* buffer = m_ColObjBuffer + COL_OBJ_SIZE * index;
		(*(CollisionObject*)buffer).type = type;
		(*(CollisionObject*)buffer).id = id;
		(*(CollisionObject*)buffer).position = position;
		(*(CollisionObject*)buffer).offset = offset;
		(*(CollisionObject*)buffer).extent = extent;
		(*(CollisionObject*)buffer).state = true;
		(*(CollisionObject*)buffer).index = index;
	}
	return index; 
}

// ---------------------------------------------------------------
//
// Gets a collision object for the defined handle
//
// [in] handle - the handle of the object
// [in] colObj - the collision object that will contain the data
// [out] true if found
//
// ---------------------------------------------------------------
bool CollisionDetector::getCollisionObject(CDHandle handle,CollisionObject& colObj) {
	char* buffer = m_ColObjBuffer;
	for (uint32 i = 0; i < m_Counter;++i ) {
		CollisionObject object = (*(CollisionObject*)buffer);
		if ( object.index == handle ) {
			colObj.extent = object.extent;
			colObj.id = object.id;
			colObj.index = object.index;
			colObj.offset = object.offset;
			colObj.position = object.position;
			colObj.state = object.state;
			colObj.type = object.type;
			return true;
		}
		buffer += COL_OBJ_SIZE;
	}
	return false;
}

// ---------------------------------------------------------------
//
// Takes every collision object and checks the intersection
// with all others
//
// ---------------------------------------------------------------
void CollisionDetector::update() {    
	CollisionObject object;
	char* buffer = m_ColObjBuffer;
	for (uint32 i = 0; i < m_Counter;++i ) {
		object = (*(CollisionObject*)buffer);
		if ( object.state ) {			
			intersects(object.position,object.offset,object.extent,i,object.id,object.type);        			
		}
		buffer += COL_OBJ_SIZE;
	}
}

// ---------------------------------------------------------------
// Processes the internal collision buffer
// ---------------------------------------------------------------
void CollisionDetector::processBuffer() {
    if ( m_CB->getCount() > 0 ) {
        for ( size_t i = 0; i < m_Handlers.size(); ++i ) {
            CollisionHandler* handler = m_Handlers[i];
            handler->processBuffer(*m_CB);                
        }
    }
}

// -----------------------------------------------------------------
//
// Checks if there will be an intersection at the new position
// for the collision object
//
// [in] handle - the handle of the collision object to check
// [in] newPos - the desired new position of the object
// [out] true if there will be an intersection at the new position
// -----------------------------------------------------------------
bool CollisionDetector::checkIntersection(CDHandle handle,const Vec2& newPos,uint32 id) {
	CollisionObject co;
	if ( getCollisionObject(handle,co)) {		
		CollisionObject object;
		char* buffer = m_ColObjBuffer;
		for (uint32 i = 0; i < m_Counter;++i ) {
			object = (*(CollisionObject*)buffer);
			if ( object.id != co.id && object.state && object.id == id ) {
				if ( co.type == CDT_BOX && object.type == CDT_BOX && object.state ) {
					if (checkBoxIntersection(newPos,co.extent.x,co.extent.y,object.position,object.extent.x,object.extent.y)) {
						return true;
					}
				}
				if ( co.type == CDT_BOX && object.type == CDT_CIRCLE ) {				
					Vec2 bp = newPos + co.offset;
					Vec2 cp = object.position + object.offset;
					if ( checkBoxCircle(cp,co.extent.x,co.extent.y,bp,object.extent.x) ) {
						return true;
					}
				}
				if ( co.type == CDT_CIRCLE && object.type == CDT_BOX ) {				
					Vec2 bp = newPos + co.offset;
					Vec2 cp = object.position + object.offset;
					if ( checkBoxCircle(cp,object.extent.x,object.extent.y,bp,co.extent.x) ) {
						return true;
					}
				}
				// circle to circle intersection
				if ( co.type == CDT_CIRCLE && object.type == CDT_CIRCLE && object.state ) {
					if ( checkCircle(newPos,co.offset,co.extent.x,object.position,object.offset,object.extent.x) ) {
						return true;
					}
				}
				// line to circle
				if ( co.type == CDT_LINE && object.type == CDT_CIRCLE && object.state ) {
					if ( checkLineCircle(object.position,object.extent.x,newPos,co.offset) ) {
						return true;
					}
				}
				// circle to line
				if ( co.type == CDT_CIRCLE && object.type == CDT_LINE && object.state ) {
					if ( checkLineCircle(newPos,co.extent.x,object.position,object.extent) ) {
						return true;
					}
				}    
			}
			buffer += COL_OBJ_SIZE;
		}
	}
	return false;
}

// ---------------------------------------------------------------
// Check intersection
// ---------------------------------------------------------------
void CollisionDetector::intersects(const Vec2& pos,const Vec2& offset,const Vec2& ext,uint32 handle,uint32 identifier,CDType type) const {	
	CollisionObject object;
	char* buffer = m_ColObjBuffer;
	for (uint32 i = 0; i < m_Counter;++i ) {
		object = (*(CollisionObject*)buffer);
		if ( object.id != identifier && object.state ) {
			bool collision = false;
			if ( type == CDT_BOX && object.type == CDT_BOX && object.state ) {
				if ( checkBoxIntersection(pos,ext.x,ext.y,object.position,object.extent.x,object.extent.y)) {  
					m_CB->add(handle,identifier,pos,object.index,object.id,object.position);                
				}
			}
			if ( type == CDT_BOX && object.type == CDT_CIRCLE ) {				
				Vec2 bp = pos + offset;
				Vec2 cp = object.position + object.offset;
				if ( checkBoxCircle(cp,ext.x,ext.y,bp,object.extent.x)) {
					m_CB->add(handle,identifier,pos,object.index,object.id,object.position);                
				}
			}
			// circle to circle intersection
			if ( type == CDT_CIRCLE && object.type == CDT_CIRCLE && object.state ) {
				if ( checkCircle(pos,offset,ext.x,object.position,object.offset,object.extent.x) ) {  		
					m_CB->add(handle,identifier,pos,object.index,object.id,object.position);                
				}
			}
			// line to circle
			if ( type == CDT_LINE && object.type == CDT_CIRCLE && object.state ) {
				if ( checkLineCircle(object.position,object.extent.x,pos,offset) ) {  					
					m_CB->add(handle,identifier,pos,object.index,object.id,object.position);                
				}
			}
			// circle to line
			if ( type == CDT_CIRCLE && object.type == CDT_LINE && object.state ) {
				if ( checkLineCircle(pos,ext.x,object.position,object.extent) ) {  					
					m_CB->add(handle,identifier,pos,object.index,object.id,object.position);                
				}
			}    
		}
		buffer += COL_OBJ_SIZE;
	}

}

// ---------------------------------------------------------------
// Box -> Circle
// ---------------------------------------------------------------
bool CollisionDetector::checkBoxCircle(const Vec2& p1,float w,float h,const Vec2& pos,float radius) const {
	float halfWidth = w * 0.5f;
	float halfHeight = h * 0.5f;
	float cx = abs(pos.x - p1.x - halfWidth);
	float xDist = halfWidth + radius;
	if (cx > xDist) {
		return false;
	}
	float cy = abs(pos.y - p1.y - halfHeight);
	float yDist = halfHeight + radius;
	if (cy > yDist) {
		return false;
	}
	if (cx <= halfWidth || cy <= halfHeight) {
		return true;
	}
	float xCornerDist = cx - halfWidth;
	float yCornerDist = cy - halfHeight;
	float xCornerDistSq = xCornerDist * xCornerDist;
	float yCornerDistSq = yCornerDist * yCornerDist;
	float maxCornerDistSq = radius * radius;
	return xCornerDistSq + yCornerDistSq <= maxCornerDistSq;	
}

// ---------------------------------------------------------------
// Line -> Circle
// ---------------------------------------------------------------
bool CollisionDetector::checkLineCircle(const Vec2& location, float radius,const Vec2& lineFrom,const Vec2& lineTo) const {
	Vec2 ac = location - lineFrom;
	Vec2 ab = lineTo - lineFrom;
	float ab2 = vector::dot(ab,ab);
	float acab = vector::dot(ac,ab);
	float t = acab / ab2;

	if (t < 0)
		t = 0;
	else if (t > 1)
		t = 1;

	Vec2 h = ((ab * t) + lineFrom) - location;
	float h2 = vector::dot(h,h);

	return (h2 <= (radius * radius));
}

// ---------------------------------------------------------------
// Circle -> Circle
// ---------------------------------------------------------------
bool CollisionDetector::checkCircle(const Vec2& p1, const Vec2& o1, float r1, const Vec2& p2, const Vec2& o2, float r2) const {
	Vec2 fp = p1 + o1;
	Vec2 sp = p2 + o2;
	Vec2 diff = fp - sp;
	if ( vector::sqrLength(diff) <= (( r1+r2) * (r1+r2)) ) {
		return true;
	}
	return false;
}

// ---------------------------------------------------------------
// Box -> Box
// ---------------------------------------------------------------
bool CollisionDetector::checkBoxIntersection(const Vec2& p1,float w1,float h1,const Vec2& p2,float w2,float h2) const {    
    float left1 = p1.x;
    float left2 = p2.x;
    float right1 = left1 + w1;
    float right2 = left2 + w2;
    float top1 = p1.y;
    float top2 = p2.y;
    float bottom1 = top1 + h1;
    float bottom2 = top2 + h2;
    if ( bottom1 < top2 ) {
        return false;
    }
    if ( top1 > bottom2 ) {
        return false;
    }
    if (right1 < left2) {
        return false;
    }
    if (left1 > right2) {
        return false;
    }
    return true;

}

// ---------------------------------------------------------------
// Debug output to Logfile
// ---------------------------------------------------------------
void CollisionDetector::debug() {
	CollisionObject object;
	char* buffer = m_ColObjBuffer;
	for (uint32 i = 0; i < m_Counter;++i ) {
		object = (*(CollisionObject*)buffer);
		if ( object.type == CDT_CIRCLE ) {
			LOGC(logINFO,"CollisionDetector") << "handle: " << object.index << " id: " << object.id << " CIRCLE pos: " << object.position.x << "," << object.position.y << " offset: " << object.offset.x << "," << object.offset.y << " radius: " << object.extent.x << " state: " << object.state;		
		}
		else if ( object.type == CDT_LINE ) {
			LOGC(logINFO,"CollisionDetector") << "handle: " << object.index << " id: " << object.id << " LINE startpos: " << object.position.x << "," << object.position.y << " endpos: " << object.offset.x << "," << object.offset.y << " state: " << object.state;		
		}
		else if ( object.type == CDT_BOX ) {
			LOGC(logINFO,"CollisionDetector") << "handle: " << object.index << " id: " << object.id << " BOX pos: " << object.position.x << "," << object.position.y << " offset: " << object.offset.x << "," << object.offset.y << " extent: " << object.extent.x << "," << object.extent.y << " state: " << object.state;		
		}
		buffer += COL_OBJ_SIZE;
	}
}

}
