#include "Renderer.h"
#include "GraphicsDevice.h"
#include "..\utils\Log.h"
#include "Camera.h"
#include "..\utils\Profiler.h"
#include "..\lib\container\List.h"
#include "VertexDeclaration.h"
#include "..\utils\PlainTextReader.h"
#include <d3dcommon.h>
#include "..\math\matrix.h"
#include "..\utils\font.h"
#include "shader.h"
#include "assets.h"

namespace ds {

RenderContext renderContext;

namespace renderer {

	VDStruct& getVertexDeclaration(int declarationType) {
		return renderContext.vdStructs[declarationType];
	}
}

#ifdef DEBUG
	DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
#else
	DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE;
#endif

	const int MAX_VB_ENTRIES = 4096;
	const int MAX_VB_VERTICES = MAX_VB_ENTRIES * 4;
	const int MAX_IN_VERTICES = MAX_VB_ENTRIES * 6;
//-----------------------------------------------
// The scene manager implementation
//-----------------------------------------------
Renderer::Renderer(HWND hWnd,const Settings& settings) {//: m_Hwnd(hWnd) {
	renderContext.hwnd = hWnd;
	LOG << "RenderDevice::RenderDevice";
	LOG << "Preparing internal structs";
	// vertex declarations
	for ( int i = 0; i < MAX_VERDECLS; ++i) {
		renderContext.vdStructs[i].vertexSize = 0;
		renderContext.vdStructs[i].declaration = 0;
	}
	for ( int i = 0; i < MAX_TEXTURES; ++i ) {
		renderContext.textures[i].texture = 0;
		renderContext.textures[i].flags = 0;
		renderContext.textures[i].name = 0;
	}
	for ( int i = 0; i < MAX_SHADERS; ++i ) {
		m_Shaders[i].flag = 0;
		m_Shaders[i].m_FX = 0;
	}
	m_BMCounter = 0;
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
	graphics::initializeDevice(settings);
	m_Viewport = new Viewport(settings.width,settings.height);
	int centerX = settings.width / 2;
	int centerY = settings.height / 2;
	m_Viewport->setPosition(centerX,centerY);
	// the camera
    float aspect = (float)settings.width / (float)settings.height;
	m_Camera = new Camera(settings.width,settings.height);
	m_Camera->setLens(D3DX_PI * 0.25f, aspect, 0.1f, 1000.0f);
	renderContext.device->SetTransform( D3DTS_PROJECTION, &matrix::convert(m_Camera->getProjectionMatrix()));	
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

	//m_PostProcessing = settings.postProcessing;	
	m_DefaultShaderID = shader::createPTCShader(this,-1);
	m_DefaultBS = createBlendState(BL_SRC_ALPHA,BL_ONE_MINUS_SRC_ALPHA,true);
	gfx::init(this,m_DefaultShaderID,m_DefaultBS);
	LOG << "default blendstate " << m_DefaultBS;
	m_CurrentBS = -1;
	createBasicVertexDeclarations();
	renderContext.device->GetRenderTarget(0,&m_BackBuffer);
	char buffer[128];
	sprintf(buffer,"Backbuffer 0x%p",m_BackBuffer);
	LOGC("Renderer") << "Address of " << buffer;
	// create default buffers
	createVertexBuffer(PT_TRI,VD_TTC,4096,true);
	createVertexBuffer(PT_TRI,VD_PTC,MAX_VB_VERTICES,true);
	createVertexBuffer(PT_TRI,VD_PTC,4096);
	createIndexBuffer(MAX_IN_VERTICES, true);
	createVertexBuffer(PT_TRI,VD_TTC,4096);
	createIndexBuffer(MAX_IN_VERTICES);
	// prepare debug
	loadSystemFont("Verdana","Verdana",10,false);
	D3DXCreateSprite(renderContext.device, &m_DebugSprite );
	m_BFCounter = 0;
}


Renderer::~Renderer(void) {	
	LOG << "destructing Renderer";		
	m_RasterizerStates.deleteContents();
	LOGC("Renderer") << "Releasing textures";
	for ( int i = 0; i < MAX_TEXTURES; ++i ) {
		if ( renderContext.textures[i].flags != 0 ) {
			SAFE_RELEASE(renderContext.textures[i].texture);
		}
	}
	LOGC("Renderer") << "Releasing vertex declarations";
	for ( int i = 0; i < MAX_VERDECLS; ++i) {
		if (renderContext.vdStructs[i].vertexSize != 0) {
			delete renderContext.vdStructs[i].declaration;
		}
	}
	LOGC("Renderer") << "Releasing shaders";
	for ( int i = 0; i < MAX_SHADERS; ++i ) {
		if ( m_Shaders[i].flag != 0 ) {
			delete m_Shaders[i].constants;
			SAFE_RELEASE(m_Shaders[i].m_FX);
		}
	}
	LOGC("Renderer") << "Releasing system font";
	SAFE_RELEASE(m_SystemFont);
	LOGC("Renderer") << "Releasing buffers";
	for ( int i = 0; i < MAX_BUFFERS; ++i ) {
		SAFE_RELEASE(m_Buffers[i].vertexBuffer);
		SAFE_RELEASE(m_Buffers[i].indexBuffer);
	}
	SAFE_RELEASE(m_DebugSprite);
	delete m_Camera;
	//delete device;		
	if( renderContext.device != NULL ) {
		renderContext.device->Release();
	}
	if( renderContext.pD3D != NULL ) {
		renderContext.pD3D->Release();
	}
	delete m_Viewport;
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
	m_CurrentRT = -1;
	for ( int i = 0; i < 5; ++i ) {
		m_CurrentTextures[i] = -1;
	}
	// Clear the back buffer to a blue color
    HR(renderContext.device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0 ));		
    // Begin the scene
	if( SUCCEEDED( renderContext.device->BeginScene() ) ) {		
		setBlendState(m_DefaultBS);
		set2DCameraOn();
		//m_SpriteBatch->prepare();
		//m_SpriteBatch->begin();
		//m_SpriteBatch->setBlendState(m_DefaultBS);
		//m_SpriteBatch->setShader(m_DefaultShaderID);
		return true;
	}
	else {
		LOGEC("Renderer") << "cannot begin scene";
	}	
	return false;
}

