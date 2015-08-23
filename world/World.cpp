#include "World.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "actions\MoveToAction.h"
#include "actions\ScalingAction.h"
#include "actions\AlphaFadeToAction.h"
#include "actions\RotateByAction.h"
#include "actions\FollowPathAction.h"
#include "actions\FollowCurveAction.h"
#include "actions\MoveByAction.h"
#include "actions\WaitAction.h"
#include "actions\MoveWithAction.h"
#include "actions\RotateAction.h"
#include "actions\FollowTargetAction.h"
#include "actions\FollowStraightPathAction.h"
#include "actions\ColorFadeToAction.h"
#include "actions\RemoveAfterAction.h"
#include "..\physics\ColliderArray.h"
#include "..\utils\Profiler.h"

namespace ds {

	World::World(void) {
		m_MoveToAction = new MoveToAction;
		m_Actions.push_back(m_MoveToAction);
		m_ScalingAction = new ScalingAction;
		m_Actions.push_back(m_ScalingAction);
		m_AlphaFadeToAction = new AlphaFadeToAction;
		m_Actions.push_back(m_AlphaFadeToAction);
		m_RotateByAction = new RotateByAction;
		m_Actions.push_back(m_RotateByAction);
		m_FollowPathAction = new FollowPathAction;
		m_Actions.push_back(m_FollowPathAction);
		m_FollowCurveAction = new FollowCurveAction;
		m_Actions.push_back(m_FollowCurveAction);
		m_MoveByAction = new MoveByAction;
		m_Actions.push_back(m_MoveByAction);
		m_WaitAction = new WaitAction;
		m_Actions.push_back(m_WaitAction);
		m_MoveWithAction = new MoveWithAction;
		m_Actions.push_back(m_MoveWithAction);
		m_RotateAction = new RotateAction;
		m_Actions.push_back(m_RotateAction);
		m_FollowTargetAction = new FollowTargetAction;
		m_Actions.push_back(m_FollowTargetAction);
		m_FollowStraightPathAction = new FollowStraightPathAction;
		m_Actions.push_back(m_FollowStraightPathAction);
		m_ColorFadeToAction = new ColorFadeToAction;
		m_Actions.push_back(m_ColorFadeToAction);
		_removeAfterAction = new RemoveAfterAction;
		m_Actions.push_back(_removeAfterAction);
		
	}


	World::~World(void) {
		Actions::iterator it = m_Actions.begin();
		while ( it != m_Actions.end()) {
			delete (*it);
			it = m_Actions.erase(it);
		}		
		if ( m_Data.buffer != 0 ) {
			delete[] m_Data.buffer;
		}
		
	}

	void World::allocate(int size) {
		if ( size > m_Data.total ) {
			SpriteArray sad;
			int sz = size * (sizeof(SpriteArrayIndex) + sizeof(SID) + sizeof(Vector2f) + sizeof(Vector2f) + sizeof(float) + sizeof(Texture) + sizeof(Color) + sizeof(float) + sizeof(int) + sizeof(int));
			sad.buffer = new char[sz];
			sad.total = size;
			sad.num = 0;
			sad.indices = (SpriteArrayIndex*)(sad.buffer);
			sad.ids = (SID*)(sad.indices + size);
			sad.positions = (Vector2f*)(sad.ids + size);
			sad.scales = (Vector2f*)(sad.positions + size);
			sad.rotations = (float*)(sad.scales + size);
			sad.textures = (ds::Texture*)(sad.rotations + size);
			sad.colors = (ds::Color*)(sad.textures + size);
			sad.timers = (float*)(sad.colors + size);
			sad.types = (int*)(sad.timers + size);
			sad.layers = (int*)(sad.types + size);
			if ( m_Data.buffer != 0 ) {
				memcpy(sad.indices, m_Data.indices, m_Data.num * sizeof(SpriteArrayIndex));
				memcpy(sad.ids, m_Data.ids, m_Data.num * sizeof(SID));
				memcpy(sad.positions, m_Data.positions, m_Data.num * sizeof(Vector2f));
				memcpy(sad.scales, m_Data.scales, m_Data.num * sizeof(Vector2f));
				memcpy(sad.rotations, m_Data.rotations, m_Data.num * sizeof(float));
				memcpy(sad.textures, m_Data.textures, m_Data.num * sizeof(Texture));
				memcpy(sad.colors, m_Data.colors, m_Data.num * sizeof(Color));
				memcpy(sad.timers, m_Data.timers, m_Data.num * sizeof(float));
				memcpy(sad.types, m_Data.types, m_Data.num * sizeof(int));
				memcpy(sad.layers, m_Data.layers, m_Data.num * sizeof(int));
				sad.free_dequeue = m_Data.free_dequeue;
				sad.free_enqueue = m_Data.free_enqueue;
				sad.num = m_Data.num;
				delete[] m_Data.buffer;
			}
			else {
				sar::clear(sad);
			}
			m_Data = sad;
			m_Physics.setDataPtr(&m_Data);
		}
	}

	
	SID World::create(const Vector2f& pos,const Texture& r,int type,int layer) {
		if ( m_Data.total == 0 ) {
			allocate(256);
		}
		if ( m_Data.num >= m_Data.total ) {
			allocate(m_Data.total * 2);
		}
		return sar::create(m_Data,pos,r,type,layer);		
	}

