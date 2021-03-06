#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <ctime>

namespace ds {


	StringStream& StringStream::push(const char *data, uint32 n) {
		for (uint32 i = 0; i < n; ++i) {
			_buffer.push_back(data[i]);
		}
		return *this;
	}

	StringStream& StringStream::operator<<(char c) {
		_buffer.push_back(c);
		return *this;
	}

	StringStream& StringStream::operator<<(const char *s) {
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(float f) {
		char s[32];
		sprintf_s(s, 32, "%g", f);
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(int i) {
		char s[32];
		sprintf_s(s, 32, "%d", i);
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(uint32 i) {
		char s[32];
		sprintf_s(s, 32, "%u", i);
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(const v2& v) {
		char s[32];
		sprintf_s(s, 32, "x: %g y: %g", v.x, v.y);
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(const v3& v) {
		char s[32];
		sprintf_s(s, 32, "x: %g y: %g z: %g", v.x, v.y, v.z);
		return push(s, strlen(s));
	}

	StringStream& StringStream::operator<<(const Color& v) {
		return *this;
	}

	StringStream& StringStream::operator<<(const Rect& v) {
		return *this;
	}

	StringStream& StringStream::format(const char *format, ...) {
		va_list args;

		va_start(args, format);
		int n = vsnprintf(NULL, 0, format, args);
		va_end(args);
		char* tmp = new char[n];

		va_start(args, format);
		vsnprintf(tmp, n, format, args);
		va_end(args);
		push(tmp, n);
		delete[] tmp;
		return *this;
	}

	const char* StringStream::c_str() {
		_buffer.push_back('\0');
		_buffer.pop_back();
		return _buffer.data();
	}


	namespace string {

		// -------------------------------------------------------
		// Formats current date and time to string
		// -------------------------------------------------------
		void formatDateTime(std::string& str) {
			std::stringstream datetime;
			time_t t = time(0);   // get time now
			struct tm * now = localtime(&t);
			datetime << now->tm_mday << "." << (now->tm_mon + 1) << "." << (now->tm_year + 1900) << " " << now->tm_hour << ":" << now->tm_min;
			str = datetime.str();
		}

		void formatPercentage(float percentage,char* ret) {
			if (percentage < 100.0f) {
				sprintf(ret, "% 2.2f", percentage);
			}
			else {
				sprintf(ret, "%3.2f", percentage);
			}
		}
		/*
		Array<std::string> split(const std::string& str, const char delimiter) {
			Array<std::string> words;
			std::string word;
			std::stringstream stream(str);
			while (getline(stream, word, delimiter)) {
				words.push_back(word);
			}
			return words;
		}
		*/
		bool isNumeric(const char* text) {
			const char* p = text;
			int cnt = 0;
			while (*p) {
				if (!isDigit(*p)) {
					return false;
				}
				++p;
			}
			return true;
		}

		int count_delimiters(const char* text, const char delimiter) {
			const char* p = text;
			int cnt = 0;
			while (*p) {
				if (*p == delimiter) {
					++cnt;
				}
				++p;
			}
			return cnt;
		}

		int split(const char* text, float* array, int max, const char delimiter) {
			std::string word;
			std::stringstream stream(text);
			int cnt = 0;
			while (getline(stream, word, delimiter)) {
				array[cnt++] = atof(word.c_str());
			}
			return cnt;
		}

		void split(const std::string& str, Array<std::string>& words, const char delimiter) {
			std::string word;
			std::stringstream stream(str);
			while (getline(stream, word, delimiter)) {
				words.push_back(word);
			}
		}

		void trim(std::string& str) {
			trim(str, " ");
			trim(str, "\t");
		}

		void trim(std::string& str, const std::string& ws) {
			std::string::size_type pos = str.find_last_not_of(ws);
			if (pos != std::string::npos) {
				str.erase(pos + 1);
				pos = str.find_first_not_of(ws);
				if (pos != std::string::npos) {
					str.erase(0, pos);
				}
			}
			else {
				str.erase(str.begin(), str.end());
			}
		}

		// -------------------------------------------------------
		// Formats time in minutes and seconds to string
		// -------------------------------------------------------
		void formatTime(int minutes, int seconds, std::string& str) {
			std::string secStr;
			formatInt(seconds, secStr, 2);
			std::string minStr;
			formatInt(minutes, minStr, 2);
			str = minStr + ":" + secStr;
		}

		void formatInt(int value, char* buffer, int maxBuffer, int length) {
			int idx = 0;
			if (value < 0) {
				buffer[0] = '-';
				++idx;
				value = -value;
			}
			int tmp = value;
			int div = 1;
			for (int i = 0; i < length; ++i) {
				if (i > 0) {
					div *= 10;
				}
			}
			for (int i = 0; i < length; ++i) {
				int r = tmp / div;
				buffer[idx++] = ('0' + r);
				tmp = tmp - r * div;
				div /= 10;
			}
			buffer[idx] = '\0';
		}
		// -------------------------------------------------------
		// Formats an int to a string using fixed length
		// -------------------------------------------------------
		void formatInt(int value, std::string& str, int length) {
			str = "";

			if (value < 0) {
				str += '-';
				value = -value;
			}
			int tmp = value;
			int div = 1;
			for (int i = 0; i < length; ++i) {
				if (i > 0) {
					div *= 10;
				}
			}
			for (int i = 0; i < length; ++i) {
				int r = tmp / div;
				str += ('0' + r);
				tmp = tmp - r * div;
				div /= 10;
			}
		}

		// -------------------------------------------------------
		// Formats an int to a string using fixed length
		// -------------------------------------------------------
		void formatInt(int value, char* str, int length) {
			//str = "";
			int index = 0;
			if (value < 0) {
				str[index++] = '-';
				//str += '-';
				value = -value;
			}
			int tmp = value;
			int div = 1;
			for (int i = 0; i < length; ++i) {
				if (i > 0) {
					div *= 10;
				}
			}
			for (int i = 0; i < length; ++i) {
				int r = tmp / div;
				str[index++] = ('0' + r);
				tmp = tmp - r * div;
				div /= 10;
			}
			str[index++] = '\0';
		}

		// -------------------------------------------------------
		// Formats an int to a string
		// -------------------------------------------------------
		void formatInt(int value, std::string& str) {
			char buffer[20];
			sprintf(buffer, "%d", value);
			int length = strlen(buffer);
			return formatInt(value, str, length);
		}

		void format_number(int n, char * out,const char delimiter) {
			int digit = 0;
			int out_index = 0;
			for (int i = n; i != 0; i /= 10) {
				digit = i % 10;

				if ((out_index + 1) % 4 == 0) {
					out[out_index++] = delimiter;
				}
				out[out_index++] = digit + '0';
			}
			out[out_index] = '\0';
			for (int i = 0; i < out_index / 2; ++i) {
				char c = out[out_index - i - 1];
				out[out_index - i - 1] = out[i];
				out[i] = c;
			}
		}

		void file_name(const char* file, char* name) {
			char n[256];
			char e[4];
			_splitpath(file, 0, 0, name, e);
		}

		IdString murmur_hash(const char* text) {
			return murmur_hash(text, strlen(text), 0);
		}
		//-----------------------------------------------------------------------------
		// MurmurHash2, by Austin Appleby
		//-----------------------------------------------------------------------------
		IdString murmur_hash(const void * key, int len, unsigned int seed) {
			// 'm' and 'r' are mixing constants generated offline.
			// They're not really 'magic', they just happen to work well.

			const unsigned int m = 0x5bd1e995;
			const int r = 24;

			// Initialize the hash to a 'random' value

			IdString h = seed ^ len;

			// Mix 4 bytes at a time into the hash

			const unsigned char * data = (const unsigned char *)key;

			while (len >= 4)
			{
				unsigned int k = *(unsigned int *)data;

				k *= m;
				k ^= k >> r;
				k *= m;

				h *= m;
				h ^= k;

				data += 4;
				len -= 4;
			}

			// Handle the last few bytes of the input array

			switch (len)
			{
			case 3: h ^= data[2] << 16;
			case 2: h ^= data[1] << 8;
			case 1: h ^= data[0];
				h *= m;
			};

			// Do a few final mixes of the hash to ensure the last few
			// bytes are well-incorporated.

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			return h;
		}

		bool isDigit(const char c) {
			return ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.');
		}

		bool isNumeric(const char c) {
			return ((c >= '0' && c <= '9'));
		}

		bool isWhitespace(const char c) {
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
				return true;
			}
			return false;
		}

		float strtof(const char* p, char** endPtr) {
			while (isWhitespace(*p)) {
				++p;
			}
			float sign = 1.0f;
			if (*p == '-') {
				sign = -1.0f;
				++p;
			}
			else if (*p == '+') {
				++p;
			}
			float value = 0.0f;
			while (isNumeric(*p)) {
				value *= 10.0f;
				value = value + (*p - '0');
				++p;
			}
			if (*p == '.') {
				++p;
				float dec = 1.0f;
				float frac = 0.0f;
				while (isNumeric(*p)) {
					frac *= 10.0f;
					frac = frac + (*p - '0');
					dec *= 10.0f;
					++p;
				}
				value = value + (frac / dec);
			}
			if (endPtr) {
				*endPtr = (char *)(p);
			}
			return value * sign;
		}

		bool isCharacter(const char p) {
			if (p >= 'a' && p <= 'z') {
				return true;
			}
			else if (p >= 'A' && p <= 'Z') {
				return true;
			}
			else if (p == '_' || isDigit(p) || p == '.' || p == ',') {
				return true;
			}
			return false;
		}

		bool contains(const char c, const char* array, int length) {
			for (int i = 0; i < length; ++i) {
				if (array[i] == c) {
					return true;
				}
			}
			return false;
		}

		int extract_used_characters(const char* text, char* array, int max) {
			const char* p = text;
			int cnt = 0;
			while (*p) {
				if (!contains(*p, array, cnt)) {
					array[cnt++] = *p;
				}
				++p;
			}
			return cnt;
		}


	}

}
