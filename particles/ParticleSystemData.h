#pragma once

namespace ds {

	struct ParticleSystemDataTest {

		char* buffer;
		int index;
		int num;
		int entries[64];

		ParticleSystemDataTest() : buffer(0), index(0), num(0) {}

		~ParticleSystemDataTest() {
			if (buffer != 0) {
				delete[] buffer;
			}
		}

		void allocate(int size) {
			buffer = new char[size];
		}

		template<class T>
		const bool get(int id, T* t, int size) const {
			int idx = entries[id];
			char* data = buffer + idx;
			memcpy(t, data, size);
			return true;
		}

		template<class T>
		void set(int id, T* t, int size) {
			int idx = entries[id];
			char* data = buffer + idx;
			memcpy(data, t, size);
		}

		template<class T>
		int add(const T& t) {
			int current = index;
			entries[num++] = current;
			char* data = buffer + index;
			memcpy(data, &t, sizeof(t));
			index += sizeof(t);
			return current;
		}
	};

}