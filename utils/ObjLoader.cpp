#include "ObjLoader.h"
#include <fstream>
#include <vector>
#include "Log.h"

namespace ds {

	ObjLoader::ObjLoader() {
	}

	ObjLoader::ObjLoader(const ObjLoader& orig) {
	}

	ObjLoader::~ObjLoader() {
	}

	bool ObjLoader::startsWith(const std::string& line, const std::string& txt) {
		int len = txt.length();
		for (int i = 0; i < len; ++i) {
			if (line[i] != txt[i]) {
				return false;
			}
		}
		if (line[len] != ' ') {
			return false;
		}
		return true;
	}

	void ObjLoader::parse(const std::string& fileName, MeshData& data) {
		std::ifstream InFile(fileName.c_str());
		LOG << "reading obj file: " << fileName;
		std::string line;
		Material* m = 0;
		bool hasNormals = false;
		bool hasUV = false;
		while (std::getline(InFile, line)) {
			if (startsWith(line, "mtllib")) {
				std::string mtrlFile = readString(line);
				readMaterials("content\\data\\" + mtrlFile);
			}
			if (startsWith(line, "o")) {
				std::string name = readString(line);
				LOG << "found object name: " << name;
			}
			else if (startsWith(line, "v")) {
				Vector3f v;
				readVector3(line, &v);
				m_Vertices.push_back(v);
			}
			else if (startsWith(line, "vn")) {
				Vector3f vn;
				readVector3(line, &vn);
				m_Normals.push_back(vn);
				hasNormals = true;
			}
			else if (startsWith(line, "vt")) {
				Vector2f uv;
				readVector2(line, &uv);
				m_UVCache.push_back(uv);
				hasUV = true;
			}
			else if (startsWith(line, "usemtl")) {
				std::string mName = readString(line);
				m = &m_Materials[mName];
				LOG << "using material: " << mName;
			}
			else if (startsWith(line, "f")) {
				int x, y, z;
				std::string dummy;
				char c;
				std::istringstream iss(line);
				iss >> dummy;
				for (int i = 0; i < 4; ++i) {
					x = -1;
					y = -1;
					z = -1;
					if (hasNormals && hasUV) {
						iss >> x >> c >> z >> c >> y;
					}
					else if (hasNormals && !hasUV) {
						iss >> x >> c >> c >> y;
					}
					else if (!hasNormals && hasUV) {
						iss >> x >> c >> z >> c;
					}
					else {
						iss >> x;// >> c >> c;
					}
					VertexDefinition f;
					f.vertexIndex = x;
					f.normalIndex = y;
					f.uvIndex = z;
					if (m != 0) {
						f.color = m->diffuse;
					}
					m_Faces.push_back(f);
				}
			}
		}
		LOG << "vertex cache  : " << m_Vertices.size();
		LOG << "normals cache : " << m_Normals.size();
		LOG << "uv cache      : " << m_UVCache.size();
		LOG << "faces         : " << m_Faces.size();
		int index = 0;
		int count = m_Faces.size() / 4;
		data.faces = new Surface[count];
		Vector3f extMax(0, 0, 0);
		Vector3f extMin(2000, 2000, 2000);
		for (int i = 0; i < count; ++i) {
			Surface f;
			for (int j = 0; j < 4; ++j) {
				VertexDefinition& current = m_Faces[j + index];
				if (current.vertexIndex != -1) {
					f.v[j] = m_Vertices[current.vertexIndex - 1];
					f.v[j].z *= -1.0f;
					for (int k = 0; k < 3; ++k) {
						if (f.v[j][k] > extMax[k]) {
							extMax[k] = f.v[j][k];
						}
						if (f.v[j][k] < extMin[k]) {
							extMin[k] = f.v[j][k];
						}
					}
				}				
				else {
					f.v[j] = Vector3f(0, 0, 0);
				}
				if (current.uvIndex != -1) {
					f.uv[j] = m_UVCache[current.uvIndex - 1];
					f.uv[j].y = 1.0f - f.uv[j].y;
				}
				else {
					f.uv[j] = Vector2f(0, 0);
				}
			}
			if (hasNormals) {
				f.n = m_Normals[m_Faces[index].normalIndex - 1];
				f.n.z *= -1.0f;
			}
			index += 4;
			if (m != 0) {
				f.color = m_Faces[i * 4].color;
			}

			data.faces[i] = f;
			++data.size;
		}
		Vector3f extent = Vector3f(fabs(extMin.x - extMax.x), fabs(extMin.y - extMax.y), fabs(extMin.z - extMax.z));
		Vector3f center = (extMax + extMin) * 0.5f;
		LOG << "---> extent: " << DBG_V3(extent) << " center: " << DBG_V3(center);
		//data.aabBox.offset = center.xy();
		data.aabBox.extent = extent.xy();
		/*
		for (int i = 0; i < data.size; ++i) {
		LOG << DBG_V3(data.faces[i].v[0]);
		LOG << DBG_V3(data.faces[i].v[1]);
		LOG << DBG_V3(data.faces[i].v[2]);
		LOG << DBG_V3(data.faces[i].v[3]);
		LOG << "n: " << DBG_V3(data.faces[i].n);
		LOG << "c: " << DBG_CLR(data.faces[i].color);

		}
		*/
	}


