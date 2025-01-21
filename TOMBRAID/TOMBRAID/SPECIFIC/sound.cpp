#include <windows.h>
#include "sound.h"
#include "vars.h"
#include "phd_math.h"
#include <stdio.h>

#include <cmath>

//static int bPlaying[256] = {0};

LPDIRECTSOUNDBUFFER g_pPrimaryBuffer = NULL;

my_sound activeBuffers[256];

int m_DecibelLUT[DECIBEL_LUT_SIZE] = { 0 };


double log2(double x)
{
    return log(x) / log(2.0); // »спользуем логарифм по основанию e
}


int FindSlot()
{

	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			return i;
	}

	return -1;
}

void CleanupBuffers()
{
	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			continue;

		DWORD status;
		activeBuffers[i].Buffers->GetStatus(&status);
		
		if (!(status & DSBSTATUS_PLAYING))
		{
			activeBuffers[i].Buffers->Release();
			activeBuffers[i].Buffers = NULL;
			//int sample = activeBuffers[i].SFX_num;
			//bPlaying[sample] = 0;
			activeBuffers[i].SFX_num = -1;
		}
	}
}

void SetCurrVolume(int nSample, int Volume)
{	
	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			continue;


		if(activeBuffers[i].SFX_num == nSample)
		{
			activeBuffers[i].Buffers->SetVolume(Volume);
		}
	}
}

void DS_StopSample(int nSample)
{
	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			continue;


		if(activeBuffers[i].SFX_num == nSample)
		{

			activeBuffers[i].Buffers->Stop();
			activeBuffers[i].Buffers->Release();
			activeBuffers[i].Buffers = NULL;
			//int sample = activeBuffers[i].SFX_num;
			//bPlaying[sample] = 0;
			activeBuffers[i].SFX_num = -1;

			/*
		DWORD status;
		activeBuffers[i].Buffers->GetStatus(&status);
		
		if ((status & DSBSTATUS_PLAYING))
		{
			activeBuffers[i].Buffers->Stop();
			activeBuffers[i].Buffers->Release();
			activeBuffers[i].Buffers = NULL;
			//int sample = activeBuffers[i].SFX_num;
			//bPlaying[sample] = 0;
			activeBuffers[i].SFX_num = -1;
		}
		*/
		}
	}

}

int IsPlaying(int nSample, int distance)
{
	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			continue;

		DWORD status;
		activeBuffers[i].Buffers->GetStatus(&status);
		
		if ((status & DSBSTATUS_PLAYING))
		{
			if(activeBuffers[i].SFX_num == nSample)
			{
					return 1;
			}
		}
	}

	return 0;
}


int IsDistance(int nSample, int distance)
{
	for (int i = 0; i < 256; i++)
	{
		if (activeBuffers[i].Buffers == NULL)
			continue;

		DWORD status;
		activeBuffers[i].Buffers->GetStatus(&status);
		
		if ((status & DSBSTATUS_PLAYING))
		{
			if(activeBuffers[i].SFX_num == nSample)
			{
				if(activeBuffers[i].distance < distance)
				{
					return 0;
				}
				else
				{
					activeBuffers[i].distance = distance;
					return 1;
				}
			}
		}
	}

	return 0;
}



LPDIRECTSOUND8 g_DirectSound;
IDirectSoundBuffer* g_DSBuffer[256];
int LanSampleFrequency[256];


/*

inline bool DX_TRY(HRESULT hr)
	{
	if (SUCCEEDED(hr))
		return false;
	//Log(ERR_Describe_DX(hr));
	return true;
	}
*/
void Sound_UpdateEffects()
{
	
    for (int i = 0; i < g_NumberSoundEffects; i++)
	{
        OBJECT_VECTOR *sound = &g_SoundEffectsTable[i];
        
		if (g_FlipStatus && (sound->flags & SOUND_FLIPFLAG))
		{
            Sound_Effect(sound->data, (PHD_3DPOS *)sound, SPM_NORMAL);
        }
		else if (!g_FlipStatus && (sound->flags & SOUND_UNFLIPFLAG))
		{
            Sound_Effect(sound->data, (PHD_3DPOS *)sound, SPM_NORMAL);
        }
    }

	if (g_FlipEffect != -1)
	{
        g_EffectRoutines[g_FlipEffect](NULL);
    }
}


