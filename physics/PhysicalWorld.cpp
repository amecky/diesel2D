#include "PhysicalWorld.h"
#include "..\utils\Log.h"

namespace ds {

	PhysicalWorld::PhysicalWorld() {
	}


	PhysicalWorld::~PhysicalWorld() {
		if (m_ColliderData.buffer != 0) {
			//delete[] m_ColliderData.buffer;
			DEALLOC(m_ColliderData.buffer);
		}
	}

	// --------------------------------------------------------------------------
	// set sprite array pointer
	// --------------------------------------------------------------------------
	void PhysicalWorld::setDataPtr(SpriteArray* sprites) {
		m_Sprites = sprites;
	}

	// --------------------------------------------------------------------------
	// remove by SID
	// --------------------------------------------------------------------------
	void PhysicalWorld::remove(SID id) {
		for (int i = 0; i < m_ColliderData.num; ++i) {
			if (m_ColliderData.sids[i] == id) {
				m_ColliderData.remove(m_ColliderData.ids[i]);
			}
		}
		/*
		if (m_ColliderMap.find(id) != m_ColliderMap.end()) {
			CID cid = m_ColliderMap[id];
			//LOG << "removing: " << id << " CID:" << cid;
			//debug();		
			m_ColliderData.remove(cid);
			m_ColliderMap.erase(id);
		}
		*/
	}

	// --------------------------------------------------------------------------
	// tick
	// --------------------------------------------------------------------------
	void PhysicalWorld::tick(float dt) {
		for (int i = 0; i < m_ColliderData.num; ++i) {
			m_ColliderData.moveTo(m_ColliderData.ids[i], m_Sprites->getPosition(m_ColliderData.sids[i]));
		}
		/*
		ColliderMap::iterator it = m_ColliderMap.begin();
		while (it != m_ColliderMap.end()) {
			SID sid = it->first;
			CID cid = it->second;
			m_ColliderData.moveTo(cid, m_Sprites->getPosition(sid));
			++it;
		}
		*/
		_collisions.clear();
		checkCollisions();
	}

	// --------------------------------------------------------------------------
	// allocate collider data
	// --------------------------------------------------------------------------
	void PhysicalWorld::allocateCollider(int size) {
		if (size > m_ColliderData.total) {
			ColliderArray<Vector2f> ca;
			int sz = size * (sizeof(ColliderArrayIndex) + sizeof(CID) + sizeof(SID) + sizeof(Vector2f) + sizeof(Vector2f) + sizeof(Vector2f) + sizeof(int) + sizeof(int) + sizeof(ColliderShape));
			//ca.buffer = new char[sz];
			ca.buffer = (char*)ALLOC(sz);
			ca.total = size;
			ca.num = 0;
			ca.indices = (ColliderArrayIndex*)(ca.buffer);
			ca.ids = (CID*)(ca.indices + size);
			ca.sids = (SID*)(ca.ids + size);
			ca.positions = (Vector2f*)(ca.sids + size);
			ca.previous = (Vector2f*)(ca.positions + size);
			ca.extents = (Vector2f*)(ca.previous + size);
			ca.types = (int*)(ca.extents + size);
			ca.layers = (int*)(ca.types + size);
			ca.shapeTypes = (ColliderShape*)(ca.layers + size);
			if (m_ColliderData.buffer != 0) {
				memcpy(ca.indices, m_ColliderData.indices, m_ColliderData.num * sizeof(ColliderArrayIndex));
				memcpy(ca.ids, m_ColliderData.ids, m_ColliderData.num * sizeof(CID));
				memcpy(ca.positions, m_ColliderData.positions, m_ColliderData.num * sizeof(Vector2f));
				memcpy(ca.previous, m_ColliderData.previous, m_ColliderData.num * sizeof(Vector2f));
				memcpy(ca.extents, m_ColliderData.extents, m_ColliderData.num * sizeof(Vector2f));
				memcpy(ca.types, m_ColliderData.types, m_ColliderData.num * sizeof(int));
				memcpy(ca.layers, m_ColliderData.layers, m_ColliderData.num * sizeof(int));
				memcpy(ca.shapeTypes, m_ColliderData.shapeTypes, m_ColliderData.num * sizeof(ColliderShape));
				ca.free_dequeue = m_ColliderData.free_dequeue;
				ca.free_enqueue = m_ColliderData.free_enqueue;
				ca.num = m_ColliderData.num;
				//delete[] m_ColliderData.buffer;
				DEALLOC(m_ColliderData.buffer);
			}
			else {
				ca.clear();
			}
			m_ColliderData = ca;
		}
	}

