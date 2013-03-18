#include "Camera.h"
#include <d3dx9math.h>
#include "..\math\vector.h"

namespace ds {
/*
Camera::Camera(void) {		
	D3DXMatrixIdentity(&m_View);
	D3DXMatrixIdentity(&m_Proj);
	D3DXMatrixIdentity(&mViewProj);
	m_Speed = 10.0f;
	m_LastMousePosition = Vec2(0,0);
	m_ViewPos = Vec3(0,0,0);
	m_UpVec = Vec3(0,1,0);
	m_LookAt = Vec3(0,0,1);
	m_RightVec = Vec3(1,0,0);
	buildView();

	float w = (float)gEngine->getWidth();
	float h = (float)gEngine->getHeight();

	setLens(0.25f*PI, 1.0f, 1.0f, 1000.0f);

	//D3DXMatrixOrthoOffCenterLH(&m_OrthoProj,0.0f,w,0.0f,h,0.0f,100.0f);
	D3DXMatrixOrthoLH(&m_OrthoProj, w, h, 0.1f, 100.0f);
	//D3DXMatrixIdentity(&m_OrthoView);
	D3DXMatrixLookAtLH(&m_OrthoView,&m_ViewPos,&m_LookAt,&m_UpVec);
	m_Ortho = false;
}
*/
Camera::Camera(int width,int height) {		
	D3DXMatrixIdentity(&m_View);
	D3DXMatrixIdentity(&m_Proj);
	D3DXMatrixIdentity(&mViewProj);
	m_Speed = 10.0f;
	m_LastMousePosition = Vec2(0,0);
	m_ViewPos = Vec3(0,0,0);
	m_UpVec = Vec3(0,1,0);
	m_LookAt = Vec3(0,0,1);
	m_RightVec = Vec3(1,0,0);
	

	float w = width;
	float h = height;

	setLens(0.25f*PI, 1.0f, 1.0f, 1000.0f);

	//D3DXMatrixOrthoOffCenterLH(&m_OrthoProj,0.0f,w,0.0f,h,0.0f,100.0f);
	D3DXMatrixOrthoLH(&m_OrthoProj, w, h, 0.1f, 100.0f);
	//D3DXMatrixIdentity(&m_OrthoView);
	D3DXVECTOR3 vp = convert(m_ViewPos);
	D3DXMatrixLookAtLH(&m_OrthoView,&vp,&convert(m_LookAt),&convert(m_UpVec));
	m_Ortho = false;
	buildView();
}

Camera::~Camera(void) {
}

void Camera::setLens(float fov, float aspect, float nearZ, float farZ) {
	D3DXMatrixPerspectiveFovLH(&m_Proj, fov, aspect, nearZ, farZ);	
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
	//D3DXVec3Normalize(&L, &L);

	Vec3 R = vector::cross(up,L);
	//D3DXVec3Cross(&R, &up, &L);
	//D3DXVec3Normalize(&R, &R);
	R = vector::normalize(R);

	Vec3 U = vector::cross(L,R);
	//D3DXVec3Cross(&U, &L, &R);
	//D3DXVec3Normalize(&U, &U);
	U = vector::normalize(U);
	m_ViewPos = pos;
	m_RightVec = R;
	m_UpVec = U;
	m_LookAt = L;
	buildView();	
}

void Camera::move(float unit) {
	//Vec3 dir;
	//D3DXVec3Normalize(&dir,&(m_LookAt - m_ViewPos));
	Vec3 tmp = m_LookAt * unit;
	m_ViewPos = m_ViewPos + tmp;
	buildView();
}

void Camera::strafe(float unit) {
	Vec3 tmp = m_RightVec * unit;
	m_ViewPos = m_ViewPos + tmp;
		/*
	Vec3 dir(0.0f, 0.0f, 0.0f);
	float step = unit;
	if ( unit > 0.0f ) {
		dir += worldRightPosition;
	}
	else {
		step *= -1.0f;
		dir -= worldRightPosition;
	}
	m_ViewPos += dir*step;
	*/
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
	D3DXMATRIX R;
	D3DXVECTOR3 rv = convert(m_RightVec);
	D3DXMatrixRotationAxis(&R, &rv, angle);
	D3DXVECTOR3 la = convert(m_LookAt);
	D3DXVec3TransformCoord(&la, &la, &R);
	D3DXVECTOR3 uv = convert(m_UpVec);
	D3DXVec3TransformCoord(&la, &uv, &R);
	buildView();
}

void Camera::setYAngle(float angle) {
	// Rotate camera's look and up vectors around the camera's right vector.
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, angle);
	D3DXVECTOR3 rv = convert(m_RightVec);
	D3DXVec3TransformCoord(&rv, &rv, &R);
	D3DXVECTOR3 uv = convert(m_UpVec);
	D3DXVec3TransformCoord(&uv, &uv, &R);
	D3DXVECTOR3 la = convert(m_LookAt);
	D3DXVec3TransformCoord(&la, &la, &R);
	buildView();
}

const D3DXMATRIX& Camera::getViewMatrix() const {
	if ( m_Ortho ) {
		return m_OrthoView;
	}
    return m_View;
}

const D3DXMATRIX& Camera::getProjectionMatrix() const {
	if ( m_Ortho ) {
		return m_OrthoProj;
	}
	return m_Proj;
}

const D3DXMATRIX& Camera::getViewProjectionMatrix() const {
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
	//D3DXVec3Normalize(&L, &m_LookAt);
	Vec3 U = vector::cross(m_LookAt,m_RightVec);
	//D3DXVec3Cross(&U, &m_LookAt, &m_RightVec);
	//D3DXVec3Normalize(&U, &U);
	U = vector::normalize(U);
	Vec3 R = vector::cross(U,L);
	//D3DXVec3Cross(&R, &U, &L);
	//D3DXVec3Normalize(&R, &R);
	R = vector::normalize(R);
	//Vec3 la = Vec3(0,0,0);
	//D3DXMatrixLookAtLH(&m_View,&m_ViewPos,&m_LookAt,&m_UpVec);
	// Fill in the view matrix entries.
	
	m_LookAt = L;
	m_RightVec = R;
	m_UpVec = U;

	float x = -vector::dot(m_ViewPos,R);//-D3DXVec3Dot(&m_ViewPos, &R);
	float y = -vector::dot(m_ViewPos,U);//D3DXVec3Dot(&m_ViewPos, &U);
	float z = -vector::dot(m_ViewPos,L);//D3DXVec3Dot(&m_ViewPos, &L);

	m_View(0,0) = m_RightVec.x; 
	m_View(1,0) = m_RightVec.y; 
	m_View(2,0) = m_RightVec.z; 
	m_View(3,0) = x;   

	m_View(0,1) = m_UpVec.x;
	m_View(1,1) = m_UpVec.y;
	m_View(2,1) = m_UpVec.z;
	m_View(3,1) = y;  

	m_View(0,2) = m_LookAt.x; 
	m_View(1,2) = m_LookAt.y; 
	m_View(2,2) = m_LookAt.z; 
	m_View(3,2) = z;   

	m_View(0,3) = 0.0f;
	m_View(1,3) = 0.0f;
	m_View(2,3) = 0.0f;
	m_View(3,3) = 1.0f;
	
	mViewProj = m_View * m_Proj;
}

D3DXVECTOR3 Camera::convert(const Vec3& v) {
	D3DXVECTOR3 tmp;
	tmp.x = v.x;
	tmp.y = v.y;
	tmp.z = v.z;
	return tmp;
}

};