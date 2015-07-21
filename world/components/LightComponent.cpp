#include "LightComponent.h"
#include "..\..\renderer\shader.h"

namespace ds {

const uint32 INDICES[] = {0,1,2,2,3,0};

LightComponent::LightComponent(void) : GameObject() {
}


LightComponent::~LightComponent(void) {
}

// -------------------------------------------------------
// Initialize
// -------------------------------------------------------
void LightComponent::initialize(int startLayer,int colorRT,int normalRT,float ambient,const Color& ambientColor) {
	m_ScreenW = ds::renderer::getSelectedViewport().getWidth();
	m_ScreenH = ds::renderer::getSelectedViewport().getHeight();
	float centerX = m_ScreenW * 0.5f;
	float centerY = m_ScreenH * 0.5f;

	PTCVertex VERTICES[] = {
		PTCVertex(Vector3f(-centerX,centerY,0.0f),Vector2f(0.0f,0.0f)),
		PTCVertex(Vector3f(centerX,centerY,0.0f),Vector2f(1.0f,0.0f)),
		PTCVertex(Vector3f(centerX,-centerY,0.0f),Vector2f(1.0f,1.0f)),
		PTCVertex(Vector3f(-centerX,-centerY,0.0f),Vector2f(0.0f,1.0f))
	};

	m_AddBS = renderer::createBlendState("alpha_blend",BL_ONE,BL_ONE,true);	
/*
	int layer = startLayer;
	int lightRT = m_World->createRenderTarget(layer,m_ScreenW,m_ScreenH);
	m_World->add(layer,this);
	m_Renderer->createVertexBuffer(PT_TRI,VD_PTC,20,false);
	m_Renderer->createIndexBuffer(20,false);
	m_Handle = m_Renderer->createBufferHandle(PT_TRI,VD_PTC,GBT_BOTH,false);
	WORD* ib;
	PTCVertex* vb;
	int vc = 4;
	int ic = 6;
	m_Renderer->lockBuffer(m_Handle,vc,ic,(float**)&vb,(void**)&ib);
	for ( uint32 i = 0; i < 4;++i ) {
		*vb = VERTICES[i];
		++vb;
	}		
	uint32 indexCounter = 6;
	for ( uint32 i = 0; i < indexCounter; ++i ) {
		*ib = INDICES[i];
		++ib;
	}
	m_Renderer->unlockBuffer(m_Handle);		
	m_ShaderID = createDeferredShader();
	m_LightShader = m_Renderer->getShader(m_ShaderID);
	shader::setFloat(m_LightShader,"screenWidth",m_ScreenW);
	shader::setFloat(m_LightShader,"screenHeight",m_ScreenH);
	shader::setFloat(m_LightShader,"lightStrength",1.0f);
	shader::setFloat(m_LightShader,"lightDecay",200.0f);
	shader::setVector3f(m_LightShader,"lightPosition",Vector3f(512,384,100));
	shader::setColor(m_LightShader,"lightColor",Color(255,0,0,255));
	shader::setFloat(m_LightShader,"specularStrength",2.0f);
	shader::setTexture(m_LightShader,"NormalMap",m_Renderer,normalRT);
	shader::setTexture(m_LightShader,"ColorMap",m_Renderer,colorRT);		
	
	++layer;
	int combinedRT = m_World->createRenderTarget(layer,m_ScreenW,m_ScreenH);
	int combineShaderID = createCombineShader();
	Shader combineShader = m_Renderer->getShader(combineShaderID);
	shader::setFloat(combineShader,"ambient",ambient);
	shader::setColor(combineShader,"ambientColor",ambientColor);
	shader::setFloat(combineShader,"lightAmbient",1.0f);			
	shader::setTexture(combineShader,"ShadingMap",m_Renderer,lightRT);
	shader::setTexture(combineShader,"ColorMap",m_Renderer,colorRT);
	shader::setTexture(combineShader,"NormalMap",m_Renderer,normalRT);
	int cbid = m_World->createSpriteBatch(combinedRT,m_ScreenW,m_ScreenH);
	m_World->setSpriteBatchShader(cbid,combineShaderID);
	//m_World->addSpriteEntity(3,cbid,&m_CombineEntity,centerX,centerY,Rect(0.0f,0.0f,m_ScreenW,m_ScreenH));

	++layer;
	int bbID = m_World->createSpriteBatch(combinedRT,m_ScreenW,m_ScreenH);
	//m_World->addSpriteEntity(layer,bbID,&m_OverlayEntity,centerX,centerY,Rect(0.0f,0.0f,m_ScreenW,m_ScreenH));
	*/
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void LightComponent::update(float elapsed) {
}

// -------------------------------------------------------
// Add light
// -------------------------------------------------------
int LightComponent::addLight(const Vector3f& position,float radius,const Color& color,float specular,float strength) {
	int idx = m_Lights.size();
	Light l;
	l.position = position;
	l.radius = radius;
	l.color = color;
	l.specular = specular;
	l.strength = strength;
	l.active = true;
	m_Lights.push_back(l);
	return idx;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void LightComponent::draw() {
	renderer::setCurrentShader(m_ShaderID);
	int current = renderer::getCurrentBlendState();
	renderer::setBlendState(m_AddBS);
	for ( size_t i = 0; i < m_Lights.size(); ++i ) {
		Light* l = &m_Lights[i];
		if ( l->active ) {
			Vector3f pos = l->position;
			pos.y = m_ScreenH - pos.y;
			shader::setVector3f(m_LightShader,"lightPosition",pos);
			shader::setColor(m_LightShader,"lightColor",l->color);
			shader::setFloat(m_LightShader,"lightDecay",l->radius);
			shader::setFloat(m_LightShader,"specularStrength",l->specular);
			shader::setFloat(m_LightShader,"lightStrength",l->strength);
			//m_Renderer->drawBuffer(m_Handle,-1);
		}
	}
	renderer::setBlendState(current);
}

// -------------------------------------------------------
// Deferred Shader
// -------------------------------------------------------
int LightComponent::createDeferredShader() {
	const char* g_strBuffer = 
		"uniform extern float4x4 gWVP;\r\n"
		"uniform extern float screenWidth;\r\n"
	"uniform extern float screenHeight;\r\n"
	"uniform extern float lightStrength;\r\n"
	"uniform extern float lightDecay;\r\n"
	"uniform extern float3 lightPosition;\r\n"
	"uniform extern float4 lightColor;\r\n"
	"uniform extern float specularStrength;\r\n"
	"uniform extern Texture NormalMap;\r\n"
	"sampler NormalMapSampler = sampler_state {\r\n"
	"	texture = <NormalMap>;\r\n"
	"	magfilter = LINEAR;\r\n"
	"	minfilter = LINEAR;\r\n"
	"	mipfilter = LINEAR;\r\n"
	"	AddressU = mirror;\r\n"
	"	AddressV = mirror;\r\n"
	"};\r\n"
	"uniform extern Texture ColorMap;\r\n"
	"sampler ColorMapSampler = sampler_state {\r\n"
	"	texture = <ColorMap>;\r\n"
	"	magfilter = LINEAR;\r\n"
	"	minfilter = LINEAR;\r\n"
	"	mipfilter = LINEAR;\r\n"
	"	AddressU = mirror;\r\n"
	"	AddressV = mirror;\r\n"
	"};\r\n"
	"struct VertexToPixel {\r\n"
	"	float4 Position : POSITION;\r\n"
	"	float2 TexCoord : TEXCOORD0;\r\n"
	"	float4 Color : COLOR0;\r\n"
	"};\r\n"
	"struct PixelToFrame {\r\n"
	"	float4 Color : COLOR0;\r\n"
	"};\r\n"
	"VertexToPixel MyVertexShader(float3 inPos: POSITION0, float2 texCoord: TEXCOORD0, float4 color: COLOR0)\r\n"
	"{\r\n"
	"	VertexToPixel outVS = (VertexToPixel)0; \r\n"
	"	outVS.Position = mul(float4(inPos, 1.0f), gWVP);   \r\n"
	"	outVS.TexCoord = texCoord;\r\n"
	"	outVS.Color = color;\r\n"
	"	return outVS;\r\n"
	"}\r\n"
	"float4 PointLightShader(VertexToPixel PSIn) : COLOR0 {	\r\n"
	"	float4 color;\r\n"
	"	float4 colorMap = tex2D(ColorMapSampler, PSIn.TexCoord);\r\n"
	"	float3 normal = (2.0f * (tex2D(NormalMapSampler, PSIn.TexCoord))) - 1.0f;\r\n"
	"	float3 pixelPosition;\r\n"
	"	pixelPosition.x = screenWidth * PSIn.TexCoord.x;\r\n"
	"	pixelPosition.y = screenHeight * PSIn.TexCoord.y;\r\n"
	"	pixelPosition.z = 0;\r\n"
	"	float3 lightDirection = lightPosition - pixelPosition;\r\n"
	"	float3 lightDirNorm = normalize(lightDirection);\r\n"
	"	float3 halfVec = float3(0, 0, 1);\r\n"
	"	float amount = max(dot(normal, lightDirNorm), 0);\r\n"
	"	float coneAttenuation = saturate(1.0f - length(lightDirection) / lightDecay); \r\n"
	"	float3 reflect = normalize(2 * amount * normal - lightDirNorm);\r\n"
	"	float specular = min(pow(saturate(dot(reflect, halfVec)), 10), amount);\r\n"
	"	color = colorMap * coneAttenuation * lightColor * lightStrength + (specular * coneAttenuation * specularStrength);\r\n"
	"	return color;\r\n"
	"}\r\n"
	"technique DeferredPointLight {\r\n"
	"	pass Pass1 {\r\n"
	"		VertexShader = compile vs_2_0 MyVertexShader();\r\n"
	"		PixelShader = compile ps_2_0 PointLightShader();\r\n"
	"	}\r\n"
	"}\r\n";
	int ret = renderer::createShaderFromText(g_strBuffer, "DeferredPointLight");
	return ret;
}

// -------------------------------------------------------
// Combine shader
// -------------------------------------------------------
int LightComponent::createCombineShader() {
	const char* g_strBuffer = 
		"uniform extern float4x4 gWVP;\r\n"
		"uniform extern float ambient;\r\n"
		"uniform extern float4 ambientColor;\r\n"
		"uniform extern float lightAmbient;\r\n"
		"uniform extern Texture ColorMap;\r\n"
		"sampler ColorMapSampler = sampler_state {\r\n"
		"	texture = <ColorMap>;\r\n"
		"	magfilter = LINEAR;\r\n"
		"	minfilter = LINEAR;\r\n"
		"	mipfilter = LINEAR;\r\n"
		"	AddressU = mirror;\r\n"
		"	AddressV = mirror;\r\n"
		"};\r\n"
		"uniform extern Texture ShadingMap;\r\n"
		"sampler ShadingMapSampler = sampler_state {\r\n"
		"	texture = <ShadingMap>;\r\n"
		"	magfilter = LINEAR;\r\n"
		"	minfilter = LINEAR;\r\n"
		"	mipfilter = LINEAR;\r\n"
		"	AddressU = mirror;\r\n"
		"	AddressV = mirror;\r\n"
		"};\r\n"
		"uniform extern Texture NormalMap;\r\n"
		"sampler NormalMapSampler = sampler_state {\r\n"
		"	texture = <NormalMap>;\r\n"
		"	magfilter = LINEAR;\r\n"
		"	minfilter = LINEAR;\r\n"
		"	mipfilter = LINEAR;\r\n"
		"	AddressU = mirror;\r\n"
		"	AddressV = mirror;\r\n"
		"};\r\n"
		"struct VertexToPixel {\r\n"
		"	float4 Position : POSITION;\r\n"
		"	float2 TexCoord : TEXCOORD0;\r\n"
		"	float4 Color : COLOR0;\r\n"
		"};\r\n"
		"VertexToPixel MyVertexShader(float3 inPos: POSITION0, float2 texCoord: TEXCOORD0, float4 color: COLOR0) {\r\n"
		"	VertexToPixel outVS = (VertexToPixel)0; \r\n"
		"	outVS.Position = mul(float4(inPos, 1.0f), gWVP);   \r\n"
		"	outVS.TexCoord = texCoord;\r\n"
		"	outVS.Color = color;\r\n"
		"	return outVS;\r\n"
		"}\r\n"
		"float4 CombinedPixelShader(VertexToPixel PSIn) : COLOR0 {	\r\n"
		"	float4 color2 = tex2D(ColorMapSampler, PSIn.TexCoord);\r\n"
		"	float4 shading = tex2D(ShadingMapSampler, PSIn.TexCoord);\r\n"
		"	float normal = tex2D(NormalMapSampler, PSIn.TexCoord).rgb;\r\n"
		"	if (normal > 0.0f) {\r\n"
		"		float4 finalColor = color2 * ambientColor * ambient;\r\n"
		"		finalColor += (shading * color2) * lightAmbient;\r\n"
		"		return finalColor;\r\n"
		"	}\r\n"
		"	else {\r\n"
		"		return float4(0, 0, 0, 0);\r\n"
		"	}\r\n"
		"}\r\n"
		"technique DeferredCombined2 {\r\n"
		"	pass Pass1 {\r\n"
		"		VertexShader = compile vs_2_0 MyVertexShader();\r\n"
		"		PixelShader = compile ps_2_0 CombinedPixelShader();\r\n"
		"	}\r\n"
		"}\r\n";
	int ret = renderer::createShaderFromText(g_strBuffer, "DeferredCombined2");
	return ret;
}

}