#include "Renderer.h"
#include "GraphicsDevice.h"
#include "..\utils\Log.h"
#include "Camera.h"
#include "..\nodes\ScreenOverlayNode.h"
#include "..\utils\Profiler.h"
#include "..\lib\container\List.h"
#include "..\nodes\SkyNode.h"
#include "DebugRenderer.h"
#include "VertexDeclaration.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

#ifdef DEBUG
	DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
#else
	DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE;
#endif

//-----------------------------------------------
// The scene manager implementation
//-----------------------------------------------
Renderer::Renderer(HWND hWnd,const Settings& settings) : m_Hwnd(hWnd) {
	LOG(logINFO) << "RenderDevice::RenderDevice";
	LOG(logINFO) << "Preparing internal structs";
	// vertex declarations
	for ( int i = 0; i < MAX_VERDECLS; ++i) {
		m_VDStructs[i].vertexSize = 0;
		m_VDStructs[i].declaration = 0;
	}
	for ( int i = 0; i < MAX_TEXTURES; ++i ) {
		m_Textures[i].texture = 0;
		m_Textures[i].flags = 0;
		m_Textures[i].name = 0;
	}
	for ( int i = 0; i < MAX_MATERIALS; ++i ) {
		m_Materials[i].flag = -1;
	}
	for ( int i = 0; i < MAX_SHADERS; ++i ) {
		m_Shaders[i].flag = 0;
		m_Shaders[i].m_FX = 0;
	}
	m_BMCounter = 0;
	m_AtlasCounter = 0;
	for ( int i = 0; i < MAX_SYSTEM_FONTS; ++i ) {
		m_Fonts[i].flag = 0;
	}
	for ( int i = 0; i < MAX_BLENDSTATES; ++i ) {
		m_BlendStates[i].flag = 0;
	}
	for ( int i = 0; i < MAX_BUFFERS; ++i ) {
		m_Buffers[i].initialize();
	}
	for ( int i = 0; i < MAX_BUFFER_HANDLES; ++i ) {
		m_BufferHandles[i].used = 0;
	}
	// create the internal device
	device = new GraphicsDevice(hWnd,settings);	
	m_Width = settings.width;
	m_Height = settings.height;
	// the camera
    float aspect = (float)settings.width / (float)settings.height;
	m_Camera = new Camera(settings.width,settings.height);
	m_Camera->setLens(D3DX_PI * 0.25f, aspect, 0.1f, 1000.0f);
	device->get()->SetTransform( D3DTS_PROJECTION, &matrix::convert(m_Camera->getProjectionMatrix()));	
	m_ClearColor = Color(0.0f,0.0f,0.0f,1.0f);
	m_World = matrix::m4identity();
	setRenderState(D3DRS_LIGHTING,FALSE);
	setRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	setRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	setRenderState(D3DRS_ZENABLE,true);
	setRenderState(D3DRS_ZWRITEENABLE, TRUE);
	setRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	setRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD );  
    setRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB( 255, 255, 255 ) );
	mode = settings.mode;
	m_ClearColor = settings.clearColor;
	getRenderStates();
	//m_RenderTarget = 0;
	//m_OverlayNode = 0;
	m_SkyNode = 0;
	//m_PostProcessing = settings.postProcessing;	
	rendering = false;
	m_DefaultBS = createBlendState(BL_SRC_ALPHA,BL_ONE_MINUS_SRC_ALPHA,true);
	LOG(logINFO) << "default blendstate " << m_DefaultBS;
	m_CurrentBS = -1;
	createBasicVertexDeclarations();
	m_DrawCounter = new DrawCounter;
	m_DebugRenderer = new DebugRenderer(this);
	getDevice()->GetRenderTarget(0,&m_BackBuffer);
	// create default buffers
	createVertexBuffer(PT_TRI,VD_TTC,4096,true);
	createIndexBuffer(6144,true);
	createVertexBuffer(PT_TRI,VD_TTC,4096);
	createIndexBuffer(6144);
}


Renderer::~Renderer(void) {	
	LOG(logINFO) << "destructing Renderer";		
	m_RenderTargets.deleteContents();
	m_RasterizerStates.deleteContents();
	LOG(logINFO) << "Releasing textures";
	for ( int i = 0; i < MAX_TEXTURES; ++i ) {
		if ( m_Textures[i].flags != 0 ) {
			SAFE_RELEASE(m_Textures[i].texture);
		}
	}
	for ( int i = 0; i < MAX_VERDECLS; ++i) {
		if ( m_VDStructs[i].vertexSize != 0 ) {
			delete m_VDStructs[i].declaration;
		}
	}
	for ( int i = 0; i < MAX_SHADERS; ++i ) {
		if ( m_Shaders[i].flag != 0 ) {
			SAFE_RELEASE(m_Shaders[i].m_FX);
			// FIXME: delete constants
		}
	}
	for ( int i = 0; i < MAX_SYSTEM_FONTS; ++i ) {
		if ( m_Fonts[i].flag != 0 ) {
			SAFE_RELEASE(m_Fonts[i].font);
		}
	}
	for ( int i = 0; i < MAX_BUFFERS; ++i ) {
		SAFE_RELEASE(m_Buffers[i].vertexBuffer);
	}
	delete device;	
	delete m_Camera;
}

// -------------------------------------------------------
// Begin rendering
// -------------------------------------------------------
bool Renderer::beginRendering(const Color& clearColor) {		
	setRenderState( D3DRS_LIGHTING,false);	
	setRenderState(D3DRS_ZENABLE, TRUE);
	setRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//D3DXMatrixIdentity(&m_World);
	m_World = matrix::m4identity();
	//device->get()->SetTransform(D3DTS_WORLD,&m_World);		
	
	m_UsedRTs = 0;
	m_CurrentIB = -1;
	m_CurrentVB = -1;
	m_CurrentVD = -1;
	m_CurrentShader = -1;
	m_CurrentBS = -1;
	for ( int i = 0; i < 5; ++i ) {
		m_CurrentTextures[i] = -1;
	}
	// Clear the back buffer to a blue color
    HR(device->get()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0 ));		
    // Begin the scene
	if ( !rendering ) {
		if( SUCCEEDED( device->get()->BeginScene() ) ) {		
			rendering = true;		
			setBlendState(m_DefaultBS);
			return true;
		}
		else {
			LOG(logERROR) << "cannot begin scene";
		}
	}
	return false;
}

// -------------------------------------------------------
// Clear
// -------------------------------------------------------
void Renderer::clear() {
	HR(device->get()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearColor, 1.0f, 0 ));
}

// -------------------------------------------------------
// End rendering
// -------------------------------------------------------
void Renderer::endRendering() {
	rendering = false;
	resetDynamicBuffers();
	device->get()->EndScene();
    device->get()->Present( NULL, NULL, NULL, NULL );
}

