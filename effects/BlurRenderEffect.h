#pragma once
#include "RenderEffect.h"

#include "..\renderer\render_types.h"
#include "..\sprites\Sprite.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

// -------------------------------------------------------
// BlurRenderEffect
// -------------------------------------------------------
class BlurRenderEffect : public RenderEffect {

public:
	BlurRenderEffect(void);
	virtual ~BlurRenderEffect(void);


	void start();
	void render();

	DynamicGameSettings* getSettings() {
		return 0;
	}
private:

	RTID _baseRT;
	RTID _tempRT;
	int _blurVShaderID;
	int _blurHShaderID;

};


}
