#pragma once
#include <string.h>
#include "StringUtils.h"
#include "Color.h"
#include "..\memory\DefaultAllocator.h"

namespace ds {

	
	enum JSONDataType {
		JDT_FLOAT,
		JDT_STRING,
		JDT_ARRAY,
		JDT_EOL
	};

	class SimpleJSONReader {

		struct ValueBuffer {

			unsigned int* keys;
			int* categories;
			int* indices;
			int* sizes;
			CharBuffer data;
			CharBuffer buffer;

			ValueBuffer() {
				data.resize(256 * (sizeof(unsigned int) + sizeof(int) + sizeof(int) + sizeof(int)));
				keys = (unsigned int*)(data.data);
				categories = (int*)(keys + 256);
				indices = (int*)(categories + 256);
				sizes = (int*)(indices + 256);
				buffer.size = 0;
				buffer.resize(512);
			}

			~ValueBuffer() {
			}

			int create_property(const char* name, int category) {
				keys[data.size] = string::murmur_hash(name);
				categories[data.size] = category;
				indices[data.size] = buffer.num;
				sizes[data.size] = 0;
				++data.num;
				++data.size;
				return data.size - 1;
			}

			void add(int pIndex, float value) {
				float* v = (float*)buffer.alloc(sizeof(float));
				*v = value;
				++sizes[pIndex];
			}

			void add(int pIndex, const char* c, int len) {
				int sz = ((len + 3) / 4) * 4;
				char* v = (char*)buffer.alloc(sz * sizeof(char));
				for (int i = 0; i < len; ++i) {
					*v = c[i];
					++v;
				}
				*v = '\0';
				sizes[pIndex] = sz / 4;
			}

			void add(int pIndex, char c) {
				char* v = (char*)buffer.alloc(sizeof(char));
				*v = c;
				++sizes[pIndex];
			}

			float get(int index) {
				char* p = buffer.data + index * sizeof(float);
				float* v = (float*)(p);
				return *v;
			}

			const char* get_char(int index) {
				return buffer.data + index * sizeof(float);
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
		bool parse(const char* fileName);
		int get_categories(int* result, int max, int parent = -1);
		bool matches(int category_id, const char* name);
		bool get_int(int category_id, const char* name, int* ret);
		bool get_float(int category_id, const char* name, float* ret);
		bool get_vec2(int category_id, const char* name, v2* ret);
		bool get_color(int category_id, const char* name, Color* ret);
		bool get_rect(int category_id, const char* name, Rect* ret);
		const char* get_string(int category_id, const char* name);
		const char* get_category_name(int category_id);
	private:
		char* _text;
		CategoryBuffer _category_buffer;
		ValueBuffer _value_buffer;

	};

}