void Renderer::setTransformations() {
	HR(device->get()->SetTransform( D3DTS_WORLD, &matrix::convert(m_World )));  
	HR(device->get()->SetTransform( D3DTS_VIEW, &matrix::convert(m_Camera->getViewMatrix())));
	HR(device->get()->SetTransform( D3DTS_PROJECTION, &matrix::convert(m_Camera->getProjectionMatrix() )));
	matWorldViewProj = m_World * m_Camera->getViewMatrix() * m_Camera->getProjectionMatrix();
	m_Camera->buildView();
}
//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform Matrices.
//-----------------------------------------------------------------------------
VOID Renderer::setupMatrices() {  
	m_RenderMode = RM_3D;
	//D3DXMatrixIdentity(&m_World);    
	m_World = matrix::m4identity();
	setTransformations();
	setRenderState(D3DRS_ZENABLE, TRUE);
}

//-----------------------------------------------------------------------------
// Set2DCameraOn: go into 2D mode
//-----------------------------------------------------------------------------
void Renderer::set2DCameraOn() {
	// check if we are already in 2D mode
	if ( m_RenderMode != RM_2D ) {
		m_RenderMode = RM_2D;
		m_Camera->setOrthogonal();
		setTransformations();
		setRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		setRenderState(D3DRS_ZENABLE, FALSE);
	}
}


//-----------------------------------------------------------------------------
// Set2DCameraOff: exit 2D mode
//-----------------------------------------------------------------------------
void Renderer::set2DCameraOff() {
	if ( m_RenderMode == RM_2D ) {
		m_RenderMode = RM_3D;
		m_Camera->restore();		
		setTransformations();		
		setRenderState(D3DRS_ZENABLE, TRUE);		
	}
}

// -------------------------------------------------------
// Set render target
// -------------------------------------------------------
void Renderer::setRenderTarget(const char* name) {
	bool found = false;
	IdString hash = string::murmur_hash(name);
	for ( uint32 i = 0; i < m_RenderTargets.num(); ++i ) {
		RenderTarget* rt = m_RenderTargets[i];
		if ( rt->name == hash ) {
			getDevice()->SetRenderTarget(0,rt->surface);
			HR(device->get()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearColor, 1.0f, 0 ));
			m_UsedRTs = 1;
			found = true;
		}
	}
	assert(found);
}
// -------------------------------------------------------
// restore backbuffer
// -------------------------------------------------------
void Renderer::restoreBackBuffer() {
	getDevice()->SetRenderTarget(0,m_BackBuffer);
	if ( m_UsedRTs > 0 ) {
		for ( int i = 1; i < m_UsedRTs; ++i ) {
			getDevice()->SetRenderTarget(i,NULL);
		}
		m_UsedRTs = 0;
	}
}

void Renderer::setWorldMatrix(const mat4& world) {
	for ( int x = 0; x < 4; ++x ) {
		for ( int y = 0; y < 4; ++y ) {
			m_World.m[x][y] = world.m[x][y];
		}
	}	
	//device->get()->SetTransform(D3DTS_WORLD,&m_World);
	matWorldViewProj = m_World * m_Camera->getViewMatrix() * m_Camera->getProjectionMatrix();
}
//-----------------------------------------------------------------------------
// Set the world matrix based on the node transformation
// and updates the WorldViewProjection matrix ready to use
//
// [in] Node* the actual node that is currently rendered
//-----------------------------------------------------------------------------
void Renderer::applyTransformation(Node* node) {
	//node->getTransformation(&m_World);	
	//device->get()->SetTransform(D3DTS_WORLD,&m_World);		
	matWorldViewProj = m_World * m_Camera->getViewMatrix() * m_Camera->getProjectionMatrix();
}

Camera* Renderer::getCamera() {
	return m_Camera;
}

void Renderer::getRenderStates() {
	//DWORD value;
	for ( DWORD rs = 7 ; rs < 209; rs++) {
		D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)(rs);
		/*		
		device->get()->GetRenderState(type,&value);
		m_RenderStates[type] = value;
		*/
		m_RenderStates[type] = -1;
	}
}

void Renderer::setRenderState(D3DRENDERSTATETYPE rs,DWORD value) {
	DWORD cachedValue = m_RenderStates[rs];
	if ( cachedValue == -1 || cachedValue != value ) {
		m_RenderStates[rs] = value;
		device->get()->SetRenderState(rs,value);
	}
}

DWORD Renderer::getRenderState(D3DRENDERSTATETYPE rs) {
	DWORD cachedValue = m_RenderStates[rs];
	if ( cachedValue == -1 ) {
		device->get()->GetRenderState(rs,&cachedValue);
	}
	return cachedValue;
}

// -------------------------------------------------------
// Set shader to material
// -------------------------------------------------------
void Renderer::setShader(int materialID,int shaderID) {
	assert(materialID < MAX_MATERIALS);
	Material* mtrl = &m_Materials[materialID];
	if ( mtrl->flag != 0 ) {
		mtrl->shader = shaderID;
	}
}
// -------------------------------------------------------
// Check if material has a shader applied
// -------------------------------------------------------
bool Renderer::hasShader(int mtrlID) {
	assert(mtrlID < MAX_MATERIALS);
	Material* mtrl = &m_Materials[mtrlID];
	if ( mtrl->flag != 0 && mtrl->shader != -1 ) {
		return true;
	}
	return false;
}

// -------------------------------------------------------
// Applies material
// -------------------------------------------------------
void Renderer::applyMaterial(int mtrlID) {
	assert(mtrlID < MAX_MATERIALS);
	Material* mtrl = &m_Materials[mtrlID];
	if ( mtrl->flag != 0 ) {
		IDirect3DDevice9 * pDevice = device->get();
		for ( size_t i = 0; i < 5; ++i ) {
			// check if we have a texture at this slot
			if ( mtrl->textures[i] != -1 ) {
				if ( m_CurrentTextures[i] != mtrl->textures[i] ) {
					m_CurrentTextures[i] = mtrl->textures[i];
					setTexture(mtrl->textures[i],i);
				}
			}		
			else {
				// check if we had a texture set before
				if ( m_CurrentTextures[i] != -1 ) {
					HR(getDevice()->SetTexture( i, 0));	
					m_CurrentTextures[i] = -1;
				}
			}
		}
	}
}
// -------------------------------------------------------------
//
// -------------------------------------------------------------
void Renderer::setSkyNode(SkyNode* skyNode) {
	m_SkyNode = skyNode;
}

void Renderer::debug() {	
}

// -------------------------------------------------------
// Adds vertex declaration to internal list
// -------------------------------------------------------
void Renderer::addVertexDeclaration(const char* name,VertexDeclaration* declaration) {
	/*
	VDMapping mapping;
	declaration->create(device);
	mapping.hashName = string::murmur_hash(name);
	mapping.declaration = declaration;
	m_VDMappings.append(mapping);
	*/
}