// -------------------------------------------------------
// Clear
// -------------------------------------------------------
void Renderer::clear() {
	HR(renderContext.device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ClearColor, 1.0f, 0 ));
}

void Renderer::flush() {
	//m_SpriteBatch->flush();
	//m_SpriteBatch->end();
	sprites::flush();
	resetDynamicBuffers();
}
// -------------------------------------------------------
// End rendering
// -------------------------------------------------------
void Renderer::endRendering() {
	renderContext.device->EndScene();
    renderContext.device->Present( NULL, NULL, NULL, NULL );
}

void Renderer::setTransformations() {
	HR(renderContext.device->SetTransform( D3DTS_WORLD, &matrix::convert(m_World )));  
	HR(renderContext.device->SetTransform( D3DTS_VIEW, &matrix::convert(m_Camera->getViewMatrix())));
	HR(renderContext.device->SetTransform( D3DTS_PROJECTION, &matrix::convert(m_Camera->getProjectionMatrix() )));
	matWorldViewProj = m_World * m_Camera->getViewMatrix() * m_Camera->getProjectionMatrix();
	m_Camera->buildView();
}
//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform Matrices.
//-----------------------------------------------------------------------------
VOID Renderer::setupMatrices() {  
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
	m_Camera->setOrthogonal();
	m_World = matrix::m4identity();
	setTransformations();
	setRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	setRenderState(D3DRS_ZENABLE, FALSE);
}


//-----------------------------------------------------------------------------
// Set2DCameraOff: exit 2D mode
//-----------------------------------------------------------------------------
void Renderer::set2DCameraOff() {
	m_Camera->restore();		
	setTransformations();		
	setRenderState(D3DRS_ZENABLE, TRUE);		
}

