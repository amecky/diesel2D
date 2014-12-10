/* 
 * File:   DataContainer.h
 * Author: meckya
 *
 * Created on 21. Juli 2014, 09:31
 */

#ifndef DATACONTAINER_H
#define	DATACONTAINER_H
#include <stdio.h>
#include <vector>
#include <assert.h>
#include "Bitset.h"
#include <Vector.h>
#include "..\renderer\render_types.h"
#include "..\lib\DataArray.h"

namespace ds {

	struct DataContainerElement {
		int offset;
		int type;
	};

#define DCEDECL_END() {-1,-1}

#define INDEX_MASK 0xffff
#define CHANNEL_MASK = 0xff
	const int MAX_CONTAINER_OBJECTS = 2048;
	const int MAX_CHANNELS = 256;

	// ------------------------------------------------
	// data types
	// ------------------------------------------------
	const int DAT_NONE    = 0;
	const int DAT_INT     = 1;
	const int DAT_FLOAT   = 2;
	const int DAT_VEC2    = 3;
	const int DAT_TEXTURE = 4;
	const int DAT_COLOR   = 5;

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
	private:
		uint32 m_Count;
	};

	template<class T>
	struct DataChannelTrait;

	template <>
	struct DataChannelTrait<int> {
		enum { TypeId = DAT_INT };
		static int getClearValue() { return 0; }
		static bool matchesType(int t) { return true; }
	};

	template <>
	struct DataChannelTrait<float> {
		enum { TypeId = DAT_FLOAT };
		static float getClearValue() { return 0.0f; }
		static bool matchesType(float t) { return true; }
	};

	template <>
	struct DataChannelTrait<Vector2f> {
		enum { TypeId = DAT_VEC2 };
		static Vector2f getClearValue() { return Vector2f(0, 0); }
		static bool matchesType(Vector2f t) { return true; }
	};

	template <>
	struct DataChannelTrait<Texture> {
		enum { TypeId = DAT_TEXTURE };
		static Texture getClearValue() { Texture t; return t; }
		static bool matchesType(Texture t) { return true; }
	};

	template <>
	struct DataChannelTrait<Color> {
		enum { TypeId = DAT_COLOR};
		static Color getClearValue() { Color c; return c; }
		static bool matchesType(Color t) { return true; }
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
	protected:
		T m_Data[MAX_CONTAINER_OBJECTS];
	};

	typedef Bitset<128> SystemBits;
	// ------------------------------------------------
	// data container
	// ------------------------------------------------
	class DataContainer {

		typedef std::vector<DataChannel*> Channels;

	public:
		DataContainer();
		virtual ~DataContainer();

		template<class T>
		uint32 addDataChannel() {
			uint32 ret = m_DataChannels.size();
			AbstractChannel<T>* channel = new AbstractChannel<T>();
			m_DataChannels.push_back(channel);
			return ret;
		}

		template<class T>
		void set(ID id, int channel, const T& v) {
			unsigned short index = indices[id & INDEX_MASK].index;
			DataChannel* c = m_DataChannels[channel];
			AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
			ic->set(index, v);
		}

		template<class T>
		const T& get(ID id, int channel) const {
			unsigned short index = indices[id & INDEX_MASK].index;
			DataChannel* c = m_DataChannels[channel];
			AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
			return ic->get(index);
		}

		template<class T>
		T& get(ID id, int channel) {
			unsigned short index = indices[id & INDEX_MASK].index;
			DataChannel* c = m_DataChannels[channel];
			AbstractChannel<T>* ic = static_cast<AbstractChannel<T>*>(c);
			return ic->get(index);
		}

		template<class T>
		T* getDataChannel(uint32 channel) {
			AbstractChannel<T>* c = static_cast<AbstractChannel<T>*>(m_DataChannels[channel]);
			return c->getData();
		}

		template<class T>
		const T* getDataChannel(uint32 channel) const {
			AbstractChannel<T>* c = static_cast<AbstractChannel<T>*>(m_DataChannels[channel]);
			return c->getData();
		}

		const int getDataType(int channel) const {
			return m_DataChannels[channel]->getType();
		}
		uint32 size();

		ID create();

		void remove(ID id);

		const bool contains(ID id) const;

		const int channels() const {
			return m_DataChannels.size();
		}

		SystemBits* getConnections() {
			return &connections[0];
		}

		void connect(ID id, uint32 systemIndex) {
			unsigned short index = indices[id & INDEX_MASK].index;
			connections[index].set(systemIndex);
		}

		const ID getID(int index) const {
			return idTable[index];
		}

		void disconnect(ID id, uint32 systemIndex) {
			unsigned short index = indices[id & INDEX_MASK].index;
			connections[index].clear(systemIndex);
		}

		const bool isConnected(ID id, uint32 systemIndex) const {
			unsigned short index = indices[id & INDEX_MASK].index;
			return connections[index].test(systemIndex);
		}

		void debug();
	private:
		DataContainer(const DataContainer& orig);
		uint32 numObjects;
		Index indices[MAX_CONTAINER_OBJECTS];
		ID idTable[MAX_CONTAINER_OBJECTS];
		SystemBits connections[MAX_CONTAINER_OBJECTS];
		ushort free_enqueue;
		ushort free_dequeue;
		Channels m_DataChannels;
	};

}
#endif	/* DATACONTAINER_H */

