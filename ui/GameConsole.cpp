#include "GameConsole.h"
#include "..\io\json.h"
#include "..\utils\Log.h"

namespace ds {

	GameConsole::GameConsole() {
	}


	GameConsole::~GameConsole() {
	}


	void GameConsole::add(ConsoleCommand* command) {
		_commands.push_back(command);
	}

	bool GameConsole::execute(const char* text) {
		Tokenizer t;
		t.parse(text);
		char name[64];
		if (t.size() > 0) {
			const Token& zn = t.get(0);
			if (zn.type == Token::NAME) {
				strncpy(name, text + zn.index, zn.size);
				name[zn.size] = '\0';
				for (int i = 0; i < _commands.size(); ++i) {					
					if (strcmp(name, _commands[i]->getCommand()) == 0) {
						_vars.num = 0;
						int cnt = 1;						
						while (cnt < t.size()) {
							Token current = t.get(cnt);
							if (current.type == Token::NUMBER) {
								if (_vars.num < _commands[i]->getNumVars()) {
									_vars.values[_vars.num++] = current.value;
								}
							}
							++cnt;							
						}
						if (_vars.num == _commands[i]->getNumVars()) {
							return _commands[i]->execute(_vars);
						}
					}
				}
				LOGE << "command '" << name << "' not found";
			}
		}
		return false;
	}

	void GameConsole::render() {

	}

}