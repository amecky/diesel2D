#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"
#include "..\lib\collection_types.h"
#include <map>
#include "..\sprites\Sprite.h"
#include "..\sprites\SpriteArray.h"
#include "..\physics\ColliderArray.h"
#include "..\physics\PhysicalWorld.h"
#include "..\math\tweening.h"
#include "..\math\CubicBezierPath.h"
#include "..\math\StraightPath.h"
#include "..\math\FloatArray.h"

namespace ds {



	enum ActionType {
		AT_ALPHA_FADE_TO,
		AT_COLOR_FADE_TO,
		AT_MOVE_TO,
		AT_MOVE_BY,
		AT_FOLLOW_PATH,
		AT_FOLLOW_CURVE,
		AT_FOLLOW_TARGET,
		AT_REMOVE_AFTER,
		AT_WAIT,
		AT_MOVE_WITH,
		AT_ROTATE,
		AT_ROTATE_BY,
		AT_FOLLOW_STRAIGHT_PATH,
		AT_KILL,
		AT_SCALE_BY_PATH,
		AT_SCALE,
		AT_COLOR_FLASH
	};

	enum RepeatType {
		RT_ONCE,
		RT_FORVER,
		RT_REPEATED
	};

	class AbstractAction;
	class MoveToAction;
	class ScalingAction;
	class AlphaFadeToAction;
	class RotateByAction;
	class FollowPathAction;
	class FollowCurveAction;
	class MoveByAction;
	class WaitAction;
	class MoveWithAction;
	class RotateAction;
	class FollowTargetAction;
	class FollowStraightPathAction;
	class ColorFadeToAction;
	class RemoveAfterAction;
	class ScaleByPathAction;
	class ColorFlashAction;

	typedef void (*MoveFunc)(Vector2f&,float*,float);

	struct ActionEvent {

		SID sid;
		ActionType type;
		int spriteType;
	};

	struct ActionEventBuffer {

		Array<ActionEvent> events;

		void reset() {
			events.clear();
		}

		void add(SID sid,ActionType type,int spriteType) {
			ActionEvent e;
			e.sid = sid;
			e.type = type;
			e.spriteType = spriteType;
			events.push_back(e);
		}
	};

	struct WorldLayer {

		int viewport_id;
		bool visible;

		WorldLayer() : viewport_id(0), visible(true) {}
	};

	class World {

		struct AdditionalDataHeader {
			int index;
			int size;
			bool used;
			SID sid;
		};

		struct AdditionalData {

			void* attach(SID sid, int size) {
				int idx = find_free_header(size);
				if (idx == -1) {
					AdditionalDataHeader header;
					header.sid = sid;
					header.size = size;
					header.index = data.size;
					header.used = true;
					headers.push_back(header);
					return data.alloc(size);
				}
				else {
					AdditionalDataHeader& header = headers[idx];
					header.used = true;
					header.sid = sid;
					return data.data + header.index;
				}
			}

			void remove(SID sid) {
				int idx = find_header(sid);
				if (idx != -1) {
					AdditionalDataHeader& header = headers[idx];
					if (header.used) {
						header.used = false;
					}
				}
			}

			void* get(SID sid) {
				int idx = find_header(sid);
				if (idx != -1) {
					AdditionalDataHeader& header = headers[idx];
					if (header.used) {
						return data.data + header.index;
					}
				}
				return 0;
			}

			int find_free_header(int size) {
				for (uint32 i = 0; i < headers.size(); ++i) {
					const AdditionalDataHeader& h = headers[i];
					if (h.size == size && !h.used) {
						return i;
					}
				}
				return -1;
			}

			int find_header(SID sid) {
				for (uint32 i = 0; i < headers.size(); ++i) {
					const AdditionalDataHeader& h = headers[i];
					if (h.sid == sid) {
						return i;
					}
				}
				return -1;
			}

			bool contains(SID sid) {
				int idx = find_header(sid);
				if (idx != -1) {
					AdditionalDataHeader& header = headers[idx];
					return header.used;
				}
				return false;
			}

			CharBuffer data;
			Array<AdditionalDataHeader> headers;
		};

	typedef Array<AbstractAction*> Actions;

	public:
		World();
		~World(void);
		SID create(const Vector2f& pos, const Texture& r, int type = -1, int layer = 0);
		SID create(const Vector2f& pos, const char* templateName, int layer = 0);
		SID create(const Vector2f& pos, const Texture& r,float rotation, float scaleX, float scaleY,const Color& color = Color::WHITE,int type = -1, int layer = 0);
		void remove(SID sid);
		void render();
		void renderLayers(int* layers,int num_layers);
		void renderSingleLayer(int layer);

		void setBoundingRect(const Rect& r);

		const bool contains(SID sid) const {
			return m_Data.contains(sid);
		}
		void tick(float dt);

