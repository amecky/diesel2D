#pragma once
#include <vector>
#include "..\math\math_types.h"
#include "Vector.h"
#include "..\utils\Color.h"
#include "..\io\Serializer.h"
#include "SpriteObjectDescription.h"
#include "..\script\Script.h"

namespace ds {

class SpriteAnimation;

class Sprite : public Serializer {

typedef std::vector<SpriteAnimation*> Animations;

public:
	Sprite();
	virtual ~Sprite(void);
	void update(float elapsed);
	void add(SpriteAnimation* animation) {
		m_Animations.push_back(animation);
	}	
	const float getTimer() const {
		return m_Timer;
	}
	void resetTimer() {
		m_Timer = 0.0f;
	}
	//void setTextureRect(const Rect& r,int textureID = 0);
	void setTextureRect(const Rect& r,int textureWidth = 1024,int textureHeight = 1024,int textureID = 0);
	void setActive(bool active) {
		m_Active = active;
	}
	const bool isActive() const {
		return m_Active;
	}
	void setRadius(float radius) {
		m_Radius = radius;
	}
	const float getRadius() const {
		return m_Radius;
	}
	void setPosition(const Vector2f& pos);
	const Vector2f& getPosition() const {
		return m_Position;
	}
	void setIndex(int index) {
		m_Index = index;
	}
	const int getIndex() const {
		return m_Index;
	}
	void setID(uint32 id) {
		m_ID = id;
	}
	const uint32 getID() const {
		return m_ID;
	}
	const Vector2f& getUV(int index) const {
		return m_UV[index];
	}
	void setColor(const Color& clr) {
		m_Color = clr;
	}
	const Color& getColor() const {
		return m_Color;
	}
	Color* getColorPtr() {
		return &m_Color;
	}
	const Vector3f& getVertex(int idx) const {
		return m_Vertices[idx];
	}
	void setAngle(float angle) {
		m_Angle = angle;
		setPosition(m_Position);
	}
	const float getAngle() const {
		return m_Angle;
	}
	void setScale(const Vector2f& size) {
		m_Size = size;
		setPosition(m_Position);
	}
	void setScale(float sx,float sy) {
		m_Size.x = sx;
		m_Size.y = sy;
		setPosition(m_Position);
	}
	const Vector2f& getScale() const {
		return m_Size;
	}
	void tick(float elapsed) {
		m_Elapsed = elapsed;
		m_Timer += elapsed;
	}
	void move(float elapsed) {
		m_Position += m_Velocity * elapsed;
		setPosition(m_Position);
	}
	void setVelocity(const Vector2f& velocity) {
		m_Velocity = velocity;
	}
	const Vector2f& getVelocity() const {
		return m_Velocity;
	}
	void setTarget(const Vector2f& target) {
		m_Target = target;
	}
	const Vector2f& getTarget() const {
		return m_Target;
	}
	void setTextureID(int textureID) {
		m_TextureID = textureID;
	}
	const int getTextureID() const {
		return m_TextureID;
	}
	void setUserValue(int userValue) {
		m_UserValue = userValue;
	}
	const int getUserValue() const {
		return m_UserValue;
	}
	uint32 getLayer() const {
		return m_Layer;
	}
	void setLayer(uint32 layer) {
		m_Layer = layer;
	}
	void load(BinaryLoader* loader);
	void prepare(const SpriteDescription& description);
	void run(Script& script);
private:	
	float m_Timer;
	int m_TextureID;
	Animations m_Animations;
	uint32 m_ID;
	Vector3f m_Vertices[4];
	Vector2f m_UV[4];
	Vector2f m_Position;
	Vector2f m_Velocity;
	Vector2f m_Target;
	int m_Index;
	float m_Radius;
	float m_Angle;
	Vector2f m_Size;
	Color m_Color;
	bool m_Active;
	float m_Elapsed;
	//float timer;
	float delay;
	float dimX;
	float dimY;
	int m_UserValue;
	uint32 m_Layer;

};

}