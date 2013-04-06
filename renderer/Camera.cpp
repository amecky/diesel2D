#include "Camera.h"
#include <d3dx9math.h>
#include "..\math\vector.h"
#include "..\math\matrix.h"

namespace ds {

Camera::Camera(int width,int height) {		
	m_View = matrix::m4identity();
	m_Proj = matrix::m4identity();
	mViewProj = matrix::m4identity();

	m_Speed = 10.0f;
	m_LastMousePosition = Vec2(0,0);
	m_ViewPos = Vec3(0,0,0);
	m_UpVec = Vec3(0,1,0);
	m_LookAt = Vec3(0,0,1);
	m_RightVec = Vec3(1,0,0);
	

	float w = width;
	float h = height;
	setLens(0.25f*PI, 1.0f, 1.0f, 1000.0f);
	m_OrthoProj = matrix::mat4OrthoLH(w,h,1.0f,100.0f);	// checked
	m_OrthoView = matrix::mat4LookAtLH(m_ViewPos,m_LookAt,m_UpVec);
	m_Ortho = false;
	buildView();
}

Camera::~Camera(void) {
}

void Camera::setLens(float fov, float aspect, float nearZ, float farZ) {
	m_Proj = matrix::mat4PerspectiveFovLH(fov, aspect, nearZ, farZ); // checked
	buildView();
}

void Camera::setPosition(const Vec3& pos) { 
	m_ViewPos = pos;
	buildView();
}

void Camera::lookAt(const Vec3& lookAt) { 
	m_LookAt = lookAt;
	buildView();
}

void Camera::lookAt(Vec3 pos,Vec3 target,Vec3 up) {
	Vec3 L = target - pos;
	L = vector::normalize(L);
	Vec3 R = vector::cross(up,L);
	R = vector::normalize(R);
	Vec3 U = vector::cross(L,R);
	U = vector::normalize(U);
	m_ViewPos = pos;
	m_RightVec = R;
	m_UpVec = U;
	m_LookAt = L;
	buildView();	
}

void Camera::move(float unit) {
	Vec3 tmp = m_LookAt * unit;
	m_ViewPos = m_ViewPos + tmp;
	buildView();
}

void Camera::strafe(float unit) {
	Vec3 tmp = m_RightVec * unit;
	m_ViewPos = m_ViewPos + tmp;
	buildView();
}

void Camera::setOrthogonal() {
	m_Ortho = true;
	mViewProj = m_OrthoView * m_OrthoProj;	
}

void Camera::restore() {
	m_Ortho = false;
	buildView();
}

void Camera::setPitch(float angle) {
	mat4 R = matrix::mat4Rotation(m_RightVec,angle);
	m_UpVec = R * m_UpVec;
	m_LookAt = R * m_LookAt;
	buildView();
}

void Camera::setYAngle(float angle) {
	mat3 R = matrix::mat3RotationY(angle);
	m_RightVec = R * m_RightVec;
	m_UpVec = R * m_UpVec;
	m_LookAt = R * m_LookAt;
	buildView();
}

const mat4& Camera::getViewMatrix() const {
	if ( m_Ortho ) {
		return m_OrthoView;
	}
    return m_View;
}

const mat4& Camera::getProjectionMatrix() const {
	if ( m_Ortho ) {
		return m_OrthoProj;
	}
	return m_Proj;
}

const mat4& Camera::getViewProjectionMatrix() const {
    return mViewProj;
}

void Camera::update(float elapsedTime,const Vec2& mousePosition,bool buttonPressed) {
	/*
	if ( !gEngine->isConsoleActive() ) {
	*/
		if( GetAsyncKeyState('W') & 0x8000 ) {
			move(10.0f*elapsedTime);
		}
		if( GetAsyncKeyState('S') & 0x8000 ) {
			move(-10.0f*elapsedTime);
		}
		if( GetAsyncKeyState('A') & 0x8000 ) {
			strafe(-10.0f*elapsedTime);
		}
		if( GetAsyncKeyState('D') & 0x8000 ) {
			strafe(10.0f*elapsedTime);
		}
		
		if ( buttonPressed ) {		
			// Make each pixel correspond to a quarter of a degree.
			float dx = DEGTORAD(0.25f*(mousePosition.x - m_LastMousePosition.x));
			float dy = DEGTORAD(0.25f*(mousePosition.y - m_LastMousePosition.y));
			setPitch(dy);
			setYAngle(dx);		
		}
		m_LastMousePosition.x = mousePosition.x;
		m_LastMousePosition.y = mousePosition.y;
	//}
}

void Camera::buildView() {

	// Keep camera's axes orthogonal to each other and of unit length.
	Vec3 L = vector::normalize(m_LookAt);
	Vec3 U = vector::cross(m_LookAt,m_RightVec);
	U = vector::normalize(U);
	Vec3 R = vector::cross(U,L);
	R = vector::normalize(R);

	// Fill in the view matrix entries.
	m_LookAt = L;
	m_RightVec = R;
	m_UpVec = U;

	float x = -vector::dot(m_ViewPos,R);
	float y = -vector::dot(m_ViewPos,U);
	float z = -vector::dot(m_ViewPos,L);

	m_View._11 = m_RightVec.x; 
	m_View._21 = m_RightVec.y; 
	m_View._31 = m_RightVec.z; 
	m_View._41 = x;   

	m_View._12 = m_UpVec.x;
	m_View._22 = m_UpVec.y;
	m_View._32 = m_UpVec.z;
	m_View._42 = y;  

	m_View._13 = m_LookAt.x; 
	m_View._23 = m_LookAt.y; 
	m_View._33 = m_LookAt.z; 
	m_View._43 = z;   

	m_View._14 = 0.0f;
	m_View._24 = 0.0f;
	m_View._34 = 0.0f;
	m_View._44 = 1.0f;
	
	mViewProj = m_View * m_Proj;
}

};