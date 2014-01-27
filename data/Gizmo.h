#pragma once
#include "Param.h"
#include <vector>
#include "..\utils\PlainTextReader.h"
#include "..\utils\Color.h"
#include "..\math\math_types.h"

struct Gizmo {

typedef std::vector<Param*> Params;

public:
	Gizmo(const char* name);
	~Gizmo();
	void add(const char* name,int* value) {
		IntParam* ip = new IntParam(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,uint32* value) {
		UIntParam* ip = new UIntParam(name,value);
		m_Params.push_back(ip);
	}
	void addString(const char* name,IdString* value) {
		IdStringParam* ip = new IdStringParam(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,float* value) {
		FloatParam* ip = new FloatParam(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,ds::Vec2* value) {
		Vec2Param* ip = new Vec2Param(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,ds::Color* value) {
		ColorParam* ip = new ColorParam(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,ds::Rect* value) {
		RectParam* ip = new RectParam(name,value);
		m_Params.push_back(ip);
	}
	void add(const char* name,ds::FloatArray* value) {
		FloatArrayParam* ip = new FloatArrayParam(name,value);
		m_Params.push_back(ip);
	}
	void load(Category* category);
	const char* getName() const {
		return m_Name;
	}
private:
	Gizmo* m_Parent;
	Params m_Params;
	const char* m_Name;
};