// -------------------------------------------------------
// Get vertex declaration by name
// -------------------------------------------------------
VertexDeclaration* Renderer::getVertexDeclaration(const char* name) {
	/*
	IdString hash = string::murmur_hash(name);
	for ( uint32 i = 0; i < m_VDMappings.num();++i ) {
		const VDMapping& vm = m_VDMappings[i];
		if ( vm.hashName == hash ) {
			return vm.declaration;
		}
	}
	*/
	return 0;
}

int Renderer::createBlendState(int srcAlpha,int dstAlpha,bool alphaEnabled) {
	return createBlendState(srcAlpha,srcAlpha,dstAlpha,dstAlpha,alphaEnabled);
}

int Renderer::createBlendState(int srcRGB,int srcAlpha,int dstRGB,int dstAlpha,bool alphaEnabled,bool separateAlpha) {
	int id = -1;
	for ( int i = 0; i < MAX_BLENDSTATES; ++i ) {
		if ( m_BlendStates[i].flag == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		BlendState* bs = &m_BlendStates[id];
		bs->blendEnable = alphaEnabled;
		bs->srcFactorRGB = srcRGB;
		bs->srcFactorAlpha = srcAlpha;	
		bs->blendModeRGB = BM_ADD;
		bs->dstFactorRGB = dstRGB;
		bs->dstFactorAlpha = dstAlpha;
		bs->blendModeAlpha = BM_ADD;	
		bs->alphaFunc = ALPHA_GREATER;
		bs->alphaRef = 0;
		bs->separateAlpha = separateAlpha;
		LOG(logINFO) << "created new blendstate - id: " << id;
		return id;
	}
	else {
		LOGC(logERROR,"Renderer") << "No more free slots for BlendStates available";
		return -1;
	}
}
// -----------------------------------------------------------------
// Change current blend state if necessary
// -----------------------------------------------------------------
void Renderer::changeBlendState(int id) {
	const BlendState& newState = m_BlendStates[id];
	BlendState* current = &m_BlendStates[m_CurrentBS];
	LPDIRECT3DDEVICE9 dev = device->get();
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
	m_CurrentBS = id;
		/*
		int mask = ALL;
		if (blendState != BS_NONE){
			mask = blendStates[blendState].mask;
		}

		if (mask != currentMask){
			dev->SetRenderState(D3DRS_COLORWRITEENABLE, currentMask = mask);
		}
		*/
		//m_CurrentBlendState = newState;
		//}
		/*
		if (sampleMask != currentSampleMask){
			dev->SetRenderState(D3DRS_MULTISAMPLEMASK, sampleMask);
			currentSampleMask = sampleMask;
		}
		*/
}

// ---------------------------------------------------------------------
// Sets the blend state without checking and update current blend state
// ---------------------------------------------------------------------
void Renderer::setBlendState(int id) {
	if ( id != m_CurrentBS ) {
		const BlendState& newState = m_BlendStates[id];
		LPDIRECT3DDEVICE9 dev = device->get();
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
			dev->SetRenderState(D3DRS_ALPHAFUNC,newState.alphaFunc);
		}
		m_CurrentBS = id;
	}
}

// -------------------------------------------------------
// Create rasterizer state
// -------------------------------------------------------
RasterizerState* Renderer::createRasterizerState(const char* name,int cullMode,int fillMode,bool multiSample,bool scissor) {
	RasterizerState* rs = new RasterizerState;
	rs->cullMode = cullMode;
	rs->fillMode = fillMode;
	rs->multiSample = multiSample;
	rs->scissor = scissor;
	m_RasterizerStates.append(rs);
	return rs;
}

// -------------------------------------------------------
// Sets the current rasterizer state
// -------------------------------------------------------
void Renderer::setRasterizerState(RasterizerState* rasterizerState) {
	LPDIRECT3DDEVICE9 dev = device->get();
	dev->SetRenderState(D3DRS_CULLMODE, m_RSState.cullMode = rasterizerState->cullMode);
	dev->SetRenderState(D3DRS_FILLMODE, m_RSState.fillMode = rasterizerState->fillMode);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_RSState.multiSample = rasterizerState->multiSample);
	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, m_RSState.scissor = rasterizerState->scissor);	
}

// -------------------------------------------------------
// Change the current rasterizer state
// -------------------------------------------------------
void Renderer::changeRasterizerState(RasterizerState* rasterizerState) {
	LPDIRECT3DDEVICE9 dev = device->get();
	if ( rasterizerState->cullMode != m_RSState.cullMode ) {
		dev->SetRenderState(D3DRS_CULLMODE, m_RSState.cullMode = rasterizerState->cullMode);
	}
	if ( rasterizerState->fillMode != m_RSState.fillMode ) {
		dev->SetRenderState(D3DRS_FILLMODE, m_RSState.fillMode = rasterizerState->fillMode);
	}
	if ( rasterizerState->multiSample != m_RSState.multiSample ) {
		dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_RSState.multiSample = rasterizerState->multiSample);
	}
	if ( rasterizerState->scissor != m_RSState.scissor ) {
		dev->SetRenderState(D3DRS_SCISSORTESTENABLE, m_RSState.scissor = rasterizerState->scissor);
	}	
}

// -------------------------------------------------------
// Find free material slot
// -------------------------------------------------------
int Renderer::findFreeMaterialSlot() {
	for ( int i = 0; i < MAX_MATERIALS; ++i ) {
		if ( m_Materials[i].flag == -1 ) {
			return i;
		}
	}
	return -1;
}
// -------------------------------------------------------
// Creates a new material
// -------------------------------------------------------
int Renderer::createMaterial(const char* name,int textureID) {
	int id = findFreeMaterialSlot();
	if ( id != -1 ) {
		Material* mtrl = &m_Materials[id];
		mtrl->name = string::murmur_hash(name);
		mtrl->textures[0] = textureID;
		mtrl->textureAtlas = 0;
		mtrl->flag = 1;
		LOGC(logINFO,"Renderer") << "new material " << name << " with textureID " << textureID << " created";
		return id;
	}
	else {
		LOGC(logERROR,"Renderer") << "No more free material slots available";
		return -1;
	}
}

