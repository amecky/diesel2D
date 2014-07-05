#pragma once
#include "Converter.h"
#include "..\data\DataTranslator.h"
#include "..\ecs\Blueprint.h"

namespace ds {

class BlueprintConverter : public Converter {

public:
	BlueprintConverter();
	~BlueprintConverter() {}
	void convert(JSONReader& reader,BinaryWriter& writer);
	void setWorld(World* world) {
		m_World = world;
	}

	const char* getResourceDirectory() {
		return "content\\blueprints";
	}

private:
	DataTranslator<ActorDefinition> m_ActorTranslator;
	DataTranslator<ColliderDefinition> m_ColliderTranslator;
	World* m_World;
};

}

