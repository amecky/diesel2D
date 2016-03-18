#pragma once

namespace ds {

	class CrashReporter {

	public:
		virtual void saveReport() = 0;
	};

	class CrashDump {

	public:
		CrashDump() {}
		void attach(CrashReporter* reporter);
		void dump();
	private:
		CrashReporter* _reporter;
	};

	extern CrashDump* gCrashDump;
}