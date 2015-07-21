#include "Camera.h"
#include <d3dx9math.h>
#include "..\math\vector.h"
#include "..\math\matrix.h"
#include "..\utils\Log.h"

namespace ds {

Camera::Camera(int width,int height) {		
	m_View = matrix::m4identity();
	m_Proj = matrix::m4identity();
	mViewProj = matrix::m4identity();

	m_Speed = 10.0f;
	m_LastMousePosition = Vector2f(0,0);
	m_ViewPos = Vector3f(0,0,-5);
	m_UpVec = Vector3f(0,1,0);
	m_LookAt = Vector3f(0,0,1);
	m_RightVec = Vector3f(1,0,0);
	

	float w = static_cast<float>(width);
	float h = static_cast<float>(height);
	float aspect = w / h;
	setLens(0.25f*PI, aspect, 1.0f, 1000.0f);
	m_OrthoProj = matrix::mat4OrthoLH(w,h,0.0f,1.0f);	// checked
	m_OrthoView = matrix::m4identity();//matrix::mat4LookAtLH(m_ViewPos,m_LookAt,m_UpVec); // checked
	m_Ortho = false;
	buildView();
}

Camera::~Camera(void) {
}

void Camera::setLens(float fov, float aspect, float nearZ, float farZ) {
	m_Proj = matrix::mat4PerspectiveFovLH(fov, aspect, nearZ, farZ); // checked
	buildView();
}

void Camera::setPosition(const Vector3f& pos) { 
	m_ViewPos = pos;
	buildView();
}

void Camera::lookAt(const Vector3f& lookAt) { 
	m_LookAt = lookAt;
	buildView();
}

void Camera::lookAt(Vector3f pos,Vector3f target,Vector3f up) {
	Vector3f L = target - pos;
	L = normalize(L);
	Vector3f R = cross(up,L);
	R = normalize(R);
	Vector3f U = cross(L,R);
	U = normalize(U);
	m_ViewPos = pos;
	m_RightVec = R;
	m_UpVec = U;
	m_LookAt = L;
	buildView();	
}

void Camera::move(float unit) {
	Vector3f tmp = m_LookAt * unit;
	m_ViewPos = m_ViewPos + tmp;
	buildView();
}

void Camera::strafe(float unit) {
	Vector3f tmp = m_RightVec * unit;
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

void Camera::update(float elapsedTime,const Vector2f& mousePosition,bool buttonPressed) {
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

Vector3f Camera::screenToWorld(const Vector2f& pos) {
	
	mat4 ma1 = m_View * m_Proj;
	mat4 inv1 = matrix::mat4Inverse(ma1);

	Vector4f in;
	in.x = (pos.x - 1024.0f) / 1024.0f;
	in.y = (768.0f - pos.y) / 768.0f;
	in.z = 1.0f;
	in.w = 1.0;

	// Map to range -1 to 1
	in.x = in.x * 2.0 - 1.0;
	in.y = in.y * 2.0 - 1.0;
	in.z = in.z * 2.0 - 1.0;

	Vector4f o4 = inv1 * in;
	o4 *= 1.0f / o4.w;
	/*
	//Vector4f ndc(2 * pos.x / 1024 - 1,2 * (768 - pos.y) / 768 - 1,1, 1);

	Vector3f ndc(2 * pos.x / 1024 - 1, 2 * (768 - pos.y) / 768 - 1, 1);

	Vector3f tmp = m * ndc;
	//tmp *= 1.0f / tmp.w;

	return Vector3f(tmp.x, tmp.y, tmp.z);
	*/
	
	/*
	Vector3f v;
	v.x = (((2.0f * pos.x) / 1024) - 1) / m_Proj._11;
	v.y = (-2.0f * pos.y / 768 + 1) / m_Proj._22;
	v.z = 1.0f;

	//LOG << "v: " << DBG_V3(v);

	mat4 matInvView = matrix::mat4Inverse(m_View);

	//Vector4f ndc((2 * (pos.x - 1024)) / 1024 - 1,(2 * (768 - pos.y)) / 768 - 1, 0, 1);

	Vector3f rayDir = matInvView * v;

	Vector3f rayOrigin;

	rayOrigin.x = matInvView._41;
	rayOrigin.y = matInvView._42;
	rayOrigin.z = matInvView._43;
	//LOG << "v: " << DBG_V3(v);
	return rayDir;
	*/


	D3DXMATRIX tm;
	D3DXMatrixPerspectiveFovLH(&tm, 0.25f*PI, 1024.0f / 768.0f, 0.1f, 1000.0f);
	mat4 m = m_View;// *m_Proj;
	mat4 inv = matrix::mat4Inverse(m_Proj);
	D3DXMATRIX itm;
	D3DXMatrixInverse(&itm,NULL,&tm);

	D3DXMATRIX viewMatrix;
	D3DXMatrixLookAtLH(&viewMatrix,
		&D3DXVECTOR3(0.0f, 0.0f, -16.0f), //position
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), //Look at
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMATRIX worldMatrix;
	D3DXMatrixIdentity(&worldMatrix);

	D3DXMATRIX m1, m2, m3;
	D3DXVECTOR3 vvec,outv;
	D3DXMatrixMultiply(&m1, &worldMatrix, &viewMatrix);
	D3DXMatrixMultiply(&m2, &m1, &tm);
	D3DXMatrixInverse(&m3, NULL, &m2);
	vvec.x = 2.0f * (pos.x - 1024.0f) / 1024.0f - 1.0f;
	vvec.y = 1.0f - 2.0f * (pos.y - 768.0f) / 768.0f;
	vvec.z = 1.0f;// (pv->z - pviewport->MinZ) / (pviewport->MaxZ - pviewport->MinZ);
	D3DXVec3TransformCoord(&outv, &vvec, &m3);

	//LOG << "screen pos: " << DBG_V2(pos);
	Vector3f vec;
	vec.x = (((2.0f * pos.x) / 1024.0f) - 1.0f);// / m_Proj._11;
	vec.y = -(((2.0f * pos.y) / 768.0f) - 1.0f);// / m_Proj._22;

	//vec.x = (2.0f * pos.x - 1024.0f) / 1024.0f - 1.0f;
	//vec.y = 1.0f - (2.0f * pos.y - 768.0f) / 768.0f;
	vec.z = 1.0f;// (pos.z - 0.1) / (1000 - pviewport->MinZ);
	Vector3f pout = vec * inv;
	//D3DXVec3TransformCoord(pout, &vec, &inv);
	return vec;
	
}

void Camera::buildView() {

	// Keep camera's axes orthogonal to each other and of unit length.
	Vector3f L = normalize(m_LookAt);
	Vector3f U = cross(m_LookAt,m_RightVec);
	U = normalize(U);
	Vector3f R = cross(U,L);
	R = normalize(R);

	// Fill in the view matrix entries.
	m_LookAt = L;
	m_RightVec = R;
	m_UpVec = U;

	float x = -dot(m_ViewPos,R);
	float y = -dot(m_ViewPos,U);
	float z = -dot(m_ViewPos,L);

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