#include "SimpleJSONReader.h"
#include <stdio.h>
#include <string.h>
#include <stack>
#include "..\io\FileRepository.h"
#include "Log.h"

namespace ds {

struct Token {

	enum TokenType { EMPTY, NUMBER, NAME, DELIMITER, SEPARATOR, OPEN_BRACES, CLOSE_BRACES, STRING };

	Token() {}
	Token(TokenType type) : type(type) {}
	Token(TokenType type, float v) : type(type), value(v) {}
	Token(TokenType type, int i, int s) : type(type), index(i), size(s) {}

	TokenType type;
	float value;
	int index;
	int size;
};

const char* translate(const Token& token) {
	switch (token.type) {
	case Token::NAME: return "NAME"; break;
	case Token::NUMBER: return "NUMBER"; break;
	case Token::DELIMITER: return "DELIMITER"; break;
	case Token::SEPARATOR: return "SEPARATOR"; break;
	case Token::OPEN_BRACES: return "OPEN_BRACES"; break;
	case Token::CLOSE_BRACES: return "CLOSE_BRACES"; break;
	case Token::STRING: return "STRING"; break;
	default: return "UNKNOWN"; break;
	}
}

int tokenize(const char* text, Token* tokens, int maxTokens) {
	int cnt = 0;
	const char* p = text;
	while (*p != 0) {
		Token token(Token::EMPTY);
		if (*p >= '0' && *p <= '9') {
			char *out;
			token = Token(Token::NUMBER, string::strtof(p, &out));
			p = out;
		}
		else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_')) {
			const char *identifier = p;
			while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p == '_') || (*p >= '0' && *p <= '9'))
				p++;
			token = Token(Token::NAME, identifier - text, p - identifier);
		}
		else if (*p == '"') {
			const char *identifier = p + 1;
			++p;
			while (*p != '"') {
				p++;
			}
			token = Token(Token::STRING, identifier - text, p - identifier);
			++p;
		}
		else if (*p == '/') {
			++p;
			if (*p == '/') {
				++p;
				while (*p != '\n') {
					++p;
				}
			}
		}
		else {
			switch (*p) {
			case '{': token = Token(Token::OPEN_BRACES); break;
			case '}': token = Token(Token::CLOSE_BRACES); break;
			case ' ': case '\t': case '\n': case '\r': break;
			case ':': token = Token(Token::SEPARATOR); break;
			case ',': token = Token(Token::DELIMITER); break;

			}
			++p;
		}
		if (token.type != Token::EMPTY) {
			if (cnt < maxTokens) {
				tokens[cnt++] = token;
			}
		}
	}
	return cnt;
}





SimpleJSONReader::SimpleJSONReader() : _text(0) {
}


SimpleJSONReader::~SimpleJSONReader() {
	if (_text != 0) {
		delete[] _text;
	}
}

void save_buffer(CharBuffer* buffer, FILE* fp) {
	fwrite(&buffer->size, sizeof(int), 1, fp);
	fwrite(&buffer->capacity, sizeof(int), 1, fp);
	LOG << "#### >>>> size: " << buffer->size << " capacity: " << buffer->capacity;
	for (int i = 0; i < buffer->capacity; ++i) {
		fwrite(&buffer->data[i], sizeof(char), 1, fp);
	}
}

void load_buffer(CharBuffer* buffer, FILE* fp) {
	int size = 0;
	int capacity = 0;
	fread(&size, sizeof(int), 1, fp);
	fread(&capacity, sizeof(int), 1, fp);
	LOG << "#### <<<< size: " << size << " capacity: " << capacity;
	buffer->resize(capacity);
	for (int i = 0; i < capacity; ++i) {
		fread(&buffer->data[i], sizeof(char), 1, fp);
	}
	buffer->size = size;
}

