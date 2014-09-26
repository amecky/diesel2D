#pragma once

#include <vector>
#include "..\utils\Color.h"
#include "GraphicsDevice.h"
#include "..\math\math_types.h"
#include "Vector.h"

namespace ds {

enum VDUSAGE {
	VDU_POSITION,VDU_NORMAL,VDU_COLOR,VDU_TEXCOORD,VDU_POSITIONT,VDU_BINORMAL,VDU_TANGENT
};

enum VDTYPE {
	VT_FLOAT4,VT_FLOAT3,VT_COLOR,VT_FLOAT2
};

class VertexDeclaration {

struct VDElement {
	VDTYPE type;
	VDUSAGE usage;
};

typedef std::vector<VDElement> VDElements;

public:
	VertexDeclaration(const char* handle);
	VertexDeclaration();
	~VertexDeclaration(void);
	void addElement(const VDTYPE& type,const VDUSAGE& usage);
	void create();
	void clear();
	IDirect3DVertexDeclaration9* get() { return m_Decl; }
	int getSize();
private:	
	IDirect3DVertexDeclaration9* m_Decl;
	VDElements m_DecElements;
	int m_Size;
};

// -----------------------------------------------
//
// -----------------------------------------------
class TexturedVertex {

public:

	float x,y,z;
	float tu,tv;

	TexturedVertex() {
		x = y = z = 0.0f;
		tu = tv = 0.0f;
	}

	TexturedVertex(float xp,float yp,float zp,float u,float v) {
		x = xp;
		y = yp;
		z = zp;
		tu = u;
		tv = v;
	}

	TexturedVertex(const Vector3f& pos,const Vector2f& texcoords) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		tu = texcoords.x;
		tv = texcoords.y;
	}
	/*
	static VertexDeclaration* getVertexDeclaration() {
		if ( !gEngine->getResourceManager().contains("TexturedVertex")) {
			ResourceHandle handle = gEngine->getResourceManager().createVertexDeclaration("TexturedVertex");
			VertexDeclaration* vd = gEngine->getResourceManager().getVertexDeclaration(handle);
			vd->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);
			vd->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);
			vd->create();
			return vd;
		}
		else {
			return gEngine->getResourceManager().getVertexDeclaration("TexturedVertex");
		}
	}
	*/
};

// -----------------------------------------------
//
// -----------------------------------------------
/*
class TransformedTexturedVertex {

public:

	float x,y,z,rwh;
	float tu,tv;

	TransformedTexturedVertex() {
		x = y = z = 0.0f;
		rwh = 1.0f;
		tu = tv = 0.0f;
	}

	TransformedTexturedVertex(float xp,float yp,float zp,float u,float v) {
		x = xp;
		y = yp;
		z = zp;
		rwh = 1.0f;
		tu = u;
		tv = v;
	}

	TransformedTexturedVertex(const Vector3f& pos,const Vector2f& texcoords) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		rwh = 1.0f;
		tu = texcoords.x;
		tv = texcoords.y;
	}

	static VertexDeclaration* getVertexDeclaration() {
		if ( !gEngine->getResourceManager().contains("TransformedTexturedVertex")) {
			ResourceHandle handle = gEngine->getResourceManager().createVertexDeclaration("TransformedTexturedVertex");
			VertexDeclaration* vd = gEngine->getResourceManager().getVertexDeclaration(handle);
			vd->addElement(ds::VT_FLOAT4,ds::VDU_POSITIONT);
			vd->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);
			vd->create();
			return vd;
		}
		else {
			return gEngine->getResourceManager().getVertexDeclaration("TransformedTexturedVertex");
		}
	}

};
*/

// -----------------------------------------------
//
// -----------------------------------------------
class TransformedColorVertex {

public:

	float x,y,z,rwh;
	Color color;

	TransformedColorVertex() {
		x = y = z = 0.0f;
		rwh = 1.0f;
		color = Color(1.0f,1.0f,1.0f,1.0f);
	}

	TransformedColorVertex(float xp,float yp,float zp,const Color& color) {
		x = xp;
		y = yp;
		z = zp;
		rwh = 1.0f;
		this->color = color;
	}

