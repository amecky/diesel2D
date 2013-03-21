#pragma once
#include "..\dxstdafx.h"
#include "..\lib\container\List.h"
#include "..\math\math_types.h"
#include "..\pipeline\PAKWriter.h"
#include "..\pipeline\PAKReader.h"
#include <map>
#include "..\utils\Color.h"

namespace ds {

enum TVarFlags {
	TVAR_INTEGER   = BIT(1),
	TVAR_FLOAT     = BIT(2),
	TVAR_VEC2      = BIT(3),
	TVAR_VEC3      = BIT(4),
	TVAR_BOOL      = BIT(5),
	TVAR_COLOR     = BIT(6),
	TVAR_ARRAY     = BIT(7),
	TVAR_ARCHIVE   = BIT(10),
	TVAR_READ_ONLY = BIT(11)
};

// --------------------------------------
//
// --------------------------------------
struct FloatArray {
	uint32 size;
};

// --------------------------------------
//
// --------------------------------------
class TVar {

public:
	TVar(const char* name,const char* value,int flags);
	~TVar(void);
	float getFloat();
	int getInt();
	bool getBool();
	const Color& getColor() const;
	const Vec2& getVec2() const;
	const Vec3& getVec3() const;
	float getArray(uint32 offset);
	const IdString& getHashName() const {
		return m_HashName;
	}
	int getFlags() {
		return m_Flags;
	}
	uint32 size();
	void setData(char* buffer,uint32 size);
private:	
	char* m_Data;
	uint32 m_Size;
	int m_Flags;
	IdString m_HashName;
};

// --------------------------------------
//
// --------------------------------------
struct TVarEntry {
	IdString name;
	uint32 offset;
	int flags;
};

// --------------------------------------
//
// --------------------------------------
class TVarSystem {

#ifdef DEBUG
struct DebugEntry {
	char name[32];
	IdString hashName;
};

typedef std::map<IdString,DebugEntry> DebugEntries;
#endif



public:
	TVarSystem();
	~TVarSystem();
	void add(TVar* tvar,const char* name,const char* value,bool updateOnly = false);
	void saveBinaryFile(PAKWriter& writer);
	void loadBinFile(PAKReader& reader);
#ifdef DEBUG
	void debug();
#endif
	bool contains(const char* name);
	bool update(const char* name,const char* value);
private:
	uint32 setData(uint32 offset,const char* value,int flags);
	float getFloat(const char* value,int index);
	char* m_EntryBuffer;
	char* m_DataBuffer;
	uint32 m_EntryIndex;
	uint32 m_DataSize;
#ifdef DEBUG
	DebugEntries m_DebugEntries;
#endif
};

extern TVarSystem* gTVarSystem;

}

