#include "DebugRenderer.h"
#include "..\utils\Profiler.h"
#include "render_types.h"
#include "Renderer.h"

namespace ds {

DebugRenderer::DebugRenderer(Renderer* renderer) : m_Renderer(renderer) {
	m_Font = renderer->loadSystemFont("Verdana_12","Verdana",10,false);
	IDirect3DDevice9 * pDevice = renderer->getDevice();
	HR(D3DXCreateSprite( pDevice, &m_Sprite ));
}


DebugRenderer::~DebugRenderer() {
	SAFE_RELEASE(m_Sprite);
}

void DebugRenderer::clear() {
	m_TextLines.clear();
}

void DebugRenderer::draw() {
	if ( !m_TextLines.empty() ) {
		IDirect3DDevice9 * pDevice = m_Renderer->getDevice();
		ID3DXFont *font = m_Renderer->getInternalSystemFont(m_Font);
		if ( font != 0 ) {
			RECT font_rect;	
			m_Sprite->Begin( D3DXSPRITE_ALPHABLEND );
			for ( size_t i = 0; i < m_TextLines.size(); ++i ) {	
				TextLine *tl = &m_TextLines[i];
				//FIXME: calculate rect correctly
				int xp = (int)tl->pos.x;
				int yp = (int)tl->pos.y;
				SetRect(&font_rect,xp,yp,xp+200,yp+60);		
				font->DrawTextA(m_Sprite,tl->message.c_str(),-1,&font_rect,DT_LEFT|DT_NOCLIP,tl->color);		
			}
			m_Sprite->End();
		}	
	}
}

void DebugRenderer::showDrawCounter(const Vec2& pos) {
	addText(pos,ds::Color(1.0f,1.0f,1.0f,1.0f),"DrawCounter IDX %d Vertices %d Sprites %d",m_Renderer->getDrawCounter().getIndexCounter(),m_Renderer->getDrawCounter().getPrimitiveCounter(),m_Renderer->getDrawCounter().getSpriteCounter());
	Vec2 nextPos = pos;
	nextPos.y += 20.0f;
	addText(nextPos,ds::Color(1.0f,1.0f,1.0f,1.0f),"DrawCalls %d Textures %d Shaders %d",m_Renderer->getDrawCounter().getDrawCalls(),m_Renderer->getDrawCounter().getTextures(),m_Renderer->getDrawCounter().getShaders());
	//nextPos.y += 20.0f;
	//addText(nextPos,ds::Color(1.0f,1.0f,1.0f,1.0f),"Memory %d",m_Renderer->getDrawCounter().getMemorySize());
}

void DebugRenderer::showProfile(const Vec2& pos) {
	gProfiler->show(pos,this);
}

void DebugRenderer::addText(const Vec2& pos,char* format,...) {
	va_list args;
	va_start(args,format);
	addText(pos,Color(1.0f,1.0f,1.0f,1.0f),format,args);
	va_end(args);
}

void DebugRenderer::addText(const Vec2& pos,char* format,va_list args) {
	addText(pos,Color(1.0f,1.0f,1.0f,1.0f),format,args);
}

void DebugRenderer::addText(const Vec2& pos,const Color& color,char* format,...) {
	va_list args;
	va_start(args,format);
	addText(pos,color,format,args);
	va_end(args);
}

void DebugRenderer::addText(const Vec2& pos,const Color& color,char* format,va_list args) {
	TextLine tl;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer,sizeof(buffer),_TRUNCATE,format,args);		
	tl.message = std::string(buffer);
	tl.pos = pos;
	tl.color = color;
	m_TextLines.push_back(tl);
}

void DebugRenderer::addText(const Vec2& pos,const char* text) {
	TextLine tl;	
	tl.message = std::string(text);
	tl.pos = pos;
	tl.color = Color(1.0f,1.0f,1.0f,1.0f);
	m_TextLines.push_back(tl);
}

}
