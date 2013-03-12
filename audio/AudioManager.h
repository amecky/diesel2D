#pragma once
#include <dsound.h>
#include <mmsystem.h>
#include "..\dxstdafx.h"
#include "AudioBuffer.h"
#include <map>
#include "..\lib\container\List.h"
#include "Sound.h"

namespace ds {

class AudioBuffer;
// -------------------------------------------------------
// AudioManager
//
// Loads and manages sounds. Also plays sounds.
// -------------------------------------------------------
class AudioManager {

struct AudioSound {
	IdString hash;
	char name[32];
	AudioBuffer* buffer;
};

typedef List<AudioSound> AudioBuffers;

public:
	AudioManager();
	~AudioManager(void);
	bool initialize(HWND hWnd);	
	bool play(const char* soundName,int volume = 100, bool looping = false);
	void stop(const char* soundName);
	bool isPlaying(const char* soundName);
	//
	void loadSound(const char* name);
private:
	bool createAudioBuffer(Sound* sound);
	AudioBuffer* findByName(const char* name);
	bool setPrimaryBufferFormat(DWORD dwPrimaryChannels,DWORD dwPrimaryFreq,DWORD dwPrimaryBitRate );
	bool m_Initialized;
	IDirectSound8* m_pDS;
	AudioBuffers m_AudioBuffers;

};

};