void SimpleJSONReader::allocCategoryBuffer(int size) {
	int sz = size * (sizeof(unsigned int) + sizeof(int) + sizeof(int));
	CategoryDataBuffer db;
	db.data = (char*)gDefaultMemory->allocate(sz);
	db.capacity = size;
	db.size = 0;
	db.totalSize = sz;
	db.hashes = (unsigned int*)(db.data);
	db.parents = (int*)(db.hashes + size);
	db.indices = (int*)(db.parents + size);
	if (_category_buffer.data != 0) {
		memcpy(db.hashes, _category_buffer.hashes, _category_buffer.size * sizeof(unsigned int));
		memcpy(db.parents, _category_buffer.parents, _category_buffer.size * sizeof(int));
		memcpy(db.indices, _category_buffer.indices, _category_buffer.size * sizeof(int));
		db.size = _category_buffer.size;
		gDefaultMemory->deallocate(_category_buffer.data);
	}
	_category_buffer = db;
}

int SimpleJSONReader::add_category(const char* name, int parent) {
	if (_category_buffer.size + 1 > _category_buffer.capacity) {
		allocCategoryBuffer(_category_buffer.size * 2 + 8);
	}
	_category_buffer.hashes[_category_buffer.size] = string::murmur_hash(name);
	_category_buffer.parents[_category_buffer.size] = parent;
	_category_buffer.indices[_category_buffer.size] = _name_buffer.size;
	++_category_buffer.size;
	int l = strlen(name);
	if (_name_buffer.size + l > _name_buffer.capacity) {
		_name_buffer.resize(_name_buffer.capacity + 256);
	}
	_name_buffer.append(name, l);
	return _category_buffer.size - 1;
}