// -------------------------------------------------------
// Set render target
// -------------------------------------------------------
void Renderer::setRenderTarget(uint32 id) {
	//LOG << ">> set RT " << id << " current " << m_CurrentRT;
	RenderTarget* rt = &m_RenderTargets[id];
	assert(rt->flag == 1);
	//m_SpriteBatch->flush();
	if ( m_CurrentRT != -1 ) {
		RenderTarget* activeRT = &m_RenderTargets[m_CurrentRT];
		activeRT->rts->EndScene(0);
	}
	else {
		renderContext.device->EndScene();
	}
	rt->rts->BeginScene( rt->surface, NULL );
	m_CurrentRT = id;
	renderContext.device->SetRenderTarget(0,rt->surface);			
	HR(renderContext.device->Clear( 0, NULL, D3DCLEAR_TARGET,rt->clearColor, 1.0f, 0 ));
	m_UsedRTs = 1;
}
// -------------------------------------------------------
// restore backbuffer
// -------------------------------------------------------
void Renderer::restoreBackBuffer(uint32 id) {	
	//LOG << "restore RT " << id << " current " << m_CurrentRT;
	assert(m_CurrentRT != -1);
	RenderTarget* rt = &m_RenderTargets[m_CurrentRT];
	assert(rt->flag == 1);
	//m_SpriteBatch->flush();
	rt->rts->EndScene(0);
	m_CurrentRT = -1;
	renderContext.device->BeginScene();
	//FIXME: clear buffer
	renderContext.device->SetRenderTarget(0,m_BackBuffer);
	if ( m_UsedRTs > 0 ) {
		for ( int i = 1; i < m_UsedRTs; ++i ) {
			renderContext.device->SetRenderTarget(i,NULL);
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
		renderContext.device->SetRenderState(rs,value);
	}
}

DWORD Renderer::getRenderState(D3DRENDERSTATETYPE rs) {
	DWORD cachedValue = m_RenderStates[rs];
	if ( cachedValue == -1 ) {
		renderContext.device->GetRenderState(rs,&cachedValue);
	}
	return cachedValue;
}

void Renderer::debug() {	
	for ( int i = 0; i< MAX_BUFFERS; ++i ) {
		if ( m_Buffers[i].used != 0 ) {
			GeometryBuffer* buffer = &m_Buffers[i];		
			LOGC("Renderer") << "GeometryBuffer - size: " << buffer->size << " vertexDefinition: " << buffer->vertexDefinition << " primitive type: " << buffer->primitiveType << " dynamic: " << (buffer->dynamic == 0 ? false : true);
		}
	}
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
		bs->flag = 1;
		LOGC("Renderer") << "created new blendstate - id: " << id;
		return id;
	}
	else {
		LOGEC("Renderer") << "No more free slots for BlendStates available";
		return -1;
	}
}
// -----------------------------------------------------------------
// Change current blend state if necessary
// -----------------------------------------------------------------
void Renderer::changeBlendState(int id) {
	const BlendState& newState = m_BlendStates[id];
	BlendState* current = &m_BlendStates[m_CurrentBS];
	LPDIRECT3DDEVICE9 dev = renderContext.device;
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
	//m_SpriteBatch->setBlendState(id);		
}

// ---------------------------------------------------------------------
// Sets the blend state without checking and update current blend state
// ---------------------------------------------------------------------
void Renderer::setBlendState(int id) {
	if ( id != m_CurrentBS ) {
		//m_SpriteBatch->flush();
		const BlendState& newState = m_BlendStates[id];
		LPDIRECT3DDEVICE9 dev = renderContext.device;
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
	LPDIRECT3DDEVICE9 dev = renderContext.device;
	dev->SetRenderState(D3DRS_CULLMODE, m_RSState.cullMode = rasterizerState->cullMode);
	dev->SetRenderState(D3DRS_FILLMODE, m_RSState.fillMode = rasterizerState->fillMode);
	dev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_RSState.multiSample = rasterizerState->multiSample);
	dev->SetRenderState(D3DRS_SCISSORTESTENABLE, m_RSState.scissor = rasterizerState->scissor);	
}

