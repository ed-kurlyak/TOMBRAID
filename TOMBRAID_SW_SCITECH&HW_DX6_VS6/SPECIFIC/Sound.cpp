#include "sound.h"
#include "phd_math.h"
#include "vars.h"
#include <stdio.h>
#include <windows.h>

#include <cmath>

// static int bPlaying[256] = {0};

LPDIRECTSOUNDBUFFER g_pPrimaryBuffer = NULL;

MySound activeBuffers[256];

// int m_DecibelLUT[DECIBEL_LUT_SIZE] = { 0 };

double log2(double x)
{
	return log(x) / log(2.0); // »спользуем логарифм по основанию e
}

void SetCurrVolume(SOUND_SLOT *slot, int Volume)
{
	if (slot->Buffers)
	{
		slot->Buffers->SetVolume(Volume);
	}
}

int IsPlaying(SOUND_SLOT *slot)
{
	if (slot->Buffers)
	{
		DWORD status;
		slot->Buffers->GetStatus(&status);

		if ((status & DSBSTATUS_PLAYING))
		{
			return 1;
		}
		else
		{
			// slot->Buffers->Stop();
			// slot->Buffers->Release();
			// slot->Buffers = NULL;

			// slot->flags = SOUND_FLAG_USED;

			return 0;
		}
	}

	return 0;
}

LPDIRECTSOUND g_DirectSound;
IDirectSoundBuffer *g_DSBuffer[256];
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

		// Sound_Effect(sound->data, (PHD_3DPOS*)sound, SPM_NORMAL);

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

