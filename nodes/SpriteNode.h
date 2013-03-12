#pragma once
#include "..\dxstdafx.h"
#include <string>
#include <vector>
#include "..\nodes\Node.h"
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"
//--------------------------------------------------------------
//
//--------------------------------------------------------------
namespace ds {

const int MAX_SPRITES = 256;

struct SHandle {
	SHandle() : m_Index(0) {}
	explicit SHandle(int idx) : m_Index(idx) {}
	int m_Index;
};

enum SpriteColorMode {
	SCM_ADD,SCM_SUBTRACT,SCM_MODULATE
};

enum SpriteBlendMode {
	SBM_NORMAL,SBM_ADDITIVE
};

class SpriteNode : public Node {

public:	
	SpriteNode(const char* name,Renderer* renderer,int mtrl,SpriteColorMode mode = SCM_MODULATE);
	virtual ~SpriteNode();
	// sprite handling
	SHandle add(const Vec2& position,const Rect& offset);
	void setColor(const SHandle& handle,const Color& color);
	void setPosition(const SHandle& handle,const Vec2& pos);
	Vec2& getPosition(const SHandle& handle) const;
	void setScale(const SHandle& handle,const Vec2& pos);
	void setScale(const SHandle& handle,float x,float y);
	void setVisible(const SHandle& handle,const bool visible);
	bool isVisible(const SHandle& handle) const;
	void setOffset(const SHandle& handle,const Rect& offset);
	void setOffset(const SHandle& handle,int top,int left,int width,int height);
	Rect getOffset(const SHandle& handle) const;
	void setRotation(const SHandle& handle,float angle);
	float getRotation(const SHandle& handle);
	//void removeSprite(Sprite* sprite);
	void clear();
	void prepareRendering();
	void draw();
	void setColorMode(SpriteColorMode mode) { m_ColorMode = mode; }
	void setFixedPosition(bool fixed) {
		m_IgnoreViewport = fixed;
	}
	void debug();
private:
	// members
	LPD3DXSPRITE pSprite;	
	int m_Counter;
	bool m_IgnoreViewport;
	// variables
	Vec2* m_Positions;
	RECT* m_Offsets;
	bool* m_Visible;
	Color* m_Colors;
	float* m_Rotations;
	Vec2* m_Scales;
	Vec2* m_Centers;
	Matrix* m_Matrix;
	bool* m_Dirty;
	SpriteColorMode m_ColorMode;
};

};
