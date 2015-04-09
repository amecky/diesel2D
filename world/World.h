#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"
#include <map>
#include "..\sprites\Sprite.h"
#include "..\sprites\SpriteArray.h"
#include "..\physics\ColliderArray.h"
#include "..\math\tweening.h"
#include "..\math\CubicBezierPath.h"
#include <vector>

namespace ds {

	enum ActionType {
		AT_MOVE_TO,
		AT_MOVE_BY,
		AT_FOLLOW_PATH,
		AT_FOLLOW_CURVE,
		AT_WAIT,
		AT_MOVE_WITH,
		AT_ROTATE
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

	typedef void (*MoveFunc)(Vector2f&,float*,float);

	struct ActionEvent {

		SID sid;
		ActionType type;
		int spriteType;
	};

	struct ActionEventBuffer {


		ActionEvent events[256];
		int num;

		void reset() {
			num = 0;
		}

		void add(SID sid,ActionType type,int spriteType) {
			ActionEvent& ae = events[num++];
			ae.sid = sid;
			ae.type = type;
			ae.spriteType = spriteType;
		}
	};

	class World {

	typedef std::vector<AbstractAction*> Actions;

	typedef std::map<SID,CID> ColliderMap;

	public:
		World();
		~World(void);
		SID create(const Vector2f& pos,const Texture& r,int type = -1);
		void remove(SID sid);
		void render();

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
		// scaling
		void scale(SID sid,float sx,float sy) {
			sar::setScale(m_Data,sid,sx,sy);
		}
		void scale(int index,float sx,float sy) {
			m_Data.scales[index] = Vector2f(sx,sy);
		}
		void scaleTo(SID sid,const Vector2f& startScale,const Vector2f& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);

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

		void setColor(SID sid,const Color& clr) {
			sar::setColor(m_Data,sid,clr);
		}

		void followPath(SID sid,CubicBezierPath* path,float ttl,int mode = 0);

		void followCurve(SID sid,BezierCurve* path,float ttl,int mode = 0);

		void followTarget(SID sid,float velocity, Vector2f* pos);

		void fadeAlphaTo(SID sid,float startAlpha,float endAlpha,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);

		void wait(SID sid,float ttl);

		void debug();

		void debug(SID sid);

		const ActionEventBuffer& getEventBuffer() const {
			return m_Buffer;
		}

		void attachCollider(SID sid,const Vector2f& extent,int type);
		bool hasCollisions() {
			return m_NumCollisions > 0;
		}
		int getNumCollisions() {
			return m_NumCollisions;
		}
		const Collision& getCollision(int idx) const {
			return m_Collisions[idx];
		}

		void drawColliders(const Texture& texture);

		SID pick(const Vector2f& pos);

		const SpriteArray& getSpriteArray() const {
			return m_Data;
		}
	private:
		ActionEventBuffer m_Buffer;
		void allocate(int size);
		void allocateCollider(int size);
		void checkCollisions();
		void checkCollisions(int currentIndex,const Vector2f& pos,const Vector2f& extent);
		bool containsCollision(CID firstID,CID secondID);

		SpriteArray m_Data;
		ColliderArray m_ColliderData;
		ColliderMap m_ColliderMap;
		Collision m_Collisions[256];
		int m_NumCollisions;
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
		Actions m_Actions;

	};

}