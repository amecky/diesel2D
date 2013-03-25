#include "SkyNode.h"
#include "..\renderer\Camera.h"


namespace ds {

SkyNode::SkyNode(Renderer* renderer,int material,const std::string& envmapFilename, float skyRadius) : Node("SkyNode",renderer,material) , mRadius(skyRadius) {

	//HR(D3DXCreateSphere(gEngine->getDevice(), skyRadius, 30, 30, &mSphere, 0));
	//HR(D3DXCreateCubeTextureFromFile(gEngine->getDevice(), envmapFilename.c_str(), &mEnvMap));
	/*
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gEngine->getDevice(), "sky.fx", 0, 0, 0, 
		0, &mFX, &errors));
	if( errors )
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);
	*/
	createShader();
	mhTech   = mFX->GetTechniqueByName("SkyTech");
	mhWVP    = mFX->GetParameterByName(0, "gWVP");
	mhEnvMap = mFX->GetParameterByName(0, "gEnvMap");

	// Set effect parameters that do not vary.
	mFX->SetTechnique(mhTech);
	mFX->SetTexture(mhEnvMap, mEnvMap);
}

void SkyNode::createShader() {
	ID3DXBuffer* errors = 0;
	const char* g_strBuffer = 
		"uniform extern float4x4 gWVP;\r\n"
		"uniform extern texture  gEnvMap;\r\n"
		"\r\n"
		"	sampler EnvMapS = sampler_state\r\n"
		"	{\r\n"
		"		Texture   = <gEnvMap>;\r\n"
		"		MinFilter = LINEAR; \r\n"
		"		MagFilter = LINEAR;\r\n"
		"		MipFilter = LINEAR;\r\n"
		"		AddressU  = WRAP;\r\n"
		"		AddressV  = WRAP;\r\n"
		"	};\r\n"
		"	\r\n"
		"	void SkyVS(float3 posL : POSITION0, out float4 oPosH : POSITION0, out float3 oEnvTex : TEXCOORD0) {\r\n"
		"		// Set z = w so that z/w = 1 (i.e., skydome always on far plane).\r\n"
		"		oPosH = mul(float4(posL, 1.0f), gWVP).xyww; \r\n"
		"		// Use skymesh vertex position, in local space, as index into cubemap. \r\n"
		"		oEnvTex = posL;\r\n"
		"	}\r\n"
		"	\r\n"
		"	float4 SkyPS(float3 envTex : TEXCOORD0) : COLOR {\r\n"
		"		return texCUBE(EnvMapS, envTex);\r\n"
		"	}\r\n"
		"	\r\n"
		"	technique SkyTech {\r\n"
		"		pass P0\r\n"
		"		{\r\n"
		"			vertexShader = compile vs_2_0 SkyVS();\r\n"
		"			pixelShader  = compile ps_2_0 SkyPS();\r\n"
		"			CullMode = None;\r\n"
		"			ZFunc = Always; // Always write sky to depth buffer\r\n"
		"			StencilEnable = true;\r\n"
		"			StencilFunc   = Always;\r\n"
		"			StencilPass   = Replace;\r\n"
		"			StencilRef    = 0; // clear to zero\r\n"
		"		}\r\n"
		"	}\r\n"
		"	\r\n";
	UINT dwBufferSize = ( UINT )strlen( g_strBuffer ) + 1;
	//HR(D3DXCreateEffect(gEngine->getDevice(), g_strBuffer, dwBufferSize,0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
}

SkyNode::~SkyNode() {
	SAFE_DELETE(mSphere);
	SAFE_DELETE(mEnvMap);
	SAFE_DELETE(mFX);
}

DWORD SkyNode::getNumTriangles() {
	return mSphere->GetNumFaces();
}

DWORD SkyNode::getNumVertices() {
	return mSphere->GetNumVertices();
}

IDirect3DCubeTexture9* SkyNode::getEnvMap() {
	return mEnvMap;
}

float SkyNode::getRadius() {
	return mRadius;
}

void SkyNode::onLostDevice() {
	mFX->OnLostDevice();
}

void SkyNode::onResetDevice() {
	mFX->OnResetDevice();
}

void SkyNode::draw() {
	// Sky always centered about camera's position.
	/*
	D3DXMATRIX W;
	D3DXVECTOR3 p = gEngine->getRenderer()->getCamera()->getPosition();
	D3DXMatrixTranslation(&W, p.x, p.y, p.z);
	HR(mFX->SetMatrix(mhWVP, &(W*gEngine->getRenderer()->getCamera()->getViewProjectionMatrix())));
	
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	HR(mFX->BeginPass(0));
	HR(mSphere->DrawSubset(0));
	*/
#ifdef DEBUG
	//gEngine->getDrawCounter()->addDrawCall();
	//gEngine->getDrawCounter()->addPrimitives(getNumVertices());
#endif
	mFX->EndPass();
	mFX->End();
}

}