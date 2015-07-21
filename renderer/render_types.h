#pragma once
#include <vector>
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\lib\container\List.h"
#include "vertex_types.h"
#include "..\lib\DataArray.h"
#include <d3dx9core.h>

namespace ds {

	// -------------------------------------------------------
	// Blending constants
	// -------------------------------------------------------
	const int BL_ZERO                = D3DBLEND_ZERO;
	const int BL_ONE                 = D3DBLEND_ONE;
	const int BL_SRC_COLOR           = D3DBLEND_SRCCOLOR;
	const int BL_ONE_MINUS_SRC_COLOR = D3DBLEND_INVSRCCOLOR;
	const int BL_DST_COLOR           = D3DBLEND_DESTCOLOR;
	const int BL_ONE_MINUS_DST_COLOR = D3DBLEND_INVDESTCOLOR;
	const int BL_SRC_ALPHA           = D3DBLEND_SRCALPHA;
	const int BL_ONE_MINUS_SRC_ALPHA = D3DBLEND_INVSRCALPHA;
	const int BL_DST_ALPHA           = D3DBLEND_DESTALPHA;
	const int BL_ONE_MINUS_DST_ALPHA = D3DBLEND_INVDESTALPHA;
	const int BL_SRC_ALPHA_SATURATE  = D3DBLEND_SRCALPHASAT;

	// -------------------------------------------------------
	// Blend op constants
	// -------------------------------------------------------
	const int BM_ADD              = D3DBLENDOP_ADD;
	const int BM_SUBTRACT         = D3DBLENDOP_SUBTRACT;
	const int BM_REVERSE_SUBTRACT = D3DBLENDOP_REVSUBTRACT;
	const int BM_MIN              = D3DBLENDOP_MIN;
	const int BM_MAX              = D3DBLENDOP_MAX;

	// -------------------------------------------------------
	// Alpha Func constants
	// -------------------------------------------------------
	const int ALPHA_GREATER = D3DCMP_GREATER;
	const int ALPHA_GEQ     = D3DCMP_GREATEREQUAL;
	const int ALPHA_ALWAYS  = D3DCMP_ALWAYS ;
	const int ALPHA_EQ      = D3DCMP_EQUAL;
	const int ALPHA_LESS    = D3DCMP_LESS;

	const int COP_ADD = D3DTOP_ADD;

	const int CA_TEXTURE = D3DTA_TEXTURE;
	const int CA_CURRENT = D3DTA_CURRENT;
	const int CA_DIFFUSE = D3DTA_DIFFUSE;

	// -------------------------------------------------------
	// Cull mode constants
	// -------------------------------------------------------
	const int CULL_NONE  = D3DCULL_NONE;
	const int CULL_BACK  = D3DCULL_CCW;
	const int CULL_FRONT = D3DCULL_CW;

	// -------------------------------------------------------
	// Fill mode constants
	// -------------------------------------------------------
	const int SOLID = D3DFILL_SOLID;
	const int WIREFRAME = D3DFILL_WIREFRAME;

	// -------------------------------------------------------
	// Blend state
	// -------------------------------------------------------
	struct BlendState {

		IdString hashName;
		int srcFactorRGB;
		int dstFactorRGB;
		int blendModeRGB;
		int srcFactorAlpha;
		int dstFactorAlpha;
		int blendModeAlpha;
		int mask;
		bool blendEnable;
		bool separateAlpha;
		int alphaRef;	
		int alphaFunc;
		int flag;
	};

	// -------------------------------------------------------
	// Texture stage state
	// -------------------------------------------------------
	struct TextureStageState {
		int colorOP;
		int firstColorArg;
		int secondColorArg;
//d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD );
//d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	};

	const int FLT_NONE        = D3DTEXF_NONE;
	const int FLT_POINT       = D3DTEXF_POINT;
	const int FLT_LINEAR      = D3DTEXF_LINEAR;
	const int FLT_ANISOTROPIC = D3DTEXF_ANISOTROPIC;

	const int TA_WRAP   = D3DTADDRESS_WRAP;
	const int TA_MIRROR = D3DTADDRESS_MIRROR;
	const int TA_CLAMP  = D3DTADDRESS_CLAMP;
	const int TA_BORDER = D3DTADDRESS_BORDER;

	struct SamplerState {
		D3DTEXTUREFILTERTYPE minFilter;
		D3DTEXTUREFILTERTYPE magFilter;
		D3DTEXTUREFILTERTYPE mipFilter;
		D3DTEXTUREADDRESS wrapS;
		D3DTEXTUREADDRESS wrapT;
		D3DTEXTUREADDRESS wrapR;
		DWORD maxAniso;
		float lod;
		D3DCOLOR borderColor;
	};

	// -------------------------------------------------------
	// Rasterizer state
	// -------------------------------------------------------
	struct RasterizerState {
		int cullMode;
		int fillMode;
		bool multiSample;
		bool scissor;
	};

	// -------------------------------------------------------
	// Texture
	// -------------------------------------------------------
	struct TextureAsset {
		IdString name;
		LPDIRECT3DTEXTURE9 texture;
		uint32 width;
		uint32 height;
		uint32 flags;
	};

	struct Texture {

		IdString hashName;
		Vector4f uv;
		Vector2f dim;
		int textureID;

