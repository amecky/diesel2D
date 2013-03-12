#ifndef NODE_TRANSFORMATION_H
#define NODE_TRANSFORMATION_H
#include "..\dxstdafx.h"
#include "..\utils\GameMath.h"

namespace ds {

class NodeTransformation {

public:
	NodeTransformation();
	~NodeTransformation() {};

	void reset();
    void translateAbs( float x, float y, float z );
	void setPosition( float x, float y, float z );
	void setPosition(Vec3 pos);
    void translateRel( float x, float y, float z );
	void setPositionRel( float x, float y, float z );
	void setPositionRel(Vec3 pos);
    void rotateAbs( float x, float y, float z );
    void rotateRel( float x, float y, float z );
    void scaleAbs( float x, float y, float z );
    void scaleRel( float x, float y, float z );
	void setScale(Vec3 scale);
	void setRotation(Vec3 rot);
	void getTransformation(D3DXMATRIX* transMat);
    D3DXMATRIX* getTransform();
	//void transform();
    float getXPosition() { return m_translate._41; }
    float getYPosition() { return m_Y; }
    float getZPosition() { return m_translate._43; }
	Vec3 getPosition() { return Vec3(m_translate._41,m_Y,m_translate._43);}
    float getXRotation() { return m_rotationX; }
    float getYRotation() { return m_rotationY; }
    float getZRotation() { return m_rotationZ; }
    float getXScale()    { return m_scale._11; }
    float getYScale()    { return m_scale._22; }
    float getZScale()    { return m_scale._33; }
	Vec3 getScale() { return Vec3(m_scale._11,m_scale._22,m_scale._33);}
    void setXPosition( float x ) { m_translate._41 = x; }
    void setYPosition( float y ) { m_translate._42 = y; }
    void setZPosition( float z ) { m_translate._43 = z; }	
    void setXRotation( float x ) { rotateAbs( x, m_rotationY, m_rotationZ ); }
    void setYRotation( float y ) { rotateAbs( m_rotationX, y, m_rotationZ ); }
    void setZRotation( float z ) { rotateAbs( m_rotationX, m_rotationY, z ); }
	Vec3 getRotation() { return Vec3(m_rotationX,m_rotationY,m_rotationZ);}
    void setXScale( float x )    { m_scale._11 = x; }
    void setYScale( float y )    { m_scale._22 = y; }
    void setZScale( float z )    { m_scale._33 = z; }
protected:
	bool dirty;
private:
    D3DXMATRIX m_translate;
    D3DXMATRIX m_rotate;
    D3DXMATRIX m_scale;
    D3DXMATRIX m_transform;
    float m_rotationX, m_rotationY, m_rotationZ;
	float m_Y;
};

};

#endif 

