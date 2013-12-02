#include "AudioManager.h"
#include <comdef.h>
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"

namespace ds {

AudioManager::AudioManager(void) {
	m_Initialized = false;
}

// -------------------------------------------------------
// Destructor - removes all audio buffers
// -------------------------------------------------------
AudioManager::~AudioManager(void) {
	for ( uint32 i = 0; i < m_AudioBuffers.num();++i ) {
		LOGC(logINFO,"AudioManager") << "removing sound " << m_AudioBuffers[i].name;
		delete m_AudioBuffers[i].buffer;
	}
	SAFE_RELEASE( m_pDS );
}

// -------------------------------------------------------
// Initializes AudioManager
// -------------------------------------------------------
bool AudioManager::initialize(HWND hWnd) {	
	LOGC(logINFO,"AudioManager") << "Initialize";
	if(m_Initialized) {
		LOGC(logERROR,"AudioManager") << "Already initialized";
		return true;
	}
	m_Initialized=false;

	//SAFE_RELEASE( m_pDS );

	HRESULT hr;

	// Create IDirectSound using the primary sound device
	if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDS, NULL ) ) ) {
		LOGC(logERROR,"AudioManager") << "Failed to create direct sound";
		return false;
	}

	// Set DirectSound coop level 
	if( FAILED( hr = m_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY) ) ) {
		LOGC(logERROR,"AudioManager") << "Failed to set cooperative level";
		return false;
	}
	//if( !setPrimaryBufferFormat( 8, 44100, 16 ) )
	if( !setPrimaryBufferFormat( 2, 22050, 16 ) ) {
		LOGC(logERROR,"AudioManager") << "Failed to set primary buffer format";
		return false;
	}	
	m_Initialized = true;
	//m_AllSamples.clear();
	return true;
}

// -------------------------------------------------------
// Sets primary buffer format
// -------------------------------------------------------
bool AudioManager::setPrimaryBufferFormat(DWORD dwPrimaryChannels,DWORD dwPrimaryFreq,DWORD dwPrimaryBitRate ) {
	HRESULT             hr;
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

	if( m_pDS == NULL )
		return false;

	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize        = sizeof(DSBUFFERDESC);
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;

	if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) ) {
		LOG(logERROR) << "Error while creating sound buffer";
		return false;
	}

	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wfx.nChannels       = (WORD) dwPrimaryChannels; 
	wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
	wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) ) {
		LOGC(logERROR,"AudioManager") << "Error while setting format";
		return false;
	}

	SAFE_RELEASE( pDSBPrimary );

	return true;
}
// ------------------------------------------------
// This method will create a new AudioBuffer and
// store it in the internal map
// ------------------------------------------------
bool AudioManager::createAudioBuffer(Sound* sound) {
	LPDIRECTSOUNDBUFFER sampleHandle;
	if ( sound == NULL ) {
		LOG(logERROR) <<  "No sound " << sound->getName() << " available";
		return false;
	}
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; ;
    dsbd.dwBufferBytes   = sound->GetPCMBufferSize();
    dsbd.lpwfxFormat     = const_cast<WAVEFORMATEX *>(sound->GetFormat());
	HRESULT hr;
    if( FAILED( hr = m_pDS->CreateSoundBuffer( &dsbd, &sampleHandle, NULL ) ) ) {
		LOGC(logERROR,"AudioManager") <<  "Failed to create soundbuffer";
        return false;
    }

	// Add handle to the list
	AudioSound as;
	as.buffer = new AudioBuffer(sampleHandle,sound);
	as.hash = string::murmur_hash(sound->getName());
	strncpy(as.name,sound->getName(),32);
	m_AudioBuffers.append(as);
	LOGC(logINFO,"AudioManager") << "adding audio sound with hash " << as.hash;
	//audioBuffers[soundName] = audioBuffer;	
	return true;
}

// -------------------------------------------------------
// Plays sound with given name at given volume
// -------------------------------------------------------
bool AudioManager::play(const char* soundName,int volume, bool looping) {
	AudioBuffer* buffer = findByName(soundName);
	assert(buffer != 0);
	if ( buffer != 0 ) {
		if ( buffer->isPlaying() ) {
			buffer->stop();
		}
		return buffer->play(volume,looping);
	}
	return false;
}

// -------------------------------------------------------
// Checks if a sound is already playing
// -------------------------------------------------------
bool AudioManager::isPlaying(const char* soundName) {
	AudioBuffer* buffer = findByName(soundName);
	if ( buffer != 0 ) {
		return buffer->isPlaying();
	}
	return false;
}

// -------------------------------------------------------
// Stops sound
// -------------------------------------------------------
void AudioManager::stop(const char* soundName) {
	AudioBuffer* buffer = findByName(soundName);
	if ( buffer != 0 ) {
		buffer->stop();
	}
}

// -------------------------------------------------------
// Finds sound by name
// -------------------------------------------------------
AudioBuffer* AudioManager::findByName(const char* name) {
	IdString hash = string::murmur_hash(name);
	for ( uint32 i = 0; i < m_AudioBuffers.num();++i ) {
		if ( m_AudioBuffers[i].hash == hash ) {
			return m_AudioBuffers[i].buffer;
		}
	}
	return 0;
}

// -------------------------------------------------------
// Load sound
// -------------------------------------------------------
void AudioManager::loadSound(const char* name) {
	char fileName[256];
	sprintf(fileName,"content\\sounds\\%s.wav",name);
	Sound *s = new Sound(name);
	LOGC(logINFO,"AudioManager") << "loading sound " << fileName;
	int ret = s->loadWavFile(fileName);
	if ( ret > 0 ) {		
		createAudioBuffer(s);		
	}
	else {
		LOGC(logERROR,"AudioManager") << "Unable to load wav file";
	}
	delete s;
}

};