	void ObjLoader::readVector3(const std::string& line, Vector3f* color) {
		std::string dummy;
		std::istringstream iss(line);
		iss >> dummy >> color->x >> color->y >> color->z;
	}

	void ObjLoader::readVector2(const std::string& line, Vector2f* v) {
		std::string dummy;
		std::istringstream iss(line);
		iss >> dummy >> v->x >> v->y;
	}

	void ObjLoader::readColor(const std::string& line, Color* color) {
		std::string dummy;
		std::istringstream iss(line);
		iss >> dummy >> color->r >> color->g >> color->b;
		color->a = 1.0f;
	}

	int ObjLoader::readInt(const std::string& line) {
		std::string dummy;
		std::istringstream iss(line);
		int v = 0;
		iss >> dummy >> v;
		return v;
	}

	float ObjLoader::readFloat(const std::string& line) {
		std::string dummy;
		std::istringstream iss(line);
		float v = 0.0f;
		iss >> dummy >> v;
		return v;
	}

	std::string ObjLoader::readString(const std::string& line) {
		std::string dummy;
		std::istringstream iss(line);
		iss >> dummy >> dummy;
		return dummy;
	}


	void ObjLoader::readMaterials(const std::string& mtlFileName) {
		std::ifstream InFile(mtlFileName.c_str());
		LOG << "reading material file: " << mtlFileName;
		std::string line;
		Material* m = 0;
		while (std::getline(InFile, line)) {
			if (line.find("newmtl") != std::string::npos) {
				std::istringstream iss(line);
				std::string name;
				iss >> name >> name;
				LOG << "new mtl: " << name;
				Material mtl;
				m_Materials[name] = mtl;
				m = &m_Materials[name];
			}
			else if (line.find("Ka") != std::string::npos) {
				readColor(line, &m->ambient);
			}
			else if (line.find("Kd") != std::string::npos) {
				readColor(line, &m->diffuse);
			}
			else if (line.find("Ks") != std::string::npos) {
				readColor(line, &m->specular);
			}
			else if (line.find("Ns") != std::string::npos) {
				m->specularPower = readFloat(line);
			}
		}
		LOG << "number of materials: " << m_Materials.size();
	}

	void ObjLoader::read(const std::string& fileName, MeshData& data) {
		FILE *file = fopen(fileName.c_str(), "rb");
		int count = 0;
		fread(&count, sizeof(int), 1, file);
		LOG << "number of faces: " << count;
		data.faces = new Surface[count];
		data.size = count;
		for (int i = 0; i < count; ++i) {
			for (int j = 0; j < 4; ++j) {
				fread(&data.faces[i].v[j], sizeof(Vector3f), 1, file);
			}
			for (int j = 0; j < 4; ++j) {
				fread(&data.faces[i].uv[j], sizeof(Vector2f), 1, file);
			}
			fread(&data.faces[i].n, sizeof(Vector3f), 1, file);
			fread(&data.faces[i].color, sizeof(Color), 1, file);
		}
		//fread(&data.aabBox.offset, sizeof(Vector3f), 1, file);
		fread(&data.aabBox.extent, sizeof(Vector3f), 1, file);
		fclose(file);
	}

	void ObjLoader::save(const std::string& fileName, const MeshData& data) {
		FILE *file = fopen(fileName.c_str(), "wb");
		fwrite(&data.size, sizeof(int), 1, file);
		for (int i = 0; i < data.size; ++i) {
			for (int j = 0; j < 4; ++j) {
				fwrite(&data.faces[i].v[j], sizeof(Vector3f), 1, file);
			}
			for (int j = 0; j < 4; ++j) {
				fwrite(&data.faces[i].uv[j], sizeof(Vector2f), 1, file);
			}
			fwrite(&data.faces[i].n, sizeof(Vector3f), 1, file);
			fwrite(&data.faces[i].color, sizeof(Color), 1, file);			
		}
		//fwrite(&data.aabBox.offset, sizeof(Vector3f), 1, file);
		fwrite(&data.aabBox.extent, sizeof(Vector3f), 1, file);
		fclose(file);
	}

}