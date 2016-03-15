#include "PhysicalWorld.h"
#include "..\utils\Log.h"
#include "..\utils\Profiler.h"

namespace ds {

	// --------------------------------------------------------------------------
	// PotentialColliders
	// --------------------------------------------------------------------------
	void PotentialColliders::reset() {
		//_colliders.clear();
		num = 0;
	}

	void PotentialColliders::add(int firstIndex, CID first, int firstType,int secondIndex, CID second, int secondType) {
		if (!shouldBeIgnored(firstType, secondType)) {
			if (!contains(first, second)) {
				//PotentialCollider pc;
				PotentialCollider& pc = _colliders[num++];
				pc.firstIndex = firstIndex;
				pc.first = first;
				pc.secondIndex = secondIndex;
				pc.second = second;
				//_colliders.push_back(pc);
			}
		}
	}

	int PotentialColliders::size() const {
		//return _colliders.size();
		return num;
	}

	const PotentialCollider& PotentialColliders::get(int index) const {
		return _colliders[index];
	}

	bool PotentialColliders::contains(CID first, CID second) const {
		//for (int i = 0; i < _colliders.size(); ++i) {
		for (int i = 0; i < num; ++i) {
			if (_colliders[i].first == first && _colliders[i].second == second) {
				return true;
			}
			if (_colliders[i].first == second && _colliders[i].second == first) {
				return true;
			}
		}
		return false;
	}

	bool PotentialColliders::shouldBeIgnored(int firstType, int secondType) {
		for (size_t i = 0; i < _ignored.size(); ++i) {
			IgnoredCollision& ic = _ignored[i];
			if (ic.matches(firstType, secondType)) {
				return true;
			}
		}
		return false;
	}

