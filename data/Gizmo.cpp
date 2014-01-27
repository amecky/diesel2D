#include "Gizmo.h"


Gizmo::Gizmo(const char* name) : m_Name(name) {
	LOG(logINFO) << "Creating new Gizmo: " << name;
}


Gizmo::~Gizmo(void) {
	Params::iterator it = m_Params.begin();
	while ( it != m_Params.end()) {
		delete (*it);
		it = m_Params.erase(it);
	}
}

void Gizmo::load(Category* category) {
	LOGC(logINFO,"Gizmo") << "loading data from " << category->getName();
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
		if ( p->getType() == PT_VEC2 ) {
			Vec2Param* ip = static_cast<Vec2Param*>(p);
			category->getVec2(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_ID_STRING ) {
			IdStringParam* ip = static_cast<IdStringParam*>(p);
			category->getIdString(p->getName(),ip->getPtr());
		}
		if ( p->getType() == PT_FLOAT_ARRAY ) {
			std::vector<std::string> propertyNames;
			FloatArrayParam* ip = static_cast<FloatArrayParam*>(p);
			ip->getPtr()->reset();
			category->getPropertyNames(propertyNames);
			for ( size_t i = 0; i < propertyNames.size(); ++i ) {
				float timeStep = atof(propertyNames[i].c_str());
				float value = 1.0f;
				category->getFloat(propertyNames[i],&value);
				ip->getPtr()->add(timeStep,value);
			}
		}
	}
}