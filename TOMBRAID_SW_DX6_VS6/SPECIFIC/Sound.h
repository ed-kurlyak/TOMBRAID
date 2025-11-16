#include "types.h"

#include <dsound.h>
#pragma comment(lib, "dsound.lib")

#define DECIBEL_LUT_SIZE 512
#define SOUND_FLIPFLAG 0x40
#define SOUND_UNFLIPFLAG 0x80
#define SOUND_RANGE 8
#define SOUND_RADIUS (SOUND_RANGE << 10)
#define SOUND_MAX_VOLUME 0x7FFF
#define SOUND_RANGE_MULT_CONSTANT                                              \
	((int32_t)((SOUND_MAX_VOLUME + 1) / SOUND_RADIUS))
#define SOUND_MAX_VOLUME_CHANGE 0x2000
#define SOUND_MAX_PITCH_CHANGE 10
#define SOUND_NOT_AUDIBLE -1

//#define MAX_SAMPLES 1000
#define MAX_ACTIVE_SAMPLES 10

#define AUDIO_NO_SOUND (-1)

typedef struct SOUND_SLOT
{
	int sound_id;
	PHD_3DPOS *pos;
	// PHD_3DPOS pos;
	// uint32_t loudness;
	// int16_t volume;
	// int16_t pan;
	// int16_t fxnum;
	int16_t flags;
	DWORD bufferLength;
	LPDIRECTSOUNDBUFFER Buffers;
} SOUND_SLOT;

typedef enum SOUND_MODE
{
	SOUND_MODE_WAIT = 0,
	SOUND_MODE_RESTART = 1,
	SOUND_MODE_AMBIENT = 2,
} SOUND_MODE;

typedef enum SOUND_FLAG
{
	SOUND_FLAG_UNUSED = 0,
	SOUND_FLAG_USED = 1 << 0,
	SOUND_FLAG_AMBIENT = 1 << 1,
	SOUND_FLAG_RESTARTED = 1 << 2,
} SOUND_FLAG;

typedef struct AUDIO_SAMPLE
{
	float *sample_data;
	int channels;
	int num_samples;
} AUDIO_SAMPLE;

typedef struct AUDIO_SAMPLE_SOUND
{
	bool is_used;
	bool is_looped;
	bool is_playing;
	float volume_l; // sample gain multiplier
	float volume_r; // sample gain multiplier

	float pitch;
	int volume; // volume specified in hundredths of decibel
	int pan;	// pan specified in hundredths of decibel

	// pitch shift means the same samples can be reused twice, hence float
	float current_sample;

	// AUDIO_SAMPLE* sample;
	LPDIRECTSOUNDBUFFER sample;
} AUDIO_SAMPLE_SOUND;

struct MySound
{
	LPDIRECTSOUNDBUFFER Buffers;
	int SFX_num;
	int distance;
};

// static AUDIO_SAMPLE_SOUND m_SampleSounds[MAX_ACTIVE_SAMPLES] = { 0 };

static SOUND_SLOT m_SFXPlaying[MAX_PLAYING_FX] = {0};
static int32_t m_MasterVolume = 0;
// static int32_t m_MasterVolumeDefault = 0;
// static int16_t m_AmbientLookup[MAX_AMBIENT_FX] = { 0 };
// static int32_t m_AmbientLookupIdx = 0;
static int m_DecibelLUT[DECIBEL_LUT_SIZE] = {0};
static bool m_SoundIsActive = true;

void Sound_SetMasterVolume(int8_t volume);
void Sound_UpdateEffects();
void Sound_ResetEffects();
SOUND_SLOT *Sound_GetSlot(int32_t sfx_num, PHD_3DPOS *pos);
void DS_StopSample(SOUND_SLOT *slot);
HRESULT DirectSound_Init();
bool Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags);
HRESULT DS_MakeSample(int nSample, WAVEFORMATEX *pWF, unsigned char *pWaveData,
					  int dwWaveLength);
int DS_StartSample(SOUND_SLOT *slot, int nVolume, int nPitch, int nPan,
				   DWORD dwFlags);
