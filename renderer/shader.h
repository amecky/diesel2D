#pragma once
#include "render_types.h"
#include "..\math\math_types.h"
#include "..\io\Serializer.h"

namespace ds {

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
	class Shader : public Serializer {

	public:		
		Shader(const char* name);
		~Shader() {}
		void initialize(const char* techName);
		uint32 start();
		void end();
		void beginPass(UINT p);
		void endPass();
		void commitChanges();
		bool setFloat(const char* name, float value);
		bool setVector3f(const char* name, const Vector3f& v);
		bool setVector2f(const char* name, const Vector2f& v);
		bool setValue(const char* name, void* data, UINT size);
		bool setTexture(const char* name, int textureID);
		bool setColor(const char* name, const Color& color);
		bool setMatrix(const char* name,const mat4& m);
		bool contains(const char* name);
		void release();
		void createFromText(const char* buffer, const char* techName);
		bool loadShader(const char* fxName, const char* techName);
		IdString getHashName() const {
			return _hashName;
		}
		void load(BinaryLoader* loader);

	private:
		D3DXHANDLE findHandle(const char* name);
		ID3DXEffect* _FX;
		D3DXHANDLE _hTech;
		IdString _hashName;
		ShaderConstant* _constants;
		uint32 _constantCount;
	};


	namespace shader {

		int createTransformedTextureColorShader(int textureId);
		int createPTCShader(int textureId);

		int createBloomShader(int textureID,float threshold);
		int createBlurShader(int textureID);
		int createBloomCombineShader(int colorTextureID,int bloomTextureID);

		int createCombineLightShader(int lightTextureID, int colorTextureID);

		
		
		//bool setFloat(Shader& shader,const char* name,float value);
		//D3DXHANDLE findHandle(const Shader& shader,const char* name);
	}
}