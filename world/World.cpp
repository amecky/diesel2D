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
#include "actions\ColorFlashAction.h"
#include "actions\RemoveAfterAction.h"
#include "actions\ScaleByPathAction.h"
#include "..\physics\ColliderArray.h"
#include "..\utils\Profiler.h"
#include "..\utils\Log.h"

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
		_scaleByPathAction = new ScaleByPathAction;
		m_Actions.push_back(_scaleByPathAction);
		_colorFlashAction = new ColorFlashAction;
		m_Actions.push_back(_colorFlashAction);
	}


	World::~World(void) {
		m_Actions.destroy_all();
		if ( m_Data.buffer != 0 ) {
			gDefaultMemory->deallocate(m_Data.buffer);
		}
		
	}

	// -----------------------------------------------------
	// allocate
	// -----------------------------------------------------
	void World::allocate(int size) {
		if (sar::allocate(m_Data, size)) {
			m_Physics.setDataPtr(&m_Data);
		}
	}

	// -----------------------------------------------------
	// create
	// -----------------------------------------------------
	SID World::create(const Vector2f& pos,const Texture& r,int type,int layer) {
		if ( m_Data.total == 0 ) {
			allocate(256);
		}
		if ( m_Data.num >= m_Data.total ) {
			allocate(m_Data.total * 2);
		}
		return sar::create(m_Data,pos,r,type,layer);		
	}

	// -----------------------------------------------------
	// create with all options
	// -----------------------------------------------------
	SID  World::create(const Vector2f& pos, const Texture& r, float rotation, float scaleX, float scaleY, const Color& color, int type, int layer) {
		if (m_Data.total == 0) {
			allocate(256);
		}
		if (m_Data.num >= m_Data.total) {
			allocate(m_Data.total * 2);
		}
		return m_Data.create(pos, r, rotation, scaleX, scaleY, color, type, layer);
	}

	// -----------------------------------------------------
	// create with template name
	// -----------------------------------------------------
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

	// -----------------------------------------------------
	// remove
	// -----------------------------------------------------
	void World::remove(SID id) {
		if (_buffer.contains(id)) {
			_buffer.remove(id);
		}
		m_Physics.remove(id);		
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->removeByID(id);
		}		
		sar::remove(m_Data,id);
	}

	// -----------------------------------------------------
	// render single layer
	// -----------------------------------------------------
	void World::renderSingleLayer(int layer) {
		ZoneTracker z("World:renderSingleLayer");
		renderer::selectViewport(_layers[layer].viewport_id);
		for (int i = 0; i < m_Data.num; ++i) {
			if (m_Data.layers[i] == layer) {
				sprites::draw(m_Data.positions[i], m_Data.textures[i], m_Data.rotations[i], m_Data.scales[i].x, m_Data.scales[i].y, m_Data.colors[i]);
			}
		}
	}

	// -----------------------------------------------------
	// render list of layers
	// -----------------------------------------------------
	void World::renderLayers(int* layers, int num_layers) {
		ZoneTracker z("World:renderLayers");
		for (int j = 0; j < num_layers; ++j) {
			renderer::selectViewport(_layers[layers[j]].viewport_id);
			for (int i = 0; i < m_Data.num; ++i) {
				if (m_Data.layers[i] == layers[j]) {					
					sprites::draw(m_Data.positions[i], m_Data.textures[i], m_Data.rotations[i], m_Data.scales[i].x, m_Data.scales[i].y, m_Data.colors[i]);
				}
			}
		}
	}

	// -----------------------------------------------------
	// render all layers
	// -----------------------------------------------------
	void World::render() {
		ZoneTracker z("World:render");
		int current_layer = -1;
		for ( int i = 0; i < m_Data.num; ++i ) {
			if (m_Data.layers[i] != current_layer) {
				current_layer = m_Data.layers[i];
				renderer::selectViewport(_layers[current_layer].viewport_id);
			}
			sprites::draw(m_Data.positions[i],m_Data.textures[i],m_Data.rotations[i],m_Data.scales[i].x,m_Data.scales[i].y,m_Data.colors[i]);
		}
	}

	// -----------------------------------------------------
	// tick
	// -----------------------------------------------------
	void World::tick(float dt) {
		ZoneTracker z("World:tick");
		m_Buffer.reset();
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->update(m_Data, dt, m_Buffer);
		}
		for (int i = 0; i < m_Buffer.events.size(); ++i) {
			const ActionEvent& e = m_Buffer.events[i];
			if (e.type == AT_KILL) {
				remove(e.sid);
			}
		}
		m_Physics.tick(dt);
	}

	void World::stopAction(SID sid, ActionType type) {
		for (size_t i = 0; i < m_Actions.size(); ++i) {
			if (m_Actions[i]->getActionType() == type) {
				m_Actions[i]->removeByID(sid);
			}
		}
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
		return 0;// m_MoveToAction->size();
	}

	void World::scaleByPath(SID id, Vector2fPath* path, float ttl) {
		_scaleByPathAction->attach(m_Data,id, path, ttl);
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

	// -----------------------------------------------------
	// rotate to
	// -----------------------------------------------------
	void World::rotateTo(SID sid, const Vector2f& target, float ttl, const tweening::TweeningType& tweeningType) {
		Vector2f n = normalize(target);
		float angle = vector::calculateRotation(n);
		rotateTo(sid, angle, ttl, 0, tweeningType);
	}

	// -----------------------------------------------------
	// remove after
	// -----------------------------------------------------
	void World::removeAfter(SID sid, float ttl) {
		_removeAfterAction->attach(sid, ttl);
	}

	void World::attachTo(SID sid, SID parent) {
		
	}

	// -----------------------------------------------------
	// fade alpha to
	// -----------------------------------------------------
	void World::fadeAlphaTo(SID sid,float startAlpha,float endAlpha,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		m_AlphaFadeToAction->attach(sid,startAlpha,endAlpha,ttl,mode,tweeningType);
	}

	// -----------------------------------------------------
	// fade color from A to A
	// -----------------------------------------------------
	void World::fadeColorTo(SID sid, const Color& startColor, const Color& endColor, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		m_ColorFadeToAction->attach(sid, startColor, endColor, ttl, mode, tweeningType);
	}

	// -----------------------------------------------------
	// flash color - fade from A to B to A
	// -----------------------------------------------------
	void World::flashColor(SID sid, const Color& startColor, const Color& endColor, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		_colorFlashAction->attach(sid, startColor, endColor, ttl, mode, tweeningType);
	}

	// -----------------------------------------------------
	// follow cubic bezier path
	// -----------------------------------------------------
	void World::followPath(SID sid,CubicBezierPath* path,float ttl,int mode) {
		m_FollowPathAction->attach(sid,path,ttl,mode);
	}

	// -----------------------------------------------------
	// follow path
	// -----------------------------------------------------
	void World::followPath(SID sid, StraightPath* path, float ttl, int mode) {
		m_FollowStraightPathAction->attach(sid,m_Data, path, ttl, mode);
	}

	// -----------------------------------------------------
	// follow curve
	// -----------------------------------------------------
	void World::followCurve(SID sid,BezierCurve* path,float ttl,int mode) {
		m_FollowCurveAction->attach(sid,path,ttl,mode);
	}

	// -----------------------------------------------------
	// follow target
	// -----------------------------------------------------
	void World::followTarget(SID sid,SID target,float velocity) {
		m_FollowTargetAction->attach(sid, target, velocity);
	}

	// -----------------------------------------------------
	// wait
	// -----------------------------------------------------
	void World::wait(SID sid,float ttl) {
		m_WaitAction->attach(sid,ttl);
	}

	// -----------------------------------------------------
	// clear
	// -----------------------------------------------------
	void World::clear() {
		sar::clear(m_Data);
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->clear();
		}		
	}

	// -----------------------------------------------------
	// get type
	// -----------------------------------------------------
	int World::getType(SID sid) const {
		return m_Data.getType(sid);
	}

	// -----------------------------------------------------
	// find by type
	// -----------------------------------------------------
	int World::find_by_type(int type, SID* ids, int max) const {
		int cnt = 0;
		for (int i = 0; i < m_Data.num; ++i) {
			if (m_Data.types[i] == type && cnt < max) {
				ids[cnt++] = m_Data.ids[i];
			}
		}
		return cnt;
	}

	// -----------------------------------------------------
	// get count 
	// -----------------------------------------------------
	int World::get_count(int type) {
		int cnt = 0;
		for (int i = 0; i < m_Data.num; ++i) {
			if (m_Data.types[i] == type) {
				++cnt;
			}
		}
		return cnt;
	}

	// -----------------------------------------------------
	// find by type
	// -----------------------------------------------------
	int World::find_by_types(int* types, int num, SID* ids, int max) const {
		int cnt = 0;
		for (int j = 0; j < num; ++j) {
			for (int i = 0; i < m_Data.num; ++i) {
				if (m_Data.types[i] == types[j] && cnt < max) {
					ids[cnt++] = m_Data.ids[i];
				}
			}
		}
		return cnt;
	}

	// -----------------------------------------------------
	// set bounding rect
	// -----------------------------------------------------
	void World::setBoundingRect(const Rect& r) {
		// we need to switch y
		Rect newRect;
		newRect.left = r.left;
		newRect.right = r.right;
		newRect.top = r.bottom;
		newRect.bottom = r.top;
		for (size_t i = 0; i < m_Actions.size(); ++i) {
			m_Actions[i]->setBoundingRect(newRect);
		}
	}

	// -----------------------------------------------------
	// debug
	// -----------------------------------------------------
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

	// -----------------------------------------------------
	// save report
	// -----------------------------------------------------
	void World::save(const ReportWriter& writer) {
		writer.startBox("Sprites");
		const char* HEADERS[] = { "Index", "ID", "Type", "Position", "Rotation", "Scale" };
		writer.startTable(HEADERS, 6);
		for (int i = 0; i < m_Data.num; ++i) {
			writer.startRow();
			writer.addCell(i);
			writer.addCell(m_Data.ids[i]);
			writer.addCell(m_Data.types[i]);
			writer.addCell(m_Data.positions[i]);
			writer.addCell(RADTODEG(m_Data.rotations[i]));
			writer.addCell(m_Data.scales[i]);
			writer.endRow();
		}
		writer.endTable();
		writer.endBox();
		writer.startBox("Actions");
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			m_Actions[i]->save(writer);
		}
		writer.endBox();
		m_Physics.save(writer);
	}

	void World::debug(SID sid) {		
		sar::debug(m_Data, sid);
		if (_buffer.contains(sid)) {
			LOG << "Additional data found";
		}
		m_Physics.debug(sid);
		for ( size_t i = 0; i < m_Actions.size(); ++i ) {
			if ( m_Actions[i]->contains(sid)) {
				m_Actions[i]->debug(sid);
			}
		}
	}

	// -----------------------------------------------------
	// pick
	// -----------------------------------------------------
	SID World::pick(const Vector2f& pos) {
		for ( int i = 0; i < m_Data.num; ++i ) {
			const v2& p = m_Data.positions[i];
			// transform to screen space
			v2 correct = renderer::world_to_screen(p, _layers[m_Data.layers[i]].viewport_id);
			Texture& t = m_Data.textures[i];
			float dx = t.rect.width() / 2.0f;
			float dy = t.rect.height() / 2.0f;
			if (physics::isInsideBox(pos, correct, v2(dx,dy))) {
				return m_Data.ids[i];
			}
		}
		return INVALID_SID;
	}

	// -----------------------------------------------------
	// attach viewport
	// -----------------------------------------------------
	void World::attachViewport(int layer, int viewport_id) {
		if (layer >= 0 && layer < 32) {
			_layers[layer].viewport_id = viewport_id;
		}
	}

	// -----------------------------------------------------
	// activate layer
	// -----------------------------------------------------
	void World::activateLayer(int layer) {
		if (layer >= 0 && layer < 32) {
			_layers[layer].visible = true;
		}
	}

	// -----------------------------------------------------
	// deactivate layer
	// -----------------------------------------------------
	void World::deactivateLayer(int layer) {
		if (layer >= 0 && layer < 32) {
			_layers[layer].visible = false;
		}
	}

	
}