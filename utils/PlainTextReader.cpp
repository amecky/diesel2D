#include "PlainTextReader.h"
#include <stack>
#include "StringUtils.h"
#include "Log.h"
#include "StringUtils.h"

PlainTextReader::PlainTextReader() : m_Root(0) {
}

bool PlainTextReader::readFile(const char* fileName) {
	std::vector<std::string> file;
	std::string line;
	file.clear();
	std::stack<Category*> stack;
	std::ifstream infile(fileName, std::ios_base::in);
	while (getline(infile, line, '\n')) {
		file.push_back(line);
	}
	int braces = 0;
	for (size_t i = 0; i < file.size(); ++i) {
		line = file[i];
		if (line.find('{') != std::string::npos) {
			std::string::size_type pos = line.find('{');
			std::string name = line.substr(0, pos);
			ds::string::trim(name);
			if (m_Root == 0) {
				m_Root = new Category(name);
				stack.push(m_Root);
			}
			else {
				Category* c = stack.top();
				Category* cat = new Category(name);
				c->addCategory(cat);
				stack.push(cat);
			}
			++braces;
		}
		else if (line.find('}') != std::string::npos) {
			stack.pop();
			--braces;
		} else {
			std::string::size_type pos = line.find(':');
			if (pos != std::string::npos) {
				Category* c = stack.top();
				std::string name = line.substr(0, pos);
				ds::string::trim(name);
				std::string value = line.substr(pos+1);
				ds::string::trim(value);
				c->addProperty(name,value);
				stack.top() = c;
			}
		}
	}
	return true;
}

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
		LOGC(logINFO,"JSONReader") << "Parsing " << fileName;
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
			else if ( *ptr == '{' ) {
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
				stack.pop();
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

// ------------------------------------------
// Settings reader
// ------------------------------------------
SettingsReader::SettingsReader() {
}

SettingsReader::~SettingsReader() {
}

bool SettingsReader::parse(const char* fileName) {
	LOGC(logINFO,"SettingsReader") << "parsing file: " << fileName;
	std::string line;
	std::ifstream infile(fileName, std::ios_base::in);
	std::vector<std::string> entries;
	while (getline(infile, line, '\n')) {
		if (line.find('#') == std::string::npos) {
			entries.clear();
			ds::string::split(line,entries,'=');
			if ( entries.size() == 2 ) {
				std::string name = entries[0];
				ds::string::trim(name);
				std::string value = entries[1];
				ds::string::trim(value);
				m_Settings[name] = value;
			}
			else {
				LOGC(logINFO,"SettingsReader") << "Split returned wrong number of entries: " << line;
			}
		}
	}
	infile.close();
	return true;
}

void SettingsReader::getNames(std::vector<std::string>& names) {
	names.clear();
	Settings::iterator it = m_Settings.begin();
	while ( it != m_Settings.end() ) {
		names.push_back(it->first);
		++it;
	}
}

std::string& SettingsReader::getValue(const std::string& name) {
	return m_Settings[name];
}

// -------------------------------------------------------
// NewSettingsReader
// -------------------------------------------------------
bool NewSettingsReader::parse(const char* fileName) {
	LOGC(logINFO,"NewSettingsReader") << "parsing file: " << fileName;
	std::string line;
	std::ifstream infile(fileName, std::ios_base::in);
	if ( infile) {
		std::vector<std::string> entries;
		while (getline(infile, line, '\n')) {
			if (line.find('#') == std::string::npos) {
				entries.clear();
				ds::string::split(line,entries,'=');
				if ( entries.size() == 2 ) {
					std::string name = entries[0];
					ds::string::trim(name);
					std::string value = entries[1];
					ds::string::trim(value);
					Setting set;
					// FIXME: make sure name is only 20 chars
					strcpy(set.name,name.c_str());
					set.hash = ds::string::murmur_hash(name.c_str());
					set.value = value;
					m_Settings.push_back(set);
				}
				else {
					LOGC(logINFO,"NewSettingsReader") << "NewSplit returned wrong number of entries: " << line;
				}
			}
		}
		infile.close();
		return true;
	}
	else {
		LOGC(logINFO,"NewSettingsReader") << "Cannot find file: " << fileName;
		return false;
	}
}