HRESULT DirectSound_Init()
{

	for (int i = 0; i < 256; i++)
	{
		activeBuffers[256].Buffers = NULL;
		activeBuffers[256].SFX_num = -1;

	}

	
    m_DecibelLUT[0] = -10000;
    for (int i = 1; i < DECIBEL_LUT_SIZE; i++)
	{
        m_DecibelLUT[i] = (int) ( (log2(1.0 / DECIBEL_LUT_SIZE) - log2(1.0 / i)) * 1000);
    }
	
	HRESULT hr;

    if( FAILED( hr = DirectSoundCreate8( NULL, &g_DirectSound, NULL ) ) )
        return S_OK; 

    // Set DirectSound coop level 
    if( FAILED( hr = g_DirectSound->SetCooperativeLevel( g_hWnd, DSSCL_EXCLUSIVE ) ) )
        return S_OK; 

	
	//create primary sound buffer

	// —оздание primary buffer
    DSBUFFERDESC dsbd = {};
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0; // Ќе задаетс€ дл€ primary buffer
    dsbd.lpwfxFormat = NULL; // ‘ормат будет задаватьс€ отдельно

    if (FAILED(g_DirectSound->CreateSoundBuffer(&dsbd, &g_pPrimaryBuffer, NULL)))
	{
        //std::cerr << "Failed to create primary buffer!" << std::endl;
        return false;
    }

	// ”становка формата primary buffer
    WAVEFORMATEX wfex = {};
    wfex.wFormatTag = WAVE_FORMAT_PCM;
    wfex.nChannels = 2; // —терео
    wfex.nSamplesPerSec = 44100; // „астота дискретизации
    wfex.wBitsPerSample = 16; // 16 бит
    wfex.nBlockAlign = (wfex.nChannels * wfex.wBitsPerSample) / 8;
    wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

    if (FAILED(g_pPrimaryBuffer->SetFormat(&wfex)))
	{
        //std::cerr << "Failed to set format for primary buffer!" << std::endl;
        return false;
    }



	/*
	    DSBUFFERDESC dsbd;
    //InitDXStruct(dsbd);
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwFlags=DSBCAPS_PRIMARYBUFFER;
	

	if (DX_TRY(g_DirectSound->CreateSoundBuffer(&dsbd,&pPrimary,NULL)))
		{
			//Log(LT_Error,"Can't get primary sound buffer");
			return S_OK;
		}

	WAVEFORMATEX wf;
	ZeroMemory(&wf, sizeof(WAVEFORMATEX));
	wf.wFormatTag=WAVE_FORMAT_PCM;
	wf.nChannels=2;
	wf.nSamplesPerSec=11025;
	wf.nAvgBytesPerSec=11025*2*2;
	wf.nBlockAlign=4;
	wf.wBitsPerSample=16;

	if (DX_TRY(pPrimary->SetFormat(&wf)))
		{
		//Log(LT_Error,"Can't set sound output format to 11025;16;2");
		return S_OK;
		}
	//RELEASE(pPrimary);

	pPrimary->Release();

*/


	return S_OK;

}

