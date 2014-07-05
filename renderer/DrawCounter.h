#pragma once
#include "..\dxstdafx.h"

namespace ds {
// -------------------------------------------------------
// DrawCounter
//
// Statistic class to gather rendering related metrics
// -------------------------------------------------------
class DrawCounter {

public:
	DrawCounter();
	~DrawCounter();
	void reset();
	void addIndices(int indCounter);
	void addPrimitives(int primCounter);
	void addSprite() {
		++m_Sprites;
	}
	void addDrawCall() {
		++m_DrawCalls;
	}
	void addTexture() {
		++m_Textures;
	}
	void addShader() {
		++m_Shaders;
	}
	void addMemory(uint32 size) {
		m_MemorySize += size;
	}
	void removeMemory(uint32 size) {
		m_MemorySize -= size;
	}
	const uint32 getMemorySize() {
		return m_MemorySize;
	}
	const int getIndexCounter() const { 
		return m_IndexCounter; 
	}
	const int getPrimitiveCounter() const { 
		return m_NumPrim; 
	}
	const int getSpriteCounter() const { 
		return m_Sprites; 
	}
	const int getDrawCalls() const {
		return m_DrawCalls;
	}
	const int getTextures() const {
		return m_Textures;
	}
	const int getShaders() {
		return m_Shaders;
	}
	void incFlushes() {
		++m_Flushes;
	}
	const int getFlushes() const {
		return m_Flushes;
	}
	void addParticles(int particles) {
		m_Particles += particles;
	}
	const int getParticles() const {
		return m_Particles;
	}
private:
	int m_NumPrim;
	int m_IndexCounter;
	int m_Sprites;
	int m_DrawCalls;
	int m_Textures;
	int m_Shaders;
	uint32 m_MemorySize;
	int m_Flushes;
	int m_Particles;
};

};

