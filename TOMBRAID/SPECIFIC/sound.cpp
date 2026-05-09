#include "sound.h"
#include "phd_math.h"
#include "vars.h"
#include "main.h"

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
////#include <windows.h>

#include <math.h>

//#include <stdio.h>

//FILE* fp;


/*
extern "C"
{

#include ".\hmi\sos.h"
#include ".\hmi\sosm.h"
#include ".\hmi\sosez.h"

}
*/

// static int bPlaying[256] = {0};

//LPDIRECTSOUNDBUFFER g_pPrimaryBuffer = NULL;

MySound activeBuffers[256];

// int m_DecibelLUT[DECIBEL_LUT_SIZE] = { 0 };
/*
double log2(double x)
{
        return log(x) / log(2.0); // Используем логарифм по основанию e
}
*/

SOUND_SLOT m_SFXPlaying[MAX_PLAYING_FX] = { 0 };

SoundBuffStruct Sound_Buff[256] = { 0 };

void SetCurrVolume(SOUND_SLOT* slot, W32 Volume)
{

        sosDIGISetSampleVolume(hDIGIDriver, slot->sound_handler, Volume);
}

int IsPlaying(SOUND_SLOT* slot)
{


	/*
	мы можем загрузить файл в слот сперва
	но еще не началось воспроизведение
	и поэтому slot->sound_handler не определен и
	sosDIGISampleDone будет глючить
	для этого надо переменная slot->JustLoaded
	показывает что файл только загружен
	еще не началось воспроизведение
	поэтому return 0
	если без slot->JustLoaded
	то slot->sound_handler не определен и не ясно
	какой звук проверяется в sosDIGISampleDone
	*/
	if (slot->JustLoaded == 1)
	{
		slot->JustLoaded = 0;
		return 0;
	}

        if (!sosDIGISampleDone(hDIGIDriver, slot->sound_handler))
        {
                return 1;
        }

		return 0;
                
}

