#include "PlainTextReader.h"
#include <stack>
#include "StringUtils.h"
#include "Log.h"
#include "StringUtils.h"
#include "..\io\FileRepository.h"

Category::Category(const std::string& name) : m_Name(name) { }

Category::~Category() {
	for (size_t i = 0; i < m_Entries.size(); ++i) {
		delete m_Entries[i];
	}
	ds::Array<Category*>::iterator it = m_Children.begin();
	while (it != m_Children.end()) {
		delete *(it);
		it = m_Children.remove(it);
	}
}

const std::string& Category::getName() const {
	return m_Name;
}

const char* Category::getPropertyName(int index) const {
	return m_Entries[index]->name;
}

const size_t Category::numProperties() const {
	return m_Entries.size();
}

void Category::addProperty(const char* name, const char* value) {
	SectionEntry* entry = new SectionEntry;
	entry->hash = ds::string::murmur_hash(name);
	sprintf_s(entry->str, 256, "%s", value);
	strcpy(entry->str, value);
	// determine type 
	int dels = ds::string::count_delimiters(value, ',');
	if (dels == 0) {
		if (ds::string::isNumeric(value)) {
			entry->values[0] = static_cast<float>(atof(value));
			entry->count = 1;
			entry->type = ST_FLOAT;
		}		
		else {
			entry->type = ST_STRING;
			entry->count = 0;			
		}
	}
	else {
		entry->count = ds::string::split(value, entry->values, 4, ',');
	}
	sprintf_s(entry->name, 32, "%s", name);
	m_Entries.push_back(entry);
}

const char* Category::getProperty(const char* name) const {
	IdString hash = ds::string::murmur_hash(name);
	for (size_t i = 0; i < m_Entries.size(); ++i) {
		if (m_Entries[i]->hash == hash) {
			return m_Entries[i]->str;
		}
	}
	return 0;
}

Vector2f Category::getVector2f(const char* name, const Vector2f& defaultValue) {
	if (hasProperty(name)) {
		float x = getFloat(0, name);
		float y = getFloat(1, name);
		return Vector2f(x, y);
	}
	return defaultValue;
}
void Category::getVector2f(const char* name, Vector2f* ret) {
	if (hasProperty(name)) {
		ret->x = getFloat(0, name);
		ret->y = getFloat(1, name);
	}
}
void Category::getIdString(const char* name, IdString* ret) {
	if (hasProperty(name)) {
		const char* s = getProperty(name);
		if (s != 0) {
			*ret = ds::string::murmur_hash(s);
		}
	}
}
Vector3f Category::getVector3f(const char* name) {
	float x = getFloat(0, name);
	float y = getFloat(1, name);
	float z = getFloat(2, name);
	return Vector3f(x, y, z);
}
Vector3f Category::getVector3f(const char* name, const Vector3f& defaultValue) {
	if (hasProperty(name)) {
		float x = getFloat(0, name);
		float y = getFloat(1, name);
		float z = getFloat(2, name);
		return Vector3f(x, y, z);
	}
	return defaultValue;
}
ds::Color Category::getColor(const char* name, const ds::Color& defaultValue) {
	if (hasProperty(name)) {
		assert(getElementCount(name) == 4);
		int r = getInt(0, name);
		int g = getInt(1, name);
		int b = getInt(2, name);
		int a = getInt(3, name);
		return ds::Color(r, g, b, a);
	}
	return defaultValue;
}