	SID World::create(const Vector2f& pos, const char* templateName,int layer) {
		if (m_Data.total == 0) {
			allocate(256);
		}
		if (m_Data.num >= m_Data.total) {
			allocate(m_Data.total * 2);
		}
		Sprite sp;
		if (renderer::getSpriteTemplate(templateName, &sp)) {
			SID sid = sar::create(m_Data, pos, sp.texture, sp.type,layer);
			sp.layer = layer;
			sar::set(m_Data, sid, sp);
			sar::setPosition(m_Data, sid, pos);
			return sid;
			
		}
		return INVALID_SID;
	}

	void World::remove(SID id) {
		m_Physics.remove(id);		
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->remove(id);
		}		
		sar::remove(m_Data,id);
	}

	void World::renderSingleLayer(int layer) {
		PR_START("World:render")
		for (int i = 0; i < m_Data.num; ++i) {
			if (m_Data.layers[i] == layer) {
				sprites::draw(m_Data.positions[i], m_Data.textures[i], m_Data.rotations[i], m_Data.scales[i].x, m_Data.scales[i].y, m_Data.colors[i]);
			}
		}
		PR_END("World:render")
	}

	void World::renderLayers(int* layers, int num_layers) {
		PR_START("World:render")
		for (int j = 0; j < num_layers; ++j) {
			for (int i = 0; i < m_Data.num; ++i) {
				if (m_Data.layers[i] == layers[j]) {					
					sprites::draw(m_Data.positions[i], m_Data.textures[i], m_Data.rotations[i], m_Data.scales[i].x, m_Data.scales[i].y, m_Data.colors[i]);
				}
			}
		}
		PR_END("World:render")
	}

	void World::render() {
		PR_START("World:render")
		for ( int i = 0; i < m_Data.num; ++i ) {
			sprites::draw(m_Data.positions[i],m_Data.textures[i],m_Data.rotations[i],m_Data.scales[i].x,m_Data.scales[i].y,m_Data.colors[i]);
		}
		PR_END("World:render")
	}

	void World::tick(float dt) {
		PR_START("World:tick")
		m_Buffer.reset();
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->update(m_Data, dt, m_Buffer);
		}
		// FIXME: handle AT_KILL
		for (int i = 0; i < m_Buffer.num; ++i) {
			const ActionEvent& e = m_Buffer.events[i];
			if (e.type == AT_KILL) {
				remove(e.sid);
			}
		}
		m_Physics.tick(dt);
		PR_END("World:tick")
	}

	void World::moveTo(SID sid,const Vector2f& startPos,const Vector2f& endPos,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		m_MoveToAction->attach(sid,m_Data,startPos,endPos,ttl,mode,tweeningType);
	}

	void World::moveBy(SID sid,const Vector2f& velocity,bool bounce) {
		m_MoveByAction->attach(sid,velocity,bounce);
	}

	void World::moveWith(SID sid,const MoveFunc& function,float ttl) {
		m_MoveWithAction->attach(sid,function,ttl);
	}

	const int World::getMovingNumber() const {
		return m_MoveToAction->size();
	}

	void World::scaleTo(SID sid,const Vector2f& startScale,const Vector2f& endScale,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		m_ScalingAction->attach(sid,startScale,endScale,ttl,mode,tweeningType);
	}

	void World::setRotation(SID sid,float angle) {
		sar::rotate(m_Data,sid,angle);
	}

	void World::setRotation(SID sid, const Vector2f& target) {
		float angle = vector::calculateRotation(target);
		sar::rotate(m_Data, sid, angle);
	}

	void World::rotateBy(SID sid,float angle,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		float currentAngle = sar::getRotation(m_Data,sid);
		m_RotateByAction->attach(sid,currentAngle,currentAngle + angle,ttl,mode,tweeningType);
	}

	void World::rotate(SID sid, float angleVelocity, float ttl, int mode) {
		m_RotateAction->attach(sid, angleVelocity, ttl, mode);
	}

	void World::rotateTo(SID sid,float angle,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		float currentAngle = sar::getRotation(m_Data,sid);
		float diff = angle - currentAngle;
		// FIXME: build dot product to check which direction
		//LOG << "rotateTo - current angle: " << RADTODEG(currentAngle) << " diff: " << RADTODEG(diff);
		if ( diff < 0.0f ) {
			diff += TWO_PI;
		}
		m_RotateByAction->attach(sid,currentAngle,currentAngle + diff,ttl,mode,tweeningType);
	}

	void World::rotateTo(SID sid, const Vector2f& target, float ttl, const tweening::TweeningType& tweeningType) {
		Vector2f n = normalize(target);
		float angle = vector::calculateRotation(n);
		rotateTo(sid, angle, ttl, 0, tweeningType);
	}

	void World::removeAfter(SID sid, float ttl) {
		_removeAfterAction->attach(sid, ttl);
	}

	void World::attachTo(SID sid, SID parent) {
		
	}

	void World::fadeAlphaTo(SID sid,float startAlpha,float endAlpha,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		m_AlphaFadeToAction->attach(sid,startAlpha,endAlpha,ttl,mode,tweeningType);
	}

	void World::fadeColorTo(SID sid, const Color& startColor, const Color& endColor, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		m_ColorFadeToAction->attach(sid, startColor, endColor, ttl, mode, tweeningType);
	}

	void World::followPath(SID sid,CubicBezierPath* path,float ttl,int mode) {
		m_FollowPathAction->attach(sid,path,ttl,mode);
	}

	void World::followPath(SID sid, StraightPath* path, float ttl, int mode) {
		m_FollowStraightPathAction->attach(sid,m_Data, path, ttl, mode);
	}

	void World::followCurve(SID sid,BezierCurve* path,float ttl,int mode) {
		m_FollowCurveAction->attach(sid,path,ttl,mode);
	}

	void World::followTarget(SID sid,float velocity, Vector2f* pos) {
		m_FollowTargetAction->attach(sid, velocity);
		m_FollowTargetAction->setTarget(pos);
	}

	void World::wait(SID sid,float ttl) {
		m_WaitAction->attach(sid,ttl);
	}

	void World::clear() {
		sar::clear(m_Data);
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->clear();
		}		
	}

	

	void World::setBoundingRect(const Rect& r) {
		// we need to switch y
		Rect newRect = r;
		newRect.top = r.bottom;
		newRect.bottom = r.top;
		for (size_t i = 0; i < m_Actions.size(); ++i) {
			m_Actions[i]->setBoundingRect(newRect);
		}
	}

	void World::debug() {
		LOG << "--------------------------";
		LOG << "       World";
		LOG << "--------------------------";
		LOG << "-------- Sprites ---------";
		sar::debug(m_Data);
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->debug();
		}
		
	}

	void World::debug(SID sid) {
		sar::debug(m_Data, sid);
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			if ( m_Actions[i]->contains(sid)) {
				m_Actions[i]->debug(sid);
			}
		}
	}

	SID World::pick(const Vector2f& pos) {
		for ( int i = 0; i < m_Data.num; ++i ) {
			Vector2f& p = m_Data.positions[i];
			Texture& t = m_Data.textures[i];
			if ( physics::isInsideBox(pos,p,t.dim)) {
				return m_Data.ids[i];
			}
		}
		return INVALID_SID;
	}

	
}