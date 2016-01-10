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





SimpleJSONReader::SimpleJSONReader() {
}


SimpleJSONReader::~SimpleJSONReader() {
	delete[] _text;
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
				cat = _category_buffer.add_category(name, parent);
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
				int p = _value_buffer.create_property(name, parent);
				++n;
				Token v = tokens[n];
				if (v.type == Token::STRING) {
					int start = v.index;
					int end = v.index + v.size;
					// alloc v.size align 4
					_value_buffer.add(p, _text + start, v.size);
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
							_value_buffer.add(p, v.value);
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
	//for (int i = 0; i < _value_buffer.data.size; ++i) {
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
		return _category_buffer.names + index;
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
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				float* v = (float*)(_value_buffer.buffer.data + _value_buffer.indices[i]);
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
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				*ret = _value_buffer.get(_value_buffer.indices[i]);
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_vec2(int category_id, const char* name, v2* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				ret->x = _value_buffer.get(_value_buffer.indices[i]);
				ret->y = _value_buffer.get(_value_buffer.indices[i] + 1);
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_color(int category_id, const char* name, Color* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				ret->r = _value_buffer.get(_value_buffer.indices[i]) / 255.0f;
				ret->g = _value_buffer.get(_value_buffer.indices[i] + 1) / 255.0f;
				ret->b = _value_buffer.get(_value_buffer.indices[i] + 2) / 255.0f;
				ret->a = _value_buffer.get(_value_buffer.indices[i] + 3) / 255.0f;
				return true;
			}
		}
	}
	return false;
}

bool SimpleJSONReader::get_rect(int category_id, const char* name, Rect* ret) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				ret->top = _value_buffer.get(_value_buffer.indices[i]);
				ret->left = _value_buffer.get(_value_buffer.indices[i] + 1);
				ret->right = ret->left + _value_buffer.get(_value_buffer.indices[i] + 2);
				ret->bottom = ret->top + _value_buffer.get(_value_buffer.indices[i] + 3);
				return true;
			}
		}
	}
	return false;
}

const char* SimpleJSONReader::get_string(int category_id, const char* name) {
	if (category_id >= 0 && category_id < _category_buffer.size) {
		unsigned int key = string::murmur_hash(name);
		for (int i = 0; i < _value_buffer.data.size; ++i) {
			if (_value_buffer.categories[i] == category_id && _value_buffer.keys[i] == key) {
				return _value_buffer.get_char(_value_buffer.indices[i]);
			}
		}
	}
	return '\0';
}

}