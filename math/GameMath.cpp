#include "GameMath.h"
#include "..\renderer\Camera.h"
#include "..\utils\mtrand.h"
#include "..\utils\Log.h"

// ---------------------------------------------
//
// ---------------------------------------------

namespace ds {

	namespace math {

	void clamp(int* value,int min,int max) {
		if ( *value > max ) {
			*value = max;
		}
		if ( *value < min ) {
			*value = min;
		}
	}

	void clamp(float* value,FLOAT min,float max) {
		if ( *value > max ) {
			*value = max;
		}
		if ( *value < min ) {
			*value = min;
		}
	}

	float clamp(float value,float min,float max) {
		if ( value < min ) {
			return min;
		}
		if ( value > max ) {
			return max;
		}
		return value;
	}

	void getTextureCoordinates(const Rect& textureRect,int textureSize,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel) {
		if ( useHalfTexel ) {
			float halfTexel = 0.5f;
			float const width   = textureSize;
			float const height  = textureSize;

			float kUOffset = halfTexel/width;
			float kVOffset = halfTexel/height;

			*u1 = static_cast<float>(textureRect.left)/width  + kUOffset;
			*v1 = static_cast<float>(textureRect.top)/height + kVOffset;  

			*u2 = *u1 + static_cast<float>(textureRect.width()) /width   - 2.0f*kUOffset;
			*v2 = *v1 + static_cast<float>(textureRect.height())/height  - 2.0f*kVOffset;
		}
		else {
			float size = static_cast<float>(textureSize);
			*u1 = static_cast<float>(textureRect.left)/size;
			*u2 = static_cast<float>(textureRect.right)/size;
			*v1 = static_cast<float>(textureRect.top)/size;
			*v2 = static_cast<float>(textureRect.bottom)/size;
		}
	}

	void getTextureCoordinates(const Rect& textureRect,int textureWidth,float textureHeight,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel) {
		if ( useHalfTexel ) {
			float halfTexel = 0.5f;
			float const width   = textureWidth;
			float const height  = textureHeight;

			float kUOffset = halfTexel/width;
			float kVOffset = halfTexel/height;

			*u1 = static_cast<float>(textureRect.left)/width  + kUOffset;
			*v1 = static_cast<float>(textureRect.top)/height + kVOffset;  

			*u2 = *u1 + static_cast<float>(textureRect.width()) /width   - 2.0f*kUOffset;
			*v2 = *v1 + static_cast<float>(textureRect.height())/height  - 2.0f*kVOffset;
		}
		else {
			*u1 = static_cast<float>(textureRect.left)/textureWidth;
			*u2 = static_cast<float>(textureRect.right)/textureWidth;
			*v1 = static_cast<float>(textureRect.top)/textureHeight;
			*v2 = static_cast<float>(textureRect.bottom)/textureHeight;
		}
	}

	void transformMousePosToWorld(Vec3* vPickRayDir,Vec3* vPickRayOrig) {
		// Get the Pick ray from the mouse position
		/*
		Camera *camera = gEngine->getRenderer()->getCamera();
		const D3DXMATRIX* pmatProj = &camera->getProjectionMatrix();

		ds::Vec3 mp = ds::Vec3(0,0,0);
		mp.x = gEngine->getMousePosX();
		mp.y = gEngine->getMousePosY();

		// Compute the vector of the Pick ray in screen space
		D3DXVECTOR3 v;
		v.x = ( ( ( 2.0f * mp.x ) / gEngine->getWidth() ) - 1 ) / pmatProj->_11;
		v.y = -( ( ( 2.0f * mp.y ) / gEngine->getHeight() ) - 1 ) / pmatProj->_22;
		v.z = 1.0f;

		// Get the inverse view matrix
		const D3DXMATRIX matView = camera->getViewMatrix();
		const D3DXMATRIX matWorld = gEngine->getRenderer()->getWorldMatrix();
		D3DXMATRIX mWorldView = matWorld * matView;
		D3DXMATRIX m;
		D3DXMatrixInverse( &m, NULL, &mWorldView );

		// Transform the screen space Pick ray into 3D space
		vPickRayDir->x = v.x * m._11 + v.y * m._21 + v.z * m._31;
		vPickRayDir->y = v.x * m._12 + v.y * m._22 + v.z * m._32;
		vPickRayDir->z = v.x * m._13 + v.y * m._23 + v.z * m._33;

		vPickRayOrig->x = gEngine->getRenderer()->getCamera()->getPosition().x;
		vPickRayOrig->y = gEngine->getRenderer()->getCamera()->getPosition().y;
		vPickRayOrig->z = gEngine->getRenderer()->getCamera()->getPosition().z;
		*/
	}
	// ---------------------------------------------
	// Returns a smooth value between 0.0 and 1.0
	// ---------------------------------------------
	float saturate(float x) {
		if ( x < 0.0f ) {
			return 0.0f;
		}
		else if ( x > 1.0f ) {
			return 1.0f;
		}
		return x;
	}

