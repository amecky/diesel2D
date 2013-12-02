#pragma once
#include <vector>
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\lib\container\List.h"
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
	// Character definition
	// -------------------------------------------------------
	struct CharDef {
		uint32 ascii;
		int startX;
		int startY;
		int width;
		float u1;
		float u2;
		float v1;
		float v2;
		Rect texureRect;
	};
	
	// -------------------------------------------------------
	// Bitmap Font
	// -------------------------------------------------------
	struct BitmapFont {
		uint32 startChar;
		uint32 endChar;
		uint32 width;
		uint32 height;
		uint32 padding;
		uint32 textureSize;
		uint32 charHeight;
		uint32 startX;
		uint32 startY;
		uint32 gridHeight;		
		List<CharDef> definitions;

		void addChar(uint32 ascii,int startX,int startY,int width) {
			CharDef cd;
			cd.ascii = ascii;
			cd.startX = startX;
			cd.startY = startY;
			cd.width = width;
			cd.u1 = (float)cd.startX/textureSize;
			cd.v1 = (float)cd.startY/textureSize;
			cd.u2 = cd.u1 + (float)cd.width/(textureSize);
			cd.v2 = cd.v1 + ((float)charHeight)/(textureSize);
			cd.texureRect = Rect(static_cast<float>(startY),static_cast<float>(startX),static_cast<float>(width),static_cast<float>(charHeight));
			definitions.append(cd);
		}
		const CharDef& getCharDef(char c) const {
			return definitions[(int)c-startChar];
		}
	};

	// -------------------------------------------------------
	// Texture
	// -------------------------------------------------------
	struct Texture {
		IdString name;
		LPDIRECT3DTEXTURE9 texture;
		uint32 width;
		uint32 height;
		uint32 flags;
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
		ID3DXEffect* m_FX;
		D3DXHANDLE m_hTech;
		int flag;
		ShaderConstant* constants;
		uint32 constantCount;
	};

	// -------------------------------------------------------
	// Material 
	// -------------------------------------------------------
	struct Material {
		IdString name;
		Color diffuse;
		Color ambient;
		Color specular;
		Color emissive;
		float specularPower;
		int shader;
		int textures[5];
		int textureAtlas;
		int flag;
		Material() {
			textureAtlas = -1;
			shader = -1;
			for ( int i = 0; i < 5; ++i ) {
				textures[i] = -1;
			}
		}
		const bool hasTextures() const {
			for ( int i =0; i < 5; ++i ) {
				if ( textures[i] != -1 ) {
					return true;
				}
			}
			return false;
		}
	};

	// -------------------------------------------------------
	// Font
	// -------------------------------------------------------
	struct SystemFont {
		IdString name;
		int size;
		bool bold;
		ID3DXFont* font;
		int flag;
	};

	// -------------------------------------------------------
	// Render target
	// -------------------------------------------------------
	struct RenderTarget {
		IdString name;		
		LPDIRECT3DTEXTURE9 texture;
		LPDIRECT3DSURFACE9 surface;
		LPD3DXRENDERTOSURFACE rts;
	};

	// -------------------------------------------------------
	// Sprite
	// -------------------------------------------------------
	struct Sprite {

		Vec2 position;
		float rotation;
		Vec2 center;
		float scaleX;
		float scaleY;
		Color color;
		Rect textureRect;

		Sprite() : position(0,0) , rotation(0.0f) , center(0,0) , scaleX(1.0f) , scaleY(1.0f) , color(Color::WHITE) , textureRect(0,0,32,32) {}
		Sprite(int x,int y,const Rect tr,float rot = 0.0f, float sx = 1.0f,float sy = 1.0f,const Color& clr = Color::WHITE, const Vec2& cntr = Vec2(0,0)) 
			: position(Vec2(x,y)) , textureRect(tr) , rotation(rot) , scaleX(sx) , scaleY(sy) , center(cntr) , color(clr) {}
		Sprite(const Vec2& pos,const Rect tr) : position(pos) , textureRect(tr) , rotation(0.0f) , scaleX(1.0f) , scaleY(1.0f) , center(0,0) , color(Color::WHITE) {}
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

}
