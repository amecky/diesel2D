#pragma once
#include "..\dxstdafx.h"
#include "GraphicsDevice.h"
#include "buffer_types.h"
#include <string>
#include "..\base\Settings.h"
#include <map>
#include "Camera.h"
#include <vector>
#include "..\utils\Color.h"
#include "render_types.h"
#include "DrawCounter.h"
#include "..\sprites\SpriteBatch.h"
#include "BitmapFont.h"
#include "Viewport.h"
#include "..\sprites\SpriteObjectDescription.h"


namespace ds {


// -------------------------------------------------------
// Constants
// -------------------------------------------------------	
const int MAX_VERDECLS       = 12;
const int MAX_TEXTURES       = 256;
const int MAX_SHADERS        = 128;
const int MAX_BLENDSTATES    = 10;
const int MAX_BUFFERS        = 256;
const int MAX_BUFFER_HANDLES = 256;
const int MAX_FONTS          = 16;
const int MAX_RENDER_TARGETS = 32;
// -------------------------------------------------------
// Vertex declaration types
// -------------------------------------------------------
const int VD_TTC   = 0;
const int VD_PTNBT = 1;
const int VD_PTC   = 2;

class DebugRenderer;
class VertexDeclaration;

struct VDStruct {
	int vertexSize;
	VertexDeclaration* declaration;
};

// -------------------------------------------------------
// Renderer
// -------------------------------------------------------
class Renderer {

typedef std::map<D3DRENDERSTATETYPE,DWORD> RenderStates;
typedef List<RasterizerState*> RasterizerStates;
typedef List<DebugMessage> DebugMessages;

public:
	Renderer(HWND hWnd,const Settings& settings);
	~Renderer();
	// basic rendering methods
	bool beginRendering(const Color& clearColor);
	void endRendering();	
	void setWorldMatrix(const mat4& world);
	const mat4& getWVPMatrix() const { 
		return matWorldViewProj; 
	}
	const mat4& getWorldMatrix() const { 
		return m_World; 
	}
	const mat4& getViewMatrix() const { 
		return m_Camera->getViewMatrix(); 
	}
	const mat4& getProjectionMatrix() const { 
		return m_Camera->getProjectionMatrix();
	}
	Camera* getCamera();	
	void set2DCameraOn();
	void set2DCameraOff();
	void clear();
	// render state management
	void getRenderStates();
	void setRenderState(D3DRENDERSTATETYPE rs,DWORD value);
	DWORD getRenderState(D3DRENDERSTATETYPE rs);
	void setupMatrices();
	
	void debug();
	// vertex declarations
	void addVertexDeclaration(const char* name,VertexDeclaration* declaration);
	VertexDeclaration* getVertexDeclaration(const char* name);
	const VDStruct& getVertexDeclaration(int declarationType) const {
		return m_VDStructs[declarationType];
	}
	// Shader
	int loadShader(const char* fxName,const char* techName);
	Shader& getShader(int id);
	int createShaderFromText(const char* buffer,const char* techName);
	void setTexture(int shaderID,const char* handleName,int textureID);
	void setCurrentShader(int shaderID);
	int getCurrentShaderID() {
		return m_CurrentShader;
	}
	// Textures
	int createTexture(int width,int height);
	int loadTexture(const char* name);
	int loadTexture(const char* dirName,const char* name);
	int getTextureId(const char* name);
	int loadTextureWithColorKey(const char* name,const Color& color);
	void setTexture(int id,int index = 0);
	LPDIRECT3DTEXTURE9 getDirectTexture(int textureID);
	D3DLOCKED_RECT lockTexture(int id);
	void unlockTexture(int id);
	void fillTexture(int id,const Vector2f& pos,int sizeX,int sizeY,Color* colors);
	Vector2f getTextureSize(int idx);
	// System fonts	
	ID3DXFont* getSystemFont();
	
	RasterizerState* createRasterizerState(const char* name,int cullMode,int fillMode,bool multiSample = true,bool scissor = true);
	void changeRasterizerState(RasterizerState* rasterizerState);
	void setRasterizerState(RasterizerState* rasterizerState);
	// Bitmap Font
	void initializeBitmapFont(BitmapFont& bitmapFont,int textureID,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));
	BitmapFont* createBitmapFont(const char* name);
	// render target
	int createRenderTarget(uint32 id,const Color& clearColor = Color::WHITE);
	int createRenderTarget(uint32 id,float width,float height,const Color& clearColor = Color::WHITE);
	void setRenderTarget(uint32 id);
	void restoreBackBuffer(uint32 id);
	// blend states
	int createBlendState(int srcAlpha,int dstAlpha,bool alphaEnabled);
	int createBlendState(int srcRGB,int srcAlpha,int dstRGB,int dstAlpha,bool alphaEnabled = true,bool separateAlpha = false);
	void changeBlendState(int id);
	void setBlendState(int id);
	const int getCurrentBlendState() const {
		return m_CurrentBS;
	}
	const int getDefaultBlendState() const {
		return m_DefaultBS;
	}
	// buffers
	int createVertexBuffer(PrimitiveType primitiveType,int vertexDefinition,int size,bool dynamic = false);
	int createIndexBuffer(int size,bool dynamic = false);
	int createQuadIndexBuffer(uint32 maxQuads);
	int createBufferHandle(PrimitiveType primType,int vertexDefinition,GeoBufferType bufferType,bool dynamic = false);
	void lockBuffer(int handleID,int vertexCount,int indexCount,float** vertexBuffer,void** indexBuffer);
	void unlockBuffer(int handleID);	
	int drawBuffer(int handleID,int textureID);
	void resetBuffer(int handleID);
	void resetBufferHandle() {
		m_CurrentIB = -1;
		m_CurrentVB = -1;
	}

