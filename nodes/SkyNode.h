#pragma once
#include "Node.h"
#include <string>

namespace ds {

class SkyNode : public Node {

public:
	SkyNode(Renderer* renderer,int material,const std::string& envmapFilename, float skyRadius);
	~SkyNode();

	IDirect3DCubeTexture9* getEnvMap();
	float getRadius();

	DWORD getNumTriangles();
	DWORD getNumVertices();

	void onLostDevice();
	void onResetDevice();

	void draw();

private:
	void createShader();
	ID3DXMesh* mSphere;
	float mRadius;
	IDirect3DCubeTexture9* mEnvMap;
	ID3DXEffect* mFX;
	D3DXHANDLE mhTech;
	D3DXHANDLE mhEnvMap;
	D3DXHANDLE mhWVP;
};

}
