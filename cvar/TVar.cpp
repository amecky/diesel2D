#include "TVar.h"
#include <assert.h>
#include "..\utils\Log.h"
#include <vector>
#include "..\utils\StringUtils.h"
#include <iostream>
#include <fstream>

namespace ds {

const uint32 TVAR_ENTRY_SIZE = 12;

TVarSystem* gTVarSystem = 0;

// ------------------------------------------------
//
// ------------------------------------------------
TVar::TVar(const char* name, const char* value, int flags) {
	m_Flags = flags;	
	m_HashName = string::murmur_hash(name);
	if ( gTVarSystem == 0 ) {
		gTVarSystem = new TVarSystem();
	}
	gTVarSystem->add(this,name,value);
}

TVar::~TVar() {
}

float TVar::getFloat() {
	assert(m_Flags & TVAR_FLOAT);
	return (*(float*)m_Data);
}

int TVar::getInt() {
	assert(m_Flags & TVAR_INTEGER);
	return (*(int*)m_Data);
}

bool TVar::getBool() {
	assert(m_Flags & TVAR_BOOL);
	return (*(bool*)m_Data);
}

const Color& TVar::getColor() const {
	assert(m_Flags & TVAR_COLOR);
	return (*(Color*)m_Data);
}

const Vec2& TVar::getVec2() const {
	assert(m_Flags & TVAR_VEC2);
	return (*(Vec2*)m_Data);
}

const Vec3& TVar::getVec3() const {
	assert(m_Flags & TVAR_VEC3);
	return (*(Vec3*)m_Data);
}

float TVar::getArray(uint32 offset) {
	uint32 size = (*(FloatArray*)m_Data).size;
	assert(offset < size);
	char* data = m_Data + 4 + offset *4;
	return (*(float*)data);
}

void TVar::setData(char* buffer,uint32 size) {
	m_Data = buffer;
	m_Size = size;
}

uint32 TVar::size() {
	if ( m_Flags & TVAR_ARRAY ) {
		return (*(FloatArray*)m_Data).size;
	}
	return 1;
}

// ------------------------------------------------
//
// ------------------------------------------------
const uint32 ENTRY_BUFFER_SIZE = 64 * 1024;
const uint32 DATA_BUFFER_SIZE = 64 * 1024;

TVarSystem::TVarSystem() : m_EntryIndex(0) , m_DataSize(0) {    
	m_EntryBuffer = new char[ENTRY_BUFFER_SIZE];
	m_DataBuffer = new char[DATA_BUFFER_SIZE];
}

TVarSystem::~TVarSystem() {
	LOGC(logINFO,"TVarSystem") << "destructing system";
	delete m_DataBuffer;
	delete m_EntryBuffer;
}

uint32 TVarSystem::setData(uint32 offset,const char* value,int flags) {

	char* dataBuffer = m_DataBuffer + offset;
	if ( flags & TVAR_FLOAT ) {
		(*(float*)dataBuffer) = atof(value);
		return 4;
	}
	else if ( flags & TVAR_INTEGER ) {
		(*(int*)dataBuffer) = atoi(value);
		return 4;
	}
	else if ( flags & TVAR_VEC2 ) {
		float x = getFloat(value,0);
		float y = getFloat(value,1);
		Vec2 v(x,y);
		(*(Vec2*)dataBuffer) = v;
		return 8;
	}
	else if ( flags & TVAR_VEC3 ) {
		float x = getFloat(value,0);
		float y = getFloat(value,1);
		float z = getFloat(value,2);
		Vec3 v(x,y,z);
		(*(Vec3*)dataBuffer) = v;
		return 12;
	}
	else if ( flags & TVAR_BOOL ) {
		if ( strcmp(value,"true") == 0 ) {
			(*(bool*)dataBuffer) = true;
		}
		else {
			(*(bool*)dataBuffer) = false;
		}
		return 1;
	}
	if ( flags & TVAR_COLOR ) {
		Color color(1.0f,1.0f,1.0f,1.0f);
		color.r = getFloat(value,0);
		color.g = getFloat(value,1);
		color.b = getFloat(value,2);
		color.a = getFloat(value,3);
		(*(Color*)dataBuffer) = color;
		return 16;
	}
	if ( flags & TVAR_ARRAY ) {
		std::string s(value);
		std::vector<std::string> values = ds::string::split(s);		
		(*(FloatArray*)dataBuffer).size = values.size();
		dataBuffer += 4;
		for ( size_t i = 0; i < values.size();++i ) {
			float v;
			std::istringstream ist(values[i]);
			ist >> v;
			(*(float*)dataBuffer) = v;
			dataBuffer += 4;
		}
		return 4 + values.size() * 4;
	}
	return 0;
}

float TVarSystem::getFloat(const char* value,int index) {
	std::string s(value);
	std::vector<std::string> values = ds::string::split(s);
	assert(index < values.size());
	float v;
	std::istringstream ist(values[index]);
	ist >> v;
	return v;
}

bool TVarSystem::contains(const char* name) {
	IdString hashName = string::murmur_hash(name);
	char* entryBuffer = m_EntryBuffer;
	for ( uint32 i = 0; i < m_EntryIndex;++i ) {
		IdString str = (*(TVarEntry*)entryBuffer).name;
		if ( str == hashName ) {
			LOGC(logINFO,"TVarSystem") << "found tvar at position " << i;
			return true;
		}
		entryBuffer += TVAR_ENTRY_SIZE;
	}
	return false;
}

void TVarSystem::add(TVar* tvar,const char* name,const char* value,bool updateOnly) {	
	if ( !contains(name)) {
		char* dataBuffer = m_DataBuffer + m_DataSize;
		uint32 offset = m_DataSize;
		uint32 size = setData(m_DataSize,value,tvar->getFlags());
		tvar->setData(dataBuffer,size);
		m_DataSize += size;
		LOGC(logINFO,"TVarSystem") << "not found - creating new tvar entry " << name;
		char* entryBuffer = m_EntryBuffer + m_EntryIndex * TVAR_ENTRY_SIZE;
		(*(TVarEntry*)entryBuffer).name = tvar->getHashName();
		(*(TVarEntry*)entryBuffer).flags = tvar->getFlags();
		(*(TVarEntry*)entryBuffer).offset = offset;
		++m_EntryIndex;
#ifdef DEBUG
		DebugEntry de;
		strcpy(de.name,name);
		de.hashName = tvar->getHashName();
		m_DebugEntries[de.hashName] = de;
#endif
	}
	else {
		LOGC(logINFO,"TVarSystem") << "Found tvar entry";
	}
}

bool TVarSystem::update(const char* name,const char* value) {

	IdString hashName = string::murmur_hash(name);
	char* entryBuffer = m_EntryBuffer;
	for ( uint32 i = 0; i < m_EntryIndex;++i ) {
		IdString str = (*(TVarEntry*)entryBuffer).name;
		if ( str == hashName ) {
			uint32 offset = (*(TVarEntry*)entryBuffer).offset;
			int flags = (*(TVarEntry*)entryBuffer).flags;			
			setData(offset,value,flags);
			return true;
		}
		entryBuffer += TVAR_ENTRY_SIZE;
	}
	return false;	
}

void TVarSystem::loadBinFile(PAKReader& reader) {
	fread(m_EntryBuffer,ENTRY_BUFFER_SIZE,1,reader.getFile());
	fread(m_DataBuffer,DATA_BUFFER_SIZE,1,reader.getFile());
}

void TVarSystem::saveBinaryFile(PAKWriter& writer) {
	fwrite(m_EntryBuffer,ENTRY_BUFFER_SIZE,1,writer.getFile());
	fwrite(m_DataBuffer,DATA_BUFFER_SIZE,1,writer.getFile());	
}

#ifdef DEBUG
void TVarSystem::debug() {
	char* entryBuffer = m_EntryBuffer;
	LOGC(logINFO,"TVar") << "Entries: " << m_EntryIndex;
	LOGC(logINFO,"TVar") << "EntryBuffer size: " << m_EntryIndex * TVAR_ENTRY_SIZE;
	LOGC(logINFO,"TVar") << "DataBuffer size: " << m_DataSize;
	for ( int i = 0; i < m_EntryIndex;++i ) {
		TVarEntry entry = (*(TVarEntry*)entryBuffer);
		DebugEntry de = m_DebugEntries[entry.name];
		if ( entry.flags & TVAR_FLOAT ) {
			char* data = m_DataBuffer + entry.offset;
			LOGC(logINFO,"TVar") << de.name << " = " << (*(float*)data);
		}	
		else if ( entry.flags & TVAR_INTEGER ) {
			char* data = m_DataBuffer + entry.offset;
			LOGC(logINFO,"TVar") << de.name << " = " <<  (*(int*)data);
		}
		else if ( entry.flags & TVAR_BOOL ) {
			char* data = m_DataBuffer + entry.offset;
			bool v = (*(bool*)data);
			if ( v ) {
				LOGC(logINFO,"TVar") << de.name << " = true";
			}
			else {
				LOGC(logINFO,"TVar") << de.name << " = false";
			}
		}	
		entryBuffer += TVAR_ENTRY_SIZE;
	}
}
#endif
}
