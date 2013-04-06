#pragma once
#include "..\dxstdafx.h"
#include "..\math\math_types.h"

namespace ds {

class Camera {

public:
	//Camera(void);
	Camera(int width,int height);
	~Camera(void);
	void setLens(float fov, float aspect, float nearZ, float farZ);
	const mat4& getViewMatrix() const;
	const mat4& getProjectionMatrix() const;
	const mat4& getViewProjectionMatrix() const;
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
    mat4 m_View;
	mat4 m_Proj;
	mat4 mViewProj;

	mat4 m_OrthoView;
	mat4 m_OrthoProj;

	Vec3 m_ViewPos;
	Vec3 m_UpVec;
	Vec3 m_LookAt;
	Vec3 m_RightVec;


	float m_Speed;
	bool m_Ortho;
};

};