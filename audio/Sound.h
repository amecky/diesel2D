#pragma once
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\dxstdafx.h"
#include <string>
#include <io.h>
#include <mmreg.h>

namespace ds {

class Sound {

public:
	Sound(const char* name);
	virtual ~Sound();
	bool load(const char* fileName);	
	int loadWavFile(const char* filename);
	char const *GetPCMBuffer() const { return m_PCMBuffer; }
	int GetPCMBufferSize() const { return m_PCMBufferSize; }	
	WAVEFORMATEX const *GetFormat() { return &m_WavFormatEx; }
	int GetLengthMilli() const { return m_LengthMilli; }
	const char* getName() const {
		return m_Name;
	}
private:
	const char* m_Name;
	bool parseWave(FILE* file);
	//std::string m_FileName;
	bool m_bInitialized;
	char *m_PCMBuffer;
	int m_PCMBufferSize;
	WAVEFORMATEX m_WavFormatEx;
	int m_LengthMilli;
	WORD channels;
  	WORD bits;
  	WORD sampleRate;
};

};