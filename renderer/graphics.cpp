#include "graphics.h"
#include "..\utils\Profiler.h"
#include "shader.h"
#include "..\math\matrix.h"
#include "..\sprites\SpriteTemplates.h"
#include "..\utils\font.h"
#include "..\utils\ObjLoader.h"
#include "..\utils\FileUtils.h"
#include "..\sprites\SpriteBatch.h"
#include "..\DialogResources.h"
#include "..\io\FileRepository.h"

namespace ds {

	typedef std::map<D3DRENDERSTATETYPE, DWORD> RenderStates;
	typedef List<RasterizerState*> RasterizerStates;

	struct RenderTargetQuad {

		int vertexBufferID;
		int indexBufferID;
		PTCVertex vertices[4];

	};

	const int MAX_RT = 16;
	// -------------------------------------------------------
	// RenderContext
	// -------------------------------------------------------
	struct RenderContext {

		HWND hwnd;
		Color clearColor;
		bool initialized;
		TextureAsset textures[MAX_TEXTURES];
		LPDIRECT3DDEVICE9 device;
		LPDIRECT3D9 pD3D;
		D3DCAPS9 deviceCaps;
		LPDIRECT3DSURFACE9 backBuffer;
		int screenWidth;
		int screenHeight;
		int viewportWidth;
		int viewportHeight;
		DrawCounter drawCounter;
		D3DFORMAT format;
		VDStruct vdStructs[MAX_VERDECLS];
		Array<Shader*> shaders;
		int defaultShaderID;
		int currentShaderID;
		dVector<Viewport> viewPorts;
		int selectedViewPort;
		dVector<VertexBuffer> vertexBuffers;
		dVector<IndexBuffer> indexBuffers;
		BlendState blendStates[MAX_BLENDSTATES];
		int numBlendStates;
		int currentBlendState;
		int defaultBlendState;
		int quadIndexBufferIndex;
		DescriptorData descriptorData;
		int defaultDescriptor;
		int currentDescriptor;
		Array<BitmapFont*> bitmapFonts;
		int currentTextures[5];
		dVector<RenderTarget> renderTargets;
		int usedRenderTargets;
		int currentRenderTarget;
		mat4 matWorldViewProj;
		mat4 matWorld;
		mat4 matProjection;
		mat4 matView;
		Camera* camera;
		RenderStates renderStates;
		Color m_ClearColor;
		int mode;
		RasterizerStates rasterizerStates;
		// new stuff
		RasterizerState rsState;
		// debug
		RenderTargetQuad renderTargetQuad;
		SpriteTemplates spriteTemplates;
		SpriteGroupContainer spriteGroudContainer;
		dVector<MeshData> meshData;
		MeshArray meshes;
		Vector2f mousePosition;

		RenderContext() : initialized(false), selectedViewPort(0), numBlendStates(0) {
			drawCounter.reset();
		}

	};

	RenderContext* renderContext;

	namespace renderer {

		// -------------------------------------------------------
		// Create basic vertex declarations
		// -------------------------------------------------------
		void createBasicVertexDeclarations() {
			// transformed texture color
			VertexDeclaration* vd = new VertexDeclaration;
			vd->addElement(ds::VT_FLOAT4, ds::VDU_POSITIONT);
			vd->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD);
			vd->addElement(ds::VT_FLOAT4, ds::VDU_COLOR);
			vd->create();
			renderContext->vdStructs[VD_TTC].vertexSize = 40;
			renderContext->vdStructs[VD_TTC].declaration = vd;
			// position tangent binormal normal texture coords
			VertexDeclaration* ptbntVD = new VertexDeclaration;
			ptbntVD->addElement(ds::VT_FLOAT3, ds::VDU_POSITION);
			ptbntVD->addElement(ds::VT_FLOAT3, ds::VDU_TANGENT);
			ptbntVD->addElement(ds::VT_FLOAT3, ds::VDU_BINORMAL);
			ptbntVD->addElement(ds::VT_FLOAT3, ds::VDU_NORMAL);
			ptbntVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD);
			ptbntVD->create();
			renderContext->vdStructs[VD_PTNBT].vertexSize = 56;
			renderContext->vdStructs[VD_PTNBT].declaration = ptbntVD;