// -------------------------------------------------------
// Set texture atlas to material
// -------------------------------------------------------
void Renderer::setTextureAtlas(int materialID,int textureAtlasID) {
	assert(materialID < MAX_MATERIALS);
	assert(textureAtlasID < m_AtlasCounter);
	m_Materials[materialID].textureAtlas = textureAtlasID;
}
// -------------------------------------------------------
// 
// -------------------------------------------------------
LPDIRECT3DTEXTURE9 Renderer::getDirectTexture(int textureID) {
	assert(textureID < MAX_TEXTURES);
	return m_Textures[textureID].texture;
}
// -------------------------------------------------------
// Set the requested texture
// -------------------------------------------------------
void Renderer::setTexture(int textureID,int index) {
	assert(textureID < MAX_TEXTURES);
	Texture* tr = &m_Textures[textureID];
	if ( tr->flags != 0 ) {
		IDirect3DDevice9 * pDevice = device->get();
		HR(pDevice->SetTexture( index, tr->texture));	
		HR(pDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
		HR(pDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		HR(pDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT ));
		m_DrawCounter->addTexture();
	}
}
// -------------------------------------------------------
// Find free texture slot
// -------------------------------------------------------
int Renderer::findFreeTextureSlot() {
	for ( int i = 0; i < MAX_TEXTURES; ++i ) {
		if ( m_Textures[i].flags == 0 ) {
			return i;
		}
	}
	return -1;
}

// -------------------------------------------------------
// Create empty texture
// -------------------------------------------------------
int Renderer::createTexture(int width,int height) {
	int id = findFreeTextureSlot();
	if ( id != -1 ) {
		Texture* tr = &m_Textures[id];
		tr->name = string::murmur_hash("xxxx");
		tr->flags = 1;
		HR(getDevice()->CreateTexture( width, height,1, 0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tr->texture,NULL ));
		tr->width = width;
		tr->height = height;		
		// clear the texture with white color		
		D3DLOCKED_RECT sRect;
		tr->texture->LockRect(0, &sRect, NULL, NULL);
		BYTE *bytes = (BYTE*)sRect.pBits;
		memset(bytes, 128, width*sRect.Pitch);
		tr->texture->UnlockRect(0);
	}
	return id;
}

D3DLOCKED_RECT Renderer::lockTexture(int id) {
	D3DLOCKED_RECT lockedRect;
	ZeroMemory(&lockedRect, sizeof(lockedRect));
	Texture* tr = &m_Textures[id];
	HR(tr->texture->LockRect(0,&lockedRect,NULL,0));	
	return lockedRect;
}

void Renderer::unlockTexture(int id) {
	Texture* tr = &m_Textures[id];
	HR(tr->texture->UnlockRect(0));	
}
// -------------------------------------------------------
// Creates and loads a texture
// -------------------------------------------------------
int Renderer::loadTexture(const char* name) {
	int id = findFreeTextureSlot();
	if ( id != -1 ) {
		Texture* tr = &m_Textures[id];
		tr->name = string::murmur_hash(name);
		tr->flags = 1;
		int lw = D3DX_DEFAULT;
		int lh = D3DX_DEFAULT;		
		D3DXIMAGE_INFO imageInfo;
		char fileName[256];
		sprintf(fileName,"content\\textures\\%s.png",name);
		LOGC(logINFO,"Renderer") << "Trying to load texture " << fileName;
		HR(D3DXCreateTextureFromFileEx(device->get(),fileName, 0, 0, 1, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL,&tr->texture));

		tr->width = imageInfo.Width;
		tr->height = imageInfo.Height;		
		LOGC(logINFO,"Renderer") << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
		return id;
	}
	else {
		LOGC(logERROR,"Renderer") << "No more texture slots available";
		return -1;
	}
}

// -------------------------------------------------------
// Creates and loads a texture with a defined color key
// -------------------------------------------------------
int Renderer::loadTextureWithColorKey(const char* name,const Color& color) {
	int id = findFreeTextureSlot();
	if ( id != -1 ) {
		Texture* tr = &m_Textures[id];
		tr->name = string::murmur_hash(name);
		tr->flags = 1;
		int lw = D3DX_DEFAULT;
		int lh = D3DX_DEFAULT;	

		D3DXIMAGE_INFO imageInfo;

		char fileName[256];
		sprintf(fileName,"content\\textures\\%s.png",name);
		LOGC(logINFO,"Renderer") << "Trying to load texture " << fileName;
		HR(D3DXCreateTextureFromFileEx(device->get(),fileName, 0, 0, 1, 0,
			D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, color, &imageInfo, NULL,&tr->texture));

		tr->width = imageInfo.Width;
		tr->height = imageInfo.Height;		
		LOGC(logINFO,"Renderer") << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
		return id;
	}
	else {
		LOGC(logERROR,"Renderer") << "No more texture slots available";
		return -1;
	}
}

TextureAtlas* Renderer::getTextureAtlas(int materialID) {
	assert(materialID < MAX_MATERIALS);
	int atlasID = m_Materials[materialID].textureAtlas;
	assert(atlasID < MAX_ATLAS);
	return &m_TextureAtlas[atlasID];
}
// -------------------------------------------------------
// Get texture coordinates from Texture atlas
// -------------------------------------------------------
bool Renderer::getTextureCoordinates(int materialID,const char* itemName,float *u1,float* v1,float* u2,float* v2) {
	TextureAtlas* ta = getTextureAtlas(materialID);
	return ta->getTextureCoordinates(itemName,u1,v1,u2,v2);
}

int Renderer::getAtlasItemWidth(int materialID,const char* itemName) {
	TextureAtlas* ta = getTextureAtlas(materialID);
	return ta->getWidth(itemName);
}

int Renderer::getAtlasItemHeight(int materialID,const char* itemName) {
	TextureAtlas* ta = getTextureAtlas(materialID);
	return ta->getHeight(itemName);
}
// -------------------------------------------------------
// Loads texture atlas from file
// -------------------------------------------------------
int Renderer::loadTextureAtlas(const char* name) {
	char fileName[256];
	int id = m_AtlasCounter;
	++m_AtlasCounter;
	sprintf(fileName,"content\\resources\\%s.atlas",name);
	LOGC(logINFO,"Renderer") << "trying to load " << fileName;
	JSONReader reader;
	if ( reader.parse(fileName) ) {		
		std::vector<Category*> all = reader.getCategories();
		if ( all.size() > 0 ) {
			for ( size_t i = 0; i < all.size(); ++i ) {
				Category* root = all[i];
				if ( root->getName() == "texture_atlas" ) {
					std::string atlasName = root->getProperty("name");
					float textureSize = root->read<float>("texture_size",1024.0f);
					float gridSize = root->read<float>("grid_size",30.0f);
					TextureAtlas* atlas = &m_TextureAtlas[id];
					atlas->setGridSize(gridSize);
					atlas->setTextureSize(textureSize);
					LOGC(logINFO,"Renderer") << "New TextureAtlas created - name: " << atlasName;
					std::vector<Category*> categories = root->getChildren();
					for ( size_t i = 0; i < categories.size(); ++i ) {
						Category* cat = categories[i];
						if ( cat->getName() == "item" ) {
							if ( cat->hasProperty("name") ) {					
								std::string name = cat->getProperty("name");
								if ( cat->hasProperty("rect")) {
									Rect rect = cat->getRect("rect");
									atlas->addItem(name.c_str(),rect);
								}
								else {
									Vec2 gridPos = cat->getVec2("grid_pos");
									Vec2 size = cat->getVec2("size");
									atlas->addItem(name.c_str(),gridPos,size);
								}
							}
						}						
					}
					//m_TextureAtlasList.append(atlas);
					return id;
				}
			}			
		}
		else {
			LOGC(logERROR,"Renderer") << "No texture_atlas category found in file - skipping file";
		}
	}	
	else {
		LOGC(logERROR,"TextureAtlasParser") << "Cannot parse file";
	}
	return -1;
}

int Renderer::getCharHeight(int fontID) {
	assert(fontID < MAX_BM_FONTS);
	return m_BitmapFonts[fontID].charHeight;
}
// -------------------------------------------------------
// get Char Def from Bitmap Font
// -------------------------------------------------------
const CharDef& Renderer::getCharDef(int fontID,char c) const {
	assert(fontID < MAX_BM_FONTS);
	return m_BitmapFonts[fontID].getCharDef(c);
}
// -------------------------------------------------------
// Load bitmap font settings and then create it
// -------------------------------------------------------
int Renderer::loadBitmapFont(const char* name,int textureID,Color& fillColor) {	
	assert(textureID < MAX_TEXTURES);
	char fileName[256];
	sprintf(fileName,"content\\resources\\%s.fnt",name);
	LOGC(logINFO,"Renderer") << "trying to load bitmap font " << fileName;
	SettingsReader reader;
	if ( reader.parse(fileName)) {
		BitmapFont bf;
		bf.startChar = reader.get<int>("startChar",0);
		bf.endChar = reader.get<int>("endChar",0);
		bf.charHeight = reader.get<int>("charHeight",0);
		bf.gridHeight = reader.get<int>("gridHeight",0);
		bf.startX = reader.get<int>("startX",0);
		bf.startY = reader.get<int>("startY",0);
		bf.width =	reader.get<int>("width",0);
		bf.height = reader.get<int>("height",0);
		bf.padding = reader.get<int>("padding",0);
		bf.textureSize = reader.get<uint32>("textureSize",0);
		assert(bf.textureSize != 0 );
		return createBitmapFont(bf,textureID,fillColor);
	}
	else {
		LOGC(logERROR,"Renderer") << "Failed to load bitmap font " << fileName;
	}
	return 0;
}
// -------------------------------------------------------
// Create a bitmap font
// -------------------------------------------------------
int Renderer::createBitmapFont(const BitmapFont& bitmapFont,int textureID,const Color& fillColor) {
	assert(m_BMCounter < MAX_BM_FONTS);
	int id = m_BMCounter;
	++m_BMCounter;
	D3DLOCKED_RECT lockedRect;
	assert(textureID < MAX_TEXTURES);
	Texture* texture = &m_Textures[textureID];
	HR(texture->texture->LockRect(0,&lockedRect,NULL,0));	
	m_BitmapFonts[id].startChar = bitmapFont.startChar;
	m_BitmapFonts[id].endChar = bitmapFont.endChar;
	m_BitmapFonts[id].width = bitmapFont.width;
	m_BitmapFonts[id].height = bitmapFont.height;
	m_BitmapFonts[id].padding = bitmapFont.padding;
	m_BitmapFonts[id].textureSize = bitmapFont.textureSize;
	m_BitmapFonts[id].charHeight = bitmapFont.charHeight;
	m_BitmapFonts[id].startX = bitmapFont.startX;
	m_BitmapFonts[id].startY = bitmapFont.startY;
	m_BitmapFonts[id].gridHeight = bitmapFont.gridHeight;
	uint32 x = bitmapFont.startX + bitmapFont.padding - 1;
	uint32 y = bitmapFont.startY + bitmapFont.padding;
	uint32 ascii = bitmapFont.startChar;
	Color c = getColor(lockedRect,x,y,bitmapFont.textureSize);
	bool running = true;
	bool isChar = false;
	int charStartedX = 0;
	int charStartedY = 0;
	while ( running ) {
		++x;
		if ( x > (bitmapFont.startX + bitmapFont.width) ) {
			x = bitmapFont.startX + bitmapFont.padding - 1;
			y += bitmapFont.padding + bitmapFont.gridHeight + 1;
			isChar = false;
			LOGC(logINFO,"FontParser") << "scanning next line at " << x << " " << y;
		}
		if ( y >= (bitmapFont.startY + bitmapFont.height) ) {
			running = false;
		}
		if ( y >= texture->height ) {
			running = false;
		}
		if ( running ) {
			c = getColor(lockedRect,x,y,bitmapFont.textureSize);
			if ( !isFillColor(fillColor,c) && !isChar ) {
				isChar = true;
				charStartedX = x;
				charStartedY = y;				
			}
			else if ( isFillColor(fillColor,c) && isChar ) {
				isChar = false;
				int width = x - charStartedX - 2;
				//LOG(logINFO) << "char: " << (char)ascii << " pos " << charStartedX << " " << charStartedY << " width " << width;
				m_BitmapFonts[id].addChar(ascii,charStartedX+1,charStartedY+1,width);
				++ascii;
			}
			
		}
	}
	HR(texture->texture->UnlockRect(0));		
	return id;
}

ID3DXFont* Renderer::getInternalSystemFont(int fontID) {
	assert(fontID < MAX_SYSTEM_FONTS);
	return m_Fonts[fontID].font;
}
// -------------------------------------------------------
// Loads system font
// -------------------------------------------------------
int Renderer::loadSystemFont(const char* name,const char* fontName,int size,bool bold) {
	LOGC(logINFO,"Renderer") << "Loading font " << fontName << " size " << size;
	int id = -1;
	for ( int i = 0 ; i < MAX_SYSTEM_FONTS; ++i ) {
		if ( m_Fonts[i].flag == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		SystemFont* f = &m_Fonts[id];		
		UINT type = FW_NORMAL;
		if ( bold ) {
			type = FW_BOLD;
		}
		HDC hDC = GetDC(m_Hwnd);
		int nHeight = -MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		HR(D3DXCreateFontA(getDevice(),nHeight,0,type,0,false,DEFAULT_CHARSET,
			DEFAULT_QUALITY,ANTIALIASED_QUALITY,
			DEFAULT_PITCH|FF_DONTCARE,
			fontName,&f->font));
		f->bold = bold;
		f->name = string::murmur_hash(name);
		f->size = size;		
		f->flag = 1;
		return id;	
	}
	else {
		LOGC(logERROR,"Renderer") << "No more free slots for system fonts available";
		return -1;
	}
}
// -------------------------------------------------------
// Checks if color is fill color
// -------------------------------------------------------
bool Renderer::isFillColor(const Color& fillColor,const Color& currentColor) {
	if ( fillColor.r != currentColor.r ) {
		return false;
	}
	if ( fillColor.g != currentColor.g ) {
		return false;
	}
	if ( fillColor.b != currentColor.b ) {
		return false;
	}
	if ( fillColor.a != currentColor.a ) {
		return false;
	}
	return true;
}

// -------------------------------------------------------
// Get color from a pixel of a texture
// -------------------------------------------------------
Color Renderer::getColor(D3DLOCKED_RECT& lockedRect,int x,int y,int height) {
	uchar * pBits = (uchar *)lockedRect.pBits;
	uchar b = (uchar)pBits[(x + y * height) * 4];	
	uchar g = (uchar)pBits[(x + y * height) * 4 + 1];	
	uchar r = (uchar)pBits[(x + y * height) * 4 + 2];	
	uchar a = (uchar)pBits[(x + y * height) * 4 + 3];	
	return Color(r,g,b,a);
}

// -------------------------------------------------------
// Sets color of a pixel in a texture
// -------------------------------------------------------
void Renderer::setColor(D3DLOCKED_RECT& lockedRect,int x,int y,uchar r,uchar g,uchar b,uchar a,int height) {
	uchar * pBits = (uchar *)lockedRect.pBits;
	int p = (x + y * height) * 4;
	pBits[p]   = b; // blue			
	pBits[p+1] = g; // green				
	pBits[p+2] = r; // red
	pBits[p+3] = a; // alpha
}

// -------------------------------------------------------
// Apply shader and set some constants
// -------------------------------------------------------
void Renderer::applyShader(Shader* shader) {
	HR(shader->m_FX->SetTechnique(shader->m_hTech));	
	D3DXHANDLE hndl = shader->m_FX->GetParameterByName(0,"gWVP");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getWVPMatrix(),sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gWorld");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getWorldMatrix(),sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gWorldInverseTranspose");
	if ( hndl != NULL ) {
		D3DXMATRIX worldInverseTranspose;
		D3DXMatrixInverse(&worldInverseTranspose,0,&matrix::convert(getWorldMatrix()));
		D3DXMatrixTranspose(&worldInverseTranspose,&worldInverseTranspose);
		shader->m_FX->SetMatrix(hndl,&worldInverseTranspose);
	}
	hndl = shader->m_FX->GetParameterByName(0,"gWorldInverse");
	if ( hndl != NULL ) {
		D3DXMATRIX worldInverse;
		D3DXMatrixInverse(&worldInverse,0,&matrix::convert(getWorldMatrix()));
		shader->m_FX->SetValue(hndl,&worldInverse,sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gWorld");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getWorldMatrix(),sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gView");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getViewMatrix(),sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gProjection");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getProjectionMatrix(),sizeof(mat4));
	}
}
// -------------------------------------------------------
// Draw node
// -------------------------------------------------------
void Renderer::drawNode(Node* node) {

	PR_START(node->getNodeName())
	applyTransformation(node);
	PR_START("PREPARE")
	node->prepareRendering();
	PR_END("PREPARE")
	PR_START("DRAW")
	applyMaterial(node->getMaterial());		
	int mtrl = node->getMaterial();
	if ( hasShader(mtrl) ) {
		int used = 0;
		Material* material = &m_Materials[mtrl];
		Shader* shader = &m_Shaders[material->shader];
		m_DrawCounter->addShader();
		applyShader(shader);
			// update parameters
		UINT numPasses = 0;
		HR(shader->m_FX->Begin(&numPasses,0));
		for ( UINT p = 0; p < numPasses; ++p ) {
			++used;
			HR(shader->m_FX->BeginPass(p));
			node->draw();
			HR(shader->m_FX->EndPass());
		}
		HR(shader->m_FX->End());	
		// no shader used then just draw it
		if ( used == 0 ) {				
			node->draw();
		}
	}
	// no shaders so simply draw it
	else {			
		node->draw();	
	}
	PR_END("DRAW")
	PR_START("POST")
	node->postRendering();
	PR_END("POST")	
	PR_END(node->getNodeName())
}

// -------------------------------------------------------
// Set texture shader constant by name
// -------------------------------------------------------
void Renderer::setTexture(int shaderID,const char* handleName,int textureID) {
	assert(shaderID < MAX_SHADERS);
	assert(textureID != -1);
	Shader* shader = &m_Shaders[shaderID];
	IdString hashName = string::murmur_hash(handleName);
	ShaderConstant* sh = shader->constants;
	for ( int i = 0; i < shader->constantCount; ++i ) {
		if ( hashName == sh->name ) {
			shader->m_FX->SetTexture(sh->handle,getDirectTexture(textureID));
		}
		++sh;
	}	
}

void Renderer::setCurrentShader(int shaderID) {
	m_CurrentShader = shaderID;
}
// -------------------------------------------------------
// Find free shader slot
// -------------------------------------------------------
int Renderer::findFreeShaderSlot() {
	for ( int i = 0; i < MAX_SHADERS; ++i ) {
		if ( m_Shaders[i].flag == 0 ) {
			return i;
		}
	}
	return -1;
}

int Renderer::createShaderFromText(const char* buffer,const char* techName) {
	int id = findFreeShaderSlot();
	if ( id != -1 ) {
		Shader* shader = &m_Shaders[id];
		UINT dwBufferSize = ( UINT )strlen( buffer ) + 1;
		ID3DXBuffer* errors = 0;
		D3DXCreateEffect(getDevice(), buffer, dwBufferSize,0, 0, SHADER_FLAGS, 0, &shader->m_FX, &errors);
		if ( errors != 0 ) {
			LOGC(logERROR,"Renderer") << "Error while loading shader: " << (char*)errors->GetBufferPointer();
			return -1;
		}
		initializeShader(id,techName);
	}
	return id;
}

Shader& Renderer::getShader(int id) {
	return m_Shaders[id];
}
// -------------------------------------------------------
// Load shader
// -------------------------------------------------------
int Renderer::loadShader(const char* fxName,const char* techName) {
	int id = findFreeShaderSlot();
	if ( id != -1 ) {
		char fileName[256];
		sprintf(fileName,"content\\effects\\%s.fx",fxName);
		Shader* shader = &m_Shaders[id];

		ID3DXBuffer* errors = 0;
		HRESULT hr = D3DXCreateEffectFromFileA(getDevice(), fileName,0, 0, SHADER_FLAGS, 0, &shader->m_FX, &errors);
		//if ( hr != S_OK && errors != 0 ) {
		if ( errors != 0 ) {
			LOGC(logERROR,"Renderer") << "Error while loading shader: " << (char*)errors->GetBufferPointer();
			return -1;
		}
		LOGC(logINFO,"Renderer") << "Shader created";
		initializeShader(id,techName);
		return id;
	}
	else {
		LOGC(logERROR,"Renderer") << "Cannot load shader - No more free slots available";
		return -1;
	}
}

void Renderer::initializeShader(int id,const char* techName) {
	Shader* shader = &m_Shaders[id];
	shader->m_hTech = shader->m_FX->GetTechniqueByName(techName);
	D3DXEFFECT_DESC effectDesc;
	shader->m_FX->GetDesc(&effectDesc);
	UINT nc = effectDesc.Parameters;
	shader->constants = new ShaderConstant[nc];
	shader->constantCount = nc;
	LOGC(logINFO,"Renderer") << "Got Description - number of parameters: " << nc;
	for ( UINT i = 0; i < effectDesc.Parameters;++i ) {
		D3DXHANDLE hParam = shader->m_FX->GetParameter( NULL, i );
		D3DXPARAMETER_DESC pDesc;
		// get parameter description
		shader->m_FX->GetParameterDesc( hParam, &pDesc );
		LOGC(logINFO,"Renderer") << "Parameter : " << pDesc.Name << " Type: " << pDesc.Type;
		shader->constants[i].handle = hParam;
		shader->constants[i].name = string::murmur_hash(pDesc.Name);
	}
	LOGC(logINFO,"Renderer") << "Shader finally loaded";
	shader->flag = 1;
}
// -------------------------------------------------------
// Create basic vertex declarations
// -------------------------------------------------------
void Renderer::createBasicVertexDeclarations() {
	// transformed texture color
	VertexDeclaration* vd = new VertexDeclaration;
	vd->addElement(ds::VT_FLOAT4,ds::VDU_POSITIONT);
	vd->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);
	vd->addElement(ds::VT_FLOAT4,ds::VDU_COLOR);
	//addVertexDeclaration("TTC",vd);
	vd->create(device);
	m_VDStructs[VD_TTC].vertexSize = 40;
	m_VDStructs[VD_TTC].declaration = vd;
	// position tangent binormal normal texture coords
	VertexDeclaration* ptbntVD = new VertexDeclaration;
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_TANGENT);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_BINORMAL);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_NORMAL);
	ptbntVD->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);			
	ptbntVD->create(device);
	//addVertexDeclaration("PTNBT",ptbntVD);
	m_VDStructs[VD_PTNBT].vertexSize = 56;
	m_VDStructs[VD_PTNBT].declaration = ptbntVD;

}