// -------------------------------------------------------
// Change the current rasterizer state
// -------------------------------------------------------
void Renderer::changeRasterizerState(RasterizerState* rasterizerState) {
	LPDIRECT3DDEVICE9 dev = renderContext.device;
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
// Set the requested texture
// -------------------------------------------------------
void Renderer::setTexture(int textureID,int index) {
	if ( m_CurrentTextures[index] != textureID ) {
		assert(textureID < MAX_TEXTURES);
		TextureAsset* tr = &renderContext.textures[textureID];
		assert ( tr->flags != 0 );
		m_CurrentTextures[index] = textureID;
		IDirect3DDevice9 * pDevice = renderContext.device;
		HR(pDevice->SetTexture( index, tr->texture));	
		HR(pDevice->SetSamplerState(index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
		HR(pDevice->SetSamplerState(index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
		HR(pDevice->SetSamplerState(index, D3DSAMP_MIPFILTER, D3DTEXF_POINT ));
		renderContext.drawCounter.addTexture();
		/*
		if ( m_CurrentShader != -1 ) {
			setTexture(m_CurrentShader,"gTex",textureID);
		}
		*/
		//m_SpriteBatch->setTextureID(textureID);
	}
}

BitmapFont* Renderer::createBitmapFont(const char* name) {
	IdString hash = string::murmur_hash(name);
	for ( int i = 0; i < MAX_FONTS; ++i ) {
		if ( m_BitmapFonts[i].getHashName() == hash ) {
			LOG << "Found already loaded font " << name;
			return &m_BitmapFonts[i];
		}
	}
	LOG << "Created bitmap font " << name;
	BitmapFont* font = &m_BitmapFonts[m_BFCounter];	
	font->setHashName(hash);
	++m_BFCounter;
	return font;	
	//return 0;
}

BitmapFont* Renderer::loadBitmapFont(const char* name,int textureId,const Color& fillColor) {
	IdString hash = string::murmur_hash(name);
	for ( int i = 0; i < MAX_FONTS; ++i ) {
		if ( m_BitmapFonts[i].getHashName() == hash ) {
			LOG << "Found already loaded font " << name;
			return &m_BitmapFonts[i];
		}
	}
	LOG << "Loading bitmap font " << name;
	BitmapFont* font = &m_BitmapFonts[m_BFCounter];	
	
	initializeBitmapFont(*font,textureId,fillColor);
	++m_BFCounter;
	return font;	
	//return 0;
}
// -------------------------------------------------------
// Create a bitmap font
// -------------------------------------------------------
void Renderer::initializeBitmapFont(BitmapFont& bitmapFont,int textureID,const Color& fillColor) {	
	D3DLOCKED_RECT lockedRect;
	assert(textureID < MAX_TEXTURES);
	TextureAsset* texture = &renderContext.textures[textureID];
	HR(texture->texture->LockRect(0,&lockedRect,NULL,0));		
	uint32 x = bitmapFont.getStartX() + bitmapFont.getPadding() - 1;
	uint32 y = bitmapFont.getStartY() + bitmapFont.getPadding();
	uint32 ascii = bitmapFont.getStartChar();
	Color c = getColor(lockedRect,x,y,bitmapFont.getTextureSize());
	bool running = true;
	bool isChar = false;
	int charStartedX = 0;
	int charStartedY = 0;
	int charCount = 0;
	while ( running ) {
		++x;
		if ( x > (bitmapFont.getStartX() + bitmapFont.getWidth()) ) {
			x = bitmapFont.getStartX() + bitmapFont.getPadding() - 1;
			y += bitmapFont.getPadding() + bitmapFont.getGridHeight();// - 1;
			isChar = false;
			charCount = 0;			
		}
		if ( y >= (bitmapFont.getStartY() + bitmapFont.getHeight()) ) {
			running = false;
		}
		if ( y >= texture->height ) {
			running = false;
		}
		if ( running ) {
			c = getColor(lockedRect,x,y,bitmapFont.getTextureSize());
			if ( !isFillColor(fillColor,c) && !isChar ) {
				isChar = true;
				charStartedX = x;
				charStartedY = y;				
			}
			else if ( isFillColor(fillColor,c) && isChar ) {
				isChar = false;
				int width = x - charStartedX - 1;
				++charCount;			
				bitmapFont.addChar(ascii,charStartedX+1,charStartedY,width);
				++ascii;
			}

		}
	}
	HR(texture->texture->UnlockRect(0));	
}

ID3DXFont* Renderer::getSystemFont() {	
	return m_SystemFont;
}
// -------------------------------------------------------
// Loads system font
// -------------------------------------------------------
void Renderer::loadSystemFont(const char* name,const char* fontName,int size,bool bold) {
	LOGC("Renderer") << "Loading font " << fontName << " size " << size;
	UINT type = FW_NORMAL;
	if ( bold ) {
		type = FW_BOLD;
	}
	HDC hDC = GetDC(renderContext.hwnd);
	int nHeight = -MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	HR(D3DXCreateFontA(renderContext.device,nHeight,0,type,0,false,DEFAULT_CHARSET,
		DEFAULT_QUALITY,ANTIALIASED_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE,
		fontName,&m_SystemFont));	
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

uint32 Renderer::startShader(Shader* shader) {
	HR(shader->m_FX->SetTechnique(shader->m_hTech));
	UINT numPasses = 0;
	HR(shader->m_FX->Begin(&numPasses,0));
	return numPasses;
}

void Renderer::endShader(Shader* shader) {
	HR(shader->m_FX->End());	
}
// -------------------------------------------------------
// Apply shader and set some constants
// -------------------------------------------------------
void Renderer::setShaderParameter(Shader* shader,int textureID) {		
	D3DXHANDLE hndl = shader->m_FX->GetParameterByName(0,"gWVP");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getWVPMatrix(),sizeof(mat4));
	}
	hndl = shader->m_FX->GetParameterByName(0,"gWorld");
	if ( hndl != NULL ) {
		shader->m_FX->SetValue(hndl,&getWorldMatrix(),sizeof(mat4));
	}
	if ( textureID != -1 ) {
		hndl = shader->m_FX->GetParameterByName(0,"gTex");
		if ( hndl != NULL ) {
			shader->m_FX->SetTexture(hndl,assets::getDirectTexture(textureID));
		}
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
	shader->m_FX->CommitChanges();
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
			shader->m_FX->SetTexture(sh->handle,assets::getDirectTexture(textureID));
		}
		++sh;
	}	
}

void Renderer::setCurrentShader(int shaderID) {
	m_CurrentShader = shaderID;
	//m_SpriteBatch->setShader(shaderID);
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
		D3DXCreateEffect(renderContext.device, buffer, dwBufferSize,0, 0, SHADER_FLAGS, 0, &shader->m_FX, &errors);
		if ( errors != 0 ) {
			LOGEC("Renderer") << "Error while loading shader: " << (char*)errors->GetBufferPointer();
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
		HRESULT hr = D3DXCreateEffectFromFileA(renderContext.device, fileName,0, 0, SHADER_FLAGS, 0, &shader->m_FX, &errors);
		//if ( hr != S_OK && errors != 0 ) {
		if ( errors != 0 ) {
			LOGEC("Renderer") << "Error while loading shader: " << (char*)errors->GetBufferPointer();
			return -1;
		}
		
		LOGC("Renderer") << "Shader created";
		initializeShader(id,techName);
		return id;
	}
	else {
		LOGEC("Renderer") << "Cannot load shader - No more free slots available";
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
	LOGC("Renderer") << "Got Description - number of parameters: " << nc;
	for ( UINT i = 0; i < effectDesc.Parameters;++i ) {
		D3DXHANDLE hParam = shader->m_FX->GetParameter( NULL, i );
		D3DXPARAMETER_DESC pDesc;
		// get parameter description
		shader->m_FX->GetParameterDesc( hParam, &pDesc );
		LOGC("Renderer") << "Parameter : " << pDesc.Name << " Type: " << pDesc.Type;
		shader->constants[i].handle = hParam;
		shader->constants[i].name = string::murmur_hash(pDesc.Name);
	}
	LOGC("Renderer") << "Shader finally loaded";
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
	vd->create();
	renderContext.vdStructs[VD_TTC].vertexSize = 40;
	renderContext.vdStructs[VD_TTC].declaration = vd;
	// position tangent binormal normal texture coords
	VertexDeclaration* ptbntVD = new VertexDeclaration;
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_TANGENT);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_BINORMAL);
	ptbntVD->addElement(ds::VT_FLOAT3,ds::VDU_NORMAL);
	ptbntVD->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);			
	ptbntVD->create();
	//addVertexDeclaration("PTNBT",ptbntVD);
	renderContext.vdStructs[VD_PTNBT].vertexSize = 56;
	renderContext.vdStructs[VD_PTNBT].declaration = ptbntVD;

	VertexDeclaration* ptcVD = new VertexDeclaration;
	ptcVD->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);	
	ptcVD->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);			
	ptcVD->addElement(ds::VT_FLOAT4,ds::VDU_COLOR);
	ptcVD->create();
	//addVertexDeclaration("PTNBT",ptbntVD);
	renderContext.vdStructs[VD_PTC].vertexSize = 36;
	renderContext.vdStructs[VD_PTC].declaration = ptcVD;

}

