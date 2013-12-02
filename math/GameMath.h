#pragma once
#include "..\dxstdafx.h"
#include "..\utils\Color.h"
#include "math_types.h"
#include "vector.h"

namespace ds {


	namespace math {

		float norm(float t,float max);
		bool addClamped(float *value,float add,float max,float newValue = 0.0f);
		float saturate(float x);
		float smoothstep(float min,float max,float x);
		float smoothstep(float x);
		void clamp(int* value,int min,int max);
		void clamp(float* value,float min,float max);
		float clamp(float value,float min = 0.0f,float max = 1.0f);
		Vec2 getRadialVelocity(float angle,float vel);
		Vec2 getTargetVelocity(const Vec2& targetPosition,const Vec2& currentPosition,float* angle,float vel);
		float getAngle(float x1,float y1,float x2,float y2);
		float getAngle(const Vec2& v1,const Vec2& v2);
		float getTargetAngle(const Vec2& v1,const Vec2& v2);
		Vec2 getDistantPosition(float x,float y,float angle,float radius);
		Vec2 getDistantPosition(const Vec2& initialPosition,float angle,float radius);
		float impulse(float k,float x );
		float cubicPulse( float c, float w, float x );
		float almostIdentity( float x, float m, float n );
		void transformMousePosToWorld(Vec3* vPickRayDir,Vec3* vPickRayOrig);
		void getTextureCoordinates(const Rect& textureRect,int textureSize,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel = true);
		void getTextureCoordinates(const Rect& textureRect,int textureWidth,float textureHeight,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel);
		void follow(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float add);
		void followRelative(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float percentage);
		void clipToRect(Vec2& newPos,const Rect& rect);
		bool isOutside(Vec2& pos,const Rect& rect);
		float reflect(float angle);
		void interpolateColor(const Color& firstCol,const Color& secondColor,float t,Color* outColor);
		bool outside(float value,float min,float max);

		void clamp(Vec2& v,float minX,float maxX,float minY,float maxY);
		// -------------------------------------------------------
		// 
		// -------------------------------------------------------
		bool checkLineCircle(const Vec2& center, float radius,const Vec2& lineFrom,const Vec2& lineTo);
		// -------------------------------------------------------
		// Get random float between min and max
		// -------------------------------------------------------
		float random(float min,float max);
		// -------------------------------------------------------
		// Get random int between min and max
		// -------------------------------------------------------
		int random(int min,int max);
		// -------------------------------------------------------
		// Check if two circles overlap
		// -------------------------------------------------------
		bool checkCircleIntersection(const Vec2& p1,float r1,const Vec2& p2,float r2);
		// ------------------------------------------------------------------------------------------
		// Check if two circles overlap also calculating penetration distance and penetration vector
		// ------------------------------------------------------------------------------------------
		bool checkCircleIntersection(const Vec2& p1,float r1,const Vec2& p2,float r2,float* dist,Vec2& penVec);

		Vec2 getShiftVector(const Vec2& p1,float r1,const Vec2& p2,float r2);

		bool solveQuadraticFormula(const float a,const float b,const float c,float* r1,float* r2);

		bool circleSweepTest(const Vec2& a0,const Vec2& a1,float ra,const Vec2& b0,const Vec2& b1,float rb,float* u0,float* u1);			

		struct TexCoord {

			float u1;
			float u2;
			float v1;
			float v2;

			TexCoord() {
				u1 = v1 = 0.0f;
				u2 = v2 = 1.0f;
			}	
			void scale(const Rect& rect,int size) {
				float dim = (float)size;
				u1 = static_cast<float>(rect.left) / static_cast<float>(dim);
				u2 = static_cast<float>(rect.right) / static_cast<float>(dim);
				v1 = static_cast<float>(rect.top) / static_cast<float>(dim);
				v2 = static_cast<float>(rect.bottom) / static_cast<float>(dim);
				math::getTextureCoordinates(rect,size,&u1,&v1,&u2,&v2,true);
			}
		};

	}
}
