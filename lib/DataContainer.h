#pragma once
#include "DataArray.h"
#include "..\renderer\render_types.h"
#include "..\utils\Color.h"

namespace ds {

struct DataContainerElement {
	int type;
	int offset;	
	char* name;
};

#define DCEDECL_END() {-1,-1,"END"}

#define INDEX_MASK 0xffff
#define CHANNEL_MASK = 0xff

// ------------------------------------------------
// data types
// ------------------------------------------------
const int DAT_NONE    = 0;
const int DAT_INT     = 1;
const int DAT_FLOAT   = 2;
const int DAT_VEC2    = 3;
const int DAT_VEC3    = 4;
const int DAT_TEXTURE = 5;
const int DAT_COLOR   = 6;

// ------------------------------------------------
// data channel
// ------------------------------------------------
class DataChannel {

public:
	DataChannel() : m_Count(0) {}
	virtual ~DataChannel() {}

	const uint32 size() const {
		return m_Count;
	}

	virtual void swap(uint32 first, uint32 second) {}

	virtual void clear(uint32 index) {}

	virtual int getType() {
		return DAT_NONE;
	}
	void setName(const char* name) {
		m_Name = name;
	}
	const char* getName() const {
		return m_Name;
	}
private:
	uint32 m_Count;
	const char* m_Name;
};

template<class T>
struct DataChannelTrait;

template <>
struct DataChannelTrait<int> {
	enum { TypeId = DAT_INT };
	static int getClearValue() { return 0; }
	static int getSize() { return sizeof(int); }
	static bool matchesType(int t) { return true; }
};

template <>
struct DataChannelTrait<float> {
	enum { TypeId = DAT_FLOAT };
	static float getClearValue() { return 0.0f; }
	static int getSize() { return sizeof(float); }
	static bool matchesType(float t) { return true; }
};

template <>
struct DataChannelTrait<Vector2f> {
	enum { TypeId = DAT_VEC2 };
	static Vector2f getClearValue() { return Vector2f(0, 0); }
	static int getSize() { return sizeof(Vector2f); }
	static bool matchesType(Vector2f t) { return true; }
};

template <>
struct DataChannelTrait<ds::Texture> {
	enum { TypeId = DAT_TEXTURE };
	static ds::Texture getClearValue() { ds::Texture t; return t; }
	static int getSize() { return sizeof(ds::Texture); }
	static bool matchesType(ds::Texture t) { return true; }
};

template <>
struct DataChannelTrait<ds::Color> {
	enum { TypeId = DAT_COLOR };
	static ds::Color getClearValue() { ds::Color c; return c; }
	static int getSize() { return sizeof(ds::Color); }
	static bool matchesType(ds::Color t) { return true; }
};
// ------------------------------------------------
// templated channel
// ------------------------------------------------
template<class T>
class AbstractChannel : public DataChannel {

public:

	typedef DataChannelTrait<T> Traits;

	AbstractChannel() : DataChannel() {}
	virtual ~AbstractChannel() {}
	void set(uint32 index, const T& v) {
		assert(Traits::matchesType(v));
		m_Data[index] = v;
	}
	const T& get(uint32 index) const {
		return m_Data[index];
	}
	T& get(uint32 index) {
		return m_Data[index];
	}
	void swap(uint32 first, uint32 second) {
		m_Data[first] = m_Data[second];
	}
	T* getData() {
		return &m_Data[0];
	}
	void clear(uint32 index) {
		m_Data[index] = Traits::getClearValue();
	}
	virtual int getType() {
		return Traits::TypeId;
	}
	bool matchesType(const T& v) {
		return Traits::matchesType(v);
	}
	void setPtr(T* ptr) {
		m_Data = ptr;
	}
protected:
	T* m_Data;
};

// ------------------------------------------------
// data container
// ------------------------------------------------

class DataContainer {

	typedef std::vector<DataChannel*> Channels;

public:
	DataContainer();
	virtual ~DataContainer();

	void init(const DataContainerElement* elements, int size);
	/*
	template<class T>
	uint32 addDataChannel() {
		uint32 ret = m_DataChannels.size();
		AbstractChannel<T>* channel = new AbstractChannel<T>();
		m_DataChannels.push_back(channel);
		return ret;
	}
	*/
	template<class T>
	void set(ID id, int channel, const T& v) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		unsigned short index = in.index;
		DataChannel* c = m_DataChannels[channel];
		AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
		ic->set(index, v);
	}

	template<class T>
	const T& get(ID id, int channel) const {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		unsigned short index = in.index;
		DataChannel* c = m_DataChannels[channel];
		AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
		return ic->get(index);
	}

	template<class T>
	T& get(ID id, int channel) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		unsigned short index = in.index;
		DataChannel* c = m_DataChannels[channel];
		AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
		return ic->get(index);
	}

	template<class T>
	T* getChannel(uint32 channel) {
		AbstractChannel<T>* c = static_cast<AbstractChannel<T>*>(m_DataChannels[channel]);
		return c->getData();
	}

	template<class T>
	const T* getChannel(uint32 channel) const {
		AbstractChannel<T>* c = static_cast<AbstractChannel<T>*>(m_DataChannels[channel]);
		return c->getData();
	}

	const int getDataType(int channel) const {
		return m_DataChannels[channel]->getType();
	}

	const char* getName(int channel) const {
		return m_DataChannels[channel]->getName();
	}
	const uint32 size() const;

	const int maxSize() const {
		return maxEntries;
	}

	ID create();

	void remove(ID id);

	const bool contains(ID id) const;

	const int channels() const {
		return m_DataChannels.size();
	}

	const ID getID(int index) const {
		return idTable[index];
	}

	const int getIndex(ID id) const {
		assert(m_Initialized);
		const Index& in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		return in.index;
	}
	
	void debug();

	void clear();

private:
	DataContainer(const DataContainer& orig);
	uint32 numObjects;
	int maxEntries;
	Index* indices;
	ID* idTable;
	ushort free_enqueue;
	ushort free_dequeue;
	Channels m_DataChannels;
	char* m_Buffer;
	bool m_Initialized;
};


}