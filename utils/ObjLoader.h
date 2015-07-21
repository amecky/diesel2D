#pragma once

#include <string>
#include <map>
#include <vector>
#include <Vector.h>
#include "..\renderer\render_types.h"

namespace ds {

	class ObjLoader {

		struct VertexDefinition {

			int vertexIndex;
			int normalIndex;
			int uvIndex;
			ds::Color color;

			VertexDefinition() : vertexIndex(-1), normalIndex(-1), uvIndex(-1), color(ds::Color::WHITE) {}
		};

		struct Material {
			float specularPower;
			Color ambient;
			Color diffuse;
			Color specular;
			int index;
		};

		typedef std::map<std::string, Material> Materials;
		typedef std::vector<Vector3f> VectorCache;
		typedef std::vector<VertexDefinition> Faces;
		typedef std::vector<Vector2f> UVCache;

	public:
		ObjLoader();		
		virtual ~ObjLoader();
		void parse(const std::string& fileName, MeshData& data);
		void read(const std::string& fileName, MeshData& data);
		void save(const std::string& fileName, const MeshData& data);
	private:
		ObjLoader(const ObjLoader& orig);
		void readMaterials(const std::string& mtlFileName);
		bool startsWith(const std::string& line, const std::string& txt);
		void readColor(const std::string& line, ds::Color* color);
		void readVector3(const std::string& line, Vector3f* v);
		void readVector2(const std::string& line, Vector2f* v);
		int readInt(const std::string& line);
		float readFloat(const std::string& line);
		std::string readString(const std::string& line);
		Materials m_Materials;
		VectorCache m_Vertices;
		VectorCache m_Normals;
		UVCache m_UVCache;
		Faces m_Faces;
	};

}