// ---------------------------------------------------------
// Creates a render target and returns the attached texture
// ---------------------------------------------------------
int Renderer::createRenderTarget(uint32 id,const Color& clearColor) {
	int tid = assets::findFreeTextureSlot();
	if ( tid != -1 ) {
		RenderTarget* renderTarget = &m_RenderTargets[id];
		assert(renderTarget->flag == 0);
		renderTarget->clearColor = clearColor;
		D3DXCreateRenderToSurface( renderContext.device,
			renderContext.screenWidth, 
			renderContext.screenHeight,
			D3DFMT_A8R8G8B8,
			false,
			D3DFMT_UNKNOWN ,
			&renderTarget->rts);

		renderContext.device->CreateTexture(renderContext.screenWidth,
			renderContext.screenHeight,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&renderTarget->texture,
			NULL);
		LOGC("Renderer") << "Rendertarget created - id: " << id << " texture id: " << tid << " width: " << renderContext.screenWidth << " height: " << renderContext.screenHeight;
		TextureAsset* t = &renderContext.textures[tid];
		t->height = renderContext.screenHeight;
		t->width = renderContext.screenWidth;
		t->flags = 1;
		t->texture = renderTarget->texture;
		renderTarget->texture->GetSurfaceLevel(0,&renderTarget->surface);		
		renderTarget->flag = 1;
		return tid;
	}
	else {
		LOGEC("Renderer") << "Cannot create rendertarget - No more texture slots available";
		return -1;
	}	
}

