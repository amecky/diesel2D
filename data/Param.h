#pragma once
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include <IPath.h>

enum ParamType {
	PT_FLOAT,
	PT_INT,
	PT_UINT,
	PT_Vector2f,
	PT_Vector3f,
	PT_RECT,
	PT_COLOR,
	PT_ID_STRING,
	PT_FLOAT_ARRAY,
	PT_VEC2_PATH,
	PT_COLOR_PATH
};

class Param {

public:
	Param(const char* name) : m_Name(name) {}
	virtual ~Param() {}
	virtual const ParamType getType() const  = 0;
	const char* getName() const {
		return m_Name;
	}	
private:
	const char* m_Name;
};

template<class T,ParamType PT>
class BaseParam : public Param {

public:
	BaseParam(const char* name,T* value) : Param(name) , m_Value(value) {}
	virtual ~BaseParam() {}
	const ParamType getType() const {
		return PT;
	}
	const char* getName() const {
		return m_Name;
	}
	T* getPtr() {
		return m_Value;
	}
private:
	const char* m_Name;
	T* m_Value;
};

typedef BaseParam<Vector2f,PT_Vector2f> Vector2fParam;
typedef BaseParam<Vector3f,PT_Vector3f> Vector3fParam;
typedef BaseParam<float,PT_FLOAT> FloatParam;
typedef BaseParam<uint32,PT_UINT> UIntParam;
typedef BaseParam<int,PT_INT> IntParam;
typedef BaseParam<ds::Color,PT_COLOR> ColorParam;
typedef BaseParam<ds::Rect,PT_RECT> RectParam;
typedef BaseParam<IdString,PT_ID_STRING> IdStringParam;
typedef BaseParam<IPath<20,float>,PT_FLOAT_ARRAY> FloatArrayParam;
typedef BaseParam<IPath<20,Vector2f>,PT_VEC2_PATH> Vec2PathParam;
typedef BaseParam<IPath<20,ds::Color>,PT_COLOR_PATH> ColorPathParam;