bool Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags)
{

	if (!m_SoundIsActive)
	{
		return false;
	}

	//если камера под водой то например водопад Natla Mines не звучит
	if (flags != SPM_ALWAYS && (flags & SPM_UNDERWATER) !=	(g_RoomInfo[g_Camera.pos.room_number].flags & RF_UNDERWATER))
	{
		return false;
	}

	SOUND_SAMPLE_INFO *sample = &g_SampleInfos[g_SampleLUT[sfx_num]];

	//например рычаг под водой Cistern уровень
	//имеет pSI->number = -1
	if (sample->number < 0)
		return false;

	if (sample->volume < 0)
		return false;

	if (sample->randomness != 0)
	{
		if (Random_GetControl() > (int32_t)sample->randomness)
		{
			return false;
		}
	}

	//начало вариант кода из github

	/*

	int32_t mode = sample->flags & 3;
	uint32_t distance;

	if (pos)
	{

		int32_t x = pos->x - g_Camera.target.x;
		int32_t y = pos->y - g_Camera.target.y;
		int32_t z = pos->z - g_Camera.target.z;
		


		if (ABS(x) > SOUND_RADIUS || ABS(y) > SOUND_RADIUS || ABS(z) >
	SOUND_RADIUS)
		{
			return false;
		}
		


		distance = SQUARE(x) + SQUARE(y) + SQUARE(z);
	}
	else
	{
		distance = 0;
	}

	


	//distance = phd_sqrt(distance);
	//int32_t volume = sample->volume - distance * SOUND_RANGE_MULT_CONSTANT;
	//  орректировка громкости
	int32_t volume = sample->volume - phd_sqrt(distance) * 4;

	if (volume < 0)
		volume = 0;

	if (sample->flags & SAMPLE_FLAG_VOLUME_WIBBLE)
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

	//max = 63, min = 32, оптимально 51
	//g_Config.sound_volume
	//int32_t m_MasterVolume = 51;
	volume = (m_MasterVolume * volume) >> 6;

	//int wVol = -10000;	// -> no sound
	//int wVol = 0;			// -> max volume
	int wVol = m_DecibelLUT[(volume & 0x7FFF) >> 6];

	//int nSample= pSI->number;

	//распредел€ет звук между левым правым динамиком 0 - средина
	int wPan = 0;

	//частота freqency
	int pitch = 0; // тоже работает с нулем
	//int nPitch = pSI->pitch;

	if (wVol > 0)
	{
		wVol = 0;
	}

	if(wVol < -10000)
	{
		wVol = -10000;
	}
	*/
	//конец вариант кода из github

	/*
	//р€дом с 73 водопадом шум воды Lost Valley
	if (nSample == 74)
	{
		char buff[256];
		sprintf(buff, "Sound#: %d - %d - %d\n", pSI->number, distance, wVol);
		LPSTR buffToScreen = buff;
		OutputDebugString(buffToScreen);

	}

	if (nSample == 73)
	{
		char buff[256];
		sprintf(buff, "Sound#: %d - %d - %d\n", pSI->number, distance, wVol);
		LPSTR buffToScreen = buff;
		OutputDebugString(buffToScreen);

	}

	//57 шум водопада Natla Mines
	if (nSample == 57)
	{
		char buff[256];
		sprintf(buff, "Sound#: %d - %d - %d - %d\n", pSI->number, pSI->volume,
	volume, distance); LPSTR buffToScreen = buff;
		OutputDebugString(buffToScreen);
	}
	*/

	//шаги
	/*
	if (nSample == 0)
	{
		char buff[256];
		sprintf(buff, "Sound#: %d - %d - %d - %d\n", pSI->number, pSI->volume,
	distance, wVol); LPSTR buffToScreen = buff; OutputDebugString(buffToScreen);
	}
	*/

	//начало вариант кода из reverse engineering chatGPT

	int volume = sample->volume;
	// int pitch = sample->pitch;
	// int pitch = 100;
	int pitch = 0;
	int pan = 0;
	int distance = 0;

	int32_t mode = sample->flags & 3;

	if (pos)
	{

		int32_t dx = pos->x - g_Camera.target.x;
		int32_t dy = pos->y - g_Camera.target.y;
		int32_t dz = pos->z - g_Camera.target.z;

		if (dx < -8192 || dx > 8192 || dy < -8192 || dy > 8192 || dz < -8192 ||
			dz > 8192)
		{
			return 0;
		}

		distance = dx * dx + dy * dy + dz * dz;
	}

	//  орректировка громкости
	volume = volume - phd_sqrt(distance) * 4;

	// if (volume < 0)
	// volume = 0;

	if (volume < 0)
	{
		if (flags != SOUND_MODE_AMBIENT)
		{
			return 0;
		}
		volume = 0;
	}

	// —лучайные вариации высоты тона
	if (sample->flags & 0x40)
	// if (sample->flags & SAMPLE_FLAG_PITCH_WIBBLE)
	{
		pitch += (Random_GetControl() - 0x4000) >> 6;
	}

	if (volume > SOUND_MAX_VOLUME)
	{
		volume = SOUND_MAX_VOLUME;
	}

	// int32_t m_MasterVolume = 51;
	// max = 63, min = 32, оптимально 51
	// g_Config.sound_volume
	volume = (m_MasterVolume * volume) >> 6;

	// int wVol = -10000;	// -> no sound
	// int wVol = 0;			// -> max volume
	int wVol = m_DecibelLUT[(volume & 0x7FFF) >> 6];

	//распредел€ет звук между левым правым динамиком 0 - средина
	int wPan = 0;

	//частота freqency
	// int nPitch = 0; // тоже работает с нулем
	// int nPitch = pSI->pitch;

	/*
	if (wVol > 0)
	{
		wVol = 0;
	}

	if (wVol < -10000)
	{
		wVol = -10000;
	}
	*/

	//конец вариант кода из reverse engineering chatGPT

	//обработка разных типов звуков
	switch (mode)
	{
	//обычный звук - шаги лары, выстрелы, звук играет до
	//конца пока сам не остановитс€
	case SOUND_MODE_WAIT:
	{
		SOUND_SLOT *slot = Sound_GetSlot(sample->number, NULL);

		if (!IsPlaying(slot))
		{
			DS_StartSample(slot, wVol, pitch, wPan, 0);
		}

		break;
	}

	//звук с возможностью перезапуска - используетс€
	//в инвентаре при прокрутке кольца
	//если звук уже играет - он останавливаетс€ и
	//начинает играть сначала - сразу отклик на клавиатуру
	//например два раза нажимаешь на клавиатуру
	//звук может начатьс€ два раза и будет накладка без DS_StopSample()
	case SOUND_MODE_RESTART:
	{
		SOUND_SLOT *slot = Sound_GetSlot(sample->number, NULL);

		DS_StopSample(slot);
		DS_StartSample(slot, wVol, pitch, wPan, 0);

		break;
	}

	//позиционный звук с обновлением громкости
	//например шум водопада - Ћара ближе громкость больше
	//дальше- громкость ниже
	case SOUND_MODE_AMBIENT:
	{
		SOUND_SLOT *slot = Sound_GetSlot(sample->number, pos);

		SetCurrVolume(slot, wVol);

		if (!IsPlaying(slot))
		{
			DS_StartSample(slot, wVol, pitch, wPan, 0);
		}

		break;
	}
	}

	return true;
}