void Category::getColor(const char* name, ds::Color* color) {
	if (hasProperty(name)) {
		assert(getElementCount(name) == 4);
		int r = getInt(0, name);
		int g = getInt(1, name);
		int b = getInt(2, name);
		int a = getInt(3, name);
		color->r = static_cast<float>(r) / 255.0f;
		color->g = static_cast<float>(g) / 255.0f;
		color->b = static_cast<float>(b) / 255.0f;
		color->a = static_cast<float>(a) / 255.0f;
	}
}
ds::Rect Category::getRect(const char* name) {
	float top = static_cast<float>(getInt(0, name));
	float left = static_cast<float>(getInt(1, name));
	float width = static_cast<float>(getInt(2, name));
	float height = static_cast<float>(getInt(3, name));
	return ds::Rect(top, left, width, height);
}
void Category::getRect(const char* name, ds::Rect* rect) {
	if (hasProperty(name)) {
		rect->top = static_cast<float>(getInt(0, name));
		rect->left = static_cast<float>(getInt(1, name));
		rect->right = rect->left + static_cast<float>(getInt(2, name));
		rect->bottom = rect->top + static_cast<float>(getInt(3, name));
	}
}

bool Category::getBool(const char* name, bool defaultValue) {
	if (hasProperty(name)) {
		const char* s = getProperty(name);
		if (s != 0) {
			if (strcmp(s,"true") == 0) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return defaultValue;
		}
	}
	else {
		return defaultValue;
	}
}

void Category::getBool(const char* name, bool* ret) {
	if (hasProperty(name)) {
		const char* s = getProperty(name);
		if (s != 0) {
			if (strcmp(s,"true") == 0) {
				*ret = true;
			}
			else {
				*ret = false;
			}
		}
	}
}

void Category::getColorPath(ds::ColorPath* path) {
	ds::Color value(1.0f, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < numProperties(); ++i) {
		const char* propertyName = getPropertyName(i);
		float timeStep = atof(propertyName);
		value = getColor(propertyName);
		path->add(timeStep, value);
	}
}

bool Category::hasProperty(const char* name) const {
	IdString hash = ds::string::murmur_hash(name);
	for (size_t i = 0; i < m_Entries.size(); ++i) {
		if (m_Entries[i]->hash == hash) {
			return true;
		}
	}
	return false;
}

void Category::addCategory(Category* cat) {
	m_Children.push_back(cat);
}

const ds::Array<Category*>& Category::getChildren() const {
	return m_Children;
}

Category* Category::getChild(const char* name) const {
	for (size_t i = 0; i < m_Children.size(); ++i) {
		if (m_Children[i]->getName() == name) {
			return m_Children[i];
		}
	}
	return 0;
}

void Category::getFloat(const char* name, float* ret) {
	if (hasProperty(name)) {
		*ret = getFloat(0, name);
	}
}
float Category::getFloat(const char* name, float defaultValue) {
	if (hasProperty(name)) {
		return getFloat(0, name);
	}
	return defaultValue;
}

uint32 Category::getUInt32(const char* name, uint32 defaultValue) {
	if (hasProperty(name)) {
		return static_cast<uint32>(getInt(0, name));
	}
	return defaultValue;
}
float Category::getFloat(int index, const char* name) {
	SectionEntry* entry = getEntry(name);
	if (entry != 0) {
		if (entry->count > index) {
			return entry->values[index];
		}
	}
	return 0.0f;
}
void Category::getInt(const char* name, int* value) {
	if (hasProperty(name)) {
		*value = getInt(0, name);
	}
}
int Category::getInt(const char* name, int defaultValue) {
	if (hasProperty(name)) {
		return getInt(0, name);
	}
	return defaultValue;
}
void Category::getInt(const char* name, uint32* value) {
	if (hasProperty(name)) {
		*value = static_cast<uint32>(getInt(0, name));
	}
}

int Category::getInt(int index, const char* name) {
	SectionEntry* entry = getEntry(name);
	if (entry != 0) {
		if (entry->count > index) {
			return static_cast<int>(entry->values[index]);
		}
	}
	return 0;
}

int Category::getElementCount(const char* name) {
	SectionEntry* entry = getEntry(name);
	if (entry != 0) {
		return entry->count;
	}
	return -1;
}

