#include "types.h"

#include <dsound.h>
#pragma comment (lib, "dsound.lib")

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

typedef enum SOUND_MODE {
    SOUND_MODE_WAIT = 0,
    SOUND_MODE_RESTART = 1,
    SOUND_MODE_AMBIENT = 2,
} SOUND_MODE;


struct my_sound
{
	LPDIRECTSOUNDBUFFER Buffers;
	int SFX_num;
	int distance;
};

void Sound_UpdateEffects();


HRESULT DirectSound_Init();

bool Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags);

HRESULT DS_MakeSample(int nSample, WAVEFORMATEX* pWF, unsigned char *pWaveData, int dwWaveLength);

int DS_StartSample(int nSample,int nVolume,int nPitch,int nPan,DWORD dwFlags, int distance);

bool SoundBufferIsPlaying(int nSample);