/*
LPDIRECTSOUND8 g_DirectSound;
IDirectSoundBuffer *g_DSBuffer[256];
int LanSampleFrequency[256];
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

int Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags)
{
        if (!m_SoundIsActive)
        {
                return false;
        }


		int camera_underwater = 0;

        //если камера под водой то например водопад Natla Mines не звучит
                if (flags != SPM_ALWAYS &&
                        (flags & SPM_UNDERWATER) !=
                        (g_RoomInfo[g_Camera.pos.room_number].flags & RF_UNDERWATER))
                {
                        //return false;
					camera_underwater = 1;

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

        //начало вариант кода из reverse engineering chatGPT

        int volume = sample->volume;
        // int pitch = sample->pitch;
        // int pitch = 100;
        int pitch = 0;
        int pan = 0;
        int distance = 0;

        uint32_t mode = sample->flags & 3;

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

        // Корректировка громкости
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

        // Случайные вариации высоты тона
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

		int wVol = volume & 0x7FFF;
		//int wVol = volume;

        // int wVol = -10000;   // -> no sound
        // int wVol = 0;                        // -> max volume
        //int wVol = m_DecibelLUT[(volume & 0x7FFF) >> 6];

        //распределяет звук между левым правым динамиком 0 - средина
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
                //SOUND_SLOT* slot = 0;

        //конец вариант кода из reverse engineering chatGPT
        //обработка разных типов звуков
                /*
                SOUND_SLOT* slot;
                
                if (pos != 0)
                {
                        slot = Sound_GetSlot(sample->number, pos);
                }
                else
                {
                        slot = Sound_GetSlot(sample->number, NULL);
                }
				*/

		/*
		if (sample->number != 33)
			return 0;
			*/
                
                int num_done = 0;
				SOUND_SLOT* slot;
				int i;
				/*
				fp = fopen("log.txt", "at");
                
				fprintf(fp, "--------------------\n");
				*/

                for (i = 0; i < MAX_PLAYING_FX; i++)
                {
					SOUND_SLOT* slot = &m_SFXPlaying[i];

					if (slot->flags == SOUND_FLAG_USED)
					{
						//fprintf(fp,"slot used | %d\n", slot->sound_id);
						
						if (sosDIGISampleDone(hDIGIDriver, slot->sound_handler))
						{
							//if (slot->Sound_Buff != 0)
							{
									//free(slot->Sound_Buff);
									//slot->Sound_Buff = 0;
									slot->flags = SOUND_FLAG_UNUSED;

									//slot->Looped = 0;

									num_done++;
							}
						}
						
					}
                }
				/*
                int num_used = 0;
                int num_not_used = 0;

                for (i = 0; i < MAX_PLAYING_FX; i++)
                {
                    slot = &m_SFXPlaying[i];

                    if (slot->flags == SOUND_FLAG_USED)
                    {
                            num_used++;
                    }

                    if (slot->flags == SOUND_FLAG_UNUSED)
                    {
                            num_not_used++;
                    }
                }

                
                fprintf(fp, "%d num USED ----- %d num NOT used slots ----- %d num done\n", num_used, num_not_used, num_done);
                fclose(fp);

				*/

                
                                
        switch (mode)
        {
                
        //обычный звук - шаги лары, выстрелы, звук играет до
        //конца пока сам не остановится
        case SOUND_MODE_WAIT:
        {
            SOUND_SLOT* slot = Sound_GetSlot(sample->number, NULL);

            if (IsPlaying(slot))
            {
				break;
            }
            else
            {
                Sound_StartSample(slot, wVol, pitch, wPan, 0);
            }
                
                        break;
        }
                

        //звук с возможностью перезапуска - используется
        //в инвентаре при прокрутке кольца
        //если звук уже играет - он останавливается и
        //начинает играть сначала - сразу отклик на клавиатуру
        //например два раза нажимаешь на клавиатуру
        //звук может начаться два раза и будет накладка без DS_StopSample()
                
        case SOUND_MODE_RESTART:
        {
                        
                SOUND_SLOT *slot = Sound_GetSlot(sample->number, NULL);

                                if (slot == 0)
                                        break;

				Sound_StopSample(slot);
                Sound_StartSample(slot, wVol, pitch, wPan, 0);
                                
                break;
        }
         

        //позиционный звук с обновлением громкости
        //например шум водопада - Лара ближе громкость больше
        //дальше- громкость ниже
                
        case SOUND_MODE_AMBIENT:
        {
			
			SOUND_SLOT* slot = Sound_GetSlot(sample->number, pos);

			//если камера под водой звук водопадов отключаем
			
			if (camera_underwater)
			{
				Sound_StopSample(slot);
				//SetCurrVolume(slot, 0);
				break;
			}
			


			SetCurrVolume(slot, wVol);
			//SetCurrVolume(slot, 0x4FFF);

            if (IsPlaying(slot))
            break;
            {
                    Sound_StartSample(slot, wVol, pitch, wPan, 1);
            }
            
            break;
        }

		
         
                
        }

        return true;
}

void Sound_StopSample(SOUND_SLOT* slot)
{

	/*
        if (!slot->sound_handler)
                return;
      */  
	sosDIGIStopSample(hDIGIDriver, slot->sound_handler);
}

int Sound_StartSample(SOUND_SLOT* slot, W32 nVolume, int nPitch, int nPan,
                                   unsigned int dwFlags)
{

                //PSOSSAMPLE s;
                //PSOSSAMPLE s = &sample;
        _WAVHEADER* pWaveHeader;

        unsigned char* curr_sound_buff = (unsigned char*)slot->Sound_Buff;

        pWaveHeader = (_WAVHEADER*)curr_sound_buff;

                slot->sample.pSample = (char*)(curr_sound_buff + sizeof(_WAVHEADER));
                slot->sample.wLength = pWaveHeader->dwDataLength - sizeof(_WAVHEADER);
                slot->sample.wFormat = _PCM_UNSIGNED;
                slot->sample.wRate = 22050;
                slot->sample.wBitsPerSample = pWaveHeader->wBitsPerSample;
                slot->sample.wChannels = pWaveHeader->wChannels;
                slot->sample.wPanPosition = _PAN_CENTER;
                //slot->sample.wVolume = 0xFFFF;
                                slot->sample.wVolume = nVolume;
                                
                                
                                //s->wLoopCount = 0x7FFFFFFF;
                                //slot->sample.wLoopCount = 0;
                                
								if (dwFlags == 0)
								{
									slot->sample.wLoopCount = 0;
									slot->Looped = 0;
								}
								else if (dwFlags == 1)
								{
									slot->sample.wLoopCount = 0x7FFFFFFF;
									slot->Looped = 1;
								}
                                
								/*
                                slot->sample.wLoopCount = 0;
								*/

                                

                                //slot->flags = SOUND_FLAG_USED;

                                slot->sound_handler = sosDIGIStartSample(hDIGIDriver, &slot->sample);

                /*
                while (!sosDIGISampleDone(hDIGIDriver, slot->sound_handler))
                {
                        // можно добавить защиту от зависания
                }
                */
                

                return 1;
}

