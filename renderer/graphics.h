#pragma once
#include "..\dxstdafx.h"
#include "buffer_types.h"
#include "..\base\Settings.h"
#include <vector>
#include "..\utils\Color.h"
#include "render_types.h"
#include "BitmapFont.h"
#include "Viewport.h"
#include "..\lib\container\dVector.h"
#include "VertexDeclaration.h"
#include "..\sprites\SpriteTemplates.h"

namespace ds {

	// -------------------------------------------------------
	// Constants
	// -------------------------------------------------------	
	const int MAX_VERDECLS = 12;
	const int MAX_TEXTURES = 256;
	const int MAX_SHADERS = 128;
	const int MAX_BLENDSTATES = 10;
	const int MAX_BUFFERS = 256;
	const int MAX_BUFFER_HANDLES = 256;
	const int MAX_RENDER_TARGETS = 32;
	// -------------------------------------------------------
	// Vertex declaration types
	// -------------------------------------------------------
	const int VD_TTC = 0;
	const int VD_PTNBT = 1;
	const int VD_PTC = 2;
	const int VD_PNC = 3;
	const int VD_PARTICLE = 4;


	struct Descriptor {
		int texture;
		int shader;
		int blendState;

		Descriptor() : texture(-1), shader(-1), blendState(0) {}
		Descriptor(int texture, int shader) : texture(texture), shader(shader), blendState(0) {}
		Descriptor(int texture, int shader, int blendState = 0) : texture(texture), shader(shader), blendState(blendState) {}
	};

	struct DescriptorData {

		int* textures;
		int* shaders;
		int* blendStates;
		char* buffer;
		int num;
		int total;

		DescriptorData() : buffer(0), num(0), total(0) {}

		int add(const Descriptor& desc) {
			textures[num++] = desc.texture;
			shaders[num - 1] = desc.shader;
			blendStates[num - 1] = desc.blendState;
			return num - 1;
		}

		bool get(int idx, Descriptor* desc) {
			if (idx >= 0 && idx < num) {
				desc->texture = textures[idx];
				desc->shader = shaders[idx];
				desc->blendState = blendStates[idx];
				return true;
			}
			return false;
		}
	};

	struct DrawCounter {

		int numPrim;
		int indexCounter;
		int sprites;
		int drawCalls;
		int textures;
		int shaders;
		uint32 memorySize;
		int flushes;
		int particles;

		void reset() {
			numPrim = 0;
			indexCounter = 0;
			sprites = 0;
			drawCalls = 0;
			textures = 0;
			shaders = 0;
			memorySize = 0;
			flushes = 0;
			particles = 0;
		}
	};

	class VertexDeclaration;

	struct VDStruct {
		int vertexSize;
		VertexDeclaration* declaration;
	};

	

	namespace renderer {

		void initialize(HWND hWnd, const Settings& settings);

		void shutdown();

		void initializeDevice(const Settings& settings);

		LPDIRECT3DDEVICE9 getDevice();

		//std::string D3DFormatToString(D3DFORMAT format, bool bWithPrefix);

		void writeInfo(const Settings& settings, const std::string& info);

		int getScreenWidth();

		int getScreenHeight();

		bool beginRendering(const Color& clearColor);

		void setupMatrices();

		DrawCounter& drawCounter();

		void clear();

		void flush();

		void endRendering();

		VDStruct& getVertexDeclaration(int declarationType);

		void initializeShader(int id, const char* techName);

		int createShaderFromText(const char* buffer, const char* techName);

		Shader& getShader(int id);

		int loadShader(const char* fxName, const char* techName);

		int getDefaultShaderID();

		int getCurrentShaderID();

		int createViewport(int width, int height);

		int createViewport(int screenWidth, int screenHeight, int worldWidth, int worldHeight);

		Viewport& getSelectedViewport();

		void selectViewport(int vw);

		void setTexture(int id, int index = 0);

		void setViewportPosition(int vw, const Vector2f& pos);

		int createBlendState(int srcAlpha, int dstAlpha, bool alphaEnabled);

		int createBlendState(int srcRGB, int srcAlpha, int dstRGB, int dstAlpha, bool alphaEnabled = true, bool separateAlpha = false);

		int getCurrentBlendState();

		int getDefaultBlendState();

		void changeBlendState(int id);

		void setBlendState(int id);

		void restoreDefaultBlendState();

		int createQuadIndexBuffer(int maxQuads);

		int getQuadIndexBufferIndex();

		int addDescriptor(const Descriptor& desc);

		int createVertexBuffer(int vertexDeclaration, int size, bool dynamic = true);

		VertexBuffer& getVertexBuffer(int index);

		void fillBuffer(int bufferIndex, void* data, int num);

		int createIndexBuffer(int size, bool dynamic = true);

		IndexBuffer& getIndexBuffer(int index);

		void draw(int descriptorID, int vertexBufferID,int numVerticces,int indexBufferID);

		void draw_render_target(int rtID, int shaderID = -1);

		void loadSystemFont(const char* name, const char* fontName, int size, bool bold);

		BitmapFont* createBitmapFont(const char* name);

		BitmapFont* loadBitmapFont(const char* name, int textureId, const Color& fillColor = Color(1.0f, 0.0f, 1.0f, 1.0f));

		void initializeBitmapFont(BitmapFont& bitmapFont, int textureID, const Color& fillColor = Color(1.0f, 0.0f, 1.0f, 1.0f));

		bool isFillColor(const Color& fillColor, const Color& currentColor);
		
		Color getColor(D3DLOCKED_RECT& lockedRect, int x, int y, int height);

		void setShaderParameter(Shader* shader, int textureID);

		uint32 startShader(int shaderID);

		void endShader(int shaderID);

		void setCurrentShader(int shaderID);
		
		void setTexture(int shaderID, const char* handleName, int textureID);

		int createRenderTarget(uint32 id, const Color& clearColor);

		int createRenderTarget(uint32 id, float width, float height, const Color& clearColor = Color::WHITE);

		void setRenderTarget(uint32 id);

		void restoreBackBuffer();

		int createTexture(int width, int height);

		int loadTexture(const char* name);

		int loadTextureBinary(const char* name);

		void reloadTextureBinary(const char* name);

		int loadTexture(const char* dirName, const char* name);

		int getTextureId(const char* name);

		int loadTextureWithColorKey(const char* name, const Color& color);

		LPDIRECT3DTEXTURE9 getDirectTexture(int textureID);

		D3DLOCKED_RECT lockTexture(int id);

		void unlockTexture(int id);

		Vector2f getTextureSize(int idx);

		int findFreeTextureSlot();

		void clearDebugMessages();

		const Sprite& getSpriteTemplate(int id);
		
		SpriteTemplates* getSpriteTemplates();

		bool hasSpriteTemplate(int id);
	}

	namespace debug {

		void drawDebugMessages();
		void debug(int x, int y, const char* text);
		void debug(int x, int y, char* format, ...);
		void debug(int x, int y, char* format, va_list args);
		void debug(int x, int y, const Color& color, char* format, ...);
		void debug(int x, int y, const Color& color, char* format, va_list args);
		void showProfiler(int x, int y);
		void showDrawCounter(int x, int y);
		void printDrawCounter();

	}

}