	TransformedColorVertex(const Vector3f& pos,const Color& color) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		rwh = 1.0f;
		this->color = color;
	}
	/*
	static VertexDeclaration* getVertexDeclaration() {
		if ( !gEngine->getResourceManager().contains("TransformedColorVertex")) {
			ResourceHandle handle = gEngine->getResourceManager().createVertexDeclaration("TransformedColorVertex");
			VertexDeclaration* vd = gEngine->getResourceManager().getVertexDeclaration(handle);
			vd->addElement(ds::VT_FLOAT4,ds::VDU_POSITIONT);
			vd->addElement(ds::VT_FLOAT4,ds::VDU_COLOR);
			vd->create();
			return vd;
		}
		else {
			return gEngine->getResourceManager().getVertexDeclaration("TransformedColorVertex");
		}
	}
	*/
};


class PositionTextureNormalsVertex {

public:

	float x,y,z;
	float tu,tv;
	float nx,ny,nz;
	Color color;

	PositionTextureNormalsVertex() {
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
		color = Color(1.0f,1.0f,1.0f,1.0f);
	}

	PositionTextureNormalsVertex(const Vector3f& pos,const Vector2f& texcoords,const Vector3f& normals,const Color& ncolor = Color(1.0f,1.0f,1.0f,1.0f)) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		tu = texcoords.x;
		tv = texcoords.y;
		nx = normals.x;
		ny = normals.y;
		nz = normals.z;
		color = ncolor;
	}
	/*
	static VertexDeclaration* getVertexDeclaration() {
		if ( !gEngine->getResourceManager().contains("PositionTextureNormalsVertex")) {
			ResourceHandle handle = gEngine->getResourceManager().createVertexDeclaration("PositionTextureNormalsVertex");
			VertexDeclaration* vd = gEngine->getResourceManager().getVertexDeclaration(handle);
			vd->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);
			vd->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);
			vd->addElement(ds::VT_FLOAT3,ds::VDU_NORMAL);
			vd->addElement(ds::VT_FLOAT4,ds::VDU_COLOR);
			vd->create();
			return vd;
		}
		else {
			return gEngine->getResourceManager().getVertexDeclaration("PositionTextureNormalsVertex");
		}
	}
	*/
};

class PTNCVertex {

public:

	float x,y,z;
	float tu,tv;
	float nx,ny,nz;
	Color color;

	PTNCVertex() {
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
		color = Color(1.0f,1.0f,1.0f,1.0f);
	}

	PTNCVertex(const Vector3f& pos,const Vector2f& texcoords,const Vector3f& normals,const Color& ncolor = Color(1.0f,1.0f,1.0f,1.0f)) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		tu = texcoords.x;
		tv = texcoords.y;
		nx = normals.x;
		ny = normals.y;
		nz = normals.z;
		color = ncolor;
	}
	/*
	static VertexDeclaration* getVertexDeclaration() {
		if ( !gEngine->getResourceManager().contains("PTNCVertex")) {
			ResourceHandle handle = gEngine->getResourceManager().createVertexDeclaration("PTNCVertex");
			VertexDeclaration* vd = gEngine->getResourceManager().getVertexDeclaration(handle);
			vd->addElement(ds::VT_FLOAT3,ds::VDU_POSITION);
			vd->addElement(ds::VT_FLOAT2,ds::VDU_TEXCOORD);
			vd->addElement(ds::VT_FLOAT3,ds::VDU_NORMAL);
			vd->addElement(ds::VT_FLOAT4,ds::VDU_COLOR);
			vd->create();
			return vd;
		}
		else {
			return gEngine->getResourceManager().getVertexDeclaration("PTNCVertex");
		}
	}
	*/
};

class PTNBTVertex {

public:

	float x,y,z;
	float tx,ty,tz;
	float bx,by,bz;
	float nx,ny,nz;
	float tu,tv;
	

	PTNBTVertex() {
		x = y = z = 0.0f;
		tu = tv = 0.0f;
		nx = ny = nz = 0.0f;
		bx = by = bz = 0.0f;
		tx = ty = tz = 0.0f;
	}

	PTNBTVertex(const Vector3f& pos,const Vector2f& texcoords,const Vector3f& tangents,const Vector3f& binormals,const Vector3f& normals) {
		x = pos.x;
		y = pos.y;
		z = pos.z;
		tu = texcoords.x;
		tv = texcoords.y;
		tx = tangents.x;
		ty = tangents.y;
		tz = tangents.z;
		bx = binormals.x;
		by = binormals.y;
		bz = binormals.z;
		nx = normals.x;
		ny = normals.y;
		nz = normals.z;
	}
};

};
