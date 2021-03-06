#pragma once
#include <string.h>
#include "StringUtils.h"
#include "Color.h"
#include "..\memory\DefaultAllocator.h"
#include "..\math\FloatArray.h"
#include "..\lib\BlockArray.h"

namespace ds {

	class SimpleJSONReader {

	public:
		SimpleJSONReader();
		~SimpleJSONReader();
		bool parse(const char* fileName);
		void save_binary(const char* fileName);
		bool load_binary(const char* fileName);
		int get_categories(int* result, int max, int parent = -1);
		int find_category(const char* name, int parent = -1);
		bool matches(int category_id, const char* name);
		bool contains_property(int category_id, const char* name);
		bool get_int(int category_id, const char* name, int* ret);
		bool get_bool(int category_id, const char* name, bool* ret);
		bool get_uint(int category_id, const char* name, uint32* ret);
		bool get_float(int category_id, const char* name, float* ret);
		bool get_vec2(int category_id, const char* name, v2* ret);
		bool get_vec3(int category_id, const char* name, v3* ret);
		bool get_color(int category_id, const char* name, Color* ret);
		bool get_rect(int category_id, const char* name, Rect* ret);
		bool get_color_path(int category_id, const char* name, ds::ColorPath* path);
		bool get_vec2_path(int category_id, const char* name, ds::Vector2fPath* path);
		const char* get_string(int category_id, const char* name);
		const char* get_category_name(int category_id);
	private:
		int get_index(int category_id, const char* name);
		void allocCategoryBuffer(int size);
		int add_category(const char* name, int parent);
		void alloc(int elements);
		int create_property(const char* name, int category);
		void add(int pIndex, float value);
		void add(int pIndex, const char* c, int len);
		void add(int pIndex, char c);
		float get(int index);
		const char* get_char(int index);
		char* _text;

		BlockArray _category_buffer;
		unsigned int* _hashes;
		int* _parents;
		int* _indices;

		BlockArray _data_buffer;
		unsigned int* _data_keys;
		int* _data_categories;
		int* _data_indices;
		int* _data_sizes;

		CharBuffer _name_buffer;
		CharBuffer _values;

	};

}