#include "Gizmo.h"


Gizmo::Gizmo(const char* name) : m_Name(name) {}

Gizmo::~Gizmo(void) {
	Params::iterator it = m_Params.begin();
	while ( it != m_Params.end()) {
		delete (*it);
		it = m_Params.erase(it);
	}
}

void Gizmo::load(Category* category) {
	LOG << "loading data from " << category->getName();
	for ( size_t i = 0; i < m_Params.size(); ++i ) {
		Param* p = m_Params[i];
		if ( p->getType() == PT_INT ) {
			IntParam* ip = static_cast<IntParam*>(p);
			category->getInt(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_UINT ) {
			UIntParam* ip = static_cast<UIntParam*>(p);
			category->getInt(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_FLOAT ) {
			FloatParam* ip = static_cast<FloatParam*>(p);
			category->getFloat(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_COLOR ) {
			ColorParam* ip = static_cast<ColorParam*>(p);
			category->getColor(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_RECT ) {
			RectParam* ip = static_cast<RectParam*>(p);
			category->getRect(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_Vector2f ) {
			Vector2fParam* ip = static_cast<Vector2fParam*>(p);
			category->getVector2f(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_ID_STRING ) {
			IdStringParam* ip = static_cast<IdStringParam*>(p);
			category->getIdString(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_FLOAT_ARRAY ) {
			FloatArrayParam* ip = static_cast<FloatArrayParam*>(p);
			ip->getPtr()->reset();
			for (size_t i = 0; i < category->numProperties(); ++i ) {
				const char* propertyName = category->getPropertyName(i);
				if ( strcmp(propertyName,"loop_mode") == 0 ) {
					// PLM_ZERO,PLM_LAST,PLM_LOOP
					ip->getPtr()->setLoopMode(PLM_LOOP);
				}
				else if ( strcmp(propertyName,"interpolation") ) {
					// PI_LINEAR,PI_STEP,PI_CUBIC
					std::string inMode = category->getProperty("interpolation");
					if ( inMode == "PI_STEP") {
						ip->getPtr()->setInterpolationMode(PI_STEP);
					}
					else if ( inMode == "PI_CUBIC") {
						ip->getPtr()->setInterpolationMode(PI_CUBIC);
					}
				}
				else {
					float timeStep = atof(propertyName);
					float value = 1.0f;
					category->getFloat(propertyName,&value);
					ip->getPtr()->add(timeStep,value);
				}
			}
		}
		if ( p->getType() == PT_VEC2_PATH ) {
			Vec2PathParam* ip = static_cast<Vec2PathParam*>(p);
			ip->getPtr()->reset();
			for (size_t i = 0; i < category->numProperties(); ++i) {
				const char* propertyName = category->getPropertyName(i);
				if ( strcmp(propertyName,"loop_mode") ) {
					// PLM_ZERO,PLM_LAST,PLM_LOOP
					ip->getPtr()->setLoopMode(PLM_LOOP);
				}
				else if ( strcmp(propertyName,"velocity") ) {

				}
				else if ( strcmp(propertyName,"interpolation") ) {
					// PI_LINEAR,PI_STEP,PI_CUBIC
					std::string inMode = category->getProperty("interpolation");
					if ( inMode == "PI_STEP") {
						ip->getPtr()->setInterpolationMode(PI_STEP);
					}
					else if ( inMode == "PI_CUBIC") {
						ip->getPtr()->setInterpolationMode(PI_CUBIC);
					}
				}
				else {
					float timeStep = atof(propertyName);
					Vector2f value(1.0f);
					category->getVector2f(propertyName,&value);
					ip->getPtr()->add(timeStep,value);
				}
			}
		}
		if ( p->getType() == PT_COLOR_PATH ) {
			ColorPathParam* ip = static_cast<ColorPathParam*>(p);
			ip->getPtr()->reset();
			for (size_t i = 0; i < category->numProperties(); ++i) {
				const char* propertyName = category->getPropertyName(i);
				if ( strcmp(propertyName,"loop_mode") ) {
					// PLM_ZERO,PLM_LAST,PLM_LOOP
					ip->getPtr()->setLoopMode(PLM_LOOP);
				}
				else if ( strcmp(propertyName,"interpolation") ) {
					// PI_LINEAR,PI_STEP,PI_CUBIC
					std::string inMode = category->getProperty("interpolation");
					if ( inMode == "PI_STEP") {
						ip->getPtr()->setInterpolationMode(PI_STEP);
					}
					else if ( inMode == "PI_CUBIC") {
						ip->getPtr()->setInterpolationMode(PI_CUBIC);
					}
				}
				else {
					float timeStep = atof(propertyName);
					ds::Color value(1.0f,1.0f,1.0f,1.0f);
					category->getColor(propertyName,&value);
					ip->getPtr()->add(timeStep,value);
				}
			}
		}
	}
}