		//
		void setPosition(SID sid,const Vector2f& pos) {
			sar::setPosition(m_Data,sid,pos);
		}
		void setTexture(SID sid,const Texture& t) {
			SpriteArrayIndex &in = m_Data.indices[sid];
			assert(in.index != USHRT_MAX);
			m_Data.textures[in.index] = t;
		}
		const Texture& getTexture(SID sid) const {
			SpriteArrayIndex &in = m_Data.indices[sid];
			assert(in.index != USHRT_MAX);
			return m_Data.textures[in.index];
		}
		// scaling
		void scale(SID sid,float sx,float sy) {
			sar::setScale(m_Data,sid,sx,sy);
		}
		void scale(int index,float sx,float sy) {
			m_Data.scales[index] = Vector2f(sx,sy);
		}
		void scaleTo(SID sid,const Vector2f& startScale,const Vector2f& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void scaleByPath(SID id, Vector2fPath* path, float ttl);
		void clear();

		const int size() const {
			return m_Data.num;
		}

		void moveTo(SID sid,const Vector2f& startPos,const Vector2f& endPos,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		const int getMovingNumber() const;
		void moveBy(SID sid,const Vector2f& velocity,bool bounce = false);
		const Vector2f& getPosition(SID sid) const {
			return sar::getPosition(m_Data,sid);
		}
		void moveWith(SID sid,const MoveFunc& function,float ttl);


		void setRotation(SID sid,float angle);
		void setRotation(SID sid, const Vector2f& target);
		void rotateBy(SID sid,float angle,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void rotateTo(SID sid,float angle,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void rotateTo(SID sid, const Vector2f& target, float ttl, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void rotate(SID sid, float angleVelocity, float ttl,int mode = 0);

		void removeAfter(SID sid, float ttl);

		void attachTo(SID sid, SID parent);

		void setColor(SID sid,const Color& clr) {
			sar::setColor(m_Data,sid,clr);
		}

		void followPath(SID sid,CubicBezierPath* path,float ttl,int mode = 0);

		void followPath(SID sid, StraightPath* path, float ttl, int mode = 0);

		void followCurve(SID sid,BezierCurve* path,float ttl,int mode = 0);

		void followTarget(SID sid,SID target, float velocity);

		void fadeAlphaTo(SID sid,float startAlpha,float endAlpha,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);

		void fadeColorTo(SID sid, const Color& startColor, const Color& endColor, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);

		void flashColor(SID sid, const Color& startColor, const Color& endColor, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		
		void wait(SID sid,float ttl);

		void debug();

		void save(const ReportWriter& writer);

		void debug(SID sid);

		void stopAction(SID sid, ActionType type);

		const ActionEventBuffer& getEventBuffer() const {
			return m_Buffer;
		}

		int getType(SID sid) const;

		int find_by_type(int type, SID* ids, int max) const;

		int find_by_types(const int* types, int num, SID* ids, int max) const;

		int get_count(int type);

		void attachCollider(SID sid, const Vector2f& extent, int type,int layer) {
			m_Physics.attachCollider(sid, extent, type,layer);
		}

		void attachCollider(SID sid, int type, int layer){
			m_Physics.attachCollider(sid, type, layer);
		}

		void ignoreCollisions(int firstType, int secondType) {
			m_Physics.ignore(firstType, secondType);
		}

		void ignoreLayer(int layer) {
			m_Physics.ignoreLayer(layer);
		}

		bool hasCollisions() {
			return m_Physics.hasCollisions();
		}
		int getNumCollisions() {
			return m_Physics.getNumCollisions();
		}
		const Collision& getCollision(int idx) const {
			return m_Physics.getCollision(idx);
		}

		void drawColliders(const Texture& texture) {
			m_Physics.drawColliders(texture);
		}

		SID pick(const Vector2f& pos);

		const SpriteArray& getSpriteArray() const {
			return m_Data;
		}

		void* attach_data(SID sid,int size) {
			return _buffer.attach(sid, size);
		}

		void* get_data(SID sid) {
			return _buffer.get(sid);
		}

		void attachViewport(int layer, int viewport_id);

		void activateLayer(int layer);

		void deactivateLayer(int layer);

	private:
		ActionEventBuffer m_Buffer;
		void allocate(int size);
		
		WorldLayer _layers[32];

		SpriteArray m_Data;
		AdditionalData _buffer;

		PhysicalWorld m_Physics;

		MoveToAction* m_MoveToAction;	
		ScalingAction* m_ScalingAction;
		AlphaFadeToAction* m_AlphaFadeToAction;
		RotateByAction* m_RotateByAction;
		FollowPathAction* m_FollowPathAction;
		FollowCurveAction* m_FollowCurveAction;
		MoveByAction* m_MoveByAction;
		WaitAction* m_WaitAction;
		MoveWithAction* m_MoveWithAction;
		RotateAction* m_RotateAction;
		FollowTargetAction* m_FollowTargetAction;
		FollowStraightPathAction* m_FollowStraightPathAction;
		ColorFadeToAction* m_ColorFadeToAction;
		RemoveAfterAction* _removeAfterAction;
		ScaleByPathAction* _scaleByPathAction;
		ColorFlashAction* _colorFlashAction;
		Actions m_Actions;

	};

}