	// --------------------------------------------------------------------------
	// ignore layer
	// --------------------------------------------------------------------------
	void PhysicalWorld::ignoreLayer(int layer) {
		assert(layer >= 0 && layer < 32);
		LOG << "ignoring layer: " << layer;
		_ignoredLayers.set(layer);
	}

	// --------------------------------------------------------------------------
	// attach collider
	// --------------------------------------------------------------------------
	void PhysicalWorld::attachCollider(SID sid, const Vector2f& extent, int type,int layer) {
		if (m_ColliderData.total == 0) {
			allocateCollider(256);
		}
		if (m_ColliderData.num >= m_ColliderData.total) {
			allocateCollider(m_ColliderData.total * 2);
		}
		const Vector2f& p = m_Sprites->getPosition(sid);
		CID cid = m_ColliderData.create(sid, p, extent, type,layer);
		//m_ColliderMap[sid] = cid;
	}

	// --------------------------------------------------------------------------
	// attach collider
	// --------------------------------------------------------------------------
	void PhysicalWorld::attachCollider(SID sid, int type,int layer) {
		if (m_ColliderData.total == 0) {
			allocateCollider(256);
		}
		if (m_ColliderData.num >= m_ColliderData.total) {
			allocateCollider(m_ColliderData.total * 2);
		}
		const Vector2f& p = m_Sprites->getPosition(sid);
		const Texture& t = m_Sprites->getTexture(sid);
		CID cid = m_ColliderData.create(sid, p, t.dim, type,layer);
		//m_ColliderMap[sid] = cid;
	}

	void PhysicalWorld::attachBoxCollider(SID sid, int type, int layer) {
		if (m_ColliderData.total == 0) {
			allocateCollider(256);
		}
		if (m_ColliderData.num >= m_ColliderData.total) {
			allocateCollider(m_ColliderData.total * 2);
		}
		const v2& p = m_Sprites->getPosition(sid);
		const Texture& t = m_Sprites->getTexture(sid);
		CID cid = m_ColliderData.create(sid, p, t.dim, type, layer, CS_BOX);
		//m_ColliderMap[sid] = cid;
	}

	// --------------------------------------------------------------------------
	// check collisions
	// --------------------------------------------------------------------------
	void PhysicalWorld::checkCollisions() {
		for (int i = 0; i < m_ColliderData.num; ++i) {
			if (!_ignoredLayers.isSet(m_ColliderData.layers[i])) {
				checkCollisions(i, m_ColliderData.positions[i], m_ColliderData.extents[i]);
			}
		}
	}

	// --------------------------------------------------------------------------
	// intersects
	// --------------------------------------------------------------------------
	bool PhysicalWorld::intersects(int firstIndex, int secondIndex) {
		ColliderShape firstShape = m_ColliderData.shapeTypes[firstIndex];
		ColliderShape secondShape = m_ColliderData.shapeTypes[secondIndex];
		const Vector2f& fp = m_ColliderData.positions[firstIndex];
		const Vector2f& fpp = m_ColliderData.previous[firstIndex];
		const Vector2f& fe = m_ColliderData.extents[firstIndex];
		const Vector2f& sp = m_ColliderData.positions[secondIndex];
		const Vector2f& spp = m_ColliderData.previous[secondIndex];
		const Vector2f& se = m_ColliderData.extents[secondIndex];
		if (firstShape == CS_CIRCLE && secondShape == CS_CIRCLE)  {
			// FIXME: calculate radius based on scale of sprite
			float r1 = fe.x * 0.5f;
			float r2 = se.x * 0.5f;
			float u0, u1;
			if( physics::testCircleSweepIntersection(r1, fpp, fp, r2, spp, sp, &u0, &u1) ) {
			//if (physics::testCircleIntersection(fp, r1, sp, r2)) {
				return true;
			}
		}
		else if (firstShape == CS_BOX && secondShape == CS_BOX)  {
			if (physics::testBoxIntersection(fp,fe,sp,se)) {
				return true;
			}
		}
		return false;
	}

	// --------------------------------------------------------------------------
	// check collision
	// --------------------------------------------------------------------------
	void PhysicalWorld::checkCollisions(int currentIndex, const Vector2f& pos, const Vector2f& extent) {
		
		for (int i = 0; i < m_ColliderData.num; ++i) {
			if (!_ignoredLayers.isSet(m_ColliderData.layers[i])) {
				if (i != currentIndex) {
					if (intersects(currentIndex, i)) {
						const Vector2f& p = m_ColliderData.positions[i];
						const Vector2f& pp = m_ColliderData.previous[i];
						const Vector2f& e = m_ColliderData.extents[i];
						CID firstID = m_ColliderData.ids[currentIndex];
						CID secondID = m_ColliderData.ids[i];
						if (!containsCollision(firstID, secondID) && !shouldBeIgnored(m_ColliderData.types[currentIndex], m_ColliderData.types[i])) {
							Vector2f cp = m_ColliderData.previous[currentIndex];
							//Collision& c = m_Collisions[m_NumCollisions++];
							Collision c;
							c.firstPos = pos;
							c.firstColliderID = m_ColliderData.ids[currentIndex];
							c.firstSID = m_ColliderData.sids[currentIndex];
							c.firstType = m_ColliderData.types[currentIndex];
							c.secondPos = p;
							c.secondColliderID = m_ColliderData.ids[i];
							c.secondSID = m_ColliderData.sids[i];
							c.secondType = m_ColliderData.types[i];
							_collisions.push_back(c);
						}
					}
				}
			}
		}
	}