	float smoothstep(float min,float max,float x) {
		clamp(&x,0.0f,1.0f);	
		float sp =  x * x * ( 3.0f - 2.0f * x);
		return min + sp * (max - min);
	}

	float smoothstep(float x) {
		clamp(&x,0.0f,1.0f);	
		float sp =  x * x * ( 3.0f - 2.0f * x);
		return sp;
	}

	float cubicPulse( float c, float w, float x ) {
		x = fabsf(x - c);
		if( x>w ) return 0.0f;
		x /= w;
		return 1.0f - x*x*(3.0f-2.0f*x);
	}

	float impulse( float k, float x ) {
		const float h = k*x;
		return h*expf(1.0f-h);
	}

	float almostIdentity( float x, float m, float n ) {
		if( x>m ) return x;
		const float a = 2.0f*n - m;
		const float b = 2.0f*m - 3.0f*n;
		const float t = x/m;
		return (a*t + b)*t*t + n;
	}

	// -------------------------------------------------------
	// Calculates the radial velocity. It is calculated 
	// counter clock wise
	//
	// [in] angle - angle in degrees
	// [in] velocity - the velocity
	// [out] Vec2 - a 2D vector containing the velocity
	// -------------------------------------------------------

	Vec2 getRadialVelocity(float angle,float velocity) {
		float vx = (float)cos(DEGTORAD(angle))*velocity;
		float vy = (float)sin(DEGTORAD(angle))*velocity;
		return Vec2(vx,vy);
	}

	float getAngle(float x1,float y1,float x2,float y2) {
		double x = x2 - x1;
		double y = y2 - y1;	
		double ang = atan2(y,x);
		return (float)ang;
	}

	float getAngle(const Vec2& v1,const Vec2& v2) {	
		if ( v1 != v2 ) {
			Vec2 vn1 = vector::normalize(v1);
			Vec2 vn2 = vector::normalize(v2);
			//LOG(logINFO) << "vn1 " << DBG_V2(vn1) << " vn2 " << DBG_V2(vn2);
			float dot = vector::dot(vn1,vn2);		
			//LOG(logINFO) << "dot " << dot;
			if ( dot < -1.0f ) {
				dot = -1.0f;
			}
			if ( dot > 1.0f ) {
				dot = 1.0f;
			}
			float tmp = acos(dot);
			//LOG(logINFO) << "acos " << RADTODEG(tmp);
			float cross = vector::cross(vn1,vn2);
			//LOG(logINFO) << "cross " << cross;
			if ( cross < 0.0f ) {
				tmp = TWO_PI - tmp;
			}
			//LOG(logINFO) << "new acos " << RADTODEG(tmp);
			return tmp;		
		}
		else {
			return 0.0f;
		}
	}

	float getTargetAngle(const Vec2& v1,const Vec2& v2) {	
		Vec2 diff = v2 - v1;
		//LOG(logINFO) << "diff " << DBG_V2(diff);
		float angle = getAngle(diff,V2_RIGHT);// + PI;// + D3DX_PI;
		if ( angle >= TWO_PI ) {
			angle -= TWO_PI;
		}
		return angle;
	}

	Vec2 getDistantPosition(const Vec2& initialPosition,float angle,float radius) {
		return getDistantPosition(initialPosition.x,initialPosition.y,angle,radius);
	}

	Vec2 getDistantPosition(float x,float y,float angle,float radius) {
		Vec2 ret;
		ret.x = x + cos(angle) * radius;
		ret.y = y + sin(angle) * radius;
		return ret;
	}

