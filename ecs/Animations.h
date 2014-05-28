#pragma once
#include "Actor.h"
#include "..\lib\DataArray.h"

namespace ds {

// -------------------------------------------------------
// Size animation
// -------------------------------------------------------
class SizeAnimation : public Animation {

public:
	SizeAnimation() : Animation() , m_MinScale(0,0) , m_MaxScale(1,1) , m_Amplitude(1.0f) {
		m_Settings.add("min_scale",&m_MinScale);
		m_Settings.add("max_scale",&m_MaxScale);
		m_Settings.add("amplitude",&m_Amplitude);
	}
	SizeAnimation(const Vector2f& minScale,const Vector2f& maxScale,float amplitude = 1.0f) : Animation() , m_MinScale(minScale) , m_MaxScale(maxScale) , m_Amplitude(amplitude) {
		m_Settings.add("min_scale",&m_MinScale);
		m_Settings.add("max_scale",&m_MaxScale);
		m_Settings.add("amplitude",&m_Amplitude);
	}
	void update(Actor& actor,sprite_t& sprite,float elapsed) {
		sprite.scale = m_MinScale + ( m_MaxScale - m_MinScale) * abs(sin(actor.timer * m_Amplitude));
	}
	void set(const Vector2f& minScale,const Vector2f& maxScale,float amplitude = 1.0f) {
		m_MinScale = minScale;
		m_MaxScale = maxScale;
		m_Amplitude = amplitude;
	}
private:
	float m_Amplitude;
	Vector2f m_MinScale;
	Vector2f m_MaxScale;
};

// -------------------------------------------------------
// Grow size animation
// -------------------------------------------------------
class GrowSizeAnimation : public Animation {

public:
	GrowSizeAnimation() : Animation() , m_MinScale(0,0) , m_MaxScale(1,1) , m_TTL(1.0f) {
		m_Settings.add("min_scale",&m_MinScale);
		m_Settings.add("max_scale",&m_MaxScale);
		m_Settings.add("ttl",&m_TTL);
	}
	void update(Actor& actor,sprite_t& sprite,float elapsed) {
		float norm = actor.timer / m_TTL;
		if ( norm <= 1.0f ) {
			sprite.scale = lerp(m_MinScale,m_MaxScale,norm);
		}
	}
private:
	float m_TTL;
	Vector2f m_MinScale;
	Vector2f m_MaxScale;
};

// -------------------------------------------------------
// Static rotation animation
// -------------------------------------------------------
class StaticRotationAnimation : public Animation {

public:
	StaticRotationAnimation() : Animation() , m_Amplitude(1.0f) {
		m_Settings.add("amplitude",&m_Amplitude);
	}
	void update(Actor& actor,sprite_t& sprite,float elapsed) {
		float v = sin(actor.timer * m_Amplitude);
		actor.angle = v;
	}
private:
	float m_Amplitude;
};

// -------------------------------------------------------
// Color fade animation
// -------------------------------------------------------
class ColorFadeAnimation : public Animation {

public:
	ColorFadeAnimation() : Animation() , m_Start(Color::WHITE) , m_End(Color::WHITE) , m_TTL(1.0f) {
		m_Settings.add("start",&m_Start);
		m_Settings.add("end",&m_End);
		m_Settings.add("ttl",&m_TTL);
	}
	ColorFadeAnimation(const Color& start,const Color& end,float ttl = 1.0f) : Animation() , m_Start(start) , m_End(end) , m_TTL(ttl) {
		m_Settings.add("start",&m_Start);
		m_Settings.add("end",&m_End);
		m_Settings.add("ttl",&m_TTL);
	}
	void update(Actor& actor,sprite_t& sprite,float elapsed) {
		float norm = actor.timer / m_TTL;
		if ( norm <= 1.0f ) {
			sprite.color = ds::color::lerp(m_Start,m_End,norm);
		}
	}
	void set(const Color& startColor,const Color& endColor,float ttl = 1.0f) {
		m_Start = startColor;
		m_End = endColor;
		m_TTL = ttl;
	}
private:
	float m_TTL;
	Color m_Start;
	Color m_End;
};

}