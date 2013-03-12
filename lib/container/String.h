/* 
 * File:   String.h
 * Author: mecky
 *
 * Created on 5. Dezember 2011, 14:20
 */
#ifndef DSSTR_H_
#define DSSTR_H_
#include <string.h>
#pragma warning ( disable : 4996)
#include <assert.h>
typedef unsigned int uint32;
// we allocate memory in 16 bytes chunks
const uint32 STRING_GRANULARITY = 16;

// ------------------------------------------------
//
// ------------------------------------------------
class String {

public:
    String();
	String(const String& str);
	String(const char* text);
	virtual ~String();
	
    uint32 length() const;
    uint32 size() const;
    bool isEmpty() const {
        return m_Length == 0;
    }
	const char* c_str() const {
		return m_Data;
	}
	operator const char *() const;
	operator const char *();
    char operator[] (uint32 index) const;
    char& operator[] (uint32 index);
	void at(char& c,uint32 index) const;
    void operator= (const char* text);
    
    String& operator+=(const char* text);
    String& operator+=(const String& text);
    
    void append(const char* text);
    void append(const char* text,uint32 length);
    void append(const char c);
    void append(const String& str);
    void append(int value);

    void trim();
    
    const char* substring(String& result,uint32 offset);
    const char* substring(String& result,uint32 offset,uint32 length);
    
    
    int indexOf(const char c);
    int indexOf(const char* text,int start = 0, int end = -1);
    bool endsWith(const char* text);
    bool endsWith(const String& text);
    
	void stripLeading(const char c);
	void stripTrailing(const char c);

    void freeData();
    
    static String valueOf(int value);
    static String valueOf(bool value);
    static String valueOf(float value);
private:
	void init();
    void reallocate(uint32 newSize);
	char* m_Data;
	uint32 m_Length;
    uint32 m_Size;
};

// ------------------------------------------------
//
// ------------------------------------------------
inline String::String() {
	init();	
}

// ------------------------------------------------
//
// ------------------------------------------------
inline String::String(const char* text) {
	init();
	if ( text ) {
		m_Length = strlen(text);
        reallocate(m_Length);
		strcpy(m_Data,text);
	}
}

inline String::String(const String& str) {
	init();
	m_Length = str.length();
	reallocate(m_Length);
	strcpy(m_Data,str.c_str());
}


