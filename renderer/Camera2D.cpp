#include "Camera2D.h"
#include "..\math\matrix.h"
#include "..\math\GameMath.h"

namespace ds {

Camera2D::Camera2D(void) {
}


Camera2D::~Camera2D(void) {
}

void Camera2D::setPosition(const Vector2f& position,float scale) {
	m_Position = position;
	if ( scale != 1.0f ) {
		m_Position *= scale;
	}
	m_Scale = scale;
	m_Matrix = ds::matrix::mat3Translation(-m_Position.x,-m_Position.y);
	m_WMatrix = ds::matrix::mat3Translation(m_Position.x,m_Position.y);
}

Vector2f Camera2D::transform(const Vector2f& pos) {
	return m_Matrix * pos;
}

Vector2f Camera2D::transformToWorld(const Vector2f& screenPos) {
	return m_WMatrix * screenPos;
}

}