// ---------------------------------------------------------
// Creates a render target and returns the attached texture
// ---------------------------------------------------------
int Renderer::createRenderTarget(uint32 id,float width,float height,const Color& clearColor) {
	int tid = assets::findFreeTextureSlot();
	if ( tid != -1 ) {
		RenderTarget* renderTarget = &m_RenderTargets[id];
		assert(renderTarget->flag == 0);
		renderTarget->clearColor = clearColor;
		D3DXCreateRenderToSurface( renderContext.device,
			width, 
			height,
			D3DFMT_A8R8G8B8,
			false,
			D3DFMT_UNKNOWN ,
			&renderTarget->rts);

		renderContext.device->CreateTexture(width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&renderTarget->texture,
			NULL);
		LOGC("Renderer") << "Rendertarget created - id: " << id << " texture id: " << tid << " width: " << width << " height: " <<height;
		TextureAsset* t = &renderContext.textures[tid];
		t->height = height;
		t->width = width;
		t->flags = 1;
		t->texture = renderTarget->texture;
		renderTarget->texture->GetSurfaceLevel(0,&renderTarget->surface);		
		renderTarget->flag = 1;
		return tid;
	}
	else {
		LOGEC("Renderer") << "Cannot create rendertarget - No more texture slots available";
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
		VDStruct vd = renderer::getVertexDeclaration(vertexDefinition);
		int vertexSize = vd.vertexSize;	
		D3DPOOL pool = dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
		DWORD usage = dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
		LOGC("Renderer") << "creating new vertex buffer - size: " << size << " vertexDefinition: " << vertexDefinition << " vertexSize: " << vertexSize << " dynamic: " << (dynamic == 0 ? false : true);
		HR(renderContext.device->CreateVertexBuffer( size * vertexSize,usage,0 ,pool, &m_Buffers[id].vertexBuffer, NULL ))
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
		HR(renderContext.device->CreateIndexBuffer( size * sizeof(WORD),usage,D3DFMT_INDEX16,pool,&m_Buffers[id].indexBuffer,NULL));	
		LOGC("Renderer") << "new IndexBuffer created size: " << size << " dynamic: " << (dynamic == 0 ? false : true);
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
		HR(renderContext.device->CreateIndexBuffer( indices * sizeof(WORD),usage,D3DFMT_INDEX16,pool,&m_Buffers[id].indexBuffer,NULL));	
		LOGC("Renderer") << "new IndexBuffer created size: " << indices << " dynamic: " << (dynamic == 0 ? false : true);
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
		LOGC("Renderer") << "buffer handle created - id: " << id << " bufferType: " << bufferType << " vertexDefinition: " << vertexDefinition << " dynamic: " << dynamic;
	}
	return id;
}

// ---------------------------------------------------------
// Unlock buffers
// ---------------------------------------------------------
void Renderer::resetBuffer(int handleID) {
	assert(handleID < MAX_BUFFER_HANDLES);
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	m_Buffers[handle->iBufferRef.bufferIdx].size = 0;		
	m_Buffers[handle->vBufferRef.bufferIdx].size = 0;			
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
			//LOG << "found matching buffer - id: " << i;
			uint32 current = gb->size;
			if ( (current + size) < gb->maxSize ) {
				start = gb->size;
				gb->size += size;
				//LOG << "setting start " << start << " new size is " << gb->size;
				return i;
			}
		}
	}
	LOGEC("Renderer") << "Cannot find buffer with enough space";
	LOGEC("Renderer") << "Requested - type: " << type << " vertex definition: " << vertexDefinition << " size: " << size << " dynamic: " << dynamic;
	for ( int i = 0; i < MAX_BUFFERS; ++i ) {
		GeometryBuffer* gb = &m_Buffers[i];
		if ( gb->used != 0 ) {
			int free = gb->maxSize - gb->size;
			LOGEC("Renderer") << "Buffer " << i << " buffer type: " << gb->type << " size: " << gb->size << " free: " << free << " max: " << gb->maxSize << " prim type: " << gb->primitiveType << " dynamic: " << gb->dynamic;
		}
	}
	return -1;
}