void SimpleJSONReader::save_binary(const char* fileName) {

	FILE* fp = fopen(fileName, "wb");
	if (fp) {
		// buffer

		fwrite(&_category_buffer.size, sizeof(int), 1, fp);
		for (int i = 0; i < _category_buffer.size; ++i) {
			fwrite(&_category_buffer.hashes[i], sizeof(unsigned int), 1, fp);
		}
		for (int i = 0; i < _category_buffer.size; ++i) {
			fwrite(&_category_buffer.parents[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < _category_buffer.size; ++i) {
			fwrite(&_category_buffer.indices[i], sizeof(int), 1, fp);
		}
		save_buffer(&_name_buffer, fp);

		fwrite(&_data_buffer.size, sizeof(int), 1, fp);
		for (int i = 0; i < _values.size; ++i) {
			fwrite(&_data_buffer.keys[i], sizeof(unsigned int), 1, fp);
		}
		for (int i = 0; i < _data_buffer.size; ++i) {
			fwrite(&_data_buffer.categories[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < _data_buffer.size; ++i) {
			fwrite(&_data_buffer.indices[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < _data_buffer.size; ++i) {
			fwrite(&_data_buffer.sizes[i], sizeof(int), 1, fp);
		}
	
		save_buffer(&_values, fp);
		fclose(fp);
	}
}

bool SimpleJSONReader::load_binary(const char* fileName) {
	FILE* fp = fopen(fileName, "rb");
	if (fp) {
		int size = 0;
		fread(&size, sizeof(int), 1, fp);
		allocCategoryBuffer(size);
		for (int i = 0; i < size; ++i) {
			fread(&_category_buffer.hashes[i], sizeof(unsigned int), 1, fp);
		}
		for (int i = 0; i < size; ++i) {
			fread(&_category_buffer.parents[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < size; ++i) {
			fread(&_category_buffer.indices[i], sizeof(int), 1, fp);
		}
		_category_buffer.size = size;

		load_buffer(&_name_buffer, fp);

		fread(&size, sizeof(int), 1, fp);
		alloc(size);
		for (int i = 0; i < size; ++i) {
			fread(&_data_buffer.keys[i], sizeof(unsigned int), 1, fp);
		}
		for (int i = 0; i < size; ++i) {
			fread(&_data_buffer.categories[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < size; ++i) {
			fread(&_data_buffer.indices[i], sizeof(int), 1, fp);
		}
		for (int i = 0; i < size; ++i) {
			fread(&_data_buffer.sizes[i], sizeof(int), 1, fp);
		}
		_data_buffer.size = size;
		load_buffer(&_values, fp);
		return true;
	}
	return false;
}

bool SimpleJSONReader::parse(const char* fileName) {
	Token tokens[1024];
	int fileSize = -1;
	_text = repository::load(fileName, &fileSize);
	if (fileSize == -1) {
		return false;
	}
	int cnt = tokenize(_text, tokens, 1024);
	//LOG << "Tokens found: " << cnt;
	//for (int i = 0; i < cnt; ++i) {
		//LOG << i << " = " << translate(tokens[i]);
	//}
	char name[128];
	int n = 0;
	int category_index = 0;
	int cat = -1;
	//std::stack<int> cat_stack;
	Stack<int> cat_stack;
	while (n < cnt) {
		Token t = tokens[n];
		if (t.type == Token::NAME) {
			if (tokens[n + 1].type == Token::OPEN_BRACES) {
				++n;
				strncpy(name, _text + t.index, t.size);
				name[t.size] = '\0';
				++category_index;
				int parent = -1;
				if (!cat_stack.empty()) {
					parent = cat_stack.top();
				}
				//LOG << "--> category: " << name;
				cat = add_category(name, parent);
				cat_stack.push(cat);
				++n;
			}
			else if (tokens[n + 1].type == Token::SEPARATOR) {
				++n;
				strncpy(name, _text + t.index, t.size);
				name[t.size] = '\0';
				int parent = -1;
				if (!cat_stack.empty()) {
					parent = cat_stack.top();
				}
				int p = create_property(name, parent);
				++n;
				Token v = tokens[n];
				if (v.type == Token::STRING) {
					int start = v.index;
					int end = v.index + v.size;
					// alloc v.size align 4
					add(p, _text + start, v.size);
					/*
					for (int i = start; i < end; ++i) {
						_value_buffer.add(p, _text[i]);
					}
					_value_buffer.add(p, '\0');
					*/
					++n;
				}
				else {
					bool parsing = true;
					while (parsing) {
						if (v.type == Token::NUMBER) {
							add(p, v.value);
							if (tokens[n + 1].type != Token::DELIMITER) {
								parsing = false;
							}
						}
						++n;
						v = tokens[n];

					}
				}
			}
		}

		else if (t.type == Token::CLOSE_BRACES) {
			if (!cat_stack.empty()) {
				cat_stack.pop();
			}
			--category_index;
			++n;
		}
		t = tokens[n];		
	}
	return true;
	//for (int i = 0; i < _data_buffer.size; ++i) {
		//LOG << i << " index: " << _value_buffer.indices[i] << " size: " << _value_buffer.sizes[i] << " category: " << _value_buffer.categories[i];
	//}
}

int SimpleJSONReader::get_categories(int* result, int max, int parent) {
	int cnt = 0;
	for (int i = 0; i < _category_buffer.size; ++i) {
		if (_category_buffer.parents[i] == parent && cnt < max) {
			result[cnt++] = i;
		}
	}
	return cnt;
}

const char* SimpleJSONReader::get_category_name(int category_id) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		int index = _category_buffer.indices[category_id];
		//return _category_buffer.names + index;
		return _name_buffer.data + index;
	}
	return '\0';
}

bool SimpleJSONReader::matches(int category_id, const char* name) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int hash = string::murmur_hash(name);
		if (_category_buffer.hashes[category_id] == hash) {
			return true;
		}
	}
	return false;
}

bool SimpleJSONReader::get_int(int category_id, const char* name, int* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				float* v = (float*)(_values.data + _data_buffer.indices[i]);
				*ret = static_cast<int>(*v);
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_float(int category_id, const char* name, float* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				*ret = get(_data_buffer.indices[i]);
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_vec2(int category_id, const char* name, v2* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				ret->x = get(_data_buffer.indices[i]);
				ret->y = get(_data_buffer.indices[i] + 1);
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_color(int category_id, const char* name, Color* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				ret->r = get(_data_buffer.indices[i]) / 255.0f;
				ret->g = get(_data_buffer.indices[i] + 1) / 255.0f;
				ret->b = get(_data_buffer.indices[i] + 2) / 255.0f;
				ret->a = get(_data_buffer.indices[i] + 3) / 255.0f;
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_rect(int category_id, const char* name, Rect* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				ret->top = get(_data_buffer.indices[i]);
				ret->left = get(_data_buffer.indices[i] + 1);
				ret->right = ret->left + get(_data_buffer.indices[i] + 2);
				ret->bottom = ret->top + get(_data_buffer.indices[i] + 3);
				return true;
			}
		}
	}
	return false;
}

const char* SimpleJSONReader::get_string(int category_id, const char* name) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _data_buffer.size; ++i) {
			if (_data_buffer.categories[i] == category_id && _data_buffer.keys[i] == key) {
				return get_char(_data_buffer.indices[i]);
			}
		}
	}
	return '\0';
}

void SimpleJSONReader::alloc(int elements) {
	int sz = elements * (sizeof(unsigned int) + sizeof(int) + sizeof(int) + sizeof(int));
	DataBuffer db;
	db.data = (char*)gDefaultMemory->allocate(sz);
	db.capacity = elements;
	db.size = 0;
	db.totalSize = sz;
	db.keys = (unsigned int*)(db.data);
	db.categories = (int*)(db.keys + elements);
	db.indices = (int*)(db.categories + elements);
	db.sizes = (int*)(db.indices + elements);
	if (_data_buffer.data != 0) {
		memcpy(db.keys, _data_buffer.keys, _data_buffer.size * sizeof(unsigned int));
		memcpy(db.categories, _data_buffer.categories, _data_buffer.size * sizeof(int));
		memcpy(db.indices, _data_buffer.indices, _data_buffer.size * sizeof(int));
		memcpy(db.sizes, _data_buffer.sizes, _data_buffer.size * sizeof(int));
		db.size = _data_buffer.size;
		gDefaultMemory->deallocate(_data_buffer.data);
	}
	_data_buffer = db;
}

int SimpleJSONReader::create_property(const char* name, int category) {
	if (_data_buffer.size + 1 > _data_buffer.capacity) {
		alloc(_data_buffer.size * 2 + 8);
	}
	_data_buffer.keys[_data_buffer.size] = string::murmur_hash(name);
	_data_buffer.categories[_data_buffer.size] = category;
	_data_buffer.indices[_data_buffer.size] = _values.num;
	_data_buffer.sizes[_data_buffer.size] = 0;
	++_data_buffer.size;
	return _data_buffer.size - 1;
}

void SimpleJSONReader::add(int pIndex, float value) {
	float* v = (float*)_values.alloc(sizeof(float));
	*v = value;
	++_data_buffer.sizes[pIndex];
}

void SimpleJSONReader::add(int pIndex, const char* c, int len) {
	int sz = ((len + 3) / 4) * 4;
	char* v = (char*)_values.alloc(sz * sizeof(char));
	for (int i = 0; i < len; ++i) {
		*v = c[i];
		++v;
	}
	*v = '\0';
	_data_buffer.sizes[pIndex] = sz / 4;
}

void SimpleJSONReader::add(int pIndex, char c) {
	char* v = (char*)_values.alloc(sizeof(char));
	*v = c;
	++_data_buffer.sizes[pIndex];
}

float SimpleJSONReader::get(int index) {
	char* p = _values.data + index * sizeof(float);
	float* v = (float*)(p);
	return *v;
}

const char* SimpleJSONReader::get_char(int index) {
	return _values.data + index * sizeof(float);
}

}