		Texture() : hashName(0) , uv(0, 0, 1, 1), dim(32, 32), textureID(0) {}

		const Vector2f getUV(int idx) const {
			switch (idx) {
				case 0: return Vector2f(uv.x, uv.y); break;
				case 1: return Vector2f(uv.z, uv.y); break;
				case 2: return Vector2f(uv.z, uv.w); break;
				case 3: return Vector2f(uv.x, uv.w); break;
				default: return Vector2f(0, 0);
			}
		}
	};

	// -------------------------------------------------------
	// Shader constant
	// -------------------------------------------------------
	struct ShaderConstant {
		D3DXHANDLE handle;
		IdString name;
	};
	// -------------------------------------------------------
	// Shader
	// -------------------------------------------------------
	struct Shader {

		IdString hashName;
		ID3DXEffect* m_FX;
		D3DXHANDLE m_hTech;
		ShaderConstant* constants;
		uint32 constantCount;

		Shader() : hashName(0) , m_FX(0), constants(0), constantCount(0) {}
	};

	// -------------------------------------------------------
	// Render target
	// -------------------------------------------------------
	struct RenderTarget {

		int flag;
		Color clearColor;
		LPDIRECT3DTEXTURE9 texture;
		LPDIRECT3DSURFACE9 surface;
		LPD3DXRENDERTOSURFACE rts;
		int textureID;

		RenderTarget() : flag(0) , clearColor(Color(0,0,0,255)) ,texture(0) , surface(0) , rts(0) , textureID(-1) {}

	};

	struct Sphere {

		Vector3f position;
		float radius;

		Sphere() : position(0, 0, 0), radius(0.0f) {}

		Sphere(const Vector3f& pos, float r) : position(pos), radius(r) {}

	};

	struct AABBox {

		Vector2f position;
		Vector2f extent;
		Vector2f min;
		Vector2f max;

		AABBox() {}

		AABBox(const Vector2f& pos, const Vector2f& ext) {
			position = pos;
			extent = ext * 0.5f;
			min = vec_min(position - extent, position + extent);
			max = vec_max(position - extent, position + extent);
		}

		void scale(const Vector2f& s) {
			extent.x *= s.x;
			extent.y *= s.y;
			min = vec_min(position - extent, position + extent);
			max = vec_max(position - extent, position + extent);
		}

		void transpose(const Vector2f& pos) {
			position = pos;
			min = vec_min(position - extent, position + extent);
			max = vec_max(position - extent, position + extent);
		}

		Vector2f findClosestPoint(const Vector2f& p) const {
			Vector2f ret(0, 0);
			ret.x = (p.x < min.x) ? min.x : (p.x > max.x) ? max.x : p.x;
			ret.y = (p.y < min.y) ? min.y : (p.y > max.y) ? max.y : p.y;
			return ret;
		}

		bool contains(const Vector2f& point) const {
			if (point.x < min.x || max.x < point.x) {
				return false;
			}
			else if (point.y < min.y || max.y < point.y) {
				return false;
			}
			return true;
		}

		const bool overlaps(const AABBox& b) const {
			const Vector2f T = b.position - position;
			return fabs(T.x) <= (extent.x + b.extent.x) && fabs(T.y) <= (extent.y + b.extent.y);
		}

		const bool overlaps(const Sphere& sphere) const {
			float s, d = 0;
			//find the square of the distance
			//from the sphere to the box
			for (int i = 0; i < 2; ++i) {
				if (sphere.position[i] < min_value(i)) {
					s = sphere.position[i] - min_value(i);
					d += s*s;
				}
				else if (sphere.position[i] > max_value(i)) {
					s = sphere.position[i] - max_value(i);
					d += s*s;
				}

			}
			return d <= sphere.radius * sphere.radius;
		}

		float min_value(int index) const {
			return min[index];
		}

		float max_value(int index) const {
			return max[index];
		}
	};

	// -------------------------------------------------------
	// Material
	// -------------------------------------------------------
	struct Material {

		IdString hash;
		int texture;
		int shader;
		int blendState;
		int vertexType;
		Color diffuseColor;

		Material() : hash(0), texture(-1), shader(-1), blendState(0) {}
		
	};
	
	// -------------------------------------------------------
	// MeshData
	// -------------------------------------------------------
	struct MeshData {

		IdString hash;
		ID id;
		Surface* faces;
		int size;
		AABBox aabBox;

		MeshData() : size(0) , faces(0) {}
		
	};

	// -------------------------------------------------------
	// MeshData
	// -------------------------------------------------------
	struct Mesh {

		ID id;
		IdString hashName;
		int descriptorID;
		ID meshDataID;
		int vertexType;

	};
	// -------------------------------------------------------
	// Debug message
	// -------------------------------------------------------
	struct DebugMessage {
		std::string message;
		int x;
		int y;
		Color color;
		DebugMessage() : message("") , x(0) , y(0) , color(1.0f,1.0f,1.0f,1.0f) {}
	};

	struct Polygon {

		Vector2f vertices[4];
		Vector4f uv;
		Color color;

		Polygon() : uv(0,0,0,0) , color(Color::WHITE) {
			for ( int i = 0; i < 4; ++i ) {
				vertices[i] = Vector2f(0.0f,0.0f);
			}
		}
	};

}