	Vec2 getTargetVelocity(const Vec2& targetPosition,const Vec2& currentPosition,float* angle,float vel) {
		Vec2 diff = targetPosition - currentPosition;
		*angle = getAngle(diff,V2_RIGHT);
		if ( *angle < 0.0f ) {
			*angle += 2.0f * D3DX_PI;
		}
		float correctedAngle = reflect(*angle);
		float vx = cos(correctedAngle) * vel;
		float vy = sin(correctedAngle) * vel;
		return Vec2(vx,vy);
	}

	void follow(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float add) {
		Vec2 p = newPos;
		Vec2 diff = targetPos - p;
		//*angle = 2.0f * D3DX_PI - getAngle(diff,V2_RIGHT);
		*angle = getAngle(diff,V2_RIGHT);
		if ( *angle < 0.0f ) {
			*angle += 2.0f * D3DX_PI;
		}
		float dist = vector::distance(targetPos,p);
		if ( dist > distance ) {				
			vector::addScaled(newPos,diff,add);
		}
	}

	void followRelative(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float percentage) {
		Vec2 p = newPos;
		Vec2 diff = targetPos - p;
		*angle = getAngle(diff,V2_RIGHT);
		if ( *angle < 0.0f ) {
			*angle += 2.0f * D3DX_PI;
		}
		float dist = vector::distance(targetPos,p);
		if ( dist > distance ) {				
			newPos.x = newPos.x + diff.x * percentage;
			newPos.y = newPos.y + diff.y * percentage;		
		}
	}

	void clipToRect(Vec2& newPos,const Rect& rect) {
		if ( newPos.x < rect.left ) {
			newPos.x = rect.left;
		}
		if ( newPos.x > rect.right ) {
			newPos.x = rect.right;
		}
		if ( newPos.y < rect.top ) {
			newPos.y = rect.top;
		}
		if ( newPos.y > rect.bottom ) {
			newPos.y = rect.bottom;
		}
	}

	bool isOutside(Vec2& pos,const Rect& rect) {
		bool ret = false;
		if ( pos.x > rect.right ) {
			ret = true;
		}
		if ( pos.x < rect.left ) {
			ret = true;
		}
		if ( pos.y > rect.bottom ) {
			ret = true;
		}
		if ( pos.y < rect.top ) {
			ret = true;
		}
		return ret;
	}

	bool addClamped(float *value,float add,float max,float newValue) {
		*value += add;
		if ( *value >= max ) {
			*value = newValue;
			return true;
		}
		return false;
	}

	float reflect(float angle) {
		float newAngle = 2.0f * D3DX_PI - angle;
		if ( newAngle < 0.0f ) {
			newAngle += 2.0f * D3DX_PI;
		}
		return newAngle;
	}

	float norm(float t,float max) {
		float r = t/max;
		return r;
	}

	void interpolateColor(const Color& firstColor,const Color& secondColor,float t,Color* outColor) {
		float time = clamp(t);		
		outColor->r = firstColor.r * (1.0f - time) + secondColor.r * time;
		outColor->g = firstColor.g * (1.0f - time) + secondColor.g * time;
		outColor->b = firstColor.b * (1.0f - time) + secondColor.b * time;
		outColor->a = firstColor.a * (1.0f - time) + secondColor.a * time;
	}

	bool outside(float value,float min,float max) {
		bool ret = false;
		if ( value < min || value > max ) {
			ret = true;
		}
		return ret;
	}

		// -------------------------------------------------------
		// random
		// -------------------------------------------------------
		float random(float min,float max) {
			MTRand_open rand;
			//rand.seed(GetTickCount());
			return min + (max - min)* (float)rand();
		}

		// -------------------------------------------------------
		// 
		// -------------------------------------------------------
		bool checkLineCircle(const Vec2& center, float radius,const Vec2& lineFrom,const Vec2& lineTo) {
			Vec2 ac = center - lineFrom;
			Vec2 ab = lineTo - lineFrom;
			float ab2 = vector::dot(ab,ab);
			float acab = vector::dot(ac,ab);
			float t = acab / ab2;

			if (t < 0)
				t = 0;
			else if (t > 1)
				t = 1;

			Vec2 h = ((ab * t) + lineFrom) - center;
			float h2 = vector::dot(h,h);

			return (h2 <= (radius * radius));
		}
		// -------------------------------------------------------
		// random int
		// -------------------------------------------------------
		int random(int min,int max) {
			MTRand_open rand;
			float minf = static_cast<float>(min);
			float maxf = static_cast<float>(max) + 0.99f;
			float r = minf + (maxf - minf)* (float)rand();
			return static_cast<int>(r);
		}