int DS_StartSample(SOUND_SLOT *slot, int nVolume, int nPitch, int nPan,
				   DWORD dwFlags)
{

	slot->Buffers->SetVolume(nVolume);
	slot->Buffers->SetFrequency(DSBFREQUENCY_ORIGINAL);
	slot->Buffers->SetPan(0);
	slot->Buffers->SetCurrentPosition(0);
	HRESULT hr = slot->Buffers->Play(0, 0, dwFlags);

	if (FAILED(hr))
	{
		// ќбработка ошибки
		MessageBox(NULL, "Ќе удалось воспроизвести звук", "ќшибка", MB_OK);
	}

	return 1;
}

HRESULT DS_MakeSample(int nSample, WAVEFORMATEX *pWF, unsigned char *pWaveData,
					  int dwWaveLength)
{

	DSBUFFERDESC desc;
	ZeroMemory(&desc, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof desc;
	// desc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
	// DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE | DSBCAPS_STATIC;
	desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC;
	desc.dwBufferBytes = dwWaveLength;
	desc.lpwfxFormat = pWF;
	desc.dwReserved = 0;

	if (FAILED(g_DirectSound->CreateSoundBuffer(&desc, g_DSBuffer + nSample,
												NULL)))
		return S_FALSE;

	void *pBlock;
	DWORD dwBlockLength;

	if (FAILED(g_DSBuffer[nSample]->Lock(0, dwWaveLength, &pBlock,
										 &dwBlockLength, NULL, NULL, 0)))
		return S_FALSE;

	memcpy(pBlock, (void *)pWaveData, dwBlockLength);

	if (FAILED(g_DSBuffer[nSample]->Unlock(pBlock, dwBlockLength, NULL, 0)))
		return S_FALSE;

	LanSampleFrequency[nSample] = pWF->nSamplesPerSec;

	return S_OK;
}

void Sound_ResetEffects() {}

HRESULT DirectSound_Init()
{

	for (int i = 0; i < MAX_PLAYING_FX; i++)
	{
		SOUND_SLOT *result = &m_SFXPlaying[i];
		memset(result, 0, sizeof(SOUND_SLOT));
		result->flags = SOUND_FLAG_UNUSED;
	}

	m_DecibelLUT[0] = -10000;
	//for (int i = 1; i < DECIBEL_LUT_SIZE; i++)
	for (i = 1; i < DECIBEL_LUT_SIZE; i++)
	{
		m_DecibelLUT[i] =
			(int)((log2(1.0 / DECIBEL_LUT_SIZE) - log2(1.0 / i)) * 1000);
	}

	HRESULT hr;

	if (FAILED(hr = DirectSoundCreate(NULL, &g_DirectSound, NULL)))
		return S_OK;

	// Set DirectSound coop level
	if (FAILED(hr =
				   g_DirectSound->SetCooperativeLevel(g_hWnd, DSSCL_EXCLUSIVE)))
		return S_OK;

	// create primary sound buffer

	// —оздание primary buffer
	DSBUFFERDESC dsbd = {0};
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;  // Ќе задаетс€ дл€ primary buffer
	dsbd.lpwfxFormat = NULL; // ‘ормат будет задаватьс€ отдельно

	if (FAILED(
			g_DirectSound->CreateSoundBuffer(&dsbd, &g_pPrimaryBuffer, NULL)))
	{
		// std::cerr << "Failed to create primary buffer!" << std::endl;
		return false;
	}

	// ”становка формата primary buffer
	WAVEFORMATEX wfex = {0};
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = 2;			 // —терео
	wfex.nSamplesPerSec = 44100; // „астота дискретизации
	wfex.wBitsPerSample = 16;	// 16 бит
	wfex.nBlockAlign = (wfex.nChannels * wfex.wBitsPerSample) / 8;
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

	if (FAILED(g_pPrimaryBuffer->SetFormat(&wfex)))
	{
		// std::cerr << "Failed to set format for primary buffer!" << std::endl;
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

	if (DX_TRY(g_pPrimaryBuffer->SetFormat(&wf)))
		{
		//Log(LT_Error,"Can't set sound output format to 11025;16;2");
		return S_OK;
		}
	//RELEASE(pPrimary);

	*/
	/*

	if (FAILED(g_pPrimaryBuffer->SetFormat(&wf)))
	{
		//std::cerr << "Failed to set format for primary buffer!" << std::endl;
		return false;
	}
	*/
	// pPrimary->Release();

	return S_OK;
}

SOUND_SLOT *Sound_GetSlot(int32_t sfx_num, PHD_3DPOS *pos)
{
	int i = 0;
	for (i = 0; i < MAX_PLAYING_FX; i++)
	{
		SOUND_SLOT *result = &m_SFXPlaying[i];

		if (pos != NULL)
		{

			if (result->sound_id == sfx_num && result->pos == pos &&
				result->flags != SOUND_FLAG_UNUSED)
			{
				//находим нужный слот по позиции и номеру звука
				return result;
			}
		}
		else
		{
			if (result->sound_id == sfx_num &&
				result->flags != SOUND_FLAG_UNUSED)
			{
				//находим нужный слот без позиции и номеру звука
				return result;
			}
		}
	}

	int j = 0;
	for (j = 0; j < MAX_PLAYING_FX; j++)
	{
		SOUND_SLOT *result = &m_SFXPlaying[j];

		if (pos != NULL)
		{
			//создаем нужный слот по позиции и номеру звука
			if (result->flags == SOUND_FLAG_UNUSED)
			{
				result->sound_id = sfx_num;
				result->pos = pos;
				result->flags = SOUND_FLAG_USED;

				if (FAILED(g_DirectSound->DuplicateSoundBuffer(
						g_DSBuffer[result->sound_id], &result->Buffers)))
				{
					// return 0; // ќшибка дублировани€
				}

				return result;
			}
		}
		else
		{
			//создаем нужный слот без позиции и номеру звука
			if (result->flags == SOUND_FLAG_UNUSED)
			{
				result->sound_id = sfx_num;
				result->flags = SOUND_FLAG_USED;

				if (FAILED(g_DirectSound->DuplicateSoundBuffer(
						g_DSBuffer[result->sound_id], &result->Buffers)))
				{
					// return 0; // ќшибка дублировани€
				}

				return result;
			}
		}
	}

	return NULL;
}

void DS_StopSample(SOUND_SLOT *slot)
{
	if (slot->Buffers)
	{
		slot->Buffers->Stop();
		// slot->Buffers->Release();
		// slot->Buffers = NULL;
		// slot->flags = SOUND_FLAG_UNUSED;
	}
}

void Sound_SetMasterVolume(int8_t volume)
{
	int8_t raw_volume = volume ? 6 * volume + 3 : 0;
	// m_MasterVolumeDefault = raw_volume & 0x3F;
	m_MasterVolume = raw_volume & 0x3F;
}
