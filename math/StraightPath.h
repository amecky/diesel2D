#pragma once
#include <Vector.h>
#include <vector>
#include "CubicBezierPath.h"

namespace ds {

	struct LineSegment {

		Vector2f start;
		Vector2f end;
	};

	typedef std::vector<LineSegment> LineElements;

	class StraightPath {

	public:
		StraightPath();
		virtual ~StraightPath();
		virtual void create(const Vector2f& start, const Vector2f& end);
		virtual void add(const Vector2f& end);
		void build();
		void get(float t, Vector2f* p);
		void approx(float u, Vector2f* p);
		void tanget(float u, Vector2f* tangent);
		//void load(BinaryLoader* loader);
		const int size() const {
			return m_Elements.size();
		}
	protected:
		float find(float u);
		LineElements m_Elements;
		float m_TotalLength;
		float m_ArcLength[MAX_CBP_STEPS + 1];
	};

	class GridPath : public StraightPath {

	public:
		GridPath(float gridSize,const Vector2f& offset);
		~GridPath();
		virtual void create(const Vector2f& start, const Vector2f& end);
		virtual void add(const Vector2f& end);
	private:
		float m_GridSize;
		float m_HalfGridSize;
		Vector2f m_Offset;
	};
	
	/*
	template<class T>
	class StraightPathTemplate : public PathTemplate<T> {

		struct LineSegment {

			T start;
			T end;
		};

		typedef std::vector<LineSegment> LineElements;

	public:
		StraightPathTemplate() : PathTemplate() {}
		virtual ~StraightPathTemplate() {}
		void create(const T& start, const T& end) {
			m_Elements.clear();
			LineSegment ls;
			ls.start = start;
			ls.end = end;
			m_Elements.push_back(ls);
		}

		void add(const T& end) {
			assert(m_Elements.size() > 0);
			LineSegment ls;
			ls.start = m_Elements[m_Elements.size() - 1].end;
			ls.end = end;
			m_Elements.push_back(ls);
		}

		void build() {
			assert(m_Elements.size() > 0);
			m_ArcLength[0] = 0.0f;
			T p;
			T previous;
			m_TotalLength = 0.0f;
			for (int i = 0; i <= MAX_CBP_STEPS; ++i) {
				float t = (float)i / static_cast<float>(MAX_CBP_STEPS);
				get(t, &p);
				if (i > 0) {
					float x_diff = p.x - previous.x;
					double y_diff = p.y - previous.y;
					m_TotalLength += static_cast<float>(sqrt(x_diff * x_diff + y_diff * y_diff));
					m_ArcLength[i] = m_TotalLength;
				}
				previous = p;
			}
		}

		void approx(float u, T* p) {
			assert(u >= 0.0f && u <= 1.0f);
			float t = find(u);
			get(t, p);
		}

		

		void tanget(float u, T* tangent) {
			assert(u >= 0.0f && u <= 1.0f);
			float t = find(u);
			float ds = 1.0f / static_cast<float>(m_Elements.size());
			int idx = t * m_Elements.size();
			if (idx == m_Elements.size()) {
				*tangent = m_Elements[m_Elements.size() - 1].end - m_Elements[m_Elements.size() - 1].start;
			}
			else {
				LineSegment& curve = m_Elements[idx];
				float nn = t / ds - idx;
				*tangent = lerp(curve.start, curve.end, nn) - lerp(curve.start, curve.end, nn - 1 / 60.0f);
			}
		}
		
		void get(float t, T* p) {
			float ds = 1.0f / static_cast<float>(m_Elements.size());
			int idx = t * m_Elements.size();
			if (idx == m_Elements.size()) {
				*p = m_Elements[m_Elements.size() - 1].end;
			}
			else {
				LineSegment& curve = m_Elements[idx];
				float nn = t / ds - idx;
				*p = lerp(curve.start, curve.end, nn);
			}
		}
		
		void load(BinaryLoader* loader) {
			m_Elements.clear();
			T p0, p1;
			while (loader->openChunk() == 0) {
				if (loader->getChunkID() == 1) {
					loader->read(&p0);
					loader->read(&p1);
					create(p0, p1);
				}
				else if (loader->getChunkID() == 2) {
					loader->read(&p1);
					add(p1);
				}
				loader->closeChunk();
			}
			build();
		}
		const int size() const {
			return m_Elements.size();
		}
	protected:
		float find(float u) {
			if (u == 0.0f) {
				return 0.0f;
			}
			float targetLength = u * m_TotalLength;
			int low = 0;
			int high = MAX_CBP_STEPS;
			int index = 0;
			while (low < high) {
				index = (low + (high - low) / 2);
				if (m_ArcLength[index] < targetLength) {
					low = index + 1;

				}
				else {
					high = index;
				}
			}
			if (m_ArcLength[index] > targetLength) {
				index--;
			}
			float lengthBefore = m_ArcLength[index];
			if (lengthBefore == targetLength) {
				return lengthBefore;

			}
			else {
				return (index + (targetLength - lengthBefore) / (m_ArcLength[index + 1] - lengthBefore)) / static_cast<float>(MAX_CBP_STEPS);
			}
		}
		LineElements m_Elements;
		float m_TotalLength;
		float m_ArcLength[MAX_CBP_STEPS + 1];
	};

	typedef StraightPathTemplate<Vector3f> StraightPath3D;
	typedef StraightPathTemplate<Vector2f> StraightPath;
	
	class GridPath : public StraightPath {

	public:
		GridPath(float gridSize, const Vector2f& offset);
		~GridPath();
		virtual void create(const Vector2f& start, const Vector2f& end);
		virtual void add(const Vector2f& end);
	private:
		float m_GridSize;
		float m_HalfGridSize;
		Vector2f m_Offset;
	};
	*/
}