	void PotentialColliders::ignore(int firstType, int secondType) {
		if (!shouldBeIgnored(firstType, secondType)) {
			IgnoredCollision c;
			c.firstType = firstType;
			c.secondType = secondType;
			_ignored.push_back(c);
		}
	}
	// --------------------------------------------------------------------------
	// PhysicalWorld
	// --------------------------------------------------------------------------
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
		ZoneTracker z("PhysicalWorld::remove");
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
		ZoneTracker z("PhysicalWorld::tick");
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
			ColliderArray<v2> ca;
			int sz = size * (sizeof(ColliderArrayIndex) + sizeof(CID) + sizeof(SID) + sizeof(v2) + sizeof(v2) + sizeof(v2) + sizeof(int) + sizeof(int) + sizeof(ColliderShape));
			//ca.buffer = new char[sz];
			ca.buffer = (char*)ALLOC(sz);
			ca.total = size;
			ca.num = 0;
			ca.indices = (ColliderArrayIndex*)(ca.buffer);
			ca.ids = (CID*)(ca.indices + size);
			ca.sids = (SID*)(ca.ids + size);
			ca.positions = (v2*)(ca.sids + size);
			ca.previous = (v2*)(ca.positions + size);
			ca.extents = (v2*)(ca.previous + size);
			ca.types = (int*)(ca.extents + size);
			ca.layers = (int*)(ca.types + size);
			ca.shapeTypes = (ColliderShape*)(ca.layers + size);
			if (m_ColliderData.buffer != 0) {
				memcpy(ca.indices, m_ColliderData.indices, m_ColliderData.num * sizeof(ColliderArrayIndex));
				memcpy(ca.ids, m_ColliderData.ids, m_ColliderData.num * sizeof(CID));
				memcpy(ca.positions, m_ColliderData.positions, m_ColliderData.num * sizeof(v2));
				memcpy(ca.previous, m_ColliderData.previous, m_ColliderData.num * sizeof(v2));
				memcpy(ca.extents, m_ColliderData.extents, m_ColliderData.num * sizeof(v2));
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
	void PhysicalWorld::attachCollider(SID sid, const v2& extent, int type,int layer) {
		if (m_ColliderData.total == 0) {
			allocateCollider(256);
		}
		if (m_ColliderData.num >= m_ColliderData.total) {
			allocateCollider(m_ColliderData.total * 2);
		}
		const v2& p = m_Sprites->getPosition(sid);
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
		const v2& p = m_Sprites->getPosition(sid);
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
		ZoneTracker z("PhysicalWorld::checkCollisions");
		{
			ZoneTracker z1("PhysicalWorld::potentials");
			_potentialColliders.reset();
			for (int i = 0; i < m_ColliderData.num; ++i) {
				if (!_ignoredLayers.isSet(m_ColliderData.layers[i])) {
					for (int j = 0; j < m_ColliderData.num; ++j) {
						if (!_ignoredLayers.isSet(m_ColliderData.layers[j])) {
							if (i != j) {
								_potentialColliders.add(i, m_ColliderData.ids[i], m_ColliderData.types[i], j, m_ColliderData.ids[j], m_ColliderData.types[j]);
							}
						}
					}
				}
			}
		}
		{
			ZoneTracker z2("PhysicalWorld::check");
			for (int x = 0; x < _potentialColliders.size(); ++x){
				const PotentialCollider& pc = _potentialColliders.get(x);
				if (intersects(pc.firstIndex, pc.secondIndex)) {
					CID firstID = m_ColliderData.ids[pc.firstIndex];
					CID secondID = m_ColliderData.ids[pc.secondIndex];
					if (!containsCollision(firstID, secondID)) {
						Collision c;
						c.firstPos = m_ColliderData.positions[pc.firstIndex];
						c.firstColliderID = m_ColliderData.ids[pc.firstIndex];
						c.firstSID = m_ColliderData.sids[pc.firstIndex];
						c.firstType = m_ColliderData.types[pc.firstIndex];
						c.secondPos = m_ColliderData.positions[pc.secondIndex];
						c.secondColliderID = m_ColliderData.ids[pc.secondIndex];
						c.secondSID = m_ColliderData.sids[pc.secondIndex];
						c.secondType = m_ColliderData.types[pc.secondIndex];
						_collisions.push_back(c);
					}
				}
			}
		}
	}

	// --------------------------------------------------------------------------
	// intersects
	// --------------------------------------------------------------------------
	bool PhysicalWorld::intersects(int firstIndex, int secondIndex) {
		ColliderShape firstShape = m_ColliderData.shapeTypes[firstIndex];
		ColliderShape secondShape = m_ColliderData.shapeTypes[secondIndex];
		const v2& fp = m_ColliderData.positions[firstIndex];
		const v2& fpp = m_ColliderData.previous[firstIndex];
		const v2& fe = m_ColliderData.extents[firstIndex];
		const v2& sp = m_ColliderData.positions[secondIndex];
		const v2& spp = m_ColliderData.previous[secondIndex];
		const v2& se = m_ColliderData.extents[secondIndex];
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
	void PhysicalWorld::checkCollisions(int currentIndex, const v2& pos, const v2& extent) {
		
		for (int i = 0; i < m_ColliderData.num; ++i) {
			if (!_ignoredLayers.isSet(m_ColliderData.layers[i])) {
				if (i != currentIndex) {
					if (intersects(currentIndex, i)) {
						const v2& p = m_ColliderData.positions[i];
						const v2& pp = m_ColliderData.previous[i];
						const v2& e = m_ColliderData.extents[i];
						CID firstID = m_ColliderData.ids[currentIndex];
						CID secondID = m_ColliderData.ids[i];
						if (!containsCollision(firstID, secondID) ) {
							v2 cp = m_ColliderData.previous[currentIndex];
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
	/*
	bool PhysicalWorld::shouldBeIgnored(int firstType, int secondType) {
		for (size_t i = 0; i < m_Ignored.size(); ++i) {
			IgnoredCollision& ic = m_Ignored[i];
			if (ic.matches(firstType, secondType)) {
				return true;
			}
		}
		return false;
	}
	*/
	// --------------------------------------------------------------------------
	// ignore
	// --------------------------------------------------------------------------
	void PhysicalWorld::ignore(int firstType, int secondType) {
		_potentialColliders.ignore(firstType, secondType);
		/*
		if (!shouldBeIgnored(firstType, secondType)) {
			IgnoredCollision c;
			c.firstType = firstType;
			c.secondType = secondType;
			m_Ignored.push_back(c);
		}
		*/
	}

	// --------------------------------------------------------------------------
	// draw colliders
	// --------------------------------------------------------------------------
	void PhysicalWorld::drawColliders(const Texture& texture) {
		for (int i = 0; i < m_ColliderData.num; ++i) {
			v2& e = m_ColliderData.extents[i];
			const v2& scale = m_Sprites->getScale(m_ColliderData.sids[i]);
			float sx = e.x / texture.dim.x * scale.x;
			float sy = e.y / texture.dim.y * scale.y;
			sprites::draw(m_ColliderData.positions[i], texture, 0.0f, sx, sy);
		}
	}

}