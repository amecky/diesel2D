#pragma once
#include "..\renderer\graphics.h"

namespace ds {

	class PostFX : public DataFile {

	public:
		PostFX();
		~PostFX();
		void start();
		void tick(float dt);
		void render();
		const bool isActive() const {
			return _active;
		}
		void activate() {
			_active = true;
		}
		void deactivate() {
			_active = false;
		}
		bool saveData(JSONWriter& writer);
		bool loadData(const JSONReader& loader);
		const char* getFileName() const {
			return "resources\\post_fx.json";
		}
	private:
		RTID rts[2];
		bool _active;
	};

}

