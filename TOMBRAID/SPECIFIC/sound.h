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

	

}

VOID TickerInc(VOID);

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
	//int sound_id;
	W32 sound_id;
	PHD_3DPOS* pos;
	uint32_t loudness;
	int16_t volume;
	int16_t pan;
	int16_t fxnum;
	int16_t flags;
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

		_SOS_SAMPLE sample;
         
} AUDIO_SAMPLE_SOUND;


#pragma pack (pop)

extern SOUND_SLOT m_SFXPlaying[MAX_PLAYING_FX];

static int32_t m_MasterVolume = 0;
static int m_DecibelLUT[DECIBEL_LUT_SIZE] = {0};
static int32_t m_SoundIsActive = 1;

void Sound_SetMasterVolume(int8_t volume);
void Sound_UpdateEffects();
void Sound_ResetEffects();
void Sound_ResetAmbientLoudness();
void Sound_StopAmbientSounds();
W32 Play_Driver_Sound_Ambient(int sample_index, int volume, int pan, int pitch);
W32 S_SoundPlayAmbientSample(int sample_index, int volume, int pan, int pitch);
static void Sound_ClearSlot(SOUND_SLOT* slot);
bool S_Audio_SampleSoundIsPlaying(int sound_id);
static SOUND_SLOT* Sound_GetSlot(int32_t sfx_num, uint32_t loudness,
	PHD_3DPOS* pos, int16_t mode);
void Sound_Init();
int Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags);
void Sound_MakeSample(unsigned int i, unsigned char* sample, unsigned int size);
void ZeroSoundBuff();
W32 S_SoundPlaySample(int sample_index, int volume, int pitch, int pan);
W32 Play_Driver_Sound(int sample_index, int volume, int pan, int pitch);
int Adjust_Pan(int pan);
void Sound_UnInit();
void Sound_StopAllSamples();
void SetCurrVolume(SOUND_SLOT* slot, W32 Volume);
//void SetCurrVolume(SOUND_SLOT* slot);
static void Sound_UpdateSlotParams(SOUND_SLOT* slot);

static int16_t m_AmbientLookup[MAX_AMBIENT_FX] = { 0 };
static int32_t m_AmbientLookupIdx = 0;
static AUDIO_SAMPLE_SOUND m_SampleSounds[MAX_ACTIVE_SAMPLES] = { 0 };

//extern unsigned char* Sound_Buff_Data[256];

struct Sound_Buff
{
	_SOS_SAMPLE sample;
	unsigned char* Buff_Data;
};

extern Sound_Buff Sound_Buff_Data[256];



#endif