	// --------------------------------------------------------------------------
	// contains collision
	// --------------------------------------------------------------------------
	bool PhysicalWorld::containsCollision(CID firstID, CID secondID) {
		for (Array<Collision>::iterator it = _collisions.begin(), itEnd = _collisions.end(); it != itEnd; ++it) {
			if (it->firstColliderID == firstID && it->secondColliderID == secondID) {
				return true;
			}
			if (it->secondColliderID == firstID && it->firstColliderID == secondID) {
				return true;
			}
		}
		return false;
	}

	// --------------------------------------------------------------------------
	// debug
	// --------------------------------------------------------------------------
	void PhysicalWorld::debug() {
		LOG << "------- Colliders --------";
		for (int i = 0; i < m_ColliderData.num; ++i) {
			LOG << i << " sid: " << m_ColliderData.sids[i] << " cid: " << m_ColliderData.ids[i] << " type: " << m_ColliderData.types[i] << " pos: " << DBG_V2(m_ColliderData.positions[i]) << " extent: " << DBG_V2(m_ColliderData.extents[i]);
		}
	}

	// --------------------------------------------------------------------------
	// debug
	// --------------------------------------------------------------------------
	void PhysicalWorld::debug(SID sid) {
		int idx = -1;
		for (int i = 0; i < m_ColliderData.num; ++i) {
			if (m_ColliderData.sids[i] == sid) {
				idx = i;
			}
		}
		if (idx != -1) {
			LOG << "Physical object:";
			LOG << "sid   : " << m_ColliderData.sids[idx];
			LOG << "cid   : " << m_ColliderData.ids[idx];
			LOG << "type  : " << m_ColliderData.types[idx];
			LOG << "pos   : " << DBG_V2(m_ColliderData.positions[idx]);
			LOG << "extent: " << DBG_V2(m_ColliderData.extents[idx]);
		}
	}

	void PhysicalWorld::save(const ReportWriter& writer) {
		writer.startBox("Colliders");
		const char* HEADERS[] = { "Index", "SID", "CID", "Type", "Position", "Extent" };
		writer.startTable(HEADERS, 6);
		for (int i = 0; i < m_ColliderData.num; ++i) {
			writer.startRow();
			writer.addCell(i);
			writer.addCell(m_ColliderData.sids[i]);
			writer.addCell(m_ColliderData.ids[i]);
			writer.addCell(m_ColliderData.types[i]);
			writer.addCell(m_ColliderData.positions[i]);
			writer.addCell(m_ColliderData.extents[i]);
			writer.endRow();
		}
		writer.endTable();
		writer.endBox();
	}

	// --------------------------------------------------------------------------
	// should be ignored
	// --------------------------------------------------------------------------
	bool PhysicalWorld::shouldBeIgnored(int firstType, int secondType) {
		for (size_t i = 0; i < m_Ignored.size(); ++i) {
			IgnoredCollision& ic = m_Ignored[i];
			if (ic.matches(firstType, secondType)) {
				return true;
			}
		}
		return false;
	}

	// --------------------------------------------------------------------------
	// ignore
	// --------------------------------------------------------------------------
	void PhysicalWorld::ignore(int firstType, int secondType) {
		if (!shouldBeIgnored(firstType, secondType)) {
			IgnoredCollision c;
			c.firstType = firstType;
			c.secondType = secondType;
			m_Ignored.push_back(c);
		}
	}

	// --------------------------------------------------------------------------
	// draw colliders
	// --------------------------------------------------------------------------
	void PhysicalWorld::drawColliders(const Texture& texture) {
		for (int i = 0; i < m_ColliderData.num; ++i) {
			Vector2f& e = m_ColliderData.extents[i];
			const Vector2f& scale = m_Sprites->getScale(m_ColliderData.sids[i]);
			float sx = e.x / texture.dim.x * scale.x;
			float sy = e.y / texture.dim.y * scale.y;
			sprites::draw(m_ColliderData.positions[i], texture, 0.0f, sx, sy);
		}
	}

}