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
#include "..\lib\container\dVector.h"
#include "VertexDeclaration.h"

struct SimpleParticleArray;
class Viewport;

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
const int VD_PNC   = 3;

class VertexDeclaration;

struct VDStruct {
	int vertexSize;
	VertexDeclaration* declaration;
};

// -------------------------------------------------------
// RenderContext
// -------------------------------------------------------
struct RenderContext {

	HWND hwnd;
	bool initialized;
	TextureAsset textures[MAX_TEXTURES];
	LPDIRECT3DDEVICE9 device;
	LPDIRECT3D9 pD3D;	
	D3DCAPS9 deviceCaps;
	int screenWidth;
	int screenHeight;
	D3DFORMAT format;
	DrawCounter drawCounter;
	VDStruct vdStructs[MAX_VERDECLS];
	dVector<Shader> shaders;
	//Shader shaders[MAX_SHADERS];
	//int numShaders;
	dVector<Viewport> viewPorts;
	int selectedViewPort;

	RenderContext() : initialized(false) , selectedViewPort(0) {}//, numShaders(0) {}

};

namespace renderer {

	VDStruct& getVertexDeclaration(int declarationType);

	void initializeShader(int id, const char* techName);

	int createShaderFromText(const char* buffer, const char* techName);

	Shader& getShader(int id);

	int loadShader(const char* fxName, const char* techName);

	int createViewport(int width, int height);

	int createViewport(int screenWidth, int screenHeight,int worldWidth,int worldHeight);

	Viewport& getSelectedViewport();

	void selectViewport(int vw);

	void setViewportPosition(int vw, const Vector2f& pos);

}

class DebugRenderer;


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
	
	void setTexture(int shaderID,const char* handleName,int textureID);
	void setCurrentShader(int shaderID);
	int getCurrentShaderID() {
		return m_CurrentShader;
	}
	
	void setTexture(int id,int index = 0);
	
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
	void restoreBackBuffer();
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
	void lockIndexBuffer(int handleID, int indexCount, void** indexBuffer);
	void unlockBuffer(int handleID);	
	void unlockIndexBuffer(int handleID);
	int drawBuffer(int handleID,int textureID);
	void resetBuffer(int handleID);
	void resetBufferHandle() {
		m_CurrentIB = -1;
		m_CurrentVB = -1;
	}
	
	
	DebugRenderer& getDebugRenderer() {
		return *m_DebugRenderer;
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
	
	void draw(int textureID, const Rect& textureRect, const ParticleArray& particleArray) {
		//m_SpriteBatch->draw(textureID, textureRect, particleArray);
	}

	BitmapFont* loadBitmapFont(const char* name,int textureId,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

	int getDefaultShaderID() {
		return m_DefaultShaderID;
	}

	int getDefaultBlendState() {
		return m_DefaultBS;
	}

	void flush();
private:	
	void loadSystemFont(const char* name,const char* fontName,int size,bool bold);
	bool isFillColor(const Color& fillColor,const Color& currentColor);
	void setTransformations();
	void handleRenderTargets();
	void createBasicVertexDeclarations();
	void setColor(D3DLOCKED_RECT& lockedRect,int x,int y,uchar r,uchar g,uchar b,uchar a,int height);
	Color getColor(D3DLOCKED_RECT& lockedRect,int x,int y,int height);
	//int findFreeTextureSlot();
	int findFreeMaterialSlot();
	
	void initializeShader(int id,const char* techName);
	int allocateBuffer(GeoBufferType type,int vertexDefinition,int size,int& start,bool dynamic);
	void resetDynamicBuffers();
	
	Camera* m_Camera;	
	mat4 matWorldViewProj;
	mat4 m_World;
	RenderStates m_RenderStates;
	Color m_ClearColor;
	int mode;
	
	DebugRenderer* m_DebugRenderer;
	RasterizerStates m_RasterizerStates;
	// new stuff
	LPDIRECT3DSURFACE9 m_BackBuffer;	
	RasterizerState m_RSState;
	int m_DefaultBS;
	int m_UsedRTs;
	// all data structs	
	//Shader m_Shaders[MAX_SHADERS];
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
	//SpriteBatch* m_SpriteBatch;
};

extern RenderContext* renderContext;

};