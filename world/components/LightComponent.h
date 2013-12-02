#pragma once
#include "..\World.h"
#include "..\SpriteEntity.h"

namespace ds {

struct Light {
	Vec3 position;
	Color color;
	float radius;
	float specular;
	float strength;
	bool active;
};

class LightComponent : public GameObject, public Entity {

typedef std::vector<Light> Lights;

public:
	LightComponent(void);
	~LightComponent(void);
	void update(float elapsed);
	void init() {}
	const EntityType getType() const {
		return ET_CUSTOM;
	}
	void draw();
	void initialize(int startLayer,int colorRT,int normalRT,float ambient,const Color& ambientColor);
	int addLight(const Vec3& position,float radius,const Color& color,float specular,float strength = 1.0f);
	Light* getLight(int index) {
		return &m_Lights[index];
	}	
private:
	int createDeferredShader();
	int createCombineShader();
	SpriteEntity m_CombineEntity;
	SpriteEntity m_OverlayEntity;
	int m_ShaderID;
	Shader m_LightShader;
	int m_Handle;
	int m_AddBS;
	Lights m_Lights;
	float m_ScreenW;
	float m_ScreenH;
};

}