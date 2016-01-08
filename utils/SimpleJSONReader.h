#pragma once
#include <string.h>
#include "StringUtils.h"

namespace ds {

	class SimpleJSONReader {

		struct ValueBuffer {

			unsigned int* keys;
			int* categories;
			int* indices;
			int* sizes;
			int size;
			char* data;
			int capacity;

			struct DataBuffer {
				char* data;
				int size;
				int capacity;
				int num;

				DataBuffer() : data(0), size(0), capacity(0), num(0) {}

				void* alloc(int sz) {
					if (size + sz > capacity)
						return nullptr;
					auto res = data + size;
					size += sz;
					++num;
					return res;
				}

				float get(int index) {
					char* p = data + index * sizeof(float);
					float* v = (float*)(p);
					return *v;
				}
			};

			DataBuffer buffer;

			ValueBuffer() {
				capacity = 256;
				size = 0;
				data = new char[capacity * (sizeof(unsigned int) + sizeof(int) + sizeof(int) + sizeof(int))];
				keys = (unsigned int*)(data);
				categories = (int*)(keys + capacity);
				indices = (int*)(categories + capacity);
				sizes = (int*)(indices + capacity);
				buffer.size = 0;
				buffer.capacity = 512;
				buffer.data = new char[buffer.capacity];
			}

			~ValueBuffer() {
				delete[] buffer.data;
				delete[] data;
			}

			int create_property(const char* name, int category) {
				keys[size] = string::murmur_hash(name);
				categories[size] = category;
				indices[size] = buffer.num;
				sizes[size] = 0;
				++size;
				return size - 1;
			}

			void add(int pIndex, float value) {
				float* v = (float*)buffer.alloc(sizeof(float));
				*v = value;
				++sizes[pIndex];
			}
		};

		struct CategoryBuffer {

			unsigned int* hashes;
			int* parents;
			int* indices;
			char* data;
			int size;
			int capacity;
			char* names;
			int names_index;

			CategoryBuffer() {
				size = 0;
				capacity = 100;
				data = new char[capacity * (sizeof(unsigned int) + sizeof(int) + sizeof(int))];
				hashes = (unsigned int*)(data);
				parents = (int*)(hashes + capacity);
				indices = (int*)(parents + capacity);
				names = new char[1024];
				names_index = 0;
			}

			~CategoryBuffer() {
				delete[] names;
				delete[] data;
			}

			int add_category(const char* name, int parent) {
				// FIXME: check if we need to resize the buffer
				hashes[size] = string::murmur_hash(name);
				parents[size] = parent;
				indices[size] = names_index;
				++size;
				int l = strlen(name);
				for (int i = 0; i < l; ++i) {
					names[names_index++] = name[i];
				}
				names[names_index++] = '\0';
				return size - 1;
			}

		};

	public:
		SimpleJSONReader();
		~SimpleJSONReader();
		void parse(const char* fileName);
		int get_categories(int* result, int max, int parent = -1);
		bool matches(int category_id, const char* name);
		int get_int(int category_id, const char* name, int defaultValue = 0);
		float get_float(int category_id, const char* name, float defaultValue = 0.0f);
		const char* get_category_name(int category_id);
	private:
		char* _text;
		CategoryBuffer _category_buffer;
		ValueBuffer _value_buffer;

	};

}