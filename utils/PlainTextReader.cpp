#include "PlainTextReader.h"
#include <stack>
#include "StringUtils.h"
#include "Log.h"
#include "StringUtils.h"
// ------------------------------------------
// JSON reader
// ------------------------------------------
JSONReader::JSONReader() {
}

JSONReader::~JSONReader() {
	std::vector<Category*>::iterator it = m_Categories.begin();
	while ( it != m_Categories.end()) {
		delete (*it);
		it = m_Categories.erase(it);
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
	std::vector<Category*>::iterator it = m_Categories.begin();
	while ( it != m_Categories.end() ) {
		delete (*it);
		it = m_Categories.erase(it);
	}
}
// -------------------------------------------------------
// Parse JSON file
// -------------------------------------------------------
bool JSONReader::parse(const char* fileName) {
	clear();
	FILE *fp = fopen(fileName, "rb");
	char* text;
	if (fp) {
		LOGC("JSONReader") << "Parsing " << fileName;
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		text = (char*)malloc(sizeof(char) * (size + 1));
		fread (text, 1, size, fp);
		fclose(fp);
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
		free(text);
		return true;
	}
	return false;
}