bool Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags)
{
	//если камера под водой то например водопад Natla Mines не звучит
	if (flags != SPM_ALWAYS && (flags & SPM_UNDERWATER) != (g_RoomInfo[g_Camera.pos.room_number].flags & RF_UNDERWATER))
	{
		return false;
	}


	if (g_SampleLUT[sfx_num] < 0)
	{
        return false;
    }

	SOUND_SAMPLE_INFO *pSI = &g_SampleInfos[g_SampleLUT[sfx_num]];


	if (pSI->randomness && Random_GetDraw() > (int32_t)pSI->randomness)
	{
        return false;
    }

    //flags = 0;
    //int32_t pan = 0x7FFF;
    int32_t mode = pSI->flags & 3;
    uint32_t distance;

	if (pos)
	{
        int32_t x = pos->x - g_Camera.target.x;
        int32_t y = pos->y - g_Camera.target.y;
        int32_t z = pos->z - g_Camera.target.z;
        
		if (ABS(x) > SOUND_RADIUS || ABS(y) > SOUND_RADIUS || ABS(z) > SOUND_RADIUS)
		{
            return false;
        }
        
		distance = SQUARE(x) + SQUARE(y) + SQUARE(z);
        
		/*
		if (!distance)
		{
            pan = 0;
        }
		*/
    }
	else
	{
        distance = 0;
        //pan = 0;
    }
    
	distance = phd_sqrt(distance);


	
	int d = distance * SOUND_RANGE_MULT_CONSTANT;
	//volume может быть отрицательным
	//тогда будут искажени€
	//поэтому ограничиваем d
	if(d > pSI->volume)
		d = pSI->volume;
	int32_t volume = pSI->volume - d;
	
	
	//int32_t volume = pSI->volume - distance * SOUND_RANGE_MULT_CONSTANT;
	//if(volume < 0)
	//	volume = 0;

	/*
	if(nSample == 74)
	{
					char buff[256];
					sprintf(buff, "Volume#: %d\ - %d\n", nSample, wVol); 
					LPSTR buffToScreen = buff;
					OutputDebugString(buffToScreen);
	}
	*/


	
	//57 шум водопада Natla Mines
	/*
	if(pSI->number == 74)
	{
		char buff[256];
		sprintf(buff, "Sound#: %d - %d - %d - %d\n", pSI->volume, volume, distance, pSI->number); 
		LPSTR buffToScreen = buff;
		OutputDebugString(buffToScreen);
	}
	*/
	

	if (pSI->flags & SAMPLE_FLAG_VOLUME_WIBBLE)
	{
        volume -= Random_GetDraw() * SOUND_MAX_VOLUME_CHANGE >> 15;
    }
	
    if (volume <= 0 && mode != SOUND_MODE_AMBIENT)
	{
        return false;
    }

    if (volume > SOUND_MAX_VOLUME)
	{
        volume = SOUND_MAX_VOLUME;
    }

	int32_t m_MasterVolume = 51;

	volume = (m_MasterVolume * volume) >> 6;

	//int wVol = -10000;	// -> no sound
	//int wVol = 0;			// -> max volume
	int wVol = m_DecibelLUT[(volume & 0x7FFF) >> 6];

	/*
	char buff[256];
	sprintf(buff, "Volume#: %d\n", pSI->number); 
	LPSTR buffToScreen = buff;
	OutputDebugString(buffToScreen);
	*/
	

	int nSample=pSI->number;

	//например рычаг под водой Cistern уровень
	//имеет nSample = -1
	if(nSample < 0)
		return false;

	//распредел€ет звук между левым правым динамиком 0 - средина
	int wPan = 0;

	//частота freqency
	int nPitch = 0; // тоже работает с нулем
	//int nPitch = pSI->pitch;


	

	switch (mode)
	{
		case SOUND_MODE_WAIT:
		{
			//например Ћара наталкиваетс€ на стену- звук
			//открываетс€ дверь например в конце Lost Valley
			//под водопадом
			CleanupBuffers();
			if(!IsPlaying(nSample, distance))
				DS_StartSample(nSample, wVol ,nPitch,wPan, 0, distance);

			break;
		}

		case SOUND_MODE_RESTART:
		{
			//шум катитьс€ камень в горки Tomb Of Qualopec
			//шум мотора механизма moving_bar Natla Mines
			//звук шагов лары номер 0
			DS_StopSample(nSample);
			DS_StartSample(nSample, wVol ,nPitch,wPan, 0, distance);

			break;
		}

		case SOUND_MODE_AMBIENT:
		{
			//если убрать эту функцию то звук водопада
			//будет мен€тьс€ резко, шагами а не плавно
			//например началс€ звук водопада, Ћара на одном
			//рассто€нии - потом Ћара отбежала от водопада,
			//и водопад должен быть тише, но взук мен€етс€ резко
			//каждый раз при начале воспроизведени€ звука водопада
			//а эта функци€ плавно мен€ет звук водопада в зависимости
			//от рассто€ни€ Ћары от водопада
			//но надо добработать в уровне Lost Valley глюки со звуком
			if(IsDistance(nSample, distance))
				SetCurrVolume(nSample, wVol);

			if(!IsPlaying(nSample, distance))
				DS_StartSample(nSample, wVol ,nPitch,wPan, 0, distance);

			//шум водопада 57 начало Natla Mines
			CleanupBuffers();

			break;
		}
	}


	/*
	//провер€ем закончилось ли воспроизведение
	CleanupBuffers();

	//if(!bPlaying[nSample])
	if(!IsPlaying(nSample))
	{
		DS_StartSample(nSample, wVol ,nPitch,wPan, 0);
		//DS_StartSample(nSample, wVol ,nPitch,wPan,DSBPLAY_LOOPING);
		//DS_StartSample(7, wVol ,nPitch,wPan,DSBPLAY_LOOPING);
		//bPlaying[nSample] = 1;
	}
	*/

	/*
	if(!SoundBufferIsPlaying(nSample))
	{
		bPlaying[nSample] = 0;
	}*/
 
	return true;
}