SectionEntry* Category::getEntry(const char* name) const {
	IdString hash = ds::string::murmur_hash(name);
	for (size_t i = 0; i < m_Entries.size(); ++i) {
		if (m_Entries[i]->hash == hash) {
			return m_Entries[i];
		}
	}
	return 0;
}
// ------------------------------------------
// JSON reader
// ------------------------------------------
JSONReader::JSONReader() {
}

JSONReader::~JSONReader() {
	ds::Array<Category*>::iterator it = m_Categories.begin();
	while ( it != m_Categories.end()) {
		delete (*it);
		it = m_Categories.remove(it);
	}
}

// -------------------------------------------------------
// Get specific category
// -------------------------------------------------------
Category* JSONReader::getCategory(const std::string& name) {
	for ( size_t i = 0; i < m_Categories.size(); ++i ) {
		Category* cat = m_Categories[i];
		if ( cat->getName() == name ) {
			return cat;
		}
	}
	return 0;
}

// -------------------------------------------------------
// Clear all internal data
// -------------------------------------------------------
void JSONReader::clear() {
	ds::Array<Category*>::iterator it = m_Categories.begin();
	while ( it != m_Categories.end() ) {
		delete (*it);
		it = m_Categories.remove(it);
	}
}
// -------------------------------------------------------
// Parse JSON file
// -------------------------------------------------------
bool JSONReader::parse(const char* fileName) {
	clear();
	int size = -1;
	char* text = ds::repository::load(fileName, &size);
	if (size != -1) {
		LOG << "Parsing " << fileName;
		char* ptr = text;
		char* name = 0;
		char* value = 0;
		bool naming = true;
		int bracketCounter = 0;
		std::stack<Category*> stack;
		while ( *ptr ) {
			if ( *ptr == '\t' ) {
				++ptr;
			}
			else if ( *ptr == '/' ) {				
				++ptr;
				if ( *ptr == '*' ) {
					while ( *ptr != '/' ) {
						++ptr;
					}
				}
			}
			else if ( *ptr == '{' && name != 0 ) {
				Category* c = new Category(name);
				if ( !stack.empty() ) {
					Category* parent = stack.top();
					parent->addCategory(c);
				}
				if ( bracketCounter == 0 ) {
					m_Categories.push_back(c);
				}
				stack.push(c);
				++bracketCounter;
				++ptr;
				naming = true;
			}
			else if ( *ptr == '}' ) {
				--bracketCounter;
				++ptr;
				if ( !stack.empty() ) {
					stack.pop();
				}
				naming = true;
			}
			else if ( *ptr == '"' ) {
				++ptr;
				char* first = ptr;
				char* last = ptr;
				while ( *ptr ) {
					if ( *ptr == '"' ) {
						*last = 0;
						++ptr;
						break;
					}
					else {
						*last++ = *ptr++;
					}
				}
				if ( naming ) {
					name = first;
					naming = false;
				}
				else {
					value = first;
					naming = true;
				}
				if ( name != 0 && value != 0 ) {
					Category* c = stack.top();				
					c->addProperty(name,value);
					stack.top() = c;
					name = 0;
					value = 0;
				}
			}
			else {
				++ptr;
			}
		}
		if (text != 0) {
			delete[] text;
		}
		return true;
	}
	return false;
}



namespace json {

