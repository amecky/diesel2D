#pragma once
#include "..\dxstdafx.h"
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include "..\utils\Log.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\io\BinaryLoader.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"

namespace ds {


	class AbstractGameSettings {


	public:
		AbstractGameSettings() {}

		virtual ~AbstractGameSettings() {}

		virtual void save() = 0;

		virtual void load() = 0;

		virtual void exportJSON() = 0;
		
		virtual void importJSON() = 0;

		//virtual void showDialog() = 0;
	};

// -------------------------------------------------------
// Data types
// -------------------------------------------------------
enum DataType {DTR_INT,DTR_FLOAT,DTR_VEC2,DTR_RECT,DTR_COLOR,DTR_COLOR_PATH,DTR_VEC2_PATH,DTR_FLOAT_PATH,DTR_VEC3,DTR_UNKNOWN,DTR_BOOL};

// -------------------------------------------------------
// DataTranslator
// -------------------------------------------------------
template<class T>
class DataTranslator : public AbstractGameSettings {

	struct DataDefinition {

		unsigned index;
		DataType type;
		const char* name;
	};

	typedef std::vector<DataDefinition> Definitions;

public:
	typedef int T::*IntMember;
	typedef float T::*FloatMember;
	typedef bool T::*BoolMember;
	typedef Vector2f T::*Vec2Member;
	typedef Vector3f T::*Vec3Member;
	typedef Rect T::*RectMember;
	typedef Color T::*ColorMember;
	typedef ColorPath T::*ColorPathMember;
	typedef FloatArray T::*FloatPathMember;
	typedef Vector2fPath T::*Vec2PathMember;