// ------------------------------------------------
//
// ------------------------------------------------
inline String::~String() {
    freeData();
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::init() {
	m_Length = 0;
    m_Size = 0;
	m_Data = new char[STRING_GRANULARITY];
	m_Data[0] = '\0';
}

// ------------------------------------------------
//
// ------------------------------------------------
inline uint32 String::length() const {
	return m_Length;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline uint32 String::size() const {
	return m_Size;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::freeData() {
    delete[] m_Data;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::append(const char* text) {    
    if ( text ) {
        int len = strlen(text);
		int newLen = m_Length + len;
		reallocate(m_Length + len + 1);
		for ( int i = 0; i < len; ++i ) {
			m_Data[m_Length + i] = text[i];
		}
		m_Length = newLen;
		m_Data[m_Length] = '\0';
	}
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::append(const char* text,uint32 length) {    
    if ( text ) {
        int len = length;
		int newLen = m_Length + len;
		reallocate(m_Length + len + 1);
		for ( int i = 0; i < len; ++i ) {
			m_Data[m_Length + i] = text[i];
		}
		m_Length = newLen;
		m_Data[m_Length] = '\0';
	}
}
  
// ------------------------------------------------
//
// ------------------------------------------------
inline void String::append(const char c) {
	// FIXME: remove magic number
    reallocate(m_Length+50);
    m_Data[m_Length] = c;
    ++m_Length;
    m_Data[m_Length] = '\0';
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::append(int value) {
	char buffer[128];
	sprintf(buffer,"%d",value);
	append(buffer);
}


// ------------------------------------------------
//
// ------------------------------------------------
inline void String::append(const String& str) {
	uint32 newLen = m_Length + str.length();
	reallocate(newLen + 1);
	for (uint32 i = 0; i < str.length(); ++i ) {
		m_Data[m_Length + i] = str[i];
	}
	m_Length = newLen;
	m_Data[m_Length] = '\0';
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::at(char& c,uint32 index) const {
	assert( ( index >= 0 ) && ( index <= m_Length) );
	c = m_Data[index];
}
// ------------------------------------------------
//
// ------------------------------------------------
inline char String::operator [](uint32 index) const {
	assert( ( index >= 0 ) && ( index <= m_Length) );
    return m_Data[index];
}

// ------------------------------------------------
//
// ------------------------------------------------
inline char& String::operator [](uint32 index) {
	assert( ( index >= 0 ) && ( index <= m_Length) );
    return m_Data[index];
}
    
// ------------------------------------------------
//
// ------------------------------------------------
inline void String::reallocate(uint32 newSize) {
    uint32 required = m_Length + newSize;
    if ( required > m_Size ) {
        int mod = newSize % STRING_GRANULARITY;
        int alloc = m_Size + newSize;    
        if ( mod ) {		
            alloc = newSize + STRING_GRANULARITY - mod;
        }      
        char* tmp = new char[alloc];
        m_Size = alloc;
        m_Data[m_Length] = '\0';
        strcpy( tmp, m_Data );
        delete[] m_Data;
        m_Data = tmp;    
    }
}

// ------------------------------------------------
//
// ------------------------------------------------
inline int String::indexOf(const char c) {
    for ( uint32 i = 0; i < m_Length; ++i ) {
        if ( m_Data[i] == c ) {
            return i;
        }
    }
    return -1;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline int String::indexOf(const char* text,int start,int end) {
	if ( end == -1 ) {
		end = m_Length;
	}
	int l = end - strlen(text);
    int j = 0;
    int max = strlen(text);
	for ( int i = start; i <= l; ++i ) {
		for ( j = 0; j < max; ++j ) {
			if ( m_Data[i+j] != text[j] ) {
				break;
			}
		}	
		if ( j == max ) {
			return i;
		}
	}
	return -1;    
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::trim() {
    uint32 len = m_Length;
    uint32 start = 0;
    while ( ( start < len ) && (m_Data[start] <= ' ' ) ) {
        ++start;
    }
    while ( ( start < len) && (m_Data[len-1] <= ' ') ) {
        --len;
    }
    if ( start > 0 || len < m_Length ) {
        uint32 diff = len - start;
        char* tmp = new char[diff];
        for ( uint32 i = 0; i < diff; ++i ) {
            tmp[i] = m_Data[start+i];
        }
        m_Length = diff;
        delete[] m_Data;
        m_Data = tmp;
    }    
}

// ------------------------------------------------
//
// ------------------------------------------------
inline const char* String::substring(String& result,uint32 offset) {
	if ( offset >= m_Length ) {
		return result;
	}    
    uint32 diff = m_Length - offset;
	result.append( &m_Data[offset], diff );
	return result;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline const char* String::substring(String& result,uint32 offset,uint32 length) {
	//result.clear();
	if ( offset >= m_Length ) {
		return result;
	}        
	result.append( &m_Data[offset], length );
	return result;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::stripLeading(const char c) {
	while( m_Data[ 0 ] == c ) {
		memmove( &m_Data[ 0 ], &m_Data[ 1 ], m_Length );
		--m_Length;
	}
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::stripTrailing(const char c) {
	for( int i = m_Length ; i > 0 && m_Data[ i - 1 ] == c; i-- ) {
		m_Data[i - 1] = '\0';
		--m_Length;
	}
}

// ------------------------------------------------
//
// ------------------------------------------------
inline String String::valueOf(int value) {
    char buffer[32];
    sprintf(buffer,"%d",value);
    return String(buffer);
}

// ------------------------------------------------
//
// ------------------------------------------------
inline String String::valueOf(bool value) {
    if ( value ) {
        return String("true");
    }
    return String("false");    
}

// ------------------------------------------------
//
// ------------------------------------------------
inline String String::valueOf(float value) {
    char buffer[32];
    sprintf(buffer,"%.2f",value);
    return String(buffer);
}

// ------------------------------------------------
//
// ------------------------------------------------
inline void String::operator=(const char* text) {
    init();
	if ( text ) {
		m_Length = strlen(text);
        reallocate(m_Length);
		strcpy(m_Data,text);
	}
}

inline String::operator const char *() const {
	return c_str();
}
inline String::operator const char *() {
	return c_str();
}
// ------------------------------------------------
//
// ------------------------------------------------
inline String& String::operator +=(const char* text) {
    append(text);
    return *this;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline String& String::operator +=(const String& text) {
    append(text);
    return *this;
}

// ------------------------------------------------
//
// ------------------------------------------------
inline bool operator==(const String& first,const String& second) {
    if ( strcmp(first.c_str(),second.c_str()) == 0 ) {
        return true;
    }
    return false;
}

inline bool operator==(const String& first,const char* second) {
    if ( strcmp(first.c_str(),second) == 0 ) {
        return true;
    }
    return false;
}

inline bool operator==(const char* first,const String& second) {
    if ( strcmp(first,second.c_str()) == 0 ) {
        return true;
    }
    return false;
}
#endif /* DSSTR_H_ */