	Category* read_simplified_json(const char* file_name) {
		int size = -1;
		char* text = ds::repository::load(file_name, &size);
		if ( size != -1 ) {
			Category* root = new Category("root");
			LOG << "read_simplified_json - parsing " << file_name;
			char* ptr = text;
			char* name = 0;
			char* value = 0;
			bool naming = true;
			int bracketCounter = 0;
			// validate file
			while (*ptr) {
				if (*ptr == '{') {
					++bracketCounter;
				}
				if (*ptr == '}') {
					--bracketCounter;
				}
				++ptr;
			}
			if (bracketCounter != 0) {
				LOGE << "Number of opening and closing brackets do not match - count: " << bracketCounter;
				return 0;
			}
			LOG << "Valid JSON";
			std::stack<Category*> stack;
			stack.push(root);
			ptr = text;
			while (*ptr) {
				if (*ptr == '\t') {
					++ptr;
				}
				else if (*ptr == '/') {
					++ptr;
					if (*ptr == '*') {
						while (*ptr != '/') {
							++ptr;
						}
					}
					if (*ptr == '/') {
						while (*ptr != '\n') {
							++ptr;
						}
					}
				}
				else if (*ptr == '{' && name != 0) {
					Category* c = new Category(name);
					if (!stack.empty()) {
						Category* parent = stack.top();
						parent->addCategory(c);
					}
					stack.push(c);
					++ptr;
					naming = true;
				}
				else if (*ptr == '}') {
					--bracketCounter;
					++ptr;
					if (!stack.empty()) {
						stack.pop();
					}
					naming = true;
				}
				else if (ds::string::isCharacter(*ptr)) {
					char* first = ptr;
					char* last = ptr;
					while (*ptr) {
						if (!ds::string::isCharacter(*ptr)) {
							*last = 0;
							++ptr;
							break;
						}
						else {
							*last++ = *ptr++;
						}
					}
					if (naming) {
						name = first;
						naming = false;
					}
					else {
						value = first;
						naming = true;
					}
					if (name != 0 && value != 0) {
						Category* c = stack.top();
						c->addProperty(name, value);
						stack.top() = c;
						name = 0;
						value = 0;
					}
				}
				else {
					++ptr;
				}
			}
			//free(text);
			if (text != 0) {
				delete[] text;
			}
			return root;
		}
		return 0;
	}

	bool read_simplified_json(const char* file_name,Category* root) {
		int size = -1;
		char* text = ds::repository::load(file_name, &size);
		if ( size != -1 ) {
			LOG << "read_simplified_json - parsing " << file_name;
			char* ptr = text;
			char* name = 0;
			char* value = 0;
			bool naming = true;
			int bracketCounter = 0;
			// validate file
			while (*ptr) {
				if (*ptr == '{') {
					++bracketCounter;
				}
				if (*ptr == '}') {
					--bracketCounter;
				}
				++ptr;
			}
			if (bracketCounter != 0) {
				LOGE << "Number of opening and closing brackets do not match - count: " << bracketCounter;
				return false;
			}
			LOG << "Valid JSON";
			std::stack<Category*> stack;
			stack.push(root);
			ptr = text;
			while (*ptr) {
				if (*ptr == '\t') {
					++ptr;
				}
				else if (*ptr == '/') {
					++ptr;
					if (*ptr == '*') {
						while (*ptr != '/') {
							++ptr;
						}
					}
					if (*ptr == '/') {
						while (*ptr != '\n') {
							++ptr;
						}
					}
				}
				else if (*ptr == '{' && name != 0) {
					Category* c = new Category(name);
					if (!stack.empty()) {
						Category* parent = stack.top();
						parent->addCategory(c);
					}
					stack.push(c);
					++ptr;
					naming = true;
				}
				else if (*ptr == '}') {
					--bracketCounter;
					++ptr;
					if (!stack.empty()) {
						stack.pop();
					}
					naming = true;
				}
				else if (ds::string::isCharacter(*ptr)) {
					char* first = ptr;
					char* last = ptr;
					while (*ptr) {
						if (!ds::string::isCharacter(*ptr)) {
							*last = 0;
							++ptr;
							break;
						}
						else {
							*last++ = *ptr++;
						}
					}
					if (naming) {
						name = first;
						naming = false;
					}
					else {
						value = first;
						naming = true;
					}
					if (name != 0 && value != 0) {
						Category* c = stack.top();
						c->addProperty(name, value);
						stack.top() = c;
						name = 0;
						value = 0;
					}
				}
				else {
					++ptr;
				}
			}
			if (text != 0) {
				delete[] text;
			}
			return true;
		}
		return false;
	}



}