/*
bool SoundBufferIsPlaying(int nSample)
{

	if(nSample == -1)
		return false;

	DWORD dwStatus;
	g_DSBuffer[nSample]->GetStatus(&dwStatus);

	if (dwStatus&DSBSTATUS_PLAYING)
		return true;

	return false;
}
*/

HRESULT DS_MakeSample(int nSample, WAVEFORMATEX* pWF, unsigned char *pWaveData, int dwWaveLength)
{
	
	DSBUFFERDESC desc;
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize=sizeof desc;
	desc.dwFlags=DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_LOCSOFTWARE | DSBCAPS_STATIC;	//DSBCAPS_STATIC;
	desc.dwBufferBytes=dwWaveLength;
	desc.lpwfxFormat=pWF;
	desc.dwReserved=0;


	if (FAILED(g_DirectSound->CreateSoundBuffer(&desc,g_DSBuffer+nSample,NULL)))
		return S_FALSE;


	void* pBlock;
	DWORD dwBlockLength;

	if (FAILED(g_DSBuffer[nSample]->Lock(0,dwWaveLength,&pBlock,&dwBlockLength,NULL,NULL,0)))
		return S_FALSE;
	
	memcpy(pBlock,(void*)pWaveData,dwBlockLength);
	
	if (FAILED(g_DSBuffer[nSample]->Unlock(pBlock,dwBlockLength,NULL,0)))
		return S_FALSE;

	LanSampleFrequency[nSample]=pWF->nSamplesPerSec;

	return S_OK;
}


int DS_StartSample(int nSample,int nVolume,int nPitch,int nPan,DWORD dwFlags, int distance)
{

	
	//этот вариант напр€мую проигрывать тоже работает
	//без копировани€ буфера в буфер
	/*
	g_DSBuffer[nSample]->SetVolume(0);
	g_DSBuffer[nSample]->SetFrequency(DSBFREQUENCY_ORIGINAL);
	g_DSBuffer[nSample]->SetPan(0);
	g_DSBuffer[nSample]->SetCurrentPosition(0);
	g_DSBuffer[nSample]->Play(0,0,dwFlags);
	
	
	return 1;
	*/
	

	LPDIRECTSOUNDBUFFER pDuplicateBuffer = NULL;
if (FAILED(g_DirectSound->DuplicateSoundBuffer(g_DSBuffer[nSample], &pDuplicateBuffer)))
{
    return 0; // ќшибка дублировани€
}

// Ќастраиваем параметры дублированного буфера
//pDuplicateBuffer->SetVolume(0);
pDuplicateBuffer->SetVolume(nVolume);
pDuplicateBuffer->SetFrequency(DSBFREQUENCY_ORIGINAL);
pDuplicateBuffer->SetPan(0);
pDuplicateBuffer->SetCurrentPosition(0);
pDuplicateBuffer->Play(0, 0, dwFlags);

int slot = FindSlot();
activeBuffers[slot].Buffers = pDuplicateBuffer;
activeBuffers[slot].SFX_num = nSample;
activeBuffers[slot].distance = distance;


//pDuplicateBuffer->Release();


return 1;
/*

	IDirectSoundBuffer* pOrigBuffer=g_DSBuffer[nSample], *pBuffer;
	
	
	//if (DX_TRY(g_DirectSound->DuplicateSoundBuffer(pOrigBuffer,&pBuffer)))
	//	return 0;

	//pBuffer = CreateCopy(pOrigBuffer);

	//unsigned int nFrequency=((unsigned)nPitch*LanSampleFrequency[nSample])>>16;
	//unsigned int nFrequency= DSBFREQUENCY_ORIGINAL;
	unsigned int nFrequency= LanSampleFrequency[nSample];
	//int nFrequency=(nPitch*LanSampleFrequency[nSample])>>16;
	
	if (DX_TRY(pBuffer->SetVolume(nVolume)))
		return 0;
	if (DX_TRY(pBuffer->SetFrequency(nFrequency)))
		return 0;

	if (DX_TRY(pBuffer->SetPan(nPan)))
		return 0;

	if (DX_TRY(pBuffer->SetCurrentPosition(0)))
		return 0;

	if (DX_TRY(pBuffer->Play(0,0,dwFlags)))
		return 0;

	return 1;
*/	
}