void Sound_UnInit()
{
        sosEZUnInitSystem();
}

//HRESULT DS_MakeSample(int nSample, WAVEFORMATEX *pWF, unsigned char *pWaveData,
/*
void Sound_MakeSample(int nSample, _WAVHEADER *pWF, unsigned char *pWaveData,
                                          int dwWaveLength)
{
        */

/*
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
        unsigned int dwBlockLength;

        if (FAILED(g_DSBuffer[nSample]->Lock(0, dwWaveLength, &pBlock,
                                                                                 &dwBlockLength, NULL, NULL, 0)))
                return S_FALSE;

        memcpy(pBlock, (void *)pWaveData, dwBlockLength);

        if (FAILED(g_DSBuffer[nSample]->Unlock(pBlock, dwBlockLength, NULL, 0)))
                return S_FALSE;

        LanSampleFrequency[nSample] = pWF->nSamplesPerSec;
*/
        /*
        return S_OK;
}

*/

void Sound_ResetEffects()
{

}

//HRESULT DirectSound_Init()
void Sound_Init()
{
        
        sosEZGetConfig("HMISET.CFG");
                 
        sDIGIDriver.wDriverRate = _SOSEZ_SAMPLE_RATE;
        sDIGIDriver.wDMABufferSize = _SOSEZ_DMA_BUFFER;

        sDIGIDriver.wID = wDIGIDeviceID;

        //sosTIMERInitSystem(_TIMER_DOS_RATE, _SOS_DEBUG_NORMAL);
        sosTIMERInitSystem(0, 0);

        sosTIMERRegisterEvent(60, TickerInc, &G_timerHandle);

        sosDIGIInitDriver(&sDIGIDriver, &hDIGIDriver);

        if (wDIGIDeviceID != -1)
                {
                        /*
                sosTIMERRegisterEvent(_SOSEZ_TIMER_RATE, sDIGIDriver.pfnMixFunction,
                        &hDIGITimer);
                                                */

                        sosTIMERRegisterEvent(120, sDIGIDriver.pfnMixFunction,
                                &hDIGITimer);
                }

        
                /*
        m_DecibelLUT[0] = -10000;
        for (int i = 1; i < DECIBEL_LUT_SIZE; i++)
        {
                m_DecibelLUT[i] =
                        (int)((log2(1.0 / DECIBEL_LUT_SIZE) - log2(1.0 / i)) * 1000);
        }

        HRESULT hr;

        if (FAILED(hr = DirectSoundCreate8(NULL, &g_DirectSound, NULL)))
                return S_OK;

        // Set DirectSound coop level
        if (FAILED(hr =
                                   g_DirectSound->SetCooperativeLevel(g_hWnd, DSSCL_EXCLUSIVE)))
                return S_OK;

        // create primary sound buffer

        // Создание primary buffer
        DSBUFFERDESC dsbd = {};
        dsbd.dwSize = sizeof(DSBUFFERDESC);
        dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
        dsbd.dwBufferBytes = 0;  // Не задается для primary buffer
        dsbd.lpwfxFormat = NULL; // Формат будет задаваться отдельно

        if (FAILED(
                        g_DirectSound->CreateSoundBuffer(&dsbd, &g_pPrimaryBuffer, NULL)))
        {
                // std::cerr << "Failed to create primary buffer!" << std::endl;
                return false;
        }

        // Установка формата primary buffer
        WAVEFORMATEX wfex = {};
        wfex.wFormatTag = WAVE_FORMAT_PCM;
        wfex.nChannels = 2;                      // Стерео
        wfex.nSamplesPerSec = 44100; // Частота дискретизации
        wfex.wBitsPerSample = 16;       // 16 бит
        wfex.nBlockAlign = (wfex.nChannels * wfex.wBitsPerSample) / 8;
        wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

        if (FAILED(g_pPrimaryBuffer->SetFormat(&wfex)))
        {
                // std::cerr << "Failed to set format for primary buffer!" << std::endl;
                return false;
        }
*/
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

//        return S_OK;
}

