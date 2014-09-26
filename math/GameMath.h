#pragma once
#include "..\dxstdafx.h"
#include "..\utils\Color.h"
#include "math_types.h"
#include <Vector.h>

Vector2f rotate(float angle,float vel);

namespace ds {


	namespace math {

		float hill(float x);
		float fastSin(float x);
		float fastCos(float x);

		float norm(float t,float max);
		bool addClamped(float *value,float add,float max,float newValue = 0.0f);
		float saturate(float x);
		float smoothstep(float min,float max,float x);
		float smoothstep(float x);
		void clamp(int* value,int min,int max);
		void clamp(float* value,float min,float max);
		float clamp(float value,float min = 0.0f,float max = 1.0f);
		Vector2f getRadialVelocity(float angle,float vel);
		Vector2f getTargetVelocity(const Vector2f& targetPosition,const Vector2f& currentPosition,float* angle,float vel);
		float getAngle(float x1,float y1,float x2,float y2);
		float getAngle(const Vector2f& v1,const Vector2f& v2);
		float getTargetAngle(const Vector2f& v1,const Vector2f& v2);
		Vector2f getDistantPosition(float x,float y,float angle,float radius);
		Vector2f getDistantPosition(const Vector2f& initialPosition,float angle,float radius);
		float impulse(float k,float x );
		float cubicPulse( float c, float w, float x );
		float almostIdentity( float x, float m, float n );
		void transformMousePosToWorld(Vector3f* vPickRayDir,Vector3f* vPickRayOrig);

		// texture coordinates
		Vector4f getTextureCoordinates(const Rect& textureRect,float textureWidth = 1024.0f,float textureHeight = 1024.0f,bool useHalfTexel = true);
		void getTextureCoordinates(const Rect& textureRect,int textureSize,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel = true);
		void getTextureCoordinates(const Rect& textureRect,int textureWidth,float textureHeight,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel = true);
		Vector4f getTextureCoordinates(float top,float left,float width,float height,float textureWidth = 1024.0f,float textureHeight = 1024.0f,bool useHalfTexel = true);

		void follow(const Vector2f& targetPos,Vector2f& newPos,float* angle,float distance,float add);
		void followRelative(const Vector2f& targetPos,Vector2f& newPos,float* angle,float distance,float percentage);
		void clipToRect(Vector2f& newPos,const Rect& rect);
		bool isOutside(Vector2f& pos,const Rect& rect);
		float reflect(float angle);
		//void interpolateColor(const Color& firstCol,const Color& secondColor,float t,Color* outColor);
		bool outside(float value,float min,float max);

		void clamp(Vector2f& v,float minX,float maxX,float minY,float maxY);
		// -------------------------------------------------------
		// 
		// -------------------------------------------------------
		bool checkLineCircle(const Vector2f& center, float radius,const Vector2f& lineFrom,const Vector2f& lineTo);
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
		bool checkCircleIntersection(const Vector2f& p1,float r1,const Vector2f& p2,float r2);
		// ------------------------------------------------------------------------------------------
		// Check if two circles overlap also calculating penetration distance and penetration vector
		// ------------------------------------------------------------------------------------------
		bool checkCircleIntersection(const Vector2f& p1,float r1,const Vector2f& p2,float r2,float* dist,Vector2f& penVec);

		Vector2f getShiftVector(const Vector2f& p1,float r1,const Vector2f& p2,float r2);

		bool solveQuadraticFormula(const float a,const float b,const float c,float* r1,float* r2);

		bool circleSweepTest(const Vector2f& a0,const Vector2f& a1,float ra,const Vector2f& b0,const Vector2f& b1,float rb,float* u0,float* u1);			

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
