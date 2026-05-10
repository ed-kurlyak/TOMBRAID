#ifndef _SOUND_
#define _SOUND_

#include "types.h"

extern "C"
{


#include ".\hmi\sos.h"
#include ".\hmi\sosm.h"
#include ".\hmi\sosez.h"


	// local data for digital driver
	extern _SOS_DIGI_DRIVER sDIGIDriver;               // digital driver structure
	extern HANDLE hDIGIDriver;                              // handle to digital driver
	extern HANDLE hDIGITimer;                               // handle to digital mixer

	//HANDLE hDIGITimer_My = -1;                               // handle to digital mixer

	extern W32 wDIGIDeviceID;
	extern W32 wMIDIDeviceID;
	extern W32 hDIGIDriver;
	//W32 hMIDIDriver;

	extern HANDLE G_timerHandle;

	//extern HANDLE G_timerHandle;

	/*

	#include ".\hmi\sos.h"
	#include ".\hmi\sosm.h"
	#include ".\hmi\sosez.h"

			extern W32 wDIGIDeviceID;
			extern W32 wMIDIDeviceID;
			extern W32 hDIGIDriver;
			extern W32 hMIDIDriver;

			*/

}

//PSOSSAMPLE s;

//W32 h;

VOID TickerInc(VOID);



//#include <dsound.h>
//#pragma comment(lib, "dsound.lib")

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


#pragma pack (push,1)


typedef struct SOUND_SLOT
{
        int sound_id;
        PHD_3DPOS *pos;

		int Looped; //удалить не используется

		uint32_t JustLoaded;
        // PHD_3DPOS pos;
        // uint32_t loudness;
        int16_t volume;
        // int16_t pan;
        // int16_t fxnum;
        int16_t flags;
        unsigned int bufferLength;
        //LPDIRECTSOUNDBUFFER Buffers;
                unsigned char* Sound_Buff;

                W32 sound_handler;

				_SOS_SAMPLE sample;

                //SoundBuffStruct Sound_Buff;
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
        int32_t is_used;
        int32_t is_looped;
        int32_t is_playing;
        float volume_l; // sample gain multiplier
        float volume_r; // sample gain multiplier

        float pitch;
        int volume; // volume specified in hundredths of decibel
        int pan;        // pan specified in hundredths of decibel

        // pitch shift means the same samples can be reused twice, hence float
        float current_sample;

        // AUDIO_SAMPLE* sample;
        //LPDIRECTSOUNDBUFFER sample;
} AUDIO_SAMPLE_SOUND;

struct MySound
{
        //LPDIRECTSOUNDBUFFER Buffers;
        int SFX_num;
        int distance;
};

#pragma pack (pop)

// static AUDIO_SAMPLE_SOUND m_SampleSounds[MAX_ACTIVE_SAMPLES] = { 0 };

extern SOUND_SLOT m_SFXPlaying[MAX_PLAYING_FX];

static int32_t m_MasterVolume = 0;
// static int32_t m_MasterVolumeDefault = 0;
// static int16_t m_AmbientLookup[MAX_AMBIENT_FX] = { 0 };
// static int32_t m_AmbientLookupIdx = 0;
static int m_DecibelLUT[DECIBEL_LUT_SIZE] = {0};
static int32_t m_SoundIsActive = 1;

void Sound_StopSample(SOUND_SLOT* slot);
void Sound_SetMasterVolume(int8_t volume);
void Sound_UpdateEffects();
void Sound_ResetEffects();
SOUND_SLOT* Sound_GetSlot(int32_t sfx_num, PHD_3DPOS* pos);
SOUND_SLOT* Sound_GetSlot(int32_t sfx_num, PHD_3DPOS* pos);
void DS_StopSample(SOUND_SLOT *slot);
//HRESULT DirectSound_Init();
void Sound_Init();

int Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags);

void Sound_MakeSample(unsigned int i, unsigned char* sample, unsigned int size);

void ZeroSoundBuff();

#pragma pack (push,1)

/*
struct SoundBuffStruct
{
	unsigned char* Buff_data; //256 max sound samples
    unsigned int data_size;
};

//тут можно просто сделать unsigned char * Sound_Buff[256] = { 0 };
//extern SoundBuffStruct Sound_Buff[256];
*/


extern unsigned char* Sound_Buff_Data[256];

#pragma pack (pop)






/*
extern "C"
{

#include ".\hmi\sos.h"
#include ".\hmi\sosm.h"
#include ".\hmi\sosez.h"

}
*/


//HRESULT DS_MakeSample(int nSample, WAVEFORMATEX *pWF, unsigned char *pWaveData,

/*
void Sound_MakeSample(int nSample, _WAVHEADER *pWF, unsigned char *pWaveData,
                                          int dwWaveLength);
*/

//int DS_StartSample(SOUND_SLOT *slot, int nVolume, int nPitch, int nPan,
int Sound_StartSample(SOUND_SLOT *slot, W32 nVolume, int nPitch, int nPan,
                                   unsigned int dwFlags);

int Sound_StartLoopedSample(SOUND_SLOT* slot, W32 nVolume, int nPitch, int nPan,
	unsigned int dwFlags);

void SetCurrVolume(SOUND_SLOT* slot, W32 Volume);

void Sound_UnInit();

void Sound_StopAllSamples();

#endif
