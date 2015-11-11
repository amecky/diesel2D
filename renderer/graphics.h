#pragma once
#include "..\dxstdafx.h"
#include "buffer_types.h"
#include <vector>
#include "..\utils\Color.h"
#include "render_types.h"
#include "BitmapFont.h"
#include "Viewport.h"
#include "..\lib\container\dVector.h"
#include "VertexDeclaration.h"
#include "..\sprites\SpriteTemplates.h"
#include "..\utils\StringUtils.h"
#include "..\compiler\AssetCompiler.h"
#include "Camera.h"
#include "..\base\Settings.h"
#include "..\sprites\SpriteGroup.h"

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
	const int VD_TTC      = 0;
	const int VD_PTNBT    = 1;
	const int VD_PTC      = 2;
	const int VD_PNC      = 3;
	const int VD_PARTICLE = 4;
	const int VD_PNTC     = 5;
	const int VD_UNKNOWN  = -1;

	namespace renderer {

		int getTextureId(IdString hash);

		int getShaderID(IdString hash);

		int getBlendStateID(IdString hash);

		int getDescriptorID(IdString hash);

		ID getMeshDataID(IdString hash);

		int getMeshID(IdString hash);
	}

	struct Descriptor {

		IdString hash;
		int texture;
		int shader;
		int blendState;

		Descriptor() : hash(0) , texture(-1), shader(-1), blendState(0) {}
		explicit Descriptor(const char* name) : texture(-1), shader(-1), blendState(0) {
			hash = string::murmur_hash(name);
		}
		Descriptor(int texture, int shader) : hash(0), texture(texture), shader(shader), blendState(0) {}
		Descriptor(int texture, int shader, int blendState = 0) : hash(0), texture(texture), shader(shader), blendState(blendState) {}
		Descriptor(const char* name,int texture, int shader) : texture(texture), shader(shader), blendState(0) {
			hash = string::murmur_hash(name);
		}
	};

	struct DescriptorData : public Serializer {

		IdString* hashes;
		int* textures;
		int* shaders;
		int* blendStates;
		char* buffer;
		int num;
		int total;

		DescriptorData() : buffer(0), num(0), total(0) {}

		int add(const Descriptor& desc) {
			hashes[num++] = desc.hash;
			textures[num - 1] = desc.texture;
			shaders[num - 1] = desc.shader;
			blendStates[num - 1] = desc.blendState;
			return num - 1;
		}

		bool get(int idx, Descriptor* desc) {
			if (idx >= 0 && idx < num) {
				desc->hash = hashes[idx];
				desc->texture = textures[idx];
				desc->shader = shaders[idx];
				desc->blendState = blendStates[idx];
				return true;
			}
			return false;
		}

		bool find(const char* name, Descriptor* desc) {
			int idx = -1;
			IdString myHash = string::murmur_hash(name);
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					idx = i;
				}
			}
			if (idx >= 0 && idx < num) {
				desc->hash = hashes[idx];
				desc->texture = textures[idx];
				desc->shader = shaders[idx];
				desc->blendState = blendStates[idx];
				return true;
			}
			return false;
		}

		int getID(const char* name) {
			IdString myHash = string::murmur_hash(name);
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					return i;
				}
			}			
			return -1;
		}

		int getID(IdString myHash) {
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					return i;
				}
			}
			return -1;
		}

		void load(BinaryLoader* loader) {
			while (loader->openChunk() == 0) {
				if (loader->getChunkID() == CHNK_DESCRIPTOR) {
					IdString name;
					loader->read(&name);
					IdString texture;
					loader->read(&texture);
					int tid = -1;
					if (texture != 0) {
						tid = renderer::getTextureId(texture);
						assert(tid != -1);
					}
					IdString shader;
					loader->read(&shader);
					int sid = renderer::getShaderID(shader);
					assert(sid != -1);
					IdString blendState;
					loader->read(&blendState);
					int bid = renderer::getBlendStateID(blendState);
					assert(bid != -1);
					int idx = getID(name);
					if (idx == -1) {
						// new descriptor
						Descriptor desc;
						desc.hash = name;
						desc.texture = tid;
						desc.shader = sid;
						desc.blendState = bid;
						add(desc);
					}
					else {
						Descriptor desc;
						get(idx, &desc);
						desc.texture = tid;
						desc.shader = sid;
						desc.blendState = bid;
					}					
				}
				loader->closeChunk();
			}
		}
	};

	struct MeshArray : public Serializer {

		IdString* hashes;
		int* descriptors;
		ID* dataIDs;
		int* vertexTypes;
		char* buffer;
		int num;
		int total;

		MeshArray() : buffer(0), num(0), total(0) {}

		int add(const Mesh& m) {
			hashes[num++] = m.hashName;
			descriptors[num - 1] = m.descriptorID;
			dataIDs[num - 1] = m.meshDataID;
			vertexTypes[num - 1] = m.vertexType;
			return num - 1;
		}

		bool get(int idx, Mesh* m) {
			if (idx >= 0 && idx < num) {
				m->hashName = hashes[idx];
				m->descriptorID = descriptors[idx];
				m->meshDataID= dataIDs[idx];
				m->vertexType= vertexTypes[idx];
				return true;
			}
			return false;
		}

		bool find(const char* name, Mesh* m) {
			int idx = -1;
			IdString myHash = string::murmur_hash(name);
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					idx = i;
				}
			}
			if (idx >= 0 && idx < num) {
				m->hashName = hashes[idx];
				m->descriptorID = descriptors[idx];
				m->meshDataID = dataIDs[idx];
				m->vertexType = vertexTypes[idx];
				return true;
			}
			return false;
		}

		int getID(const char* name) {
			IdString myHash = string::murmur_hash(name);
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					return i;
				}
			}
			return -1;
		}

		int getID(IdString myHash) {
			for (int i = 0; i < num; ++i) {
				if (hashes[i] == myHash) {
					return i;
				}
			}
			return -1;
		}

		void load(BinaryLoader* loader) {
			while (loader->openChunk() == 0) {
				if (loader->getChunkID() == CHNK_MESH) {
					IdString name;
					loader->read(&name);
					IdString dataName;
					loader->read(&dataName);
					int vt = -1;
					loader->read(&vt);
					IdString descName;
					loader->read(&descName);
					int descID = renderer::getDescriptorID(descName);
					XASSERT(descID != -1, "Unable to find matching descriptor");
					ID dataID = renderer::getMeshDataID(dataName);
					XASSERT(dataID != INVALID_ID, "Unable to find mesh data");
					int idx = getID(name);
					if (idx == -1) {
						// new descriptor
						Mesh m;
						m.hashName = name;
						m.descriptorID = descID;
						m.vertexType = vt;
						m.meshDataID = dataID;
						add(m);
					}
					else {
						descriptors[idx] = descID;
						vertexTypes[idx] = vt;
						dataIDs[idx] = dataID;
						
					}
				}
				loader->closeChunk();
			}
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

		Camera* getCamera();

		void setWorldMatrix(const mat4& world);

		void set2DCameraOff();

		bool beginRendering(const Color& clearColor);

		void setupMatrices();

		DrawCounter& drawCounter();

		void clear();

		void flush();

		void endRendering();		

		VDStruct& getVertexDeclaration(int declarationType);

		int createShaderFromText(const char* buffer, const char* techName);

		Shader* getShader(int id);

		//int loadShader(const char* fxName, const char* techName);

		int createEmptyShader(const char* name);

		int getDefaultShaderID();

		int getCurrentShaderID();

		int createViewport(int width, int height);

		int createViewport(int screenWidth, int screenHeight, int worldWidth, int worldHeight);

		Viewport& getSelectedViewport();

		void selectViewport(int vw);

		void setTexture(int id, int index = 0);

		void setViewportPosition(int vw, const Vector2f& pos);

		int createBlendState(const char* name, int srcAlpha, int dstAlpha, bool alphaEnabled);

		int createBlendState(const char* name, int srcRGB, int srcAlpha, int dstRGB, int dstAlpha, bool alphaEnabled = true, bool separateAlpha = false);

		int getCurrentBlendState();

		int getDefaultBlendState();

		void changeBlendState(int id);

		void setBlendState(int id);

		void restoreDefaultBlendState();

		int createQuadIndexBuffer(int maxQuads);

		int getQuadIndexBufferIndex();

		// -------------------------------------------------
		// descriptor
		// -------------------------------------------------
		DescriptorData* getDescriptorData();

		int getDescriptorID(const char* name);

		int addDescriptor(const Descriptor& desc);

		int createVertexBuffer(int vertexDeclaration, int size, bool dynamic = true);

		VertexBuffer& getVertexBuffer(int index);

		void fillBuffer(int bufferIndex, void* data, int num);

		int createIndexBuffer(int size, bool dynamic = true);

		IndexBuffer& getIndexBuffer(int index);

		void draw(int descriptorID, int vertexBufferID,int numVerticces,int indexBufferID);

		void draw_render_target(RTID rtID, int shaderID = -1);

		void draw_render_target_additive(RTID rtID, int shaderID = -1);

		// bitmap fonts

		std::vector<BitmapFont*>& getBitmapFontList();

		BitmapFont* getBitmapFont(const char* name);

		BitmapFont* createBitmapFont(const char* name, int textureID);

		BitmapFont* loadBitmapFont(const char* name, int textureId, const Color& fillColor = Color(1.0f, 0.0f, 1.0f, 1.0f));

		void initializeBitmapFont(BitmapFont* bitmapFont, int textureID, const Color& fillColor = Color(1.0f, 0.0f, 1.0f, 1.0f));

		bool isFillColor(const Color& fillColor, const Color& currentColor);
		
		Color getColor(D3DLOCKED_RECT& lockedRect, int x, int y, int height);

		// shader

		void setShaderParameter(Shader* shader, int textureID);

		void setCurrentShader(int shaderID);
		
		void setTexture(int shaderID, const char* handleName, int textureID);

		RTID createRenderTarget(const Color& clearColor);

		RTID createRenderTarget(float width, float height, const Color& clearColor = Color::WHITE);

		void setRenderTarget(RTID rtid);

		void restoreBackBuffer();

		int createTexture(int width, int height);

		int loadTexture(const char* name);

		int loadTextureBinary(const char* name);

		void reloadTextureBinary(const char* name);

		int loadTexture(const char* dirName, const char* name);

		int getTextureId(const char* name);

		//int getTextureId(IdString hash);

		int loadTextureWithColorKey(const char* name, const Color& color);

		LPDIRECT3DTEXTURE9 getDirectTexture(int textureID);

		D3DLOCKED_RECT lockTexture(int id);

		void unlockTexture(int id);

		Vector2f getTextureSize(int idx);

		int findFreeTextureSlot();

		//void loadSpriteTemplates(const char* name);

		bool getSpriteTemplate(const char* name, Sprite* sprite);
		
		SpriteTemplates* getSpriteTemplates();

		bool hasSpriteTemplate(int id);

		// -------------------------------------------------
		// meshes
		// -------------------------------------------------
		ID createMeshData(const char* name);

		ID loadMeshData(const char* name,const char* fileName);

		MeshData& getMeshData(ID id);

		void getMesh(int id,Mesh* m);

		int createMesh(const char* name, int descriptorID, ID dataID, int vertexType);

		int getMeshID(const char* name);

		int getVDType(const char* name);

		MeshArray* getMeshArray();

		Vector2f& getMousePosition();

		void setMousePosition(int x, int y);

		// -------------------------------------------------
		// sprite group
		// -------------------------------------------------
		SpriteGroup* getSpriteGroup(const char* name);

		SpriteGroupContainer* getSpriteGroupContainer();
	}

	namespace debug {

		Vector2f getTextSize(const char* text);
		void loadSystemFont(const char* name, const char* fontName, int size, bool bold);
		void reset();
		//void initialize(BitmapFont* font,float ts = 1.0f);
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
