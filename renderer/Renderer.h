#pragma once
#include "..\dxstdafx.h"
#include "GraphicsDevice.h"
#include "buffer_types.h"
#include <string>
#include "..\base\Settings.h"
#include <map>
#include "Camera.h"
#include "..\nodes\Node.h"
#include <vector>
#include "..\utils\Color.h"
#include "render_types.h"
#include "DrawCounter.h"
#include "..\content\TextureAtlas.h"

namespace ds {

// -------------------------------------------------------
// Constants
// -------------------------------------------------------	
const int MAX_VERDECLS       = 12;
const int MAX_TEXTURES       = 256;
const int MAX_MATERIALS      = 256;
const int MAX_ATLAS          = 10;
const int MAX_SHADERS        = 128;
const int MAX_BM_FONTS       = 16;
const int MAX_SYSTEM_FONTS   = 6;
const int MAX_BLENDSTATES    = 10;
const int MAX_BUFFERS        = 256;
const int MAX_BUFFER_HANDLES = 256;
// -------------------------------------------------------
// Vertex declaration types
// -------------------------------------------------------
const int VD_TTC   = 0;
const int VD_PTNBT = 1;

class ScreenOverlayNode;
class SkyNode;
class DebugRenderer;
class VertexDeclaration;

enum RenderMode {
	RM_2D, RM_3D
};

struct VDStruct {
	int vertexSize;
	VertexDeclaration* declaration;
};

class Renderer {

typedef std::map<D3DRENDERSTATETYPE,DWORD> RenderStates;
typedef List<RenderTarget*> RenderTargets;
typedef List<RasterizerState*> RasterizerStates;

public:
	Renderer(HWND hWnd,const Settings& settings);
	~Renderer();
	LPDIRECT3DDEVICE9 getDevice() { return device->get();}
	GraphicsDevice* getGraphicsDevice() {
		return device;
	}
	// basic rendering methods
	bool beginRendering(const Color& clearColor);
	void endRendering();	
	bool Windowed;
	bool rendering;
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
	RenderMode getRenderMode() { return m_RenderMode; }
	void setupMatrices();
	void applyTransformation(Node* node);
	
	//
	
	void setSkyNode(SkyNode* skyNode);
	//
	
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
	// Textures
	int createTexture(int width,int height);
	int loadTexture(const char* name);
	int loadTextureWithColorKey(const char* name,const Color& color);
	void setTexture(int id,int index = 0);
	LPDIRECT3DTEXTURE9 getDirectTexture(int textureID);
	D3DLOCKED_RECT lockTexture(int id);
	void unlockTexture(int id);
	// Materials
	int createMaterial(const char* name,int textureID = -1);
	void applyMaterial(int mtrlID);
	bool hasShader(int mtrlID);
	void setShader(int materialID,int shaderID);
	void setTextureAtlas(int materialID,int textureAtlasID);

	void drawNode(Node* node);
	// System fonts
	int loadSystemFont(const char* name,const char* fontName,int size,bool bold);
	ID3DXFont* getInternalSystemFont(int fontID);
	// Texture atlas
	int loadTextureAtlas(const char* name);
	bool getTextureCoordinates(int materialID,const char* itemName,float *u1,float* v1,float* u2,float* v2);
	int getAtlasItemWidth(int materialID,const char* itemName);
	int getAtlasItemHeight(int materialID,const char* itemName);

	RasterizerState* createRasterizerState(const char* name,int cullMode,int fillMode,bool multiSample = true,bool scissor = true);
	void changeRasterizerState(RasterizerState* rasterizerState);
	void setRasterizerState(RasterizerState* rasterizerState);
	// Bitmap Font
	int createBitmapFont(const BitmapFont& bitmapFont,int textureID,const Color& fillColor);
	int loadBitmapFont(const char* name,int textureID,Color& fillColor);
	const CharDef& getCharDef(int fontID,char c) const;
	int getCharHeight(int fontID);
	// render target
	int createRenderTarget(const char* name);
	// blend states
	int createBlendState(int srcAlpha,int dstAlpha,bool alphaEnabled);
	int createBlendState(int srcRGB,int srcAlpha,int dstRGB,int dstAlpha,bool alphaEnabled = true,bool separateAlpha = false);
	void changeBlendState(int id);
	void setBlendState(int id);
	// buffers
	int createVertexBuffer(PrimitiveType primitiveType,int vertexDefinition,int size,bool dynamic = false);
	int createIndexBuffer(int size,bool dynamic = false);
	int createQuadIndexBuffer(uint32 maxQuads);
	int createBufferHandle(PrimitiveType primType,int vertexDefinition,GeoBufferType bufferType,bool dynamic = false);
	void lockBuffer(int handleID,int vertexCount,int indexCount,float** vertexBuffer,void** indexBuffer);
	void unlockBuffer(int handleID);	
	int drawBuffer(int handleID);

	DrawCounter& getDrawCounter() {
		return *m_DrawCounter;
	}
	DebugRenderer& getDebugRenderer() {
		return *m_DebugRenderer;
	}
	HWND getHWND() {
		return m_Hwnd;
	}

	void setRenderTarget(const char* name);
	void restoreBackBuffer();
	const int getWidth() const {
		return m_Width;
	}
	const int getHeight() const {
		return m_Height;
	}
	void applyShader(Shader* shader);
private:	
	bool isFillColor(const Color& fillColor,const Color& currentColor);
	void setTransformations();
	void handleRenderTargets();
	void createBasicVertexDeclarations();
	void setColor(D3DLOCKED_RECT& lockedRect,int x,int y,uchar r,uchar g,uchar b,uchar a,int height);
	Color getColor(D3DLOCKED_RECT& lockedRect,int x,int y,int height);
	int findFreeTextureSlot();
	int findFreeMaterialSlot();
	int findFreeShaderSlot();
	TextureAtlas* getTextureAtlas(int materialID);
	
	void initializeShader(int id,const char* techName);
	int allocateBuffer(GeoBufferType type,int vertexDefinition,int size,int& start,bool dynamic);
	void resetDynamicBuffers();

	RenderMode m_RenderMode;	
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
	//Assignments m_Assignments;
	HWND m_Hwnd;
	RenderTargets m_RenderTargets;
	RasterizerStates m_RasterizerStates;
	SkyNode* m_SkyNode;
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
	Material m_Materials[MAX_MATERIALS];
	TextureAtlas m_TextureAtlas[MAX_ATLAS];
	Shader m_Shaders[MAX_SHADERS];
	BitmapFont m_BitmapFonts[MAX_BM_FONTS];
	int m_AtlasCounter;
	int m_BMCounter;
	SystemFont m_Fonts[MAX_SYSTEM_FONTS];
	BlendState m_BlendStates[MAX_BLENDSTATES];
	GeometryBuffer m_Buffers[MAX_BUFFERS];
	GeoBufferHandle m_BufferHandles[MAX_BUFFER_HANDLES];
	int m_CurrentIB;
	int m_CurrentVB;
	int m_CurrentVD;
	int m_CurrentTextures[5];
	int m_CurrentShader;
	int m_CurrentBS;
};

};