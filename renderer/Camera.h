#pragma once
#include "..\dxstdafx.h"
#include "..\utils\GameMath.h"

namespace ds {

class Camera {

public:
	//Camera(void);
	Camera(int width,int height);
	~Camera(void);
	void setLens(float fov, float aspect, float nearZ, float farZ);
	const D3DXMATRIX& getViewMatrix() const;
	const D3DXMATRIX& getProjectionMatrix() const;
	const D3DXMATRIX& getViewProjectionMatrix() const;
	void setOrthogonal();
	void restore();
	Vec3 getPosition() { return m_ViewPos;}
	void setPosition(const Vec3& pos);
	void lookAt(const Vec3& target);
	void lookAt(Vec3 pos,Vec3 target,Vec3 up);
	const Vec3& getLookAt() const { return m_LookAt; }
	void setSpeed(float spd) { 
		m_Speed = spd;
	}	
	// moving the camera
	void move(float unit);
	void strafe(float unit);
	void rotateLR(bool right);
	void rotateUD(bool down);
	void setPitch(float angle);
	void setYAngle(float angle);

	void update(float elapsedTime,const Vec2& mousePosition,bool buttonPressed);
	void buildView();
private:	
	Vec2 m_LastMousePosition;

    D3DXMATRIX m_View;
	D3DXMATRIX m_Proj;
	D3DXMATRIX mViewProj;

	D3DXMATRIX m_OrthoView;
	D3DXMATRIX m_OrthoProj;

	Vec3 m_ViewPos;
	Vec3 m_UpVec;
	Vec3 m_LookAt;
	Vec3 m_RightVec;
	// all vectors
	//Vec3 worldPosition;
	//Vec3 worldRightPosition;
	//Vec3 worldUpPosition;
	//Vec3 wLookVec;

	float m_Speed;
	bool m_Ortho;
};

};