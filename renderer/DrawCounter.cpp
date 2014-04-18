#include "DrawCounter.h"

namespace ds {

DrawCounter::DrawCounter(void) : m_MemorySize(0) {
	reset();
}


DrawCounter::~DrawCounter(void) {
}

void DrawCounter::reset() {
	m_IndexCounter = 0;
	m_NumPrim = 0;
	m_Sprites = 0;
	m_Textures = 0;
	m_DrawCalls = 0;
	m_Shaders = 0;	
	m_Flushes = 0;
}

void DrawCounter::addIndices(int indCounter) {
	m_IndexCounter += indCounter;
}

void DrawCounter::addPrimitives(int primCounter) {
	m_NumPrim += primCounter;
}

}