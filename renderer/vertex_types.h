#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "Vector.h"

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

		TTCVertex(const Vector3f& pos,const Vector2f& texcoords) {
			x = pos.x;
			y = pos.y;
			z = pos.z;
			rwh = 1.0f;
			tu = texcoords.x;
			tv = texcoords.y;
			color = Color(1.0f,1.0f,1.0f,1.0f);
		}

		TTCVertex(const Vector3f& pos,const Vector2f& texcoords,const Color& clr) {
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
	// TTC Vertex
	// -------------------------------------------------------
	struct PTCVertex {

		float x,y,z;
		Vector2f uv;
		Color color;

		PTCVertex() {
			x = y = z = 0.0f;
			uv = Vector2f(0,0);//tu = tv = 0.0f;
			color = Color(1.0f,1.0f,1.0f,1.0f);
		}

		PTCVertex(float xp,float yp,float zp,float u,float v,const Color& clr = Color::WHITE) {
			x = xp;
			y = yp;
			z = zp;
			//tu = u;
			//tv = v;
			uv = Vector2f(u,v);	
			color = clr;
		}

		PTCVertex(const Vector3f& pos,const Vector2f& texcoords,const Color& clr = Color::WHITE) {
			x = pos.x;
			y = pos.y;
			z = pos.z;
			//tu = texcoords.x;
			//tv = texcoords.y;
			uv = texcoords;
			color = clr;
		}
	};

	struct SpritePlane {
		PTCVertex v[4];
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
		Vector2f center;
		TTCVertex v1;
		TTCVertex v2;
		TTCVertex v3;
		TTCVertex v4;
	};

	// -------------------------------------------------------
	// Surface is a plane in 3D space
	// -------------------------------------------------------
	struct Surface {

		Vector3f v[4];		
		Vector2f uv[4];		
		Vector3f n;
		Vector3f t;
		Vector3f b;

		Surface() {

		}

		Surface(const Vector3f& cv1,const Vector3f& cv2,const Vector3f& cv3,const Vector3f& cv4,const Vector3f& nn,const Vector3f& tt,const Vector3f& bb,float ttu1,float ttv1,float ttu2,float ttv2) {
			v[0] = cv1;
			v[1] = cv2;
			v[2] = cv3;
			v[3] = cv4;
			//tu1 = ttu1;
			//tv1 = ttv1;
			//tu2 = ttu2;
			//tv2 = ttv2;
			n = nn;
			t = tt;
			b = bb;
		}

		Surface(const Vector3f& cv1,const Vector3f& cv2,const Vector3f& cv3,const Vector3f& cv4,const Vector3f& nn,const Vector3f& tt,const Vector3f& bb,const Vector2f& _uv1,const Vector2f& _uv2,const Vector2f& _uv3,const Vector2f& _uv4) {
			v[0] = cv1;
			v[1] = cv2;
			v[2] = cv3;
			v[3] = cv4;
			uv[0] = _uv1;
			uv[1] = _uv2;
			uv[2] = _uv3;
			uv[3] = _uv4;
			n = nn;
			t = tt;
			b = bb;
		}
	};


}