// ---------------------------------------------------------
// Lock buffers
// ---------------------------------------------------------
void Renderer::lockBuffer(int handleID,int vertexCount,int indexCount,float** vertexBuffer,void** indexBuffer) {
	//LOG << " --- Lock Buffer ----";
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
			//LOG << "IndexBuffer - start " << handle->iBufferRef.start << " count " << indexCount;
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
			//LOG << "VertexBuffer - idx: " << handle->vBufferRef.bufferIdx << " start " << handle->vBufferRef.start << " count " << vertexCount;
		//}
		assert(handle->vBufferRef.bufferIdx != -1 );
		DWORD flag = 0;
		if ( m_Buffers[handle->vBufferRef.bufferIdx].dynamic ) {
			flag = D3DLOCK_DISCARD;
		}
		int vDef = m_Buffers[handle->vBufferRef.bufferIdx].vertexDefinition;
		int vSize = renderContext.vdStructs[vDef].vertexSize;
		IDirect3DVertexBuffer9 *VB = m_Buffers[handle->vBufferRef.bufferIdx].vertexBuffer;
		assert(VB != 0);	
		HR(VB->Lock( handle->vBufferRef.start *vSize, vertexCount * vSize, ( void** )vertexBuffer, flag ));
	}
}

void Renderer::lockIndexBuffer(int handleID, int indexCount, void** indexBuffer) {
	//LOG << " --- Lock Buffer ----";
	assert(handleID < MAX_BUFFER_HANDLES);
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	handle->locked = 1;
	int ret = -1;
	if (handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH) {
		handle->iBufferRef.start = 0;
		handle->iBufferRef.count = indexCount;
		//if ( handle->iBufferRef.bufferIdx == -1 ) {
		handle->iBufferRef.bufferIdx = allocateBuffer(GBT_INDEX, -1, indexCount, handle->iBufferRef.start, handle->dynamic);
		//LOG << "IndexBuffer - start " << handle->iBufferRef.start << " count " << indexCount;
		//}
		assert(handle->iBufferRef.bufferIdx != -1);
		DWORD flag = 0;
		if (m_Buffers[handle->iBufferRef.bufferIdx].dynamic) {
			flag = D3DLOCK_DISCARD;
		}
		IDirect3DIndexBuffer9 *IB = m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer;
		assert(IB != 0);
		HR(IB->Lock(handle->iBufferRef.start * sizeof(WORD), indexCount * sizeof(WORD), (void**)indexBuffer, flag));
	}	
}

