#pragma once
#include "..\lib\collection_types.h"

namespace ds {

	struct ConsoleVars {
		float values[8];
		int num;
	};

	class ConsoleCommand {

	public:
		ConsoleCommand() {}
		virtual ~ConsoleCommand() {}
		virtual bool execute(const ConsoleVars& vars) = 0;
		virtual const char* getCommand() const = 0;
		virtual int getNumVars() const = 0;
	};

	class GameConsole {

	public:
		GameConsole();
		~GameConsole();
		void add(ConsoleCommand* command);
		bool execute(const char* text);
		void render();
		void logHelp();
	private:
		Array<ConsoleCommand*> _commands;
		ConsoleVars _vars;
	};

}

