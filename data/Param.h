#pragma once
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include "..\math\FloatArray.h"

enum ParamType {
	PT_FLOAT,
	PT_INT,
	PT_UINT,
	PT_VEC2,
	PT_VEC3,
	PT_RECT,
	PT_COLOR,
	PT_ID_STRING,
	PT_FLOAT_ARRAY
};

class Param {

public:
	Param(const char* name) : m_Name(name) {}
	virtual ~Param() {}
	virtual const ParamType getType() const = 0;
	const char* getName() const {
		return m_Name;
	}
private:
	const char* m_Name;
};

class IntParam : public Param {

public:
	IntParam(const char* name,int* value) : Param(name) , m_Value(value) {}
	virtual ~IntParam() {}
	const ParamType getType() const {
		return PT_INT;
	}
	int* getPtr() {
		return m_Value;
	}
private:
	int* m_Value;
};

class UIntParam : public Param {

public:
	UIntParam(const char* name,uint32* value) : Param(name) , m_Value(value) {}
	virtual ~UIntParam() {}
	const ParamType getType() const {
		return PT_UINT;
	}
	uint32* getPtr() {
		return m_Value;
	}
private:
	uint32* m_Value;
};

class FloatParam : public Param {

public:
	FloatParam(const char* name,float* value) : Param(name) , m_Value(value) {}
	virtual ~FloatParam() {}
	const ParamType getType() const {
		return PT_FLOAT;
	}
	float* getPtr() {
		return m_Value;
	}
private:
	float* m_Value;
};

class Vec2Param : public Param {

public:
	Vec2Param(const char* name,ds::Vec2* value) : Param(name) , m_Value(value) {}
	virtual ~Vec2Param() {}
	const ParamType getType() const {
		return PT_VEC2;
	}
	ds::Vec2* getPtr() {
		return m_Value;
	}
private:
	ds::Vec2* m_Value;
};

class ColorParam : public Param {

public:
	ColorParam(const char* name,ds::Color* value) : Param(name) , m_Value(value) {}
	virtual ~ColorParam() {}
	const ParamType getType() const {
		return PT_COLOR;
	}
	ds::Color* getPtr() {
		return m_Value;
	}
private:
	ds::Color* m_Value;
};

class RectParam : public Param {

public:
	RectParam(const char* name,ds::Rect* value) : Param(name) , m_Value(value) {}
	virtual ~RectParam() {}
	const ParamType getType() const {
		return PT_RECT;
	}
	ds::Rect* getPtr() {
		return m_Value;
	}
private:
	ds::Rect* m_Value;
};

class IdStringParam : public Param {

public:
	IdStringParam(const char* name,IdString* value) : Param(name) , m_Value(value) {}
	virtual ~IdStringParam() {}
	const ParamType getType() const {
		return PT_ID_STRING;
	}
	IdString* getPtr() {
		return m_Value;
	}
private:
	IdString* m_Value;
};

class FloatArrayParam : public Param {

public:
	FloatArrayParam(const char* name,ds::FloatArray* value) : Param(name) , m_Value(value) {}
	virtual ~FloatArrayParam() {}
	const ParamType getType() const {
		return PT_FLOAT_ARRAY;
	}
	ds::FloatArray* getPtr() {
		return m_Value;
	}
private:
	ds::FloatArray* m_Value;
};