	DataTranslator() : AbstractGameSettings(), _guiState(1), _modelOffset(0) {}
	// -------------------------------------------------------
	// add int member
	// -------------------------------------------------------
	void add(const char* name, IntMember intMember) {
		unsigned idx = m_Members.size();
		m_Members.push_back(intMember);
		addDefinition(name, idx, DTR_INT);
	}
	// -------------------------------------------------------
	// add float member
	// -------------------------------------------------------
	void add(const char* name, FloatMember floatMember) {
		unsigned idx = m_FloatMembers.size();
		m_FloatMembers.push_back(floatMember);
		addDefinition(name, idx, DTR_FLOAT);
	}
	// -------------------------------------------------------
	// add Vector2f member
	// -------------------------------------------------------
	void add(const char* name, Vec2Member vec2Member) {
		unsigned idx = m_Vec2Members.size();
		m_Vec2Members.push_back(vec2Member);
		addDefinition(name, idx, DTR_VEC2);
	}
	// -------------------------------------------------------
	// add Vector3f member
	// -------------------------------------------------------
	void add(const char* name, Vec3Member vec3Member) {
		unsigned idx = m_Vec3Members.size();
		m_Vec3Members.push_back(vec3Member);
		addDefinition(name, idx, DTR_VEC3);
	}
	// -------------------------------------------------------
	// add Rect member
	// -------------------------------------------------------
	void add(const char* name, RectMember rectMember) {
		unsigned idx = m_RectMembers.size();
		m_RectMembers.push_back(rectMember);
		addDefinition(name, idx, DTR_RECT);
	}
	// -------------------------------------------------------
	// add Color member
	// -------------------------------------------------------
	void add(const char* name, ColorMember colorMember) {
		unsigned idx = m_ColorMembers.size();
		m_ColorMembers.push_back(colorMember);
		addDefinition(name, idx, DTR_COLOR);
	}
	// -------------------------------------------------------
	// add Color path member
	// -------------------------------------------------------
	void add(const char* name, ColorPathMember colorPathMember) {
		unsigned idx = m_ColorPathMembers.size();
		m_ColorPathMembers.push_back(colorPathMember);
		addDefinition(name, idx, DTR_COLOR_PATH);
	}
	// -------------------------------------------------------
	// add float path member
	// -------------------------------------------------------
	void add(const char* name, FloatPathMember floatPathMember) {
		unsigned idx = m_FloatPathMembers.size();
		m_FloatPathMembers.push_back(floatPathMember);
		addDefinition(name, idx, DTR_FLOAT_PATH);
	}
	// -------------------------------------------------------
	// add vector2f path member
	// -------------------------------------------------------
	void add(const char* name, Vec2PathMember vec2PathMember) {
		unsigned idx = m_Vec2PathMembers.size();
		m_Vec2PathMembers.push_back(vec2PathMember);
		addDefinition(name, idx, DTR_VEC2_PATH);
	}
	// -------------------------------------------------------
	// add bool member
	// -------------------------------------------------------
	void add(const char* name, BoolMember b) {
		unsigned idx = m_BoolMembers.size();
		m_BoolMembers.push_back(b);
		addDefinition(name, idx, DTR_BOOL);
	}
	// -------------------------------------------------------
	// set int value
	// -------------------------------------------------------
	void set(const char* name, int value, T* t) {
		const DataDefinition* def = find(name, DTR_INT);
		if (def != 0) {
			t->*m_Members[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get int value
	// -------------------------------------------------------
	int get(const char* name, T* t, int defaultValue) const {
		const DataDefinition* def = find(name, DTR_INT);
		if (def != 0) {
			return t->*m_Members[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set float value
	// -------------------------------------------------------
	void set(const char* name, float value, T* t) {
		const DataDefinition* def = find(name, DTR_FLOAT);
		if (def != 0) {
			t->*m_FloatMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get float value
	// -------------------------------------------------------
	float get(const char* name, T* t, float defaultValue) const {
		const DataDefinition* def = find(name, DTR_FLOAT);
		if (def != 0) {
			return t->*m_FloatMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Vector2f value
	// -------------------------------------------------------
	void set(const char* name, const Vector2f& value, T* t) {
		const DataDefinition* def = find(name, DTR_VEC2);
		if (def != 0) {
			t->*m_Vec2Members[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Vector2f value
	// -------------------------------------------------------
	const Vector2f& get(const char* name, T* t, const Vector2f& defaultValue) const {
		const DataDefinition* def = find(name, DTR_VEC2);
		if (def != 0) {
			return t->*m_Vec2Members[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Vector3f value
	// -------------------------------------------------------
	void set(const char* name, const Vector3f& value, T* t) {
		const DataDefinition* def = find(name, DTR_VEC3);
		if (def != 0) {
			t->*m_Vec3Members[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Vector3f value
	// -------------------------------------------------------
	const Vector3f& get(const char* name, T* t, const Vector3f& defaultValue) const {
		const DataDefinition* def = find(name, DTR_VEC3);
		if (def != 0) {
			return t->*m_Vec3Members[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Rect value
	// -------------------------------------------------------
	void set(const char* name, const Rect& value, T* t) {
		const DataDefinition* def = find(name, DTR_RECT);
		if (def != 0) {
			t->*m_RectMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Rect value
	// -------------------------------------------------------
	const Rect& get(const char* name, T* t, const Rect& defaultValue) const {
		const DataDefinition* def = find(name, DTR_RECT);
		if (def != 0) {
			return t->*m_RectMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set Color value
	// -------------------------------------------------------
	void set(const char* name, const Color& value, T* t) {
		const DataDefinition* def = find(name, DTR_COLOR);
		if (def != 0) {
			t->*m_ColorMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Color value
	// -------------------------------------------------------
	const Color& get(const char* name, T* t, const Color& defaultValue) const {
		const DataDefinition* def = find(name, DTR_COLOR);
		if (def != 0) {
			return t->*m_ColorMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set color path value
	// -------------------------------------------------------
	void set(const char* name, const ColorPath& value, T* t) {
		const DataDefinition* def = find(name, DTR_COLOR_PATH);
		if (def != 0) {
			t->*m_ColorPathMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Rect value
	// -------------------------------------------------------
	const ColorPath& get(const char* name, T* t, const ColorPath& defaultValue) const {
		const DataDefinition* def = find(name, DTR_COLOR_PATH);
		if (def != 0) {
			return t->*m_ColorPathMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set float path value
	// -------------------------------------------------------
	void set(const char* name, const FloatArray& value, T* t) {
		const DataDefinition* def = find(name, DTR_FLOAT_PATH);
		if (def != 0) {
			t->*m_FloatPathMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get float path value
	// -------------------------------------------------------
	const FloatArray& get(const char* name, T* t, const FloatArray& defaultValue) const {
		const DataDefinition* def = find(name, DTR_FLOAT_PATH);
		if (def != 0) {
			return t->*m_FloatPathMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set vec2 path value
	// -------------------------------------------------------
	void set(const char* name, const Vector2fPath& value, T* t) {
		const DataDefinition* def = find(name, DTR_VEC2_PATH);
		if (def != 0) {
			t->*m_Vec2PathMembers[def->index] = value;
		}
	}
	// -------------------------------------------------------
	// get Rect value
	// -------------------------------------------------------
	const Vector2fPath& get(const char* name, T* t, const Vector2fPath& defaultValue) const {
		const DataDefinition* def = find(name, DTR_VEC2_PATH);
		if (def != 0) {
			return t->*m_Vec2PathMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// set bool value
	// -------------------------------------------------------
	void set(const char* name, const bool b, T* t) {
		const DataDefinition* def = find(name, DTR_BOOL);
		if (def != 0) {
			t->*m_BoolMembers[def->index] = b;
		}
	}
	// -------------------------------------------------------
	// get float path value
	// -------------------------------------------------------
	const bool get(const char* name, T* t, bool defaultValue) const {
		const DataDefinition* def = find(name, DTR_BOOL);
		if (def != 0) {
			return t->*m_BoolMembers[def->index];
		}
		return defaultValue;
	}
	// -------------------------------------------------------
	// contains
	// -------------------------------------------------------
	bool contains(const char* name) {
		for (size_t i = 0; i < m_Definitions.size(); ++i) {
			DataDefinition& in = m_Definitions[i];
			if (strcmp(name, in.name) == 0) {
				return true;
			}
		}
		return false;
	}
	// -------------------------------------------------------
	// get data type
	// -------------------------------------------------------
	const DataType& getType(const char* name) const {
		for (size_t i = 0; i < m_Definitions.size(); ++i) {
			const DataDefinition& in = m_Definitions[i];
			if (strcmp(name, in.name) == 0) {
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


	void save() {}

	void load(){}

	void exportJSON() {}
	
	void importJSON() {}

	void showDialog(T* t) {
		
	}


	// -------------------------------------------------------
	// load file
	// -------------------------------------------------------
	void load(const char* fileName,const char* categoryName,T* t) {
		char buffer[256];
		sprintf(buffer,"content\\resources\\%s.json",fileName);
		JSONReader reader;
		if ( reader.parse(buffer) ) {
			Category* c = reader.getCategory(categoryName);
			for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
				DataDefinition& def = m_Definitions[i];
				switch ( def.type ) {
					case DTR_INT :
						set(def.name,c->getInt(def.name,0),t);
						break;
					case DTR_FLOAT :
						set(def.name,c->getFloat(def.name,0.0f),t);
						break;
					case DTR_VEC2 :
						set(def.name,c->getVector2f(def.name,Vector2f(0,0)),t);
						break;
					case DTR_VEC3:
						set(def.name, c->getVector3f(def.name, Vector3f(0, 0, 0)), t);
						break;
					case DTR_BOOL:
						set(def.name, c->getBool(def.name,false), t);
						break;
					case DTR_RECT : {
							Rect r;
							c->getRect(def.name,&r);
							set(def.name,r,t);
						}
						break;
					case DTR_COLOR:
						set(def.name,c->getColor(def.name,Color::WHITE),t);
						break;
					case DTR_COLOR_PATH:
						set(def.name,getColorPathValues(c),t);
						break;
					case DTR_FLOAT_PATH:
						set(def.name, getFloatPathValues(c), t);
						break;
					case DTR_VEC2_PATH:
						set(def.name,getVec2PathValues(c),t);
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
	void read(Category* category,T* t) {		
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
				case DTR_INT : {
						int tmp = 0;
						category->getInt(def.name,&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_FLOAT :
					set(def.name,category->getFloat(def.name,0.0f),t);
					break;
				case DTR_VEC2 :
					set(def.name,category->getVector2f(def.name,Vector2f(0,0)),t);
					break;
				case DTR_VEC3:
					set(def.name, category->getVector3f(def.name, Vector3f(0,0,0)), t);
					break;
				case DTR_BOOL:
					set(def.name, category->getBool(def.name, false), t);
					break;
				case DTR_RECT : {
						Rect r;
						category->getRect(def.name,&r);
						set(def.name,r,t);
					}
					break;
				case DTR_COLOR:
					set(def.name,category->getColor(def.name,Color::WHITE),t);
					break;
				case DTR_COLOR_PATH:
					set(def.name,getColorPathValues(category),t);
					break;
				case DTR_FLOAT_PATH:
					set(def.name, getFloatPathValues(category), t);
					break;
				case DTR_VEC2_PATH:
					set(def.name,getVec2PathValues(category),t);
					break;
			}
		}
	}
	// -------------------------------------------------------
	// save binary version
	// -------------------------------------------------------
	void save(const char* fileName,uint32 chunkID,T* t) {
		char out[256];
		sprintf(out,"data\\%s.dsb",fileName);
		BinaryWriter writer;
		int signature[] = {0,8,15};
		writer.open(out,signature,3);
		saveChunk(writer,chunkID,t);
		writer.close();
	}
	// -------------------------------------------------------
	// save binary chunk
	// -------------------------------------------------------
	void saveChunk(BinaryWriter& writer,uint32 chunkID,T* t,bool append = false) {
		if ( !append ) {
			writer.startChunk(chunkID,1);
		}
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
			case DTR_INT:
				writer.write(get(def.name,t,0));
				break;
			case DTR_FLOAT:
				writer.write(get(def.name,t,0.0f));
				break;
			case DTR_VEC2:
				writer.write(get(def.name,t,Vector2f(0,0)));
				break;
			case DTR_VEC3:
				writer.write(get(def.name, t, Vector3f(0,0, 0)));
				break;
			case DTR_BOOL:
				writer.write(get(def.name, t, false));
				break;
			case DTR_COLOR:
				writer.write(get(def.name,t,Color::WHITE));
				break;
			case DTR_RECT:
				writer.write(get(def.name,t,Rect(0,0,100,100)));
				break;
			case DTR_COLOR_PATH: {
					ColorPath p;
					writer.write(get(def.name,t,p));
				}
				break;
			case DTR_FLOAT_PATH: {
					FloatArray p;
					writer.write(get(def.name, t, p));
				}
				break;
			case DTR_VEC2_PATH: {
					Vector2fPath p;
					writer.write(get(def.name,t,p));
				}
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
	void read(const char* fileName,uint32 chunkID,T* t) {
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
									set(def.name,tmp,t);
								}
								break;
							case DTR_FLOAT: {
									float tmp = 0.0f;
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_VEC2: {
									Vector2f tmp;
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_VEC3: {
									Vector3f tmp;
									loader.read(&tmp);
									set(def.name, tmp, t);
								}
								break;
							case DTR_BOOL: {
								bool tmp;
								loader.read(&tmp);
								set(def.name, tmp, t);
							}
								break;
							case DTR_COLOR: {
									Color tmp = Color::WHITE;
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_COLOR_PATH: {
									ColorPath tmp;
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_RECT: {
									Rect tmp(0,0,0,0);
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_VEC2_PATH: {
									Vector2fPath tmp;
									loader.read(&tmp);
									set(def.name,tmp,t);
								}
								break;
							case DTR_FLOAT_PATH: {
									FloatArray tmp;
									loader.read(&tmp);
									set(def.name, tmp, t);
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
	void readChunk(BinaryLoader& loader,T* t) {
		for ( size_t i = 0; i < m_Definitions.size(); ++i ) {
			DataDefinition& def = m_Definitions[i];
			switch ( def.type ) {
				case DTR_INT: {
						int tmp = 0;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_FLOAT: {
						float tmp = 0.0f;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_VEC2: {
						Vector2f tmp;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_VEC3: {
						Vector3f tmp;
						loader.read(&tmp);
						set(def.name, tmp, t);
					}
					break;
				case DTR_BOOL: {
					bool tmp;
					loader.read(&tmp);
					set(def.name, tmp, t);
					}
					break;
				case DTR_COLOR: {
						Color tmp = Color::WHITE;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_RECT: {
						Rect tmp(0,0,0,0);
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_COLOR_PATH: {
						ColorPath tmp;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
				case DTR_FLOAT_PATH: {
						FloatArray tmp;
						loader.read(&tmp);
						set(def.name, tmp, t);
					}
					break;
				case DTR_VEC2_PATH: {
						Vector2fPath tmp;
						loader.read(&tmp);
						set(def.name,tmp,t);
					}
					break;
			}
		}
	}
private:
	ColorPath getColorPathValues(Category* category) {
		ColorPath path;
		std::vector<std::string> propertyNames;
		category->getPropertyNames(propertyNames);
		Color value(1.0f,1.0f,1.0f,1.0f);
		for ( size_t i = 0; i < propertyNames.size(); ++i ) {
			if ( propertyNames[i] == "loop_mode" ) {
				// PLM_ZERO,PLM_LAST,PLM_LOOP
				path.setLoopMode(PLM_LOOP);
			}
			else if ( propertyNames[i] == "interpolation" ) {
				// PI_LINEAR,PI_STEP,PI_CUBIC
				std::string inMode = category->getProperty("interpolation");
				if ( inMode == "PI_STEP") {
					path.setInterpolationMode(PI_STEP);
				}
				else if ( inMode == "PI_CUBIC") {
					path.setInterpolationMode(PI_CUBIC);
				}
			}
			else {
				float timeStep = 0.0f;
				std::istringstream ist(propertyNames[i]);
				ist >> timeStep;
				category->getColor(propertyNames[i],&value);
				path.add(timeStep,value);
			}
		}
		return path;
	}

	Vector2fPath getVec2PathValues(Category* category) {
		Vector2fPath path;
		std::vector<std::string> propertyNames;
		category->getPropertyNames(propertyNames);
		Vector2f value;
		for ( size_t i = 0; i < propertyNames.size(); ++i ) {
			if ( propertyNames[i] == "loop_mode" ) {
				// PLM_ZERO,PLM_LAST,PLM_LOOP
				path.setLoopMode(PLM_LOOP);
			}
			else if ( propertyNames[i] == "interpolation" ) {
				// PI_LINEAR,PI_STEP,PI_CUBIC
				std::string inMode = category->getProperty("interpolation");
				if ( inMode == "PI_STEP") {
					path.setInterpolationMode(PI_STEP);
				}
				else if ( inMode == "PI_CUBIC") {
					path.setInterpolationMode(PI_CUBIC);
				}
			}
			else {
				float timeStep = 0.0f;
				std::istringstream ist(propertyNames[i]);
				ist >> timeStep;
				category->getVector2f(propertyNames[i],&value);
				path.add(timeStep,value);
			}
		}
		return path;
	}

	FloatArray getFloatPathValues(Category* category) {
		FloatArray path;
		std::vector<std::string> propertyNames;
		category->getPropertyNames(propertyNames);
		
		for (size_t i = 0; i < propertyNames.size(); ++i) {
			if (propertyNames[i] == "loop_mode") {
				// PLM_ZERO,PLM_LAST,PLM_LOOP
				path.setLoopMode(PLM_LOOP);
			}
			else if (propertyNames[i] == "interpolation") {
				// PI_LINEAR,PI_STEP,PI_CUBIC
				std::string inMode = category->getProperty("interpolation");
				if (inMode == "PI_STEP") {
					path.setInterpolationMode(PI_STEP);
				}
				else if (inMode == "PI_CUBIC") {
					path.setInterpolationMode(PI_CUBIC);
				}
			}
			else {
				float value = 0.0f;
				float timeStep = 0.0f;
				std::istringstream ist(propertyNames[i]);
				ist >> timeStep;
				category->getFloat(propertyNames[i], &value);
				LOG << "adding " << value << " at " << timeStep;
				path.add(timeStep, value);
			}
		}
		return path;
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
		_model.add(name, def);
	}

	std::vector<IntMember> m_Members;
	std::vector<FloatMember> m_FloatMembers;
	std::vector<Vec2Member> m_Vec2Members;
	std::vector<Vec3Member> m_Vec3Members;
	std::vector<RectMember> m_RectMembers;
	std::vector<ColorMember> m_ColorMembers;
	std::vector<ColorPathMember> m_ColorPathMembers;
	std::vector<FloatPathMember> m_FloatPathMembers;
	std::vector<Vec2PathMember> m_Vec2PathMembers;
	std::vector<BoolMember> m_BoolMembers;
	Definitions m_Definitions;
	gui::ComponentModel<DataDefinition> _model;
	int _guiState;
	int _modelOffset;
	const static DataType unknownType = DTR_UNKNOWN;
};

}
