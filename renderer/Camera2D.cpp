#include "Camera2D.h"
#include "..\math\matrix.h"
#include "..\math\GameMath.h"

namespace ds {

Camera2D::Camera2D(void) {
}


Camera2D::~Camera2D(void) {
}

void Camera2D::setPosition(const Vec2& position,float scale) {
	m_Position = position;
	if ( scale != 1.0f ) {
		m_Position *= scale;
	}
	m_Scale = scale;
	m_Matrix = ds::matrix::mat3Translation(-m_Position.x,-m_Position.y);
	m_WMatrix = ds::matrix::mat3Translation(m_Position.x,m_Position.y);
}

Vec2 Camera2D::transform(const Vec2& pos) {
	return m_Matrix * pos;
}

Vec2 Camera2D::transformToWorld(const Vec2& screenPos) {
	return m_WMatrix * screenPos;
}

}
