#pragma once
#include <Vector.h>
#include <vector>
#include "BezierCurve.h"

namespace ds {

	const int MAX_CBP_STEPS = 64;

	class CubicBezierPath {

	typedef std::vector<BezierCurve> BezierElements;

	public:
		CubicBezierPath();
		~CubicBezierPath();
		void create(const Vector2f& p0,const Vector2f& p1,const Vector2f& p2,const Vector2f& p3);
		void add(const Vector2f& p1,const Vector2f& p2,const Vector2f& p3);	
		void build();
		void get(float t,Vector2f* p);
		void approx(float u, Vector2f* p);
		void tanget(float u, Vector2f* tangent);
		const int size() const {
			return m_Elements.size();
		}
		const BezierCurve& getElement(int idx) const {
			return m_Elements[idx];
		}
	private:
		float find(float u);
		BezierElements m_Elements;
		float m_TotalLength;
		float m_ArcLength[MAX_CBP_STEPS+1];
	};

}