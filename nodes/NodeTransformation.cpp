#include "NodeTransformation.h"
#include "..\renderer\Renderer.h"

namespace ds {

NodeTransformation::NodeTransformation() {
    reset();
}

void NodeTransformation::reset() {
    D3DXMatrixIdentity( &m_translate );
    D3DXMatrixIdentity( &m_rotate );
    D3DXMatrixIdentity( &m_scale );
    D3DXMatrixIdentity( &m_transform );
    m_rotationX = m_rotationY = m_rotationZ = 0.0f;
	m_Y = 0.0f;
	m_transform = m_scale * m_rotate * m_translate; 
	dirty = false;
}

void NodeTransformation::translateAbs( float x, float y, float z ) {
    m_translate._41 = x;
	//m_translate._42 = y;	
	m_Y = y;
    m_translate._43 = z;
	dirty = true;
}

void NodeTransformation::translateRel( float x, float y, float z ) {
    m_translate._41 += x;
	//m_translate._42 += y;	
	m_Y += y;
    m_translate._43 += z;
	dirty = true;
}

void NodeTransformation::setPositionRel( float x, float y, float z ) {
	translateRel(x,y,z);
}

void NodeTransformation::setPositionRel(Vec3 pos) {
	translateRel(pos.x,pos.y,pos.z);
}

void NodeTransformation::setPosition( float x, float y, float z ) {
	translateAbs(x,y,z);
}

void NodeTransformation::setPosition(Vec3 pos) {
	translateAbs(pos.x,pos.y,pos.z);
}

void NodeTransformation::rotateAbs( float x, float y, float z ) {
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
    D3DXMatrixRotationYawPitchRoll( &m_rotate, y, x, z );
	dirty = true;
}

void NodeTransformation::rotateRel( float x, float y, float z ) {
    m_rotationX += x;
    m_rotationY += y;
    m_rotationZ += z;
    D3DXMatrixRotationYawPitchRoll( &m_rotate,  m_rotationY,  m_rotationX,  m_rotationZ );
	dirty = true;
}

void NodeTransformation::setScale(Vec3 scale) {
	scaleAbs(scale.x,scale.y,scale.z);
}

void NodeTransformation::setRotation(Vec3 rot) {
	rotateAbs(rot.x,rot.y,rot.z);
}

void NodeTransformation::scaleAbs( float x, float y, float z ) {
    m_scale._11 = x;
    m_scale._22 = y;
    m_scale._33 = z;
	dirty = true;
}

void NodeTransformation::scaleRel( float x, float y, float z ) {
    m_scale._11 += x;
    m_scale._22 += y;
    m_scale._33 += z;
	dirty = true;
}

void NodeTransformation::getTransformation(D3DXMATRIX* transMat) {
	//if ( gEngine->getRenderMode() == RM_2D ) {
			//m_translate._42 = gEngine->getHeight()-m_Y;
	//}
	//else {
		m_translate._42 = m_Y;
	//}
	*transMat = m_scale * m_rotate * m_translate; 
}

D3DXMATRIX* NodeTransformation::getTransform() {
	if ( dirty ) {
		//if ( gEngine->getRenderMode() == RM_2D ) {
			//m_translate._42 = gEngine->getHeight()-m_Y;
		//}
		//else {
			m_translate._42 = m_Y;
		//}
		m_transform = m_scale * m_rotate * m_translate; 
		dirty = false;
	}
    return &m_transform;
}

};
