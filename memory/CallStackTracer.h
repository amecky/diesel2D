#pragma once
#include "..\lib\collection_types.h"

namespace ds {

	struct CallStack {
		int entries[32];
		int num;
	};

	class CallStackTracer {

		struct Callee {
			IdString hash;
			int index;
			int length;
		};


	public:
		CallStackTracer();
		~CallStackTracer();
		int create();
		void print(int index);
		void getCalleess(int index,char* buffer, int max);
	private:
		CharBuffer _names;
		Array<Callee> _callees;
		Array<CallStack> _callStacks;
	};

}