// ---------------------------------------------------------
// Unlock buffers
// ---------------------------------------------------------
void Renderer::unlockIndexBuffer(int handleID) {
	assert(handleID < MAX_BUFFER_HANDLES);
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	if (handle->locked == 1) {
		if (handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH) {
			HR(m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer->Unlock());
		}		
		handle->locked = 0;
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
int Renderer::drawBuffer(int handleID,int textureID) {
	PR_START("Renderer::drawBuffer")
	GeoBufferHandle* handle = &m_BufferHandles[handleID];
	assert(handle->used != 0);
	int vDef = m_Buffers[handle->vBufferRef.bufferIdx].vertexDefinition;
	assert( vDef >= 0 );
	//LOG << "vDef " << vDef << " current " << m_CurrentVD;
	if ( m_CurrentVD != vDef ) {
		m_CurrentVD = vDef;
		renderContext.device->SetVertexDeclaration(renderContext.vdStructs[vDef].declaration->get());
	}
	if ( handle->bufferType == GBT_INDEX || handle->bufferType == GBT_BOTH ) {
		if ( handle->iBufferRef.bufferIdx != m_CurrentIB ) {
			m_CurrentIB = handle->iBufferRef.bufferIdx;
			HR(renderContext.device->SetIndices( m_Buffers[handle->iBufferRef.bufferIdx].indexBuffer)); 
		}
	}
	if ( handle->bufferType == GBT_VERTEX || handle->bufferType == GBT_BOTH ) {
		if ( handle->vBufferRef.bufferIdx != m_CurrentVB ) {
			m_CurrentVB = handle->vBufferRef.bufferIdx;
			HR(renderContext.device->SetStreamSource(0, m_Buffers[handle->vBufferRef.bufferIdx].vertexBuffer, 0, renderContext.vdStructs[m_CurrentVD].vertexSize));
		}
	}	
	D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
	int numPrimitives = handle->vBufferRef.count / 2;
	renderContext.drawCounter.addDrawCall();
	renderContext.drawCounter.addPrimitives(handle->vBufferRef.count);
	renderContext.drawCounter.addIndices(handle->iBufferRef.count);
	if ( m_CurrentShader != -1 ) {
		Shader* shader = &m_Shaders[m_CurrentShader];
		renderContext.drawCounter.addShader();
		uint32 numPasses = startShader(shader);
		//applyShader(shader,textureID);
		//shader->m_FX->CommitChanges();
		// update parameters
		//UINT numPasses = 0;
		//HR(shader->m_FX->Begin(&numPasses,0));
		for ( UINT p = 0; p < numPasses; ++p ) {		
			HR(shader->m_FX->BeginPass(p));			
			setShaderParameter(shader);
			HR(renderContext.device->DrawIndexedPrimitive( pt, handle->vBufferRef.start, 0, handle->vBufferRef.count, handle->iBufferRef.start, numPrimitives ));
			HR(shader->m_FX->EndPass());
		}
		//HR(shader->m_FX->End());	
		endShader(shader);
	}
	else {
		HR(renderContext.device->DrawIndexedPrimitive( pt, handle->vBufferRef.start, 0, handle->vBufferRef.count, handle->iBufferRef.start, numPrimitives ));
	}
	PR_END("Renderer::drawBuffer")
	return handle->iBufferRef.start;
}

// -------------------------------------------------------
// Draw debug messages
// -------------------------------------------------------
void Renderer::drawDebugMessages() {
	if ( m_DebugMessages.num() > 0 ) {
		IDirect3DDevice9 * pDevice = renderContext.device;
		ID3DXFont *font = getSystemFont();
		if ( font != 0 ) {
			RECT font_rect;	
			m_DebugSprite->Begin( D3DXSPRITE_ALPHABLEND );
			for ( size_t i = 0; i < m_DebugMessages.num(); ++i ) {	
				DebugMessage* message = &m_DebugMessages[i];
				SetRect(&font_rect,message->x,message->y,message->x+200,message->y+60);		
				font->DrawTextA(m_DebugSprite,message->message.c_str(),-1,&font_rect,DT_LEFT|DT_NOCLIP,message->color);		
			}
			m_DebugSprite->End();
		}	
	}
}

void Renderer::debug(int x,int y,char* format,...) {
	va_list args;
	va_start(args,format);
	debug(x,y,Color(1.0f,1.0f,1.0f,1.0f),format,args);
	va_end(args);
}

void Renderer::debug(int x,int y,char* format,va_list args) {
	debug(x,y,Color(1.0f,1.0f,1.0f,1.0f),format,args);
}

void Renderer::debug(int x,int y,const Color& color,char* format,...) {
	va_list args;
	va_start(args,format);
	debug(x,y,color,format,args);
	va_end(args);
}

void Renderer::debug(int x,int y,const Color& color,char* format,va_list args) {
	DebugMessage message;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	int written = vsnprintf_s(buffer,sizeof(buffer),_TRUNCATE,format,args);		
	message.message = std::string(buffer);
	message.x = x;
	message.y = y;
	message.color = color;
	m_DebugMessages.append(message);
}

void Renderer::debug(int x,int y,const char* text) {
	DebugMessage message;
	message.message = std::string(text);
	message.x = x;
	message.y = y;
	message.color = Color(1.0f,1.0f,1.0f,1.0f);
	m_DebugMessages.append(message);
}

void Renderer::showProfiler(int x,int y) {
	//gProfiler->show(x,y,this);
}

void Renderer::showDrawCounter(int x,int y) {
	int ty = y;
	debug(x,ty,ds::Color(1.0f,1.0f,1.0f,1.0f),"DrawCounter IDX: %d Vertices: %d Sprites: %d Flushes: %d",renderContext.drawCounter.getIndexCounter(),renderContext.drawCounter.getPrimitiveCounter(),renderContext.drawCounter.getSpriteCounter(),renderContext.drawCounter.getFlushes());
	ty += 20;
	debug(x,ty,ds::Color(1.0f,1.0f,1.0f,1.0f),"DrawCalls: %d Textures: %d Shaders: %d",renderContext.drawCounter.getDrawCalls(),renderContext.drawCounter.getTextures(),renderContext.drawCounter.getShaders());
}

void Renderer::printDrawCounter() {
	LOG << "DrawCounter";
	LOG << "-------------------------------------------";
	LOG << "Indices:   " << renderContext.drawCounter.getIndexCounter();
	LOG << "Vertices:  " << renderContext.drawCounter.getPrimitiveCounter();
	LOG << "Sprites:   " << renderContext.drawCounter.getSpriteCounter();
	LOG << "Flushes:   " << renderContext.drawCounter.getFlushes();
	LOG << "DrawCalls: " << renderContext.drawCounter.getDrawCalls();
	LOG << "Textures:  " << renderContext.drawCounter.getTextures();
	LOG << "Shaders:   " << renderContext.drawCounter.getShaders();
}

};

