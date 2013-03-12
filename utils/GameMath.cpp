#include "GameMath.h"
#include "..\renderer\Camera.h"

namespace ds {

Vec2 const Vec2::ZERO  = Vec2(0.0f,0.0f);
Vec2 const Vec2::RIGHT = Vec2(1.0f,0.0f);
Vec2 const Vec2::LEFT  = Vec2(0.0f,-1.0f);
Vec2 const Vec2::UP    = Vec2(0.0f,1.0f);
Vec2 const Vec2::DOWN  = Vec2(0.0f,-1.0f);

Vec2 operator + (const Vec2& v1,const Vec2& v2) {
	return Vec2(v1.x + v2.x,v1.y + v2.y);
}
Vec2 operator + (const Vec2 &v, const float s) {
	return Vec2(v.x + s,v.y + s);
}
Vec2 operator + (const float s, const Vec2 &v) {
	return Vec2(v.x + s,v.y + s);
}

Vec2 operator - (const Vec2& v1,const Vec2& v2) {
	return Vec2(v1.x - v2.x,v1.y - v2.y);
}
Vec2 operator - (const Vec2 &v, const float s) {
	return Vec2(v.x -s,v.y-s);
}
Vec2 operator - (const float s, const Vec2 &v) {
	return Vec2(v.x -s,v.y-s);
}

Vec2 operator * (const Vec2& v,float s) {
	return Vec2(v.x*s,v.y*s);
}
Vec2 operator * (float s,const Vec2& v) {
	return Vec2(v.x*s,v.y*s);
}

bool operator == (const Vec2 &u, const Vec2 &v) {
	return (u.x == v.x && u.y == v.y);
}
bool operator != (const Vec2 &u, const Vec2 &v) {
	return (u.x != v.x || u.y != v.y);
}
Vec3 const Vec3::ZERO   = Vec3(0.0f);
};

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
		Vec2 vn1 = v1.unit();
		Vec2 vn2 = v2.unit();
		float dot = vn1.Dot(vn2);		
		if ( dot < -1.0f ) {
			dot = -1.0f;
		}
		if ( dot > 1.0f ) {
			dot = 1.0f;
		}
		float tmp = acos(dot);
		float cross = (vn1.x * vn2.y) - (vn2.x * vn1.y);
		if ( cross < 0.0f ) {
			tmp = 2.0f * D3DX_PI - tmp;
		}
		return tmp;
		/*
		Vec2 vc = vn1.Cross(vn2);
		float mag = vc.Length();

		float ab = atan2(mag,dot);
		if ( dot < 0.0f ) {
			ab = 2.0f * D3DX_PI - ab;
		}
		return ab;
		*/
	}
	else {
		return 0.0f;
	}
}

float getTargetAngle(const Vec2& v1,const Vec2& v2) {	
	Vec2 diff = v1 - v2;
	return getAngle(Vec2::RIGHT,diff) + D3DX_PI;
}

Vec2 getDistantPosition(const Vec2& initialPosition,float angle,float radius) {
	return getDistantPosition(initialPosition.x,initialPosition.y,angle,radius);
}

Vec2 getDistantPosition(float x,float y,float angle,float radius) {
	ds::Vec2 ret;
	ret.x = x + cos(angle) * radius;
	ret.y = y + sin(angle) * radius;
	return ret;
}

void follow(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float add) {
	Vec2 p = newPos;
	Vec2 diff = targetPos - p;
	*angle = 2.0f * D3DX_PI - getAngle(diff,Vec2::RIGHT);
	if ( *angle < 0.0f ) {
		*angle += 2.0f * D3DX_PI;
	}
	float dist = targetPos.distance(p);
	if ( dist > distance ) {				
		newPos.addScale(diff,add);
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

}

}