		bool solveQuadraticFormula(const float a,const float b,const float c,float* r1,float* r2) {
			const float q = b * b - 4.0f * a * c; 
			if( q >= 0 ) {
				const float sq = sqrt(q);
				const float d = 1.0f / (2.0f * a);
				*r1 = ( -b + sq ) * d;
				*r2 = ( -b - sq ) * d;
				return true;//real roots
			}
			else {
				return false;//complex roots

			}
		}
		// -------------------------------------------------------
		// Check if two circles overlap
		// -------------------------------------------------------
		bool checkCircleIntersection(const Vec2& p1,float r1,const Vec2& p2,float r2) {
			Vec2 diff = p1 - p2;
			float distance = vector::sqrLength(diff);
			float rsqr = (r1 + r2) * (r1 + r2);			
			if ( distance <= rsqr ) {
				return true;
			}
			return false;
		}

		// -------------------------------------------------------
		// Check if two circles overlap
		// -------------------------------------------------------
		bool checkCircleIntersection(const Vec2& p1,float r1,const Vec2& p2,float r2,float* dist,Vec2& penVec) {
			Vec2 diff = p1 - p2;
			float distance = vector::sqrLength(diff);
			float rsqr = (r1 + r2) * (r1 + r2);
			*dist = vector::length(diff) - (r1 + r2);
			penVec = vector::normalize(diff);
			penVec = penVec * (*dist);
			if ( distance <= rsqr ) {
				return true;
			}
			return false;
		}

		Vec2 getShiftVector(const Vec2& p1,float r1,const Vec2& p2,float r2) {
			Vec2 diff = p1 - p2;
			float maxSquareDistance = r1 + r2;
			maxSquareDistance *= maxSquareDistance;
			float squareDistance = vector::dot(diff, diff);
			if (squareDistance > maxSquareDistance) {
				return ds::Vec2(0, 0);
			}
			float distance = sqrtf(squareDistance);
			if (distance > 0.0f ){
				diff.x /= distance;
				diff.y /= distance;
			}
			else{
				diff = ds::Vec2(1, 0);
			}

			float scaleFactor = r1 + r2 - distance;

			diff.x *= scaleFactor;
			diff.y *= scaleFactor;

			return diff;        
		}


		void clamp(Vec2& v,float minX,float maxX,float minY,float maxY) {
			if ( v.x < minX ) {
				v.x = minX;
			}
			if ( v.x > maxX ) {
				v.x = maxX;
			}
			if ( v.y < minY ) {
				v.y = minY;
			}
			if ( v.y > maxY ) {
				v.y = maxY;
			}
		}

		bool circleSweepTest(const Vec2& a0,const Vec2& a1,float ra,const Vec2& b0,const Vec2& b1,float rb,float* u0,float* u1) {
			Vec2 va = a1 - a0;
			//vector from A0 to A1
			Vec2 vb = b1 - b0;
			//vector from B0 to B1
			Vec2 AB = b0 - a0;
			//vector from A0 to B0
			Vec2 vab = vb - va;
			//relative velocity (in normalized time)
			float rab = ra + rb;
			float a = vector::dot(vab,vab);
			//u*u coefficient
			float b = 2.0f * vector::dot(vab,AB);
			//u coefficient
			float c = vector::dot(AB,AB) - rab*rab;
			//constant term
			//check if they're currently overlapping
			if( vector::dot(AB,AB) <= rab*rab )	{
				*u0 = 0;
				*u1 = 0;
				return true;
			}
			//check if they hit each other 
			// during the frame
			if( solveQuadraticFormula( a, b, c, u0, u1 ) ) {
				if( u0 > u1 ) {
					float tmp = *u0;
					*u0 = *u1;
					*u1 = tmp;
				}
				return true;
			}
			return false;
		}
	}

}