/*

SOUND_SLOT *Sound_GetSlot(int32_t sfx_num, PHD_3DPOS *pos)
{

                unsigned int data_size = 0;


                SOUND_SLOT* result;
                int i = 0;

                for (i = 0; i < MAX_PLAYING_FX; i++)
                {
                                result = &m_SFXPlaying[i];

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
                                result = &m_SFXPlaying[j];

                                if (pos != NULL)
                                {
                                                //создаем нужный слот по позиции и номеру звука
                                                if (result->flags == SOUND_FLAG_UNUSED)
                                                {
                                                                result->sound_id = sfx_num;
                                                                result->pos = pos;
                                                                result->flags = SOUND_FLAG_USED;

                                                                if (result->Sound_Buff != 0)
                                                                {
                                                                                free(result->Sound_Buff);
                                                                                result->Sound_Buff = 0;
                                                                                result->sound_handler = 0;
                                                                }



                                                                data_size = Sound_Buff[sfx_num].data_size;
                                                                result->Sound_Buff = (unsigned char*)malloc(data_size);
                                                                memcpy(result->Sound_Buff, Sound_Buff[sfx_num].Buff_data, data_size);
                                                                result->bufferLength = data_size;



                                                                return result;
                                                }
                                }
                                else

                                {
                                                //создаем нужный слот без позиции и номеру звука
                                                if (result->flags == SOUND_FLAG_UNUSED)
                                                {
                                                                result->sound_id = sfx_num;
                                                                result->pos = 0;
                                                                result->flags = SOUND_FLAG_USED;

                                                                if (result->Sound_Buff != 0)
                                                                {
                                                                                free(result->Sound_Buff);
                                                                                result->Sound_Buff = 0;
                                                                                result->sound_handler = 0;
                                                                }

                                                                data_size = Sound_Buff[sfx_num].data_size;
                                                                result->Sound_Buff = (unsigned char*)malloc(data_size);
                                                                memcpy(result->Sound_Buff, Sound_Buff[sfx_num].Buff_data, data_size);
                                                                result->bufferLength = data_size;

                                                                return result;
                                                }
                                }
                }

                return NULL;
}

*/

