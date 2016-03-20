#pragma comment(lib, "dbghelp.lib")

#include "CallStackTracer.h"
#include <crtdbg.h>
#include <DbgHelp.h>
#include "..\utils\StringUtils.h"
#include "..\utils\Log.h"
#include "..\lib\collection_types.h"

namespace ds {

	namespace stacktrace {

		void logStackTrace() {
			char buf[255];
			CallStack cs;
			cs.num = 0;

			SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME);
			HANDLE hProc = GetCurrentProcess();
			SymInitialize(hProc, buf, TRUE);
			PVOID addrs[25] = { 0 };

			USHORT frames = CaptureStackBackTrace(2, 25, addrs, NULL);
			for (USHORT i = 0; i < frames; i++) {
				ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
				SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
				info->SizeOfStruct = sizeof(SYMBOL_INFO);
				info->MaxNameLen = 255;
				DWORD64 displacement = 0;
				if (::SymFromAddr(::GetCurrentProcess(), (DWORD64)addrs[i], &displacement, info)) {
					LOG << info->Name;
				}
			}
		}
	}

	CallStackTracer::CallStackTracer() {
	}


	CallStackTracer::~CallStackTracer() {
	}

	int CallStackTracer::create() {
		char buf[255];
		CallStack cs;
		cs.num = 0;

		SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME);
		HANDLE hProc = GetCurrentProcess();
		SymInitialize(hProc, buf, TRUE);
		PVOID addrs[25] = { 0 };

		USHORT frames = CaptureStackBackTrace(2, 25, addrs, NULL);
		for (USHORT i = 0; i < frames; i++) {
			ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = { 0 };
			SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
			info->SizeOfStruct = sizeof(SYMBOL_INFO);
			info->MaxNameLen = 255;
			DWORD64 displacement = 0;
			if (::SymFromAddr(::GetCurrentProcess(), (DWORD64)addrs[i], &displacement, info)) {
				if (cs.num < 32) {

					IdString hash = string::murmur_hash(info->Name);
					int idx = -1;
					for (int j = 0; j < _callees.size(); ++j) {
						if (_callees[j].hash == hash) {
							idx = j;
						}
					}
					if (idx == -1) {
						int l = strlen(info->Name);
						cs.entries[cs.num++] = _names.size;
						Callee c;
						c.index = _names.size;
						c.hash = hash;
						c.length = l;
						_callees.push_back(c);
						if (_names.size + l > _names.capacity) {
							_names.resize(_names.capacity + 256);
						}
						_names.append(info->Name, l);
					}
					else {
						cs.entries[cs.num++] = idx;
					}
				}
			}
		}
		SymCleanup(GetCurrentProcess());
		_callStacks.push_back(cs);
		return _callStacks.size() - 1;
	}

	void CallStackTracer::print(int index) {
		char name[255];
		const CallStack& cs = _callStacks[index];
		for (int i = 0; i < cs.num ; ++i) {
			const Callee& c = _callees[i];// cs.entries[i]];
			strncpy(name, _names.data + c.index, c.length);
			name[c.length] = '\0';
			LOG << name;
		}
	}

	void CallStackTracer::getCalleess(int index,char* buffer, int max) {
		int cnt = 0;
		char name[255];		
		const CallStack& cs = _callStacks[index];
		for (int i = 0; i < cs.num; ++i) {
			const Callee& c = _callees[cs.entries[i]];
			strncpy(name, _names.data + c.index, c.length);
			name[c.length] = '\0';
			sprintf_s(buffer + cnt, max, "%s -> ", name);
		}
	}
}
