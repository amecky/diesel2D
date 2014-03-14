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
	Vector3f getPosition() { return m_ViewPos;}
	void setPosition(const Vector3f& pos);
	void lookAt(const Vector3f& target);
	void lookAt(Vector3f pos,Vector3f target,Vector3f up);
	const Vector3f& getLookAt() const { return m_LookAt; }
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

	void update(float elapsedTime,const Vector2f& mousePosition,bool buttonPressed);
	void buildView();
private:	
	Vector2f m_LastMousePosition;
    mat4 m_View;
	mat4 m_Proj;
	mat4 mViewProj;

	mat4 m_OrthoView;
	mat4 m_OrthoProj;

	Vector3f m_ViewPos;
	Vector3f m_UpVec;
	Vector3f m_LookAt;
	Vector3f m_RightVec;


	float m_Speed;
	bool m_Ortho;
};

};