SOUND_SLOT* Sound_GetSlot(int32_t sfx_num, PHD_3DPOS* pos)
{

        unsigned int data_size = 0;
        SOUND_SLOT* result;
        int i = 0;
        int j = 0;

        if (pos != NULL)
        {
                for (i = 0; i < MAX_PLAYING_FX; i++)
                {
                        result = &m_SFXPlaying[i];
						/*
                        if (result->sound_id == sfx_num &&
                                result->pos->x == pos->x &&
                                result->pos->y == pos->y &&
                                result->pos->z == pos->z &&
                               result->flags != SOUND_FLAG_UNUSED)
							   */
						if (result->sound_id == sfx_num &&
							result->pos == pos &&
							result->flags != SOUND_FLAG_UNUSED)
                        {
                                //находим нужный слот по позиции и номеру звука


                                return result;
                        }
                }
                
                
                for (j = 0; j < MAX_PLAYING_FX; j++)
                {
                        result = &m_SFXPlaying[j];

                                //создаем нужный слот по позиции и номеру звука
                                if (result->flags == SOUND_FLAG_UNUSED)
                                {
                                        result->sound_id = sfx_num;
                                        
                                        /*
                                        result->pos->x = pos->x;
                                        result->pos->y = pos->y;
                                        result->pos->z = pos->z;
										*/

										result->Looped = 0;

										result->JustLoaded = 1;

                                        result->pos = pos;
                                        result->flags = SOUND_FLAG_USED;
                                        //result->flags = result->flags | SOUND_FLAG_USED;
                                        
										/*
                                        data_size = Sound_Buff[sfx_num].data_size;
                                        result->Sound_Buff = (unsigned char*)malloc(data_size);
                                        memcpy((void*)result->Sound_Buff, (void*)Sound_Buff[sfx_num].Buff_data, data_size);
                                        result->bufferLength = data_size;
										*/

										result->Sound_Buff = Sound_Buff[sfx_num].Buff_data;

                                        return result;
                                }
                        }

        }


		if (pos == 0)
		{
			for (i = 0; i < MAX_PLAYING_FX; i++)
			{
				result = &m_SFXPlaying[i];

				if (result->sound_id == sfx_num &&
					result->flags != SOUND_FLAG_UNUSED)
				{
					//находим нужный слот без позиции и номеру звука
					return result;
				}
			}

			int j = 0;
			for (j = 0; j < MAX_PLAYING_FX; j++)
			{
				result = &m_SFXPlaying[j];

				//создаем нужный слот без позиции и номеру звука
				if (result->flags == SOUND_FLAG_UNUSED)
				{
					result->sound_id = sfx_num;
					result->pos = 0;
					result->flags = SOUND_FLAG_USED;
					
					result->JustLoaded = 1;
					result->Looped = 0;

					/*
					data_size = Sound_Buff[sfx_num].data_size;
					result->Sound_Buff = (unsigned char*)malloc(data_size);
					memcpy((void*)result->Sound_Buff, (void*)Sound_Buff[sfx_num].Buff_data, data_size);
					result->bufferLength = data_size;
					*/

					result->Sound_Buff = Sound_Buff[sfx_num].Buff_data;

					return result;
				}
			}
		}

        return NULL;
}


void DS_StopSample(SOUND_SLOT *slot)
{
        /*
        if (slot->Buffers)
        {
                slot->Buffers->Stop();
                // slot->Buffers->Release();
                // slot->Buffers = NULL;
                // slot->flags = SOUND_FLAG_UNUSED;
        }
        */
}

void Sound_SetMasterVolume(int8_t volume)
{
        int8_t raw_volume = volume ? 6 * volume + 3 : 0;
        // m_MasterVolumeDefault = raw_volume & 0x3F;
        m_MasterVolume = raw_volume & 0x3F;
}


void Sound_MakeSample(unsigned int i, unsigned char* sample, unsigned int size)
{
        Sound_Buff[i].Buff_data = (unsigned char*)malloc(size);
        memcpy((void*)Sound_Buff[i].Buff_data, (void*)sample, size);
        Sound_Buff[i].data_size = size;
}


VOID TickerInc(VOID)
{
        Frame_Counter++;
}


void ZeroSoundBuff()
{
        memset(Sound_Buff, 0, sizeof(Sound_Buff));

        for (int i = 0; i < MAX_PLAYING_FX; i++)
        {
                SOUND_SLOT* result = &m_SFXPlaying[i];
                memset(result, 0, sizeof(SOUND_SLOT));
                result->flags = SOUND_FLAG_UNUSED;
                //result->Sound_Buff = NULL;

        }

        

        

}


void Sound_StopAllSamples()
{
	//sosDIGIStopAllSamples(hDIGIDriver);

	SOUND_SLOT* slot;

	for (int i = 0; i < MAX_PLAYING_FX; i++)
	{
		slot = &m_SFXPlaying[i];

		if (slot->flags == SOUND_FLAG_USED)
		{
			sosDIGIStopSample(hDIGIDriver, slot->sound_handler);

			slot->flags = SOUND_FLAG_UNUSED;
		}
	}


}
