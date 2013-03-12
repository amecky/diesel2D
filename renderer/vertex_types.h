#pragma once
#include "..\utils\Color.h"
#include "..\utils\GameMath.h"

namespace ds {

	// -------------------------------------------------------
	// TTC Vertex
	// -------------------------------------------------------
	struct TTCVertex {

		float x,y,z,rwh;
		float tu,tv;
		Color color;

		TTCVertex() {
			x = y = z = 0.0f;
			rwh = 1.0f;
			tu = tv = 0.0f;
			color = Color(1.0f,1.0f,1.0f,1.0f);
		}

		TTCVertex(float xp,float yp,float zp,float u,float v) {
			x = xp;
			y = yp;
			z = zp;
			rwh = 1.0f;
			tu = u;
			tv = v;
			color = Color(1.0f,1.0f,1.0f,1.0f);
		}

		TTCVertex(float xp,float yp,float zp,float u,float v,const Color& clr) {
			x = xp;
			y = yp;
			z = zp;
			rwh = 1.0f;
			tu = u;
			tv = v;
			color = clr;
		}

		TTCVertex(const Vec3& pos,const Vec2& texcoords) {
			x = pos.x;
			y = pos.y;
			z = pos.z;
			rwh = 1.0f;
			tu = texcoords.x;
			tv = texcoords.y;
			color = Color(1.0f,1.0f,1.0f,1.0f);
		}

		TTCVertex(const Vec3& pos,const Vec2& texcoords,const Color& clr) {
			x = pos.x;
			y = pos.y;
			z = pos.z;
			rwh = 1.0f;
			tu = texcoords.x;
			tv = texcoords.y;
			color = clr;
		}
	};

	// -------------------------------------------------------
	// Quad using 4 TTCVertex
	// -------------------------------------------------------
	struct Plane {
		float scaleX;
		float scaleY;
		float x;
		float y;
		float rotation;
		float dimX;
		float dimY;
		TTCVertex v1;
		TTCVertex v2;
		TTCVertex v3;
		TTCVertex v4;
	};

	// -------------------------------------------------------
	// Surface is a plane in 3D space
	// -------------------------------------------------------
	struct Surface {

		Vec3 v1;
		Vec3 v2;
		Vec3 v3;
		Vec3 v4;
		Vec2 uv1;
		Vec2 uv2;
		Vec2 uv3;
		Vec2 uv4;
		float tu1;
		float tv1;
		float tu2;
		float tv2;
		Vec3 n;
		Vec3 t;
		Vec3 b;

		Surface() {

		}

		Surface(const Vec3& cv1,const Vec3& cv2,const Vec3& cv3,const Vec3& cv4,const Vec3& nn,const Vec3& tt,const Vec3& bb,float ttu1,float ttv1,float ttu2,float ttv2) {
			v1 = cv1;
			v2 = cv2;
			v3 = cv3;
			v4 = cv4;
			tu1 = ttu1;
			tv1 = ttv1;
			tu2 = ttu2;
			tv2 = ttv2;
			n = nn;
			t = tt;
			b = bb;
		}

		Surface(const Vec3& cv1,const Vec3& cv2,const Vec3& cv3,const Vec3& cv4,const Vec3& nn,const Vec3& tt,const Vec3& bb,const Vec2& _uv1,const Vec2& _uv2,const Vec2& _uv3,const Vec2& _uv4) {
			v1 = cv1;
			v2 = cv2;
			v3 = cv3;
			v4 = cv4;
			uv1 = _uv1;
			uv2 = _uv2;
			uv3 = _uv3;
			uv4 = _uv4;
			n = nn;
			t = tt;
			b = bb;
		}
	};


}