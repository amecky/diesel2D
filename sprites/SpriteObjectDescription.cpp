#include "SpriteObjectDescription.h"
#include "SpriteObject.h"

namespace ds {

SpriteObjectDescription::~SpriteObjectDescription(void) {
	if ( m_RotationPath != 0 ) {
		delete m_RotationPath;
	}
	if ( m_ColorPath != 0 ) {
		delete m_ColorPath;
	}
	if ( m_ScalePath != 0 ) {
		delete m_ScalePath;
	}
	if ( m_SpriteDescription != 0 ) {
		delete m_SpriteDescription;
	}
}

void SpriteObjectDescription::load(const char* name) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",name);
	JSONReader reader;
	if ( reader.parse(buffer)) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "rotation") {			
				m_RotationPath = new RotationAnimation;
				m_RotationPath->load(c);
			}
			if ( c->getName() == "settings") {			
				m_SpriteDescription = new SpriteDescription;
				m_SpriteDescription->load(c);
			}
			if ( c->getName() == "scale") {			
				m_ScalePath = new ScaleAnimation;
				m_ScalePath->load(c);
			}
			if ( c->getName() == "color") {			
				m_ColorPath = new ColorAnimation;
				m_ColorPath->load(c);
			}
		}
		gFileWatcher->registerFile(buffer,this);
	}
}

void SpriteObjectDescription::reload(const char* name) {
	JSONReader reader;
	if ( reader.parse(name)) {
		LOG(logINFO) << "found valid file";
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "rotation") {		
				if ( m_RotationPath == 0 ) {
					m_RotationPath = new RotationAnimation;
				}
				m_RotationPath->load(c);
			}
			if ( c->getName() == "scale") {	
				if ( m_ScalePath == 0 ) {
					m_ScalePath = new ScaleAnimation;
				}
				m_ScalePath->load(c);
			}
			if ( c->getName() == "color") {	
				if ( m_ColorPath == 0 ) {
					m_ColorPath = new ColorAnimation;
				}
				m_ColorPath->load(c);
			}
			if ( c->getName() == "settings") {			
				if ( m_SpriteDescription == 0 ) {
					m_SpriteDescription = new SpriteDescription;
				}
				m_SpriteDescription->load(c);
			}
		}
	}
}

void SpriteObjectDescription::setAnimations(SpriteObject* object) {
	if ( m_RotationPath != 0 ) {
		object->add(m_RotationPath);
	}
	if ( m_ScalePath != 0 ) {
		object->add(m_ScalePath);
	}
	if (m_ColorPath != 0 ) {
		object->add(m_ColorPath);
	}
}

void SpriteObjectDescription::prepare(SpriteObject* object) {
	if ( m_SpriteDescription != 0 ) {
		object->setTextureRect(m_SpriteDescription->textureRect);
	}
}

}