			VertexDeclaration* pntcVD = new VertexDeclaration;
			pntcVD->addElement(ds::VT_FLOAT3, ds::VDU_POSITION);
			pntcVD->addElement(ds::VT_FLOAT3, ds::VDU_NORMAL);
			pntcVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD);
			pntcVD->addElement(ds::VT_FLOAT4, ds::VDU_COLOR);
			pntcVD->create();
			renderContext->vdStructs[VD_PNTC].vertexSize = 48;
			renderContext->vdStructs[VD_PNTC].declaration = pntcVD;

			VertexDeclaration* ptcVD = new VertexDeclaration;
			ptcVD->addElement(ds::VT_FLOAT3, ds::VDU_POSITION);
			ptcVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD);
			ptcVD->addElement(ds::VT_FLOAT4, ds::VDU_COLOR);
			ptcVD->create();
			renderContext->vdStructs[VD_PTC].vertexSize = 36;
			renderContext->vdStructs[VD_PTC].declaration = ptcVD;

			VertexDeclaration* pncVD = new VertexDeclaration;
			pncVD->addElement(ds::VT_FLOAT3, ds::VDU_POSITION);
			pncVD->addElement(ds::VT_FLOAT3, ds::VDU_NORMAL);
			pncVD->addElement(ds::VT_FLOAT4, ds::VDU_COLOR);
			pncVD->create();
			renderContext->vdStructs[VD_PNC].vertexSize = 40;
			renderContext->vdStructs[VD_PNC].declaration = pncVD;

			VertexDeclaration* particleVD = new VertexDeclaration;
			particleVD->addElement(ds::VT_FLOAT3, ds::VDU_POSITION); // pos
			particleVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD); // uv
			particleVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD); // scale
			particleVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD); // dimension
			particleVD->addElement(ds::VT_FLOAT2, ds::VDU_TEXCOORD); // rotation / index	
			particleVD->addElement(ds::VT_FLOAT4, ds::VDU_COLOR); // color
			particleVD->create();
			renderContext->vdStructs[VD_PARTICLE].vertexSize = 60;
			renderContext->vdStructs[VD_PARTICLE].declaration = particleVD;
		}

		void getRenderStates() {
			//DWORD value;
			for (DWORD rs = 7; rs < 209; rs++) {
				D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)(rs);
				/*
				device->get()->GetRenderState(type,&value);
				m_RenderStates[type] = value;
				*/
				renderContext->renderStates[type] = -1;
			}
		}

		void setRenderState(D3DRENDERSTATETYPE rs, DWORD value) {
			DWORD cachedValue = renderContext->renderStates[rs];
			if (cachedValue == -1 || cachedValue != value) {
				renderContext->renderStates[rs] = value;
				renderContext->device->SetRenderState(rs, value);
			}
		}

		DWORD getRenderState(D3DRENDERSTATETYPE rs) {
			DWORD cachedValue = renderContext->renderStates[rs];
			if (cachedValue == -1) {
				renderContext->device->GetRenderState(rs, &cachedValue);
			}
			return cachedValue;
		}

		// ---------------------------------------------------------------------
		// build special quad buffer to render a render target
		// ---------------------------------------------------------------------
		void buildRenderTargetQuad() {
			renderContext->renderTargetQuad.vertices[0].uv.x = 0.0f;
			renderContext->renderTargetQuad.vertices[0].uv.y = 0.0f;
			renderContext->renderTargetQuad.vertices[1].uv.x = 1.0f;
			renderContext->renderTargetQuad.vertices[1].uv.y = 0.0f;
			renderContext->renderTargetQuad.vertices[2].uv.x = 1.0f;
			renderContext->renderTargetQuad.vertices[2].uv.y = 1.0f;
			renderContext->renderTargetQuad.vertices[3].uv.x = 0.0f;
			renderContext->renderTargetQuad.vertices[3].uv.y = 1.0f;

			float hw = renderContext->screenWidth / 2;
			float hh = renderContext->screenHeight / 2;

			const float VP_ARRAY[] = {
				-1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, -1.0f, -1.0f
			};
			Vector2f p;
			for (int i = 0; i < 4; ++i) {
				p.x = VP_ARRAY[i * 2] * hw;
				p.y = VP_ARRAY[i * 2 + 1] * hh;
				renderContext->renderTargetQuad.vertices[i].x = p.x;
				renderContext->renderTargetQuad.vertices[i].y = p.y;
				renderContext->renderTargetQuad.vertices[i].z = 0.0f;
				renderContext->renderTargetQuad.vertices[i].color = Color::WHITE;
			}
			renderContext->renderTargetQuad.vertexBufferID = createVertexBuffer(VD_PTC, 4, false);
			renderContext->renderTargetQuad.indexBufferID = renderContext->quadIndexBufferIndex;
			
		}

		void initialize(HWND hWnd, const Settings& settings) {
			renderContext = new RenderContext;
			renderContext->hwnd = hWnd;

			LOG << "RenderDevice::RenderDevice";
			LOG << "Preparing internal structs";
			for (int i = 0; i < MAX_TEXTURES; ++i) {
				renderContext->textures[i].texture = 0;
				renderContext->textures[i].flags = 0;
				renderContext->textures[i].name = 0;
			}
			for (int i = 0; i < MAX_BLENDSTATES; ++i) {
				renderContext->blendStates[i].flag = 0;
			}
			for (int i = 0; i < MAX_VERDECLS; ++i) {
				renderContext->vdStructs[i].vertexSize = 0;
			}
			// create the internal device
			renderer::initializeDevice(settings);
			int vp = renderer::createViewport(settings.screenWidth, settings.screenHeight, settings.screenWidth, settings.screenHeight);
			renderContext->viewportWidth = settings.screenWidth;
			renderContext->viewportHeight = settings.screenHeight;
			//int centerX = settings.width / 2;
			//int centerY = settings.height / 2;
			//renderContext->viewPort.setPosition(centerX,centerY);
			// the camera
			float aspect = (float)settings.screenWidth / (float)settings.screenHeight;
			renderContext->camera = new Camera(settings.screenWidth, settings.screenHeight);
			renderContext->camera->setLens(D3DX_PI * 0.25f, aspect, 0.1f, 1000.0f);
			renderContext->device->SetTransform(D3DTS_PROJECTION, &matrix::convert(renderContext->camera->getProjectionMatrix()));
			renderContext->matWorld = matrix::m4identity();
			setRenderState(D3DRS_LIGHTING, FALSE);
			setRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			setRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			setRenderState(D3DRS_ZENABLE, true);
			setRenderState(D3DRS_ZWRITEENABLE, TRUE);
			setRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			setRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
			setRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
			//mode = settings.mode;
			renderContext->clearColor = settings.clearColor;
			getRenderStates();
			//m_PostProcessing = settings.postProcessing;	
			renderContext->defaultShaderID = shader::createPTCShader(-1);
			renderContext->defaultBlendState = renderer::createBlendState("default",BL_SRC_ALPHA, BL_ONE_MINUS_SRC_ALPHA, true);
			//renderContext->defaultBlendState = renderer::createBlendState("rt_blend", BL_SRC_ALPHA, BL_ZERO, BL_ONE_MINUS_SRC_ALPHA, BL_ONE_MINUS_SRC_ALPHA, true);
			renderContext->defaultBlendState = renderer::createBlendState("rt_blend", BL_SRC_ALPHA, BL_ONE_MINUS_SRC_ALPHA, true);
			LOG << "'default' blendstate " << renderContext->defaultBlendState;
			int addBS = renderer::createBlendState("alpha_blend", ds::BL_ONE, ds::BL_ONE, true);
			LOG << "'alpha_blend' blendstate " << addBS;

			Descriptor desc;
			desc.hash = string::murmur_hash("default_descriptor");
			desc.texture = 0;
			desc.shader = renderContext->defaultShaderID;
			desc.blendState = renderContext->defaultBlendState;
			renderContext->defaultDescriptor = renderer::addDescriptor(desc);

			renderContext->currentBlendState = -1;
			createBasicVertexDeclarations();
			renderContext->device->GetRenderTarget(0, &renderContext->backBuffer);
			// create default buffers
			renderContext->quadIndexBufferIndex = renderer::createQuadIndexBuffer(8192);
			buildRenderTargetQuad();
		}

		// ---------------------------------------------------------------------
		// shutdown
		// ---------------------------------------------------------------------
		void shutdown() {
			LOG << "destructing Renderer";
			if (renderContext->meshes.buffer != 0) {
				delete[] renderContext->meshes.buffer;
			}
			for (int i = 0; i < renderContext->meshData.size(); ++i) {
				if (renderContext->meshData[i].faces != 0) {
					delete[] renderContext->meshData[i].faces;
				}
			}
			renderContext->rasterizerStates.deleteContents();
			LOG << "Releasing textures";
			for (int i = 0; i < MAX_TEXTURES; ++i) {
				if (renderContext->textures[i].flags != 0) {
					SAFE_RELEASE(renderContext->textures[i].texture);
				}
			}
			LOG << "Releasing shaders";
			for (int i = 0; i < renderContext->shaders.size(); ++i) {				
				renderContext->shaders[i]->release();
				delete renderContext->shaders[i];
			}
			LOG << "Releasing vertex declarations";
			for (int i = 0; i < MAX_VERDECLS; ++i) {
				if (renderContext->vdStructs[i].vertexSize != 0) {
					delete renderContext->vdStructs[i].declaration;
				}
			}
			if (renderContext->descriptorData.buffer != 0) {
				delete[] renderContext->descriptorData.buffer;
			}
			for (int i = 0; i < renderContext->vertexBuffers.size(); ++i) {
				SAFE_RELEASE(renderContext->vertexBuffers[i].buffer);
			}
			for (int i = 0; i < renderContext->indexBuffers.size(); ++i) {
				SAFE_RELEASE(renderContext->indexBuffers[i].buffer);
			}
			for (int i = 0; i < renderContext->renderTargets.size(); ++i) {
				SAFE_RELEASE(renderContext->renderTargets[i].rts);
				SAFE_RELEASE(renderContext->renderTargets[i].surface);
			}
			for (size_t i = 0; i < renderContext->bitmapFonts.size(); ++i) {
				delete renderContext->bitmapFonts[i];
			}
			delete renderContext->camera;
			if (renderContext->device != NULL) {
				renderContext->device->Release();
			}
			if (renderContext->pD3D != NULL) {
				renderContext->pD3D->Release();
			}
			delete renderContext;
		}

		// ---------------------------------------------------------------------
		// initialize DirectX device
		// ---------------------------------------------------------------------
		void initializeDevice(const Settings& settings) {
			// Create the D3D object.
			renderContext->pD3D = Direct3DCreate9(D3D_SDK_VERSION);

			D3DDISPLAYMODE d3ddm;
			HR(renderContext->pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm));

			renderContext->pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &renderContext->deviceCaps);


			// Set up the structure used to create the D3DDevice
			D3DPRESENT_PARAMETERS m_pp;
			ZeroMemory(&m_pp, sizeof(m_pp));
			D3DFORMAT adapterFormat = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(renderContext->pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8))) {
				m_pp.AutoDepthStencilFormat = D3DFMT_D24S8;
			}
			else if (SUCCEEDED(renderContext->pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8))) {
				m_pp.AutoDepthStencilFormat = D3DFMT_D24X8;
			}
			else if (SUCCEEDED(renderContext->pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16))) {
				m_pp.AutoDepthStencilFormat = D3DFMT_D16;
			}
			m_pp.BackBufferWidth = settings.screenWidth;
			m_pp.BackBufferHeight = settings.screenHeight;
			m_pp.BackBufferFormat = adapterFormat;
			m_pp.BackBufferCount = 1;
			DWORD total;

			if (SUCCEEDED(renderContext->pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, true, D3DMULTISAMPLE_NONMASKABLE, &total))) {
				m_pp.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
				m_pp.MultiSampleQuality = total - 1;
				LOG << "multi sample is supported - quality level " << total - 1;
			}
			else {
				LOG << "NO multi sample is supported";
				m_pp.MultiSampleType = D3DMULTISAMPLE_NONE;
				m_pp.MultiSampleQuality = 0;
			}
			m_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			m_pp.hDeviceWindow = renderContext->hwnd;
			if (settings.fullScreen) {
				m_pp.Windowed = false;
			}
			else {
				m_pp.Windowed = true;
			}
			m_pp.EnableAutoDepthStencil = TRUE;
			m_pp.AutoDepthStencilFormat = D3DFMT_D16;
			if (settings.synched) {
				m_pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}
			else {
				m_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			}
			//m_pp.FullScreen_RefreshRateInHz = (Windowed) ? 0 : m_displayMode.RefreshRate;

			DWORD vertexProcessing = 0;
			if (renderContext->deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			{
				vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				// Check for pure device
				if (renderContext->deviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
				{
					vertexProcessing |= D3DCREATE_PUREDEVICE;
				}
			}
			else
			{
				vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}

			// Create the D3DDevice
			HR(renderContext->pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, renderContext->hwnd, vertexProcessing, &m_pp, &renderContext->device));

			D3DFORMAT format = d3ddm.Format;
			std::string info = "xxxxx";
			HR(renderContext->device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
			HR(renderContext->device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
			renderContext->screenWidth = settings.screenWidth;
			renderContext->screenHeight = settings.screenHeight;
			writeInfo(settings, info);

		}

		// ---------------------------------------------------------------------
		// write info about graphic settings
		// ---------------------------------------------------------------------
		void writeInfo(const Settings& settings, const std::string& info) {
			LOG << "Hardware informations";
			LOG << "Maximum primitives: " << renderContext->deviceCaps.MaxPrimitiveCount;
			LOG << "Vertex shader version: " << ((renderContext->deviceCaps.VertexShaderVersion >> 8) & 0xFF) << "." << ((renderContext->deviceCaps.VertexShaderVersion >> 0) & 0xFF);
			LOG << "Pixel shader version: " << ((renderContext->deviceCaps.PixelShaderVersion >> 8) & 0xFF) << "." << ((renderContext->deviceCaps.PixelShaderVersion >> 0) & 0xFF);
			LOG << "Settings";
			LOG << "Format: " << info;
			LOG << "Width: " << settings.screenWidth;
			LOG << "Height " << settings.screenHeight;
			LOG << "Mode: " << settings.mode;
			LOG << "Synched: " << settings.synched;
		}

		LPDIRECT3DDEVICE9 getDevice() {
			return renderContext->device;
		}

		int getScreenWidth() {
			return renderContext->viewportWidth;
		}

		int getScreenHeight() {
			return renderContext->screenHeight;
		}

		Camera* getCamera() {
			return renderContext->camera;
		}

		VDStruct& getVertexDeclaration(int declarationType) {
			return renderContext->vdStructs[declarationType];
		}

		// FIXME: add name as parameter
		int createShaderFromText(const char* buffer, const char* techName) {
			int id = renderContext->shaders.size();
			Shader* shader = new Shader(id,techName);
			renderContext->shaders.push_back(shader);
			shader->createFromText(buffer, techName);
			return id;
		}

		int getShaderID(IdString hash) {
			for (int i = 0; i < renderContext->shaders.size(); ++i) {
				if (renderContext->shaders[i]->getHashName() == hash) {
					return i;
				}
			}
			return -1;
		}

		Shader* getShader(int id) {
			return renderContext->shaders[id];
		}

		Shader* getShader(const char* name) {
			IdString hash = string::murmur_hash(name);
			int sid = getShaderID(hash);
			if (sid != -1) {
				return renderContext->shaders[sid];
			}
			return 0;
		}

		int getDefaultShaderID() {
			return renderContext->defaultShaderID;
		}

		int createEmptyShader(const char* name) {
			int id = renderContext->shaders.size();
			Shader* shader = new Shader(id, name);
			renderContext->shaders.push_back(shader);
			return id;			
		}
		
		int loadShader(const char* fxName, const char* techName) {			
			int id = renderContext->shaders.size();
			Shader* shader = new Shader(id,fxName);
			renderContext->shaders.push_back(shader);
			if (shader->loadShader(fxName, techName)) {
				return id;
			}
			return -1;			
		}
		
		int createViewport(int width, int height) {
			int idx = renderContext->viewPorts.size();
			Viewport v;
			v.setDimension(width, height);
			v.setPosition(width / 2, height / 2);
			renderContext->viewPorts.add(v);
			return idx;
		}

		int createViewport(int screenWidth, int screenHeight, int worldWidth, int worldHeight) {
			int idx = renderContext->viewPorts.size();
			Viewport v;
			v.setDimension(screenWidth, screenHeight, worldWidth, worldHeight);
			v.setPosition(worldWidth / 2, worldHeight / 2);
			renderContext->viewPorts.add(v);
			return idx;
		}

		Viewport& getSelectedViewport() {
			return renderContext->viewPorts[renderContext->selectedViewPort];
		}

		Viewport& getViewport(int index) {
			return renderContext->viewPorts[index];
		}

		void selectViewport(int vw) {
			assert(vw >= 0);
			assert(vw < renderContext->viewPorts.size());
			renderContext->selectedViewPort = vw;
		}

		void setViewportPosition(int vw, const Vector2f& pos) {
			assert(vw >= 0);
			assert(vw < renderContext->viewPorts.size());
			renderContext->viewPorts[vw].setPosition(pos);
		}

		int createBlendState(const char* name, int srcRGB, int srcAlpha, int dstRGB, int dstAlpha, bool alphaEnabled, bool separateAlpha) {
			if (renderContext->numBlendStates < MAX_BLENDSTATES) {
				BlendState& bs = renderContext->blendStates[renderContext->numBlendStates++];
				bs.hashName = string::murmur_hash(name);
				bs.blendEnable = alphaEnabled;
				bs.srcFactorRGB = srcRGB;
				bs.srcFactorAlpha = srcAlpha;
				bs.blendModeRGB = BM_ADD;
				bs.dstFactorRGB = dstRGB;
				bs.dstFactorAlpha = dstAlpha;
				bs.blendModeAlpha = BM_ADD;
				bs.alphaFunc = ALPHA_GREATER;
				bs.alphaRef = 0;
				bs.separateAlpha = separateAlpha;
				bs.mask = 0x0f;
				bs.flag = 1;
				LOG << "created new blendstate '" << name << "' - id: " << renderContext->numBlendStates - 1;
				return renderContext->numBlendStates - 1;
			}
			else {
				LOGE << "No more free slots for BlendStates available";
				return -1;
			}
		}

		int getBlendStateID(IdString hash) {
			for (int i = 0; i < MAX_BLENDSTATES; ++i) {
				BlendState& bs = renderContext->blendStates[i];
				if (bs.hashName == hash) {
					return i;
				}
			}
			return -1;
		}

		int createBlendState(const char* name,int srcAlpha, int dstAlpha, bool alphaEnabled) {
			return createBlendState(name,srcAlpha, srcAlpha, dstAlpha, dstAlpha, alphaEnabled);
		}

		int getCurrentBlendState() {
			return renderContext->currentBlendState;
		}
		int getDefaultBlendState() {
			return renderContext->defaultBlendState;
		}

		// -----------------------------------------------------------------
		// Change current blend state if necessary
		// -----------------------------------------------------------------
		void changeBlendState(int id) {
			const BlendState& newState = renderContext->blendStates[id];
			BlendState* current = &renderContext->blendStates[renderContext->currentBlendState];
			LPDIRECT3DDEVICE9 dev = renderContext->device;
			//if (blendState != currentBlendState) {
			if (!newState.blendEnable){
				if (current->blendEnable){
					dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				}
			}
			else {
				if (newState.blendEnable){
					if (!current->blendEnable){
						dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					}
					if (newState.srcFactorRGB != current->srcFactorRGB){
						dev->SetRenderState(D3DRS_SRCBLEND, newState.srcFactorRGB);
					}
					if (newState.dstFactorRGB != current->dstFactorRGB){
						dev->SetRenderState(D3DRS_DESTBLEND, newState.dstFactorRGB);
					}
					if (newState.blendModeRGB != current->blendModeRGB){
						dev->SetRenderState(D3DRS_BLENDOP, newState.blendModeRGB);
					}
					if (newState.srcFactorAlpha != current->srcFactorAlpha){
						dev->SetRenderState(D3DRS_SRCBLENDALPHA, newState.srcFactorAlpha);
					}
					if (newState.dstFactorAlpha != current->dstFactorAlpha){
						dev->SetRenderState(D3DRS_DESTBLENDALPHA, newState.dstFactorAlpha);
					}
					if (newState.blendModeAlpha != current->blendModeAlpha){
						dev->SetRenderState(D3DRS_BLENDOPALPHA, newState.blendModeAlpha);
					}
					if (newState.separateAlpha != current->separateAlpha){
						dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, newState.separateAlpha);
					}
					if (newState.alphaRef != current->alphaRef){
						dev->SetRenderState(D3DRS_ALPHAREF, (DWORD)newState.alphaRef);
					}
					if (newState.alphaFunc != current->alphaFunc){
						dev->SetRenderState(D3DRS_ALPHAFUNC, newState.alphaFunc);
					}
				}
			}
			renderContext->currentBlendState = id;
		}

		// ---------------------------------------------------------------------
		// Sets the blend state without checking and update current blend state
		// ---------------------------------------------------------------------
		void setBlendState(int id) {
			if (id != renderContext->currentBlendState) {
				const BlendState& newState = renderContext->blendStates[id];
				LPDIRECT3DDEVICE9 dev = renderContext->device;
				if (!newState.blendEnable){
					dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				}
				else {
					dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
					dev->SetRenderState(D3DRS_SRCBLEND, newState.srcFactorRGB);
					dev->SetRenderState(D3DRS_DESTBLEND, newState.dstFactorRGB);
					dev->SetRenderState(D3DRS_BLENDOP, newState.blendModeRGB);
					dev->SetRenderState(D3DRS_SRCBLENDALPHA, newState.srcFactorAlpha);
					dev->SetRenderState(D3DRS_DESTBLENDALPHA, newState.dstFactorAlpha);
					dev->SetRenderState(D3DRS_BLENDOPALPHA, newState.blendModeAlpha);
					dev->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, newState.separateAlpha);
					dev->SetRenderState(D3DRS_ALPHAREF, (DWORD)newState.alphaRef);
					dev->SetRenderState(D3DRS_ALPHAFUNC, newState.alphaFunc);
				}
				renderContext->currentBlendState = id;
			}
		}

		void restoreDefaultBlendState() {
			setBlendState(renderContext->defaultBlendState);
		}

		int getQuadIndexBufferIndex() {
			return renderContext->quadIndexBufferIndex;
		}

		int createQuadIndexBuffer(int maxQuads) {
			int size = 6 * maxQuads;
			D3DPOOL pool = D3DPOOL_MANAGED;
			DWORD usage = D3DUSAGE_WRITEONLY;
			int id = createIndexBuffer(size);
			IndexBuffer& buffer = getIndexBuffer(id);
			HR(renderContext->device->CreateIndexBuffer(size * sizeof(WORD), usage, D3DFMT_INDEX16, pool, &buffer.buffer, NULL));
			LOG << "new IndexBuffer created size: " << size;
			WORD* indexBuffer;
			HR(buffer.buffer->Lock(0, size * sizeof(WORD), (void**)&indexBuffer, 0));

			for (uint32 i = 0; i < maxQuads; ++i) {
				*indexBuffer = i * 4 + 0;
				++indexBuffer;
				*indexBuffer = i * 4 + 1;
				++indexBuffer;
				*indexBuffer = i * 4 + 2;
				++indexBuffer;
				*indexBuffer = i * 4 + 2;
				++indexBuffer;
				*indexBuffer = i * 4 + 3;
				++indexBuffer;
				*indexBuffer = i * 4 + 0;
				++indexBuffer;
			}
			HR(buffer.buffer->Unlock());
			return id;
		}

		int createVertexBuffer(int vertexDeclaration, int size, bool dynamic) {
			int id = renderContext->vertexBuffers.size();
			VertexBuffer vb;
			buffer::create(vb, vertexDeclaration, size, dynamic);
			renderContext->vertexBuffers.add(vb);
			return id;
		}

		VertexBuffer& getVertexBuffer(int index) {
			return renderContext->vertexBuffers[index];
		}

		void fillBuffer(int bufferIndex, void* data, int num) {
			VertexBuffer& vertexBuffer = getVertexBuffer(bufferIndex);
			char* vb;
			int vc = num;
			DWORD flag = D3DLOCK_DISCARD;
			HR(vertexBuffer.buffer->Lock(0, vc * vertexBuffer.vertexSize, (void**)&vb, flag));
			memcpy(vb, data, num * vertexBuffer.vertexSize);
			HR(vertexBuffer.buffer->Unlock());
		}

		int createIndexBuffer(int size, bool dynamic) {
			int id = renderContext->indexBuffers.size();
			IndexBuffer ib;
			buffer::create(ib, size);
			renderContext->indexBuffers.add(ib);
			return id;
		}

		IndexBuffer& getIndexBuffer(int index) {
			return renderContext->indexBuffers[index];
		}

		void allocateDescriptorData(int size) {
			int sz = size * (sizeof(IdString) + sizeof(int) * 3);
			LOG << "allocating data - size: " << size << " total: " << sz;
			DescriptorData data;
			data.buffer = new char[sz];
			data.hashes = (IdString*)(data.buffer);
			data.textures = (int*)(data.hashes + size);
			data.shaders = (int*)(data.textures + size);
			data.blendStates = (int*)(data.shaders + size);
			data.num = 0;
			data.total = size;
			if (renderContext->descriptorData.buffer != 0) {
				memcpy(data.hashes, renderContext->descriptorData.hashes, sizeof(IdString) * renderContext->descriptorData.num);
				memcpy(data.textures, renderContext->descriptorData.textures, sizeof(int) * renderContext->descriptorData.num);
				memcpy(data.shaders, renderContext->descriptorData.shaders, sizeof(int) * renderContext->descriptorData.num);
				memcpy(data.blendStates, renderContext->descriptorData.blendStates, sizeof(int) * renderContext->descriptorData.num);
				data.num = renderContext->descriptorData.num;
				delete[] renderContext->descriptorData.buffer;
			}
			renderContext->descriptorData = data;
		}

		DescriptorData* getDescriptorData() {
			return &renderContext->descriptorData;
		}
		// -------------------------------------------------------
		// get descriptor ID
		// -------------------------------------------------------
		int getDescriptorID(IdString hash) {
			int id = renderContext->descriptorData.getID(hash);
			XASSERT(id != -1, "No descriptor %d found", hash);
			return id;
		}

		int getDescriptorID(const char* name) {
			int id = renderContext->descriptorData.getID(name);
			XASSERT(id != -1,"No descriptor '%s' found",name);
			return id;
		}

		// -------------------------------------------------------
		// add descriptor
		// -------------------------------------------------------
		int addDescriptor(const Descriptor& desc) {
			if ((renderContext->descriptorData.num + 1) >= renderContext->descriptorData.total) {
				allocateDescriptorData(renderContext->descriptorData.total + 16);
			}
			return renderContext->descriptorData.add(desc);
		}

		// -------------------------------------------------------
		// Set the requested texture
		// -------------------------------------------------------
		void setTexture(int textureID, int index) {
			if (renderContext->currentTextures[index] != textureID) {
				assert(textureID < MAX_TEXTURES);
				TextureAsset* tr = &renderContext->textures[textureID];
				assert(tr->flags != 0);
				renderContext->currentTextures[index] = textureID;
				IDirect3DDevice9 * pDevice = renderContext->device;
				HR(pDevice->SetTexture(index, tr->texture));
				HR(pDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
				HR(pDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
				HR(pDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT));
				++renderContext->drawCounter.textures;				
			}
		}


		void applyDescriptor(int newID) {
			int oldShader = renderContext->descriptorData.shaders[renderContext->currentDescriptor];
			int newShader = renderContext->descriptorData.shaders[newID];
			if (oldShader != newShader) {
				setCurrentShader(newShader);
			}

			if (renderContext->descriptorData.textures[newID] != renderContext->descriptorData.textures[renderContext->currentDescriptor]) {
				if (renderContext->descriptorData.textures[newID] != -1) {
					setTexture(renderContext->descriptorData.textures[newID], 0);
					setTexture(renderContext->currentShaderID, "gTex", renderContext->descriptorData.textures[newID]);
				}
			}
			int newBS = renderContext->descriptorData.blendStates[newID];
			int oldBS = renderContext->descriptorData.blendStates[renderContext->currentDescriptor];
			if (newBS != oldBS) {
				setBlendState(newBS);
			}
		}

		// -------------------------------------------------------
		// Apply shader and set some constants
		// -------------------------------------------------------
		void setShaderParameter(Shader* shader, int textureID) {			
			shader->setMatrix("gWVP", renderContext->matWorldViewProj);
			shader->setMatrix("gWorld", renderContext->matWorld);		
			shader->setVector3f("gCameraPos", renderContext->camera->getPosition());
			shader->setVector3f("gCameraUp", renderContext->camera->getUpVector());
			if (textureID != -1) {
				shader->setTexture("gTex",textureID);
			}
			if (shader->contains("gWorldInverseTranspose")) {
				D3DXMATRIX worldInverseTranspose;
				D3DXMatrixInverse(&worldInverseTranspose, 0, &matrix::convert(renderContext->matWorld));
				D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
				shader->setValue("gWorldInverseTranspose", &worldInverseTranspose, sizeof(D3DXMATRIX));
			}
			if (shader->contains("gWorldInverse")) {
				D3DXMATRIX worldInverse;
				D3DXMatrixInverse(&worldInverse, 0, &matrix::convert(renderContext->matWorld));
				shader->setValue("gWorldInverse", worldInverse,sizeof(D3DXMATRIX));
			}
			shader->setMatrix("gView", renderContext->matView);
			shader->setMatrix("gProjection", renderContext->matProjection);
			shader->commitChanges();
		}


		void draw(const Descriptor& desc,int size,int numPrimitives) {
			Shader* shader = renderer::getShader(desc.shader);
			D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
			shader->setVector2f("viewportPosition", renderer::getSelectedViewport().getPosition());
			uint32 numPasses = shader->start();
			for (UINT p = 0; p < numPasses; ++p) {
				shader->beginPass(p);
				setShaderParameter(shader,desc.texture);
				HR(renderContext->device->DrawIndexedPrimitive(pt, 0, 0, size, 0, numPrimitives));
				shader->endPass();
			}
			shader->end();
		}

		void draw_render_target_common(RTID rtID, int shaderID, int blendState) {
			ZoneTracker z("draw_render_target");
			sprites::flush();
			fillBuffer(renderContext->renderTargetQuad.vertexBufferID, renderContext->renderTargetQuad.vertices, 4);
			const VertexBuffer& vb = renderContext->vertexBuffers[renderContext->renderTargetQuad.vertexBufferID];
			VDStruct& vds = renderContext->vdStructs[vb.vertexDeclaration];
			HR(renderContext->device->SetVertexDeclaration(renderContext->vdStructs[vb.vertexDeclaration].declaration->get()));

			const IndexBuffer& ib = renderContext->indexBuffers[renderContext->renderTargetQuad.indexBufferID];
			HR(renderContext->device->SetIndices(ib.buffer));
			HR(renderContext->device->SetStreamSource(0, vb.buffer, 0, vb.vertexSize));

			D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
			int numPrimitives = 4;
			
			int current = renderer::getCurrentBlendState();
			renderer::setBlendState(blendState);

			RenderTarget& rt = renderContext->renderTargets[rtID.rtID];

			renderContext->drawCounter.numPrim += numPrimitives;
			renderContext->drawCounter.indexCounter += numPrimitives * 3;
			int sid = shaderID;
			if (sid == -1) {
				sid = renderContext->defaultShaderID;
			}
			Shader* shader = renderer::getShader(sid);
			shader->setVector2f("viewportPosition", renderer::getSelectedViewport().getPosition());
			uint32 numPasses = shader->start();
			for (UINT p = 0; p < numPasses; ++p) {
				shader->beginPass(p);
				setShaderParameter(shader, rt.textureID);
				HR(renderContext->device->DrawIndexedPrimitive(pt, 0, 0, 4, 0, numPrimitives));
				shader->endPass();
			}
			shader->end();
			renderer::setBlendState(current);
		}

		void draw_screen_quad(Shader* shader) {
			ZoneTracker z("draw_screen_quad");
			sprites::flush();
			fillBuffer(renderContext->renderTargetQuad.vertexBufferID, renderContext->renderTargetQuad.vertices, 4);
			const VertexBuffer& vb = renderContext->vertexBuffers[renderContext->renderTargetQuad.vertexBufferID];
			VDStruct& vds = renderContext->vdStructs[vb.vertexDeclaration];
			HR(renderContext->device->SetVertexDeclaration(renderContext->vdStructs[vb.vertexDeclaration].declaration->get()));

			const IndexBuffer& ib = renderContext->indexBuffers[renderContext->renderTargetQuad.indexBufferID];
			HR(renderContext->device->SetIndices(ib.buffer));
			HR(renderContext->device->SetStreamSource(0, vb.buffer, 0, vb.vertexSize));

			D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
			int numPrimitives = 4;

			renderContext->drawCounter.numPrim += numPrimitives;
			renderContext->drawCounter.indexCounter += numPrimitives * 3;
			shader->setVector2f("viewportPosition", renderer::getSelectedViewport().getPosition());
			uint32 numPasses = shader->start();
			for (UINT p = 0; p < numPasses; ++p) {
				shader->beginPass(p);
				setShaderParameter(shader, -1);
				HR(renderContext->device->DrawIndexedPrimitive(pt, 0, 0, 4, 0, numPrimitives));
				shader->endPass();
			}
			shader->end();
		}

		void draw_render_target(RTID rtID, int shaderID) {
			const char* name = "rt_blend";
			IdString hash = string::murmur_hash(name);
			int idx = renderer::getBlendStateID(hash);
			draw_render_target_common(rtID, shaderID, idx);			
		}

		void draw_render_target_additive(RTID rtID, int shaderID) {
			const char* name = "alpha_blend";
			IdString hash = string::murmur_hash(name);
			int idx = renderer::getBlendStateID(hash);
			draw_render_target_common(rtID, shaderID, idx);
		}

		void draw(int descriptorID, int vertexBufferID, int numVertices,int indexBufferID) {
			ZoneTracker z("drawBuffer");
			if (descriptorID != renderContext->currentDescriptor) {
				applyDescriptor(descriptorID);
				//m_Renderer->setTexture(m_ShaderID, "gTex", m_TextureID);
				renderContext->currentDescriptor = descriptorID;
			}
			// FIXME: check if we need to switch this			
			const VertexBuffer& vb = renderContext->vertexBuffers[vertexBufferID];
			VDStruct& vds = renderContext->vdStructs[vb.vertexDeclaration];
			HR(renderContext->device->SetVertexDeclaration(renderContext->vdStructs[vb.vertexDeclaration].declaration->get()));

			const IndexBuffer& ib = renderContext->indexBuffers[indexBufferID];
			HR(renderContext->device->SetIndices(ib.buffer));
			HR(renderContext->device->SetStreamSource(0, vb.buffer, 0, vb.vertexSize));

			D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
			int numPrimitives = numVertices / 2;

			renderContext->drawCounter.numPrim += numPrimitives;
			renderContext->drawCounter.indexCounter += numPrimitives * 3;
			Descriptor desc;
			renderContext->descriptorData.get(renderContext->currentDescriptor, &desc);
			draw(desc, numVertices, numPrimitives);			
			++renderContext->drawCounter.shaders;
			++renderContext->drawCounter.drawCalls;
			++renderContext->drawCounter.flushes;			
		}
		
		// -------------------------------------------------------
		// Checks if color is fill color
		// -------------------------------------------------------
		bool isFillColor(const Color& fillColor, const Color& currentColor) {
			if (fillColor.r != currentColor.r) {
				return false;
			}
			if (fillColor.g != currentColor.g) {
				return false;
			}
			if (fillColor.b != currentColor.b) {
				return false;
			}
			if (fillColor.a != currentColor.a) {
				return false;
			}
			return true;
		}

		// -------------------------------------------------------
		// Get color from a pixel of a texture
		// -------------------------------------------------------
		Color getColor(D3DLOCKED_RECT& lockedRect, int x, int y, int height) {
			uchar * pBits = (uchar *)lockedRect.pBits;
			uchar b = (uchar)pBits[(x + y * height) * 4];
			uchar g = (uchar)pBits[(x + y * height) * 4 + 1];
			uchar r = (uchar)pBits[(x + y * height) * 4 + 2];
			uchar a = (uchar)pBits[(x + y * height) * 4 + 3];
			return Color(r, g, b, a);
		}

		// -------------------------------------------------------
		// Sets color of a pixel in a texture
		// -------------------------------------------------------
		void setColor(D3DLOCKED_RECT& lockedRect, int x, int y, uchar r, uchar g, uchar b, uchar a, int height) {
			uchar * pBits = (uchar *)lockedRect.pBits;
			int p = (x + y * height) * 4;
			pBits[p] = b; // blue			
			pBits[p + 1] = g; // green				
			pBits[p + 2] = r; // red
			pBits[p + 3] = a; // alpha
		}

		// -------------------------------------------------------
		// get bitmap font list
		// -------------------------------------------------------
		Array<BitmapFont*>& getBitmapFontList() {
			return renderContext->bitmapFonts;
		}

		// -------------------------------------------------------
		// get bitmap font by name
		// -------------------------------------------------------
		BitmapFont* getBitmapFont(const char* name) {
			IdString hash = string::murmur_hash(name);
			for (int i = 0; i < renderContext->bitmapFonts.size(); ++i) {
				if (renderContext->bitmapFonts[i]->hashName == hash) {
					return renderContext->bitmapFonts[i];
				}
			}
			return 0;
		}
		// -------------------------------------------------------
		// create bitmap font
		// -------------------------------------------------------
		BitmapFont* createBitmapFont(const char* name, int textureID) {
			IdString hash = string::murmur_hash(name);
			for (int i = 0; i < renderContext->bitmapFonts.size(); ++i) {
				if (renderContext->bitmapFonts[i]->hashName == hash) {
					LOG << "Found already loaded font " << name;
					return renderContext->bitmapFonts[i];
				}
			}
			LOG << "Created bitmap font " << name;
			BitmapFont* font = new BitmapFont(name,textureID);
			renderContext->bitmapFonts.push_back(font);
			return font;
		}

		// -------------------------------------------------------
		// initialize bitmap font
		// -------------------------------------------------------
		void initializeBitmapFont(BitmapFont* bitmapFont, int textureID, const Color& fillColor) {
			D3DLOCKED_RECT lockedRect;
			assert(textureID < MAX_TEXTURES);
			TextureAsset* texture = &renderContext->textures[textureID];
			HR(texture->texture->LockRect(0, &lockedRect, NULL, 0));
			uint32 x = bitmapFont->startX + bitmapFont->padding - 1;
			uint32 y = bitmapFont->startY + bitmapFont->padding;
			uint32 ascii = bitmapFont->startChar;
			Color c = getColor(lockedRect, x, y, bitmapFont->textureSize);
			bool running = true;
			bool isChar = false;
			int charStartedX = 0;
			int charStartedY = 0;
			int charCount = 0;
			while (running) {
				++x;
				if (x > (bitmapFont->startX + bitmapFont->width)) {
					x = bitmapFont->startX + bitmapFont->padding - 1;
					y += bitmapFont->padding + bitmapFont->gridHeight;// - 1;
					isChar = false;
					charCount = 0;
				}
				if (y >= (bitmapFont->startY + bitmapFont->height)) {
					running = false;
				}
				if (y >= texture->height) {
					running = false;
				}
				if (running) {
					c = getColor(lockedRect, x, y, bitmapFont->textureSize);
					if (!isFillColor(fillColor, c) && !isChar) {
						isChar = true;
						charStartedX = x;
						charStartedY = y;
					}
					else if (isFillColor(fillColor, c) && isChar) {
						isChar = false;
						int width = x - charStartedX - 1;
						++charCount;
						bitmapFont->addChar(ascii, charStartedX + 1, charStartedY, width);
						++ascii;
					}

				}
			}
			HR(texture->texture->UnlockRect(0));
			//LOGC("Renderer") << "found characters: " << (ascii - bitmapFont->startChar);
			LOG << "found characters: " << bitmapFont->definitions.size();
		}

		// -------------------------------------------------------
		// load bitmap font
		// -------------------------------------------------------
		BitmapFont* loadBitmapFont(const char* name, int textureId, const Color& fillColor) {
			IdString hash = string::murmur_hash(name);
			for (int i = 0; i < renderContext->bitmapFonts.size(); ++i) {
				if (renderContext->bitmapFonts[i]->hashName == hash) {
					LOG << "Found already loaded font " << name;
					return renderContext->bitmapFonts[i];
				}
			}
			LOG << "Loading bitmap font " << name;
			BitmapFont* font = new BitmapFont(name,textureId);
			font->load();
			//assets::load("xscale", font, ds::CVT_FONT);
			renderContext->bitmapFonts.push_back(font);
			initializeBitmapFont(font, textureId, fillColor);
			return font;
		}
		
		void setCurrentShader(int shaderID) {
			renderContext->currentShaderID = shaderID;
			sprites::setShaderID(shaderID);
		}

		int getCurrentShaderID() {
			return renderContext->currentShaderID;
		}

		// -------------------------------------------------------
		// Set texture shader constant by name
		// -------------------------------------------------------
		void setTexture(int shaderID, const char* handleName, int textureID) {
			assert(textureID != -1);
			Shader* shader = renderContext->shaders[shaderID];
			IdString hashName = string::murmur_hash(handleName);
			shader->setTexture(handleName,textureID);
		}

		// ---------------------------------------------------------
		// Creates a render target and returns the attached texture
		// ---------------------------------------------------------
		RTID createRenderTarget(const Color& clearColor) {
			int tid = findFreeTextureSlot();
			if (tid != -1) {
				RenderTarget rt;// = renderContext->renderTargets[id];				
				// make sure it is not used so far
				rt.clearColor = clearColor;
				HR(D3DXCreateRenderToSurface(renderContext->device, renderContext->viewportWidth, renderContext->viewportHeight, D3DFMT_A8R8G8B8, false, D3DFMT_UNKNOWN, &rt.rts));
				HR(renderContext->device->CreateTexture(renderContext->viewportWidth,renderContext->viewportHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&rt.texture,NULL));				
				TextureAsset* t = &renderContext->textures[tid];
				t->height = renderContext->viewportHeight;
				t->width = renderContext->viewportWidth;
				t->flags = 1;
				t->texture = rt.texture;
				rt.texture->GetSurfaceLevel(0, &rt.surface);
				rt.textureID = tid;
				int id = renderContext->renderTargets.size();
				renderContext->renderTargets.add(rt);
				LOG << "Rendertarget created - id: " << id << " texture id: " << tid << " width: " << renderContext->viewportWidth << " height: " << renderContext->viewportHeight;
				return RTID(tid,id);
			}
			else {
				LOGE << "Cannot create rendertarget - No more texture slots available";
				return INVALID_RENDER_TARGET;
			}
		}

		RTID createRenderTarget(float width, float height, const Color& clearColor) {			
			int tid = findFreeTextureSlot();
			if ( tid != -1 ) {
				RenderTarget renderTarget;
				renderTarget.clearColor = clearColor;
				D3DXCreateRenderToSurface( renderContext->device,width,height,D3DFMT_A8R8G8B8,false,D3DFMT_UNKNOWN ,&renderTarget.rts);
				renderContext->device->CreateTexture(width,height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&renderTarget.texture,NULL);				
				TextureAsset* t = &renderContext->textures[tid];
				t->height = height;
				t->width = width;
				t->flags = 1;
				t->texture = renderTarget.texture;
				renderTarget.texture->GetSurfaceLevel(0,&renderTarget.surface);
				int id = renderContext->renderTargets.size();
				renderContext->renderTargets.add(renderTarget);
				LOG << "Rendertarget created - id: " << id << " texture id: " << tid << " width: " << width << " height: " << height;
				return RTID(tid, id);
			}
			else {
				LOGE << "Cannot create rendertarget - No more texture slots available";
				return INVALID_RENDER_TARGET;
			}			
		}

		// -------------------------------------------------------
		// Set render target
		// -------------------------------------------------------
		void setRenderTarget(RTID rtid) {
			RenderTarget* rt = &renderContext->renderTargets[rtid.rtID];
			// check that we have a valid RT
			sprites::flush();
			if (renderContext->currentRenderTarget != -1) {
				RenderTarget* activeRT = &renderContext->renderTargets[renderContext->currentRenderTarget];
				activeRT->rts->EndScene(0);
			}
			else {
				renderContext->device->EndScene();
			}
			rt->rts->BeginScene(rt->surface, NULL);
			renderContext->currentRenderTarget = rtid.rtID;
			renderContext->device->SetRenderTarget(0, rt->surface);
			HR(renderContext->device->Clear(0, NULL, D3DCLEAR_TARGET, rt->clearColor, 1.0f, 0));
			renderContext->usedRenderTargets = 1;
		}
		// -------------------------------------------------------
		// restore backbuffer
		// -------------------------------------------------------
		void restoreBackBuffer() {
			assert(renderContext->currentRenderTarget != -1);
			RenderTarget* rt = &renderContext->renderTargets[renderContext->currentRenderTarget];
			sprites::flush();
			rt->rts->EndScene(0);
			renderContext->currentRenderTarget = -1;
			renderContext->device->BeginScene();
			//FIXME: clear buffer
			renderContext->device->SetRenderTarget(0, renderContext->backBuffer);
			if (renderContext->usedRenderTargets > 0) {
				for (int i = 1; i < renderContext->usedRenderTargets; ++i) {
					renderContext->device->SetRenderTarget(i, NULL);
				}
				renderContext->usedRenderTargets = 0;
			}
		}

		// -------------------------------------------------------
		// Find free texture slot
		// -------------------------------------------------------
		int findFreeTextureSlot() {
			for (int i = 0; i < MAX_TEXTURES; ++i) {
				if (renderContext->textures[i].flags == 0) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// Create empty texture
		// -------------------------------------------------------
		int createTexture(int width, int height) {
			int id = findFreeTextureSlot();
			XASSERT(id != -1, "No more texture slots available");
			TextureAsset* tr = &renderContext->textures[id];
			tr->name = string::murmur_hash("xxxx");
			tr->flags = 1;
			HR(renderContext->device->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tr->texture, NULL));
			tr->width = width;
			tr->height = height;
			// clear the texture with white color		
			D3DLOCKED_RECT sRect;
			tr->texture->LockRect(0, &sRect, NULL, NULL);
			BYTE *bytes = (BYTE*)sRect.pBits;
			memset(bytes, 128, width*sRect.Pitch);
			tr->texture->UnlockRect(0);
			return id;
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		int loadTexture(const char* dirName, const char* name) {
			int id = findFreeTextureSlot();
			XASSERT(id != -1, "No more texture slots available");
			TextureAsset* tr = &renderContext->textures[id];
			tr->name = string::murmur_hash(name);
			tr->flags = 1;
			int lw = D3DX_DEFAULT;
			int lh = D3DX_DEFAULT;
			D3DXIMAGE_INFO imageInfo;
			char fileName[256];
			sprintf(fileName, "%s\\%s.png", dirName, name);
			//sprintf(fileName, "%s\\%s.tga", dirName, name);
			LOG << "Trying to load texture " << fileName;
			int fileSize = -1;			
			char* data = repository::load(fileName,&fileSize, repository::FT_BINARY);
			assert(fileSize != -1);
			HR(D3DXCreateTextureFromFileInMemoryEx(renderContext->device, data, fileSize, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL, &tr->texture));
			delete[] data;
			assert(tr->texture != 0);
			//HR(D3DXCreateTextureFromFileEx(renderContext->device, fileName, 0, 0, 1, 0,
			//	D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL, &tr->texture));
			tr->width = imageInfo.Width;
			tr->height = imageInfo.Height;
			LOG << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
			return id;			
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		int loadTextureBinary(const char* fileName) {
			int id = findFreeTextureSlot();
			if (id != -1) {
				TextureAsset* tr = &renderContext->textures[id];
				tr->name = string::murmur_hash(fileName);
				tr->flags = 1;
				int lw = D3DX_DEFAULT;
				int lh = D3DX_DEFAULT;
				D3DXIMAGE_INFO imageInfo;
				LOG << "Trying to load texture " << fileName;
				HR(D3DXCreateTextureFromFileEx(renderContext->device, fileName, 0, 0, 1, 0,
					D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL, &tr->texture));
				tr->width = imageInfo.Width;
				tr->height = imageInfo.Height;
				LOG << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
				return id;
			}
			else {
				LOGE << "No more texture slots available";
				return -1;
			}
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		void reloadTextureBinary(const char* fileName) {
			IdString hash = string::murmur_hash(fileName);
			// check if we have one
			int id = getTextureId(fileName);
			if (id == -1) {
				loadTextureBinary(fileName);
			}
			TextureAsset* tr = &renderContext->textures[id];
			SAFE_RELEASE(tr->texture);
			tr->flags = 1;
			int lw = D3DX_DEFAULT;
			int lh = D3DX_DEFAULT;
			D3DXIMAGE_INFO imageInfo;
			LOG << "Trying to load texture " << fileName;
			HR(D3DXCreateTextureFromFileEx(renderContext->device, fileName, 0, 0, 1, 0,
				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL, &tr->texture));
			tr->width = imageInfo.Width;
			tr->height = imageInfo.Height;
			LOG << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;

		}

		// -------------------------------------------------------
		// Creates and loads a texture
		// -------------------------------------------------------
		int loadTexture(const char* name) {
			return loadTexture("content\\textures", name);
		}

		int getTextureId(IdString hash) {
			for (int i = 0; i < MAX_TEXTURES; ++i) {
				TextureAsset* tr = &renderContext->textures[i];
				if (tr->flags == 1 && tr->name == hash) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// Get texture id
		// -------------------------------------------------------
		int getTextureId(const char* name) {
			IdString hash = string::murmur_hash(name);
			return getTextureId(hash);
		}
		// -------------------------------------------------------
		// Get texture size
		// -------------------------------------------------------
		Vector2f getTextureSize(int idx) {
			// FIXME: check if valid id
			assert(idx >= 0 && idx < MAX_TEXTURES);
			TextureAsset* tr = &renderContext->textures[idx];
			assert(tr->flags != 0);
			return Vector2f(static_cast<int>(tr->width), static_cast<int>(tr->height));
		}

		// -------------------------------------------------------
		// get direct texture pointer
		// -------------------------------------------------------
		LPDIRECT3DTEXTURE9 getDirectTexture(int textureID) {
			assert(textureID < MAX_TEXTURES);
			return renderContext->textures[textureID].texture;
		}

		// -------------------------------------------------------
		// lock texture
		// -------------------------------------------------------
		D3DLOCKED_RECT lockTexture(int id) {
			D3DLOCKED_RECT lockedRect;
			ZeroMemory(&lockedRect, sizeof(lockedRect));
			TextureAsset* tr = &renderContext->textures[id];
			HR(tr->texture->LockRect(0, &lockedRect, NULL, 0));
			return lockedRect;
		}

		// -------------------------------------------------------
		// unlock texture
		// -------------------------------------------------------
		void unlockTexture(int id) {
			TextureAsset* tr = &renderContext->textures[id];
			HR(tr->texture->UnlockRect(0));
		}
		
		void setTransformations() {
			renderContext->matView = renderContext->camera->getViewMatrix();
			renderContext->matProjection = renderContext->camera->getProjectionMatrix();
			HR(renderContext->device->SetTransform(D3DTS_WORLD, &matrix::convert(renderContext->matWorld)));
			HR(renderContext->device->SetTransform(D3DTS_VIEW, &matrix::convert(renderContext->camera->getViewMatrix())));
			HR(renderContext->device->SetTransform(D3DTS_PROJECTION, &matrix::convert(renderContext->camera->getProjectionMatrix())));
			renderContext->matWorldViewProj = renderContext->matWorld * renderContext->camera->getViewMatrix() *renderContext->camera->getProjectionMatrix();
			renderContext->camera->buildView();
		}

		//-----------------------------------------------------------------------------
		// Set2DCameraOn: go into 2D mode
		//-----------------------------------------------------------------------------
		void set2DCameraOn() {
			// check if we are already in 2D mode
			renderContext->camera->setOrthogonal();
			renderContext->matWorld = matrix::m4identity();
			setTransformations();
			setRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			setRenderState(D3DRS_ZENABLE, FALSE);
		}

		// -------------------------------------------------------
		// Begin rendering
		// -------------------------------------------------------
		bool beginRendering(const Color& clearColor) {
			setRenderState(D3DRS_LIGHTING, false);
			setRenderState(D3DRS_ZENABLE, TRUE);
			setRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			renderContext->matWorld = matrix::m4identity();
			renderer::selectViewport(0);
			renderContext->usedRenderTargets = 0;
			renderContext->currentShaderID = -1;
			renderContext->currentBlendState = -1;
			renderContext->currentDescriptor = -1;
			renderContext->currentRenderTarget = -1;
			for (int i = 0; i < 5; ++i) {
				renderContext->currentTextures[i] = -1;
			}
			// Clear the back buffer to a blue color
			HR(renderContext->device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0));
			// Begin the scene
			if (SUCCEEDED(renderContext->device->BeginScene())) {
				renderer::restoreDefaultBlendState();
				set2DCameraOn();
				//sprites::setShaderID(renderContext->defaultShaderID);
				//sprites::setDescriptorID(renderContext->defaultDescriptor);
				return true;
			}
			else {
				LOGE << "cannot begin scene";
			}
			return false;
		}

		// -------------------------------------------------------
		// Clear
		// -------------------------------------------------------
		void clear() {
			HR(renderContext->device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, renderContext->clearColor, 1.0f, 0));
		}

		void flush() {
			//m_SpriteBatch->flush();
			//m_SpriteBatch->end();
			sprites::flush();
			//resetDynamicBuffers();
		}
		// -------------------------------------------------------
		// End rendering
		// -------------------------------------------------------
		void endRendering() {
			if (renderContext->currentRenderTarget != -1) {
				RenderTarget* activeRT = &renderContext->renderTargets[renderContext->currentRenderTarget];
				activeRT->rts->EndScene(0);
			}
			else {
				renderContext->device->EndScene();
			}
			renderContext->device->Present(NULL, NULL, NULL, NULL);
		}

		//void setShaderParameter(Shader* shader,int textureID = -1);

		//-----------------------------------------------------------------------------
		// Name: SetupMatrices()
		// Desc: Sets up the world, view, and projection transform Matrices.
		//-----------------------------------------------------------------------------
		VOID setupMatrices() {
			//D3DXMatrixIdentity(&m_World);    
			renderContext->matWorld = matrix::m4identity();
			setTransformations();
			setRenderState(D3DRS_ZENABLE, TRUE);
		}

		

		DrawCounter& drawCounter() {
			return renderContext->drawCounter;
		}

		//-----------------------------------------------------------------------------
		// Set2DCameraOff: exit 2D mode
		//-----------------------------------------------------------------------------
		void set2DCameraOff() {
			renderContext->camera->restore();
			setTransformations();
			setRenderState(D3DRS_ZENABLE, TRUE);
		}

		void setWorldMatrix(const mat4& world) {
			/*
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					renderContext->matWorld.m[x][y] = world.m[x][y];
				}
			}
			*/
			renderContext->matWorld = world;
			setTransformations();
			//device->get()->SetTransform(D3DTS_WORLD,&m_World);
			//renderContext->matWorldViewProj = renderContext->matWorld * renderContext->camera->getViewMatrix() * renderContext->camera->getProjectionMatrix();
		}

		// -------------------------------------------------------
		// Create rasterizer state
		// -------------------------------------------------------
		RasterizerState* createRasterizerState(const char* name, int cullMode, int fillMode, bool multiSample, bool scissor) {
			RasterizerState* rs = new RasterizerState;
			rs->cullMode = cullMode;
			rs->fillMode = fillMode;
			rs->multiSample = multiSample;
			rs->scissor = scissor;
			renderContext->rasterizerStates.append(rs);
			return rs;
		}

		// -------------------------------------------------------
		// Sets the current rasterizer state
		// -------------------------------------------------------
		void setRasterizerState(RasterizerState* rasterizerState) {
			LPDIRECT3DDEVICE9 dev = renderContext->device;
			dev->SetRenderState(D3DRS_CULLMODE, renderContext->rsState.cullMode = rasterizerState->cullMode);
			dev->SetRenderState(D3DRS_FILLMODE, renderContext->rsState.fillMode = rasterizerState->fillMode);
			dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, renderContext->rsState.multiSample = rasterizerState->multiSample);
			dev->SetRenderState(D3DRS_SCISSORTESTENABLE, renderContext->rsState.scissor = rasterizerState->scissor);
		}

		// -------------------------------------------------------
		// Change the current rasterizer state
		// -------------------------------------------------------
		void changeRasterizerState(RasterizerState* rasterizerState) {
			LPDIRECT3DDEVICE9 dev = renderContext->device;
			if (rasterizerState->cullMode != renderContext->rsState.cullMode) {
				dev->SetRenderState(D3DRS_CULLMODE, renderContext->rsState.cullMode = rasterizerState->cullMode);
			}
			if (rasterizerState->fillMode != renderContext->rsState.fillMode) {
				dev->SetRenderState(D3DRS_FILLMODE, renderContext->rsState.fillMode = rasterizerState->fillMode);
			}
			if (rasterizerState->multiSample != renderContext->rsState.multiSample) {
				dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, renderContext->rsState.multiSample = rasterizerState->multiSample);
			}
			if (rasterizerState->scissor != renderContext->rsState.scissor) {
				dev->SetRenderState(D3DRS_SCISSORTESTENABLE, renderContext->rsState.scissor = rasterizerState->scissor);
			}
		}

		void loadSpriteTemplates() {
			renderContext->spriteTemplates.load();
		}

		bool getSpriteTemplate(const char* name,Sprite* sprite) {
			return renderContext->spriteTemplates.get(name,sprite);
		}

		SpriteTemplates* getSpriteTemplates() {
			return &renderContext->spriteTemplates;
		}

		bool hasSpriteTemplate(int id) {
			return renderContext->spriteTemplates.contains(id);
		}

		ID getMeshDataID(IdString hash) {
			for (size_t i = 0; i < renderContext->meshData.size(); ++i) {
				if (renderContext->meshData[i].hash == hash) {
					return renderContext->meshData[i].id;
				}
			}
			return INVALID_ID;
		}

		ID loadMeshData(const char* name, const char* file) {
			ID id = createMeshData(name);
			MeshData& md = getMeshData(id);
			LOG << "load meshdata - name: " << name << " id: " << md.id << " hash: " << md.hash;
			char fileName[256];
			char srcFileName[256];
			int dataID = -1;
			sprintf(fileName, "content\\data\\%s.bin", file);
			sprintf(srcFileName, "content\\data\\%s.obj", file);
			ObjLoader ml;
			if (!file::fileExists(fileName) || file::isOlder(fileName, srcFileName)) {
				LOG << "obj fileName: " << srcFileName;
				ml.parse(srcFileName, md);
				ml.save(fileName, md);
			}
			if (file::fileExists(fileName)) {
				LOG << "reading binary file";
				ml.read(fileName, md);
			}			
			return id;
		}

		ID createMeshData(const char* name) {
			size_t s = renderContext->meshData.size();
			MeshData data;
			data.hash = string::murmur_hash(name);
			data.id = static_cast<ID>(s);
			LOG << "create meshdata - name: " << name << " id: " << data.id << " hash: " << data.hash;
			data.size = 0;
			renderContext->meshData.add(data);
			return data.id;
		}

		MeshData& getMeshData(ID id) {
			return renderContext->meshData[id];
		}
		
		void allocateMeshArray(int size) {
			int sz = size * (sizeof(IdString) + sizeof(int) + sizeof(ID) + sizeof(int));
			LOG << "allocating data - size: " << size << " total: " << sz;
			MeshArray data;
			data.buffer = new char[sz];
			data.hashes = (IdString*)(data.buffer);
			data.descriptors = (int*)(data.hashes + size);
			data.dataIDs = (ID*)(data.descriptors + size);
			data.vertexTypes = (int*)(data.dataIDs + size);
			data.num = 0;
			data.total = size;
			if (renderContext->descriptorData.buffer != 0) {
				memcpy(data.hashes, renderContext->meshes.hashes, sizeof(IdString) * renderContext->meshes.num);
				memcpy(data.descriptors, renderContext->meshes.descriptors, sizeof(int) * renderContext->meshes.num);
				memcpy(data.dataIDs, renderContext->meshes.dataIDs, sizeof(ID) * renderContext->meshes.num);
				memcpy(data.vertexTypes, renderContext->meshes.vertexTypes, sizeof(int) * renderContext->meshes.num);
				data.num = renderContext->meshes.num;
				delete[] renderContext->meshes.buffer;
			}
			renderContext->meshes = data;
		}

		void getMesh(int id,Mesh* m) {
			renderContext->meshes.get(id,m);

		}

		MeshArray* getMeshArray() {
			return &renderContext->meshes;
		}

		int createMesh(const char* name,int descriptorID,ID dataID,int vertexType) {
			if ((renderContext->meshes.num + 1) >= renderContext->meshes.total) {
				allocateMeshArray(renderContext->meshes.total + 16);
			}
			Mesh m;
			m.hashName = string::murmur_hash(name);
			m.descriptorID = descriptorID;
			m.meshDataID = dataID;
			m.vertexType = vertexType;
			int idx = renderContext->meshes.add(m);
			LOG << "create mesh - name: " << name << " id: " << idx << " hash: " << m.hashName;
			return idx;
		}

		int getMeshID(const char* name) {
			IdString hash = string::murmur_hash(name);
			return getMeshID(hash);
		}

		int getMeshID(IdString hash) {
			return renderContext->meshes.getID(hash);
		}

		int getVDType(const char* name) {
			if (strcmp(name, "TTC") == 0) {
				return VD_TTC;
			}
			if (strcmp(name, "PTNBT") == 0) {
				return VD_PTNBT;
			}
			if (strcmp(name, "PTC") == 0) {
				return VD_PTC;
			}
			if (strcmp(name, "PNC") == 0) {
				return VD_PNC;
			}
			if (strcmp(name, "PARTICLE") == 0) {
				return VD_PARTICLE;
			}
			if (strcmp(name, "PNTC") == 0) {
				return VD_PNTC;
			}
			return VD_UNKNOWN;
		}

		Vector2f& getMousePosition() {
			return renderContext->mousePosition;
		}

		v2 screen_to_world(const v2& screenPos,const v2& worldPos) {
			v2 wp;
			const Viewport vp = getSelectedViewport();
			float dx = worldPos.x - renderContext->screenWidth / 2.0f;
			const v2& worldSize = vp.getWorldSize();
			float ddx = worldSize.x - renderContext->screenWidth;
			if (dx < 0.0f) {
				dx = 0.0f;
			}
			if (dx > ddx) {
				dx = ddx;
			}
			wp.x = screenPos.x + dx;

			float dy = worldPos.y - renderContext->screenHeight / 2.0f;
			float ddy = worldSize.y - renderContext->screenHeight;
			if (dy < 0.0f) {
				dy = 0.0f;
			}
			if (dy > ddy) {
				dy = ddy;
			}
			wp.y = screenPos.y + dy;
			return wp;
		}

		v2 world_to_screen(const v2& worldPos,int viewportID) {
			const Viewport vp = renderContext->viewPorts[viewportID];
			//float dx = worldPos.x - vp.getWorldSize().x / 2.0f + vp.getPosition().x / 2.0f;
			//float dy = worldPos.y - vp.getWorldSize().y / 2.0f + vp.getPosition().y / 2.0f;
			float dx = worldPos.x - vp.getPosition().x + renderContext->screenWidth / 2.0f;
			float dy = worldPos.y - vp.getPosition().y + renderContext->screenHeight / 2.0f;
			return v2(dx,dy);
		}

		void setMousePosition(int x, int y) {
			renderContext->mousePosition = Vector2f(x, y);
		}

		SpriteGroupContainer* getSpriteGroupContainer() {
			return &renderContext->spriteGroudContainer;
		}

		SpriteGroup* getSpriteGroup(const char* name) {
			if (renderContext->spriteGroudContainer.contains(name)) {
				LOG << "found existing sprite group";
				return renderContext->spriteGroudContainer.get(name);
			}
			SpriteGroup* group = renderContext->spriteGroudContainer.create(name);
			group->load();
			return group;
		}

		void showDrawCounter(v2* position) {
			gui::start(EDITOR_ID, position);
			int state = 1;
			if (gui::begin("Draw Calls", &state)) {
				char buffer[256];
				sprintf(buffer, "Indices: %d", renderContext->drawCounter.indexCounter);
				gui::Label(buffer);
				sprintf(buffer, "Vertices: %d", renderContext->drawCounter.numPrim);
				gui::Label(buffer);
				sprintf(buffer, "Sprites: %d", renderContext->drawCounter.sprites);
				gui::Label(buffer);
				sprintf(buffer, "Flushes: %d", renderContext->drawCounter.flushes);
				gui::Label(buffer);
				sprintf(buffer, "DrawCalls: %d", renderContext->drawCounter.drawCalls);
				gui::Label(buffer);
				sprintf(buffer, "Textures: %d", renderContext->drawCounter.textures);
				gui::Label(buffer);
				sprintf(buffer, "Shaders: %d", renderContext->drawCounter.shaders);
				gui::Label(buffer);
				sprintf(buffer, "Particles: %d", renderContext->drawCounter.particles);
				gui::Label(buffer);
			}
			gui::end();
		}

		void printDrawCounter() {
			LOG << "DrawCounter";
			LOG << "------------------------------------------------------------";
			LOG << "Indices  : " << renderContext->drawCounter.indexCounter;
			LOG << "Vertices : " << renderContext->drawCounter.numPrim;
			LOG << "Sprites  : " << renderContext->drawCounter.sprites;
			LOG << "Flushes  : " << renderContext->drawCounter.flushes;
			LOG << "DrawCalls: " << renderContext->drawCounter.drawCalls;
			LOG << "Textures : " << renderContext->drawCounter.textures;
			LOG << "Shaders  : " << renderContext->drawCounter.shaders;
			LOG << "Particles: " << renderContext->drawCounter.particles;
		}

		void saveDrawCounter(FILE* f) {
			fprintf(f, "DrawCounter\n");
			fprintf(f, "------------------------------------------------------------\n");
			fprintf(f, "Indices  : %d\n", renderContext->drawCounter.indexCounter);
			fprintf(f, "Vertices : %d\n", renderContext->drawCounter.numPrim);
			fprintf(f, "Sprites  : %d\n", renderContext->drawCounter.sprites);
			fprintf(f, "Flushes  : %d\n", renderContext->drawCounter.flushes);
			fprintf(f, "DrawCalls: %d\n", renderContext->drawCounter.drawCalls);
			fprintf(f, "Textures : %d\n", renderContext->drawCounter.textures);
			fprintf(f, "Shaders  : %d\n", renderContext->drawCounter.shaders);
			fprintf(f, "Particles: %d\n", renderContext->drawCounter.particles);
		}

		void saveDrawCounter(const ReportWriter& writer) {
			writer.startBox("DrawCounter");
			const char* HEADERS[] = { "Type", "Count" };
			writer.startTable(HEADERS, 2);
			writer.startRow();
			writer.addCell("Indices");
			writer.addCell(renderContext->drawCounter.indexCounter);
			writer.endRow();
			writer.startRow();
			writer.addCell("Vertices"); 
			writer.addCell(renderContext->drawCounter.numPrim);
			writer.endRow();
			writer.startRow();
			writer.addCell("Sprites"); 
			writer.addCell(renderContext->drawCounter.sprites);
			writer.endRow();
			writer.startRow();
			writer.addCell("Flushes"); 
			writer.addCell(renderContext->drawCounter.flushes);
			writer.endRow();
			writer.startRow();
			writer.addCell("DrawCalls"); 
			writer.addCell(renderContext->drawCounter.drawCalls);
			writer.endRow();
			writer.startRow();
			writer.addCell("Textures"); 
			writer.addCell(renderContext->drawCounter.textures);
			writer.endRow();
			writer.startRow();
			writer.addCell("Shaders"); 
			writer.addCell(renderContext->drawCounter.shaders);
			writer.endRow();
			writer.startRow();
			writer.addCell("Particles"); 
			writer.addCell(renderContext->drawCounter.particles);
			writer.endRow();
			writer.endTable();
			writer.endBox();
		}

	}

	
}