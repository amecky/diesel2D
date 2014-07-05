#pragma once
#include "Actor.h"
#include "..\lib\DataArray.h"

namespace ds {

// -------------------------------------------------------
// Size animation
// -------------------------------------------------------
struct SizeAnimationData {

	ID id;
	ID actorID;
	Vector2f minScale;
	Vector2f maxScale;
	float amplitude;

	SizeAnimationData() : minScale(0,0) , maxScale(1,1) , amplitude(1.0f) {
		//add("min_scale",&minScale);
		//add("max_scale",&maxScale);
		//add("amplitude",&amplitude);
	}
};

class SizeAnimation : public AbstractBehavior<SizeAnimationData> {

public:
	SizeAnimation() : AbstractBehavior<SizeAnimationData>() {}
	
	void update(Actor& actor,float elapsed) {
		SizeAnimationData& data = getData(actor);
		actor.scale = data.minScale + ( data.maxScale - data.minScale) * abs(sin(actor.timer * data.amplitude));
	}

	void setScaling(const Actor& actor,const Vector2f& minScale,const Vector2f& maxScale,float amplitude = 1.0f) {
		SizeAnimationData& data = getData(actor);
		data.maxScale = maxScale;
		data.minScale = minScale;
		data.amplitude = amplitude;
	}
	
	const char* getName() const {
		return "SizeAnimation";
	}

};

// -------------------------------------------------------
// Grow size animation
// -------------------------------------------------------
struct GrowSizeAnimationData {

	ID id;
	float ttl;
	Vector2f minScale;
	Vector2f maxScale;

	GrowSizeAnimationData() {}
};

class GrowSizeAnimation : public AbstractBehavior<GrowSizeAnimationData> {

public:
	GrowSizeAnimation() : AbstractBehavior<GrowSizeAnimationData>() {
		m_Translator.add("min_scale",&GrowSizeAnimationData::minScale);
		m_Translator.add("max_scale",&GrowSizeAnimationData::maxScale);
		m_Translator.add("ttl",&GrowSizeAnimationData::ttl);
	}	

	void update(Actor& actor,float elapsed) {
		GrowSizeAnimationData& data = getData(actor);
		float norm = actor.timer / data.ttl;
		if ( norm <= 1.0f ) {
			actor.scale = lerp(data.minScale,data.maxScale,norm);
		}
	}

	void init(const Actor& actor,ID definitionID) {
		GrowSizeAnimationData& def = m_Definitions.get(definitionID);
		GrowSizeAnimationData& data = getData(actor);
		data.ttl = def.ttl;
		data.minScale = def.minScale;
		data.maxScale = def.maxScale;
	}
	
	const char* getName() const {
		return "GrowSizeAnimation";
	}
};

// -------------------------------------------------------
// Static rotation animation
// -------------------------------------------------------
struct StaticRotationAnimationData {

	ID id;
	ID actorID;
	float amplitude;
	int type;

	StaticRotationAnimationData() : amplitude(1.0f) , type(0) {}
};

class StaticRotationAnimation : public AbstractBehavior<StaticRotationAnimationData> {

public:
	StaticRotationAnimation() : AbstractBehavior<StaticRotationAnimationData>() {
		m_Translator.add("amplitude",&StaticRotationAnimationData::amplitude);
		m_Translator.add("type",&StaticRotationAnimationData::type);
	}

	void init(const Actor& actor,ID definitionID) {
		StaticRotationAnimationData& def = m_Definitions.get(definitionID);
		StaticRotationAnimationData& data = getData(actor);
		data.amplitude = def.amplitude;
		data.type = def.type;
	}

	void update(Actor& actor,float elapsed) {
		StaticRotationAnimationData& data = getData(actor);
		if ( data.type == 0 ) {
			actor.angle += elapsed * data.amplitude;
		}
		else {
			float v = sin(actor.timer * data.amplitude);
			actor.angle = v;
		}
	}
	
	const char* getName() const {
		return "StaticRotationAnimation";
	}

};

// -------------------------------------------------------
// Color fade animation
// -------------------------------------------------------
struct ColorFadeAnimationData {

	ID id;
	ID actorID;
	float ttl;
	Color start;
	Color end;

	ColorFadeAnimationData() : start(Color::WHITE) , end(Color::BLACK) , ttl(1.0f) {
	}

};

class ColorFadeAnimation : public AbstractBehavior<ColorFadeAnimationData> {

public:
	ColorFadeAnimation() : AbstractBehavior<ColorFadeAnimationData>() {
		m_Translator.add("start",&ColorFadeAnimationData::start);
		m_Translator.add("end",&ColorFadeAnimationData::end);
		m_Translator.add("ttl",&ColorFadeAnimationData::ttl);
	}
	
	void update(Actor& actor,float elapsed) {
		ColorFadeAnimationData& data = getData(actor);
		float norm = actor.timer / data.ttl;
		if ( norm <= 1.0f ) {
			actor.color = ds::color::lerp(data.start,data.end,norm);
		}
	}	
	
	void init(const Actor& actor,ID definitionID) {
		ColorFadeAnimationData& def = m_Definitions.get(definitionID);
		ColorFadeAnimationData& data = getData(actor);
		data.ttl = def.ttl;
		data.start = def.start;
		data.end = def.end;
	}
	
	void setFading(const Actor& actor,const Color& startColor,const Color& endColor,float ttl = 1.0f) {
		ColorFadeAnimationData& data = getData(actor);
		data.start = startColor;
		data.end = endColor;
		data.ttl = ttl;
	}
	
	const char* getName() const {
		return "ColorFadeAnimation";
	}

};

}