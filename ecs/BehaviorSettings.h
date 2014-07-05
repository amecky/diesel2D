#pragma once
#include "..\dxstdafx.h"
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include "..\utils\Log.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\io\BinaryLoader.h"
#include "..\data\DataTranslator.h"

namespace ds {

// -------------------------------------------------------
// Dynamic Settings
// -------------------------------------------------------
struct BehaviorSettings {

struct DataDefinition {

	unsigned index;
	DataType type;
	const char* name;
};

typedef std::vector<DataDefinition> Definitions;


	BehaviorSettings() : numIntMembers(0) , numFloatMembers(0) {}
	~BehaviorSettings() {}
	// -------------------------------------------------------
	// add int member
	// -------------------------------------------------------
	void addInt(const char* name,int defaultValue = 0) {
		unsigned idx = numIntMembers;
		m_Members[numIntMembers++] = defaultValue;
		addDefinition(name,idx,DTR_INT);
	}
	// -------------------------------------------------------
	// add float member
	// -------------------------------------------------------
	void addFloat(const char* name,float defaultValue = 0.0f) {
		unsigned idx = numFloatMembers;
		m_FloatMembers[numFloatMembers++] = defaultValue;
		addDefinition(name,idx,DTR_FLOAT);
	}
	// -------------------------------------------------------
	// add Vector2f member
	// -------------------------------------------------------
	void addVec2(const char* name,const Vector2f& defaultValue) {
		unsigned idx = m_Vec2Members.size();
		m_Vec2Members.push_back(vec2Member);
		addDefinition(name,idx,DTR_VEC2);
	}
	// -------------------------------------------------------
	// add Rect member
	// -------------------------------------------------------
	void addRect(const char* name,const Rect& rectMember) {
		unsigned idx = m_RectMembers.size();
		m_RectMembers.push_back(rectMember);
		addDefinition(name,idx,DTR_RECT);
	}
	// -------------------------------------------------------
	// add Color member
	// -------------------------------------------------------
	void addColor(const char* name,const Color& colorMember) {
		unsigned idx = m_ColorMembers.size();
		m_ColorMembers.push_back(colorMember);
		addDefinition(name,idx,DTR_COLOR);
	}
	// -------------------------------------------------------
	// set int value
	// -------------------------------------------------------
	void set(const char* name,int value) {
		const DataDefinition* def = find(name,DTR_INT);
		if ( def != 0 ) {
			m_Members[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get int value
	// -------------------------------------------------------
	int getInt(const char* name,int defaultValue = 0) const {
		const DataDefinition* def = find(name,DTR_INT);
		if ( def != 0 ) {
			return m_Members[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set float value
	// -------------------------------------------------------
	void set(const char* name,float value) {
		const DataDefinition* def = find(name,DTR_FLOAT);
		if ( def != 0 ) {
			m_FloatMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get float value
	// -------------------------------------------------------
	float get(const char* name,float defaultValue) const {
		const DataDefinition* def = find(name,DTR_FLOAT);
		if ( def != 0 ) {
			return m_FloatMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Vector2f value
	// -------------------------------------------------------
	void set(const char* name,const Vector2f& value) {
		const DataDefinition* def = find(name,DTR_VEC2);
		if ( def != 0 ) {
			m_Vec2Members[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Vector2f value
	// -------------------------------------------------------
	const Vector2f& get(const char* name,const Vector2f& defaultValue) const {
		const DataDefinition* def = find(name,DTR_VEC2);
		if ( def != 0 ) {
			return m_Vec2Members[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Rect value
	// -------------------------------------------------------
	void set(const char* name,const Rect& value) {
		const DataDefinition* def = find(name,DTR_RECT);
		if ( def != 0 ) {
			m_RectMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Rect value
	// -------------------------------------------------------
	const Rect& get(const char* name,const Rect& defaultValue) const {
		const DataDefinition* def = find(name,DTR_RECT);
		if ( def != 0 ) {
			return m_RectMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Color value
	// -------------------------------------------------------
	void set(const char* name,const Color& value) {
		const DataDefinition* def = find(name,DTR_COLOR);
		if ( def != 0 ) {
			m_ColorMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Color value
	// -------------------------------------------------------
	const Color& get(const char* name,const Color& defaultValue) const {
		const DataDefinition* def = find(name,DTR_COLOR);
		if ( def != 0 ) {
			return m_ColorMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// contains
	// -------------------------------------------------------
	bool contains(const char* name) {
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& in = m_Definitions[i];
			if ( strcmp(name,in.name) == 0 ) {
				return true;
			}
		}
		return false;
	}
	// -------------------------------------------------------
	// get data type
	// -------------------------------------------------------
	const DataType& getType(const char* name) const {
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			const DataDefinition& in = m_Definitions[i];
			if ( strcmp(name,in.name) == 0 ) {
				return in.type;
			}
		}
		return unknownType;
	}
	// -------------------------------------------------------
	// number of definitions
	// -------------------------------------------------------
	const uint32 numDefinitions() const {
		return m_Definitions.size();
	}
	// -------------------------------------------------------
	// Get name of specific definition
	// -------------------------------------------------------
	const char* getDefinitionName(uint32 index) const {
		return m_Definitions[index].name;
	}
	// -------------------------------------------------------
	// load file
	// -------------------------------------------------------
	void load(const char* fileName,const char* categoryName) {
		char buffer[256];
		sprintf(buffer,"content\\resources\\%s.json",fileName);
		JSONReader reader;
		if ( reader.parse(buffer) ) {
			Category* c = reader.getCategory(categoryName);
			for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
				DataDefinition& def = m_Definitions[i];
				switch ( def.type ) {
				case DTR_INT :
					set(def.name,c->getInt(def.name,0));
					break;
				case DTR_FLOAT :
					set(def.name,c->getFloat(def.name,0.0f));
					break;
				case DTR_VEC2 :
					set(def.name,c->getVector2f(def.name,Vector2f(0,0)));
					break;
				case DTR_RECT : {
						Rect r;
						c->getRect(def.name,&r);
						set(def.name,r);
					}
					break;
				case DTR_COLOR:
					set(def.name,c->getColor(def.name,Color::WHITE));
					break;
				}
			}

		}
		else {
			LOGEC("DataTranslator") << "Cannot parse " << fileName;
		}
	}
	// -------------------------------------------------------
	// load file
	// -------------------------------------------------------
	void read(Category* category) {		
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
			case DTR_INT : {
				int tmp = 0;
				category->getInt(def.name,&tmp);
				set(def.name,tmp);
							}
							break;
			case DTR_FLOAT :
				set(def.name,category->getFloat(def.name,0.0f));
				break;
			case DTR_VEC2 :
				set(def.name,category->getVector2f(def.name,Vector2f(0,0)));
				break;
			case DTR_RECT : {
				Rect r;
				category->getRect(def.name,&r);
				set(def.name,r);
							}
							break;
			case DTR_COLOR:
				set(def.name,category->getColor(def.name,Color::WHITE));
				break;
			}
		}
	}
	// -------------------------------------------------------
	// save binary version
	// -------------------------------------------------------
	void save(const char* fileName,uint32 chunkID) {
		char out[256];
		sprintf(out,"data\\%s.dsb",fileName);
		BinaryWriter writer;
		int signature[] = {0,8,15};
		writer.open(out,signature,3);
		saveChunk(writer,chunkID);
		writer.close();
	}
	// -------------------------------------------------------
	// save binary chunk
	// -------------------------------------------------------
	void saveChunk(BinaryWriter& writer,uint32 chunkID,bool append = false) {
		if ( !append ) {
			writer.startChunk(chunkID,1);
		}
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
			case DTR_INT:
				writer.write(getInt(def.name,0));
				break;
			case DTR_FLOAT:
				writer.write(get(def.name,0.0f));
				break;
			case DTR_VEC2:
				writer.write(get(def.name,Vector2f(0,0)));
				break;
			case DTR_COLOR:
				writer.write(get(def.name,Color::WHITE));
				break;
			case DTR_RECT:
				writer.write(get(def.name,Rect(0,0,100,100)));
				break;
			}
		}
		if ( !append ) {
			writer.closeChunk();
		}
	}
	// -------------------------------------------------------
	// Read binary file
	// -------------------------------------------------------
	void read(const char* fileName,uint32 chunkID) {
		char buffer[256];
		BinaryLoader loader;
		sprintf(buffer,"data\\%s.dsb",fileName);
		int signature[] = {0,8,15};
		if ( loader.open(buffer,signature,3) == 0 ) {
			while ( loader.openChunk() == 0 ) {		
				if ( loader.getChunkID() == chunkID ) {	
					for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
						DataDefinition& def = m_Definitions[i];
						switch ( def.type ) {
						case DTR_INT: {
								int tmp = 0;
								loader.read(&tmp);
								set(def.name,tmp);
							}
							break;
						case DTR_FLOAT: {
								float tmp = 0.0f;
								loader.read(&tmp);
								set(def.name,tmp);
							}
							break;
						case DTR_VEC2: {
								Vector2f tmp;
								loader.read(&tmp);
								set(def.name,tmp);
							}
							break;
						case DTR_COLOR: {
								Color tmp = Color::WHITE;
								loader.read(&tmp);
								set(def.name,tmp);
							}
							break;
						case DTR_RECT: {
								Rect tmp(0,0,0,0);
								loader.read(&tmp);
								set(def.name,tmp);
							}
							break;
						}
					}
				}
				loader.closeChunk();
			}		
			loader.close();
		}
	}
	// -------------------------------------------------------
	// Read binary chunk
	// -------------------------------------------------------
	void readChunk(BinaryLoader& loader) {
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
			case DTR_INT: {
					int tmp = 0;
					loader.read(&tmp);
					set(def.name,tmp);
				}
				break;
			case DTR_FLOAT: {
					float tmp = 0.0f;
					loader.read(&tmp);
					set(def.name,tmp);
				}
				break;
			case DTR_VEC2: {
					Vector2f tmp;
					loader.read(&tmp);
					set(def.name,tmp);
				}
				break;
			case DTR_COLOR: {
					Color tmp = Color::WHITE;
					loader.read(&tmp);
					set(def.name,tmp);
				}
				break;
			case DTR_RECT: {
					Rect tmp(0,0,0,0);
					loader.read(&tmp);
					set(def.name,tmp);
				}
				break;
			}
		}
	}

	// -------------------------------------------------------
	// internal find
	// -------------------------------------------------------
	const DataDefinition* find(const char* name,DataType type) const {
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			const DataDefinition& in = m_Definitions[i];
			if ( in.type == type ) {
				if ( strcmp(name,in.name) == 0 ) {
					return &m_Definitions[i];
				}
			}
		}
		return 0;
	}
	// -------------------------------------------------------
	// internal add definition
	// -------------------------------------------------------
	void addDefinition(const char* name,uint32 index,const DataType type) {
		DataDefinition def;
		def.index = index;
		def.name = name;
		def.type = type;
		m_Definitions.push_back(def);
	}
	//std::vector<int*> m_Members;
	int m_Members[16];
	uint32 numIntMembers;
	float m_FloatMembers[16];
	uint32 numFloatMembers;
	std::vector<Vector2f> m_Vec2Members;
	std::vector<Rect> m_RectMembers;
	std::vector<Color> m_ColorMembers;
	Definitions m_Definitions;
	const static DataType unknownType = DTR_UNKNOWN;
};

}