	DrawCounter& getDrawCounter() {
		return *m_DrawCounter;
	}
	DebugRenderer& getDebugRenderer() {
		return *m_DebugRenderer;
	}
	HWND getHWND() {
		return m_Hwnd;
	}
	uint32 startShader(Shader* shader);
	void setShaderParameter(Shader* shader,int textureID = -1);
	void endShader(Shader* shader);
	// debug messages
	void clearDebugMessages() {
		m_DebugMessages.clear();
	}
	void drawDebugMessages();
	void debug(int x,int y,const char* text);
	void debug(int x,int y,char* format,...);
	void debug(int x,int y,char* format,va_list args);
	void debug(int x,int y,const Color& color,char* format,...);
	void debug(int x,int y,const Color& color,char* format,va_list args);	
	void showProfiler(int x,int y);
	void showDrawCounter(int x,int y);
	void printDrawCounter();


	void draw(const Sprite& spriteObject) {
		m_SpriteBatch->draw(spriteObject);
	}
	void draw(const Vector2f& pos,int textureID,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0)) {
		m_SpriteBatch->draw(pos.x,pos.y,textureID,textureRect,rotation,scaleX,scaleY,color,center);
	}

	BitmapFont* loadBitmapFont(const char* name,int textureId,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

	Viewport* getViewport() {
		return m_Viewport;
	}
	const SpriteDescription& getSpriteDescription(int id) const {
		return m_DescriptionManager->get(id);
	}
	void setDescriptionManager(SpriteDescriptionManager* descriptionManager) {
		m_DescriptionManager = descriptionManager;
	}
private:	
	void loadSystemFont(const char* name,const char* fontName,int size,bool bold);
	bool isFillColor(const Color& fillColor,const Color& currentColor);
	void setTransformations();
	void handleRenderTargets();
	void createBasicVertexDeclarations();
	void setColor(D3DLOCKED_RECT& lockedRect,int x,int y,uchar r,uchar g,uchar b,uchar a,int height);
	Color getColor(D3DLOCKED_RECT& lockedRect,int x,int y,int height);
	int findFreeTextureSlot();
	int findFreeMaterialSlot();
	int findFreeShaderSlot();
	
	void initializeShader(int id,const char* techName);
	int allocateBuffer(GeoBufferType type,int vertexDefinition,int size,int& start,bool dynamic);
	void resetDynamicBuffers();
	SpriteDescriptionManager* m_DescriptionManager;
	Viewport* m_Viewport;
	D3DCAPS9 m_DeviceCaps;	
	Camera* m_Camera;	
	mat4 matWorldViewProj;
	mat4 m_World;
	GraphicsDevice *device;
	RenderStates m_RenderStates;
	Color m_ClearColor;
	int mode;
	DrawCounter* m_DrawCounter;
	DebugRenderer* m_DebugRenderer;
	HWND m_Hwnd;
	RasterizerStates m_RasterizerStates;
	// new stuff
	LPDIRECT3DSURFACE9 m_BackBuffer;	
	RasterizerState m_RSState;
	int m_DefaultBS;
	int m_Width;
	int m_Height;
	int m_UsedRTs;
	// all data structs
	VDStruct m_VDStructs[MAX_VERDECLS];
	Texture m_Textures[MAX_TEXTURES];
	Shader m_Shaders[MAX_SHADERS];
	int m_BMCounter;
	ID3DXFont* m_SystemFont;
	BlendState m_BlendStates[MAX_BLENDSTATES];
	GeometryBuffer m_Buffers[MAX_BUFFERS];
	GeoBufferHandle m_BufferHandles[MAX_BUFFER_HANDLES];
	BitmapFont m_BitmapFonts[MAX_FONTS];
	RenderTarget m_RenderTargets[MAX_RENDER_TARGETS];
	int m_DefaultShaderID;
	int m_CurrentRT;
	int m_BFCounter;
	int m_CurrentIB;
	int m_CurrentVB;
	int m_CurrentVD;
	int m_CurrentTextures[5];
	int m_CurrentShader;
	int m_CurrentBS;
	// debug
	DebugMessages m_DebugMessages;
	LPD3DXSPRITE m_DebugSprite;
	SpriteBatch* m_SpriteBatch;
};

};