// ---------------------------------------------------------
// Creates a render target and returns the attached texture
// ---------------------------------------------------------
int Renderer::createRenderTarget(const char* name) {
	int id = findFreeTextureSlot();
	if ( id != -1 ) {
		RenderTarget* renderTarget = new RenderTarget;
		renderTarget->name = string::murmur_hash(name);
		getDevice()->CreateTexture(m_Width,
			m_Height,
			0,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&renderTarget->texture,
			NULL);
		/*
		if (renderTarget->surface == 0) {
			renderTarget->surface = new LPDIRECT3DSURFACE9;
		}
		*/	
		Texture* t = &m_Textures[id];
		t->height = m_Height;
		t->width = m_Width;
		t->flags = 1;
		t->texture = renderTarget->texture;
		renderTarget->texture->GetSurfaceLevel(0,&renderTarget->surface);
		m_RenderTargets.append(renderTarget);
		return id;
	}
	else {
		LOG(logERROR) << "Cannot create rendertarget - No more texture slots available";
		return -1;
	}	
}

// ---------------------------------------------------------
// Create vertex buffer
// ---------------------------------------------------------
int Renderer::createVertexBuffer(PrimitiveType primitiveType,int vertexDefinition,int size,bool dynamic) {
	int id = -1;
	for ( int i = 0; i< MAX_BUFFERS; ++i ) {
		if ( m_Buffers[i].used == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		GeometryBuffer* buffer = &m_Buffers[id];
		buffer->used = 1;
		buffer->maxSize = size;
		buffer->type = GBT_VERTEX;
		buffer->primitiveType = primitiveType;
		buffer->vertexDefinition = vertexDefinition;
		buffer->dynamic = dynamic;
		VDStruct vd = getVertexDeclaration(vertexDefinition);
		int vertexSize = vd.vertexSize;	
		D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
		DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
		LOGC(logINFO,"Renderer") << "creating new vertex buffer - size: " << size << " vertexDefinition: " << vertexDefinition << " vertexSize: " << vertexSize << " dynamic: " << (dynamic == 0 ? false : true);
		HR(getDevice()->CreateVertexBuffer( size * vertexSize,usage,0 ,pool, &m_Buffers[id].vertexBuffer, NULL ))
	}
	return id;
}

// ---------------------------------------------------------
// Create an index buffer
// ---------------------------------------------------------
int Renderer::createIndexBuffer(int size,bool dynamic) {
	int id = -1;
	for ( int i = 0; i< MAX_BUFFERS; ++i ) {
		if ( m_Buffers[i].used == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		GeometryBuffer* buffer = &m_Buffers[id];
		buffer->used = 1;
		buffer->maxSize = size;
		buffer->type = GBT_INDEX;
		buffer->dynamic = dynamic;
		buffer->vertexDefinition = 0;
		D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
		DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
		HR(getDevice()->CreateIndexBuffer( size * sizeof(WORD),usage,D3DFMT_INDEX16,pool,&m_Buffers[id].indexBuffer,NULL));	
		LOGC(logINFO,"Renderer") << "new IndexBuffer created size: " << size << " dynamic: " << (dynamic == 0 ? false : true);
	}
	return id;
}

// ---------------------------------------------------------
// Create an index buffer prepared for quads
// ---------------------------------------------------------
int Renderer::createQuadIndexBuffer(uint32 maxQuads) {
	int indices = maxQuads * 6;
	bool dynamic = false;
	int id = -1;
	for ( int i = 0; i< MAX_BUFFERS; ++i ) {
		if ( m_Buffers[i].used == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		GeometryBuffer* buffer = &m_Buffers[id];
		buffer->used = 1;
		buffer->maxSize = indices;
		buffer->type = GBT_INDEX;
		buffer->dynamic = dynamic;
		buffer->vertexDefinition = 0;
		D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
		DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
		HR(getDevice()->CreateIndexBuffer( indices * sizeof(WORD),usage,D3DFMT_INDEX16,pool,&m_Buffers[id].indexBuffer,NULL));	
		LOGC(logINFO,"Renderer") << "new IndexBuffer created size: " << indices << " dynamic: " << (dynamic == 0 ? false : true);
		WORD* indexBuffer;
		HR(m_Buffers[id].indexBuffer->Lock( 0, indices * sizeof(WORD), ( void** )&indexBuffer, 0 ));

		for ( uint32 i = 0; i < maxQuads;++i ) {
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
		HR(m_Buffers[id].indexBuffer->Unlock());
	}
	return id;
}
// ---------------------------------------------------------
// Creates a buffer handle
// ---------------------------------------------------------
int Renderer::createBufferHandle(PrimitiveType primType,int vertexDefinition,GeoBufferType bufferType,bool dynamic) {
	int id = -1;
	for ( int i = 0; i< MAX_BUFFER_HANDLES; ++i ) {
		if ( m_BufferHandles[i].used == 0 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		GeoBufferHandle* handle = &m_BufferHandles[id];
		handle->used = 1;
		handle->locked = 0;
		handle->primitiveType = primType;
		handle->iBufferRef.bufferIdx = -1;
		handle->vBufferRef.bufferIdx = -1;
		handle->bufferType = bufferType;
		handle->dynamic = dynamic;
		handle->vertexDefinition = vertexDefinition;
		LOGC(logINFO,"Renderer") << "buffer handle created - id: " << id << " bufferType: " << bufferType << " vertexDefinition: " << vertexDefinition << " dynamic: " << dynamic;
	}
	return id;
}

// ---------------------------------------------------------
// Reset dynamic buffers
// ---------------------------------------------------------
void Renderer::resetDynamicBuffers() {
	for ( int i = 0; i< MAX_BUFFERS; ++i ) {
		if ( m_Buffers[i].used != 0 ) {
			GeometryBuffer* buffer = &m_Buffers[i];
			if ( buffer->dynamic ) {
				buffer->size = 0;
			}
		}
	}
}
// ---------------------------------------------------------
// Allocate buffer
// ---------------------------------------------------------
int Renderer::allocateBuffer(GeoBufferType type,int vertexDefinition,int size,int& start,bool dynamic) {
	for ( int i = 0; i < MAX_BUFFERS; ++i ) {
		GeometryBuffer* gb = &m_Buffers[i];
		if ( gb->type == type && gb->dynamic == dynamic && ( vertexDefinition == -1 || gb->vertexDefinition == vertexDefinition ) ) {
			//LOG(logINFO) << "found matching buffer - id: " << i;
			uint32 current = gb->size;
			if ( (current + size) < gb->maxSize ) {
				start = gb->size;
				gb->size += size;
				//LOG(logINFO) << "setting start " << start << " new size is " << gb->size;
				return i;
			}
		}
	}
	return -1;
}

// ---------------------------------------------------------
// Lock buffers
// ---------------------------------------------------------
void Renderer::lockBuffer(int handleID,int vertexCount,int indexCount,float** vertexBuffer,void** indexBuffer) {
	//LOG(logINFO) << " --- Lock Buffer ----";
	assert(handleID < MAX_BUFFER_HANDLES);
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	handle->locked = 1;
	int ret = -1;
	if ( handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH ) {
		handle->iBufferRef.start = 0;
		handle->iBufferRef.count = indexCount;
		//if ( handle->iBufferRef.bufferIdx == -1 ) {
			handle->iBufferRef.bufferIdx = allocateBuffer(GBT_INDEX,-1,indexCount,handle->iBufferRef.start,handle->dynamic);
			//LOG(logINFO) << "IndexBuffer - start " << handle->iBufferRef.start << " count " << indexCount;
		//}
		assert(handle->iBufferRef.bufferIdx != -1 );
		DWORD flag = 0;
		if ( m_Buffers[handle->iBufferRef.bufferIdx].dynamic ) {
			flag = D3DLOCK_DISCARD;
		}
		IDirect3DIndexBuffer9 *IB = m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer;
		assert(IB != 0);	
		HR(IB->Lock( handle->iBufferRef.start * sizeof(WORD), indexCount * sizeof(WORD), ( void** )indexBuffer, flag ));
	}
	if ( handle->bufferType == GBT_VERTEX || handle->bufferType == GBT_BOTH ) {
		handle->vBufferRef.start = 0;
		handle->vBufferRef.count = vertexCount;
		//if ( handle->vBufferRef.bufferIdx == -1 ) {
			handle->vBufferRef.bufferIdx = allocateBuffer(GBT_VERTEX,handle->vertexDefinition,vertexCount,handle->vBufferRef.start,handle->dynamic);
			//LOG(logINFO) << "VertexBuffer - start " << handle->vBufferRef.start << " count " << vertexCount;
		//}
		assert(handle->vBufferRef.bufferIdx != -1 );
		DWORD flag = 0;
		if ( m_Buffers[handle->vBufferRef.bufferIdx].dynamic ) {
			flag = D3DLOCK_DISCARD;
		}
		int vDef = m_Buffers[handle->vBufferRef.bufferIdx].vertexDefinition;
		int vSize = m_VDStructs[vDef].vertexSize;
		IDirect3DVertexBuffer9 *VB = m_Buffers[handle->vBufferRef.bufferIdx].vertexBuffer;
		assert(VB != 0);	
		HR(VB->Lock( handle->vBufferRef.start *vSize, vertexCount * vSize, ( void** )vertexBuffer, flag ));
	}
}

// ---------------------------------------------------------
// Unlock buffers
// ---------------------------------------------------------
void Renderer::unlockBuffer(int handleID) {
	assert(handleID < MAX_BUFFER_HANDLES);
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	if ( handle->locked == 1 ) {
		if ( handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH ) {
			HR(m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer->Unlock());
		}
		if ( handle->bufferType == GBT_VERTEX || handle->bufferType == GBT_BOTH ) {
			HR(m_Buffers[handle->vBufferRef.bufferIdx].vertexBuffer->Unlock());
		}
		handle->locked = 0;
	}
}

// ---------------------------------------------------------
// Draw buffer
// ---------------------------------------------------------
int Renderer::drawBuffer(int handleID) {
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	int vDef = m_Buffers[handle->vBufferRef.bufferIdx].vertexDefinition;
	if ( m_CurrentVD != vDef ) {
		m_CurrentVD = vDef;
		getDevice()->SetVertexDeclaration(m_VDStructs[vDef].declaration->get());
	}
	if ( handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH ) {
		if ( handle->iBufferRef.bufferIdx != m_CurrentIB ) {
			m_CurrentIB = handle->iBufferRef.bufferIdx;
			HR(getDevice()->SetIndices( m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer)); 
		}
	}
	if ( handle->bufferType == GBT_VERTEX || handle->bufferType == GBT_BOTH ) {
		if ( handle->vBufferRef.bufferIdx != m_CurrentVB ) {
			m_CurrentVB = handle->vBufferRef.bufferIdx;
			HR(getDevice()->SetStreamSource( 0, m_Buffers[handle->vBufferRef.bufferIdx].vertexBuffer, 0 , m_VDStructs[m_CurrentVD].vertexSize ));
		}
	}	
	D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
	int numPrimitives = handle->vBufferRef.count / 2;
	m_DrawCounter->addDrawCall();
	m_DrawCounter->addPrimitives(handle->vBufferRef.count);
	m_DrawCounter->addIndices(handle->iBufferRef.count);
	if ( m_CurrentShader != -1 ) {
		Shader* shader = &m_Shaders[m_CurrentShader];
		m_DrawCounter->addShader();
		applyShader(shader);
		shader->m_FX->CommitChanges();
		// update parameters
		UINT numPasses = 0;
		HR(shader->m_FX->Begin(&numPasses,0));
		for ( UINT p = 0; p < numPasses; ++p ) {		
			HR(shader->m_FX->BeginPass(p));
			HR(getDevice()->DrawIndexedPrimitive( pt, handle->vBufferRef.start, 0, handle->vBufferRef.count, handle->iBufferRef.start, numPrimitives ));
			HR(shader->m_FX->EndPass());
		}
		HR(shader->m_FX->End());	
	}
	else {
		HR(getDevice()->DrawIndexedPrimitive( pt, handle->vBufferRef.start, 0, handle->vBufferRef.count, handle->iBufferRef.start, numPrimitives ));
	}
	return handle->iBufferRef.start;
}

};

