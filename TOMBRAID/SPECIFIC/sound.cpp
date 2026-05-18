#include "sound.h"
#include "phd_math.h"
#include "vars.h"
#include "main.h"

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
////#include <windows.h>

#include <math.h>

SOUND_SLOT m_SFXPlaying[MAX_PLAYING_FX] = { 0 };

unsigned char* Sound_Buff_Data[256] = { 0 };

void SetCurrVolume(SOUND_SLOT* slot, W32 Volume)
//void SetCurrVolume(SOUND_SLOT* slot)
{
	
    sosDIGISetSampleVolume(hDIGIDriver, slot->sound_id, Volume);
}



void Sound_ResetAmbientLoudness()
{
	if (!m_SoundIsActive)
	{
		return;
	}

	for (int i = 0; i < m_AmbientLookupIdx; i++)
	{
		SOUND_SLOT* slot = &m_SFXPlaying[i];
		slot->loudness = SOUND_NOT_AUDIBLE;
	}
}

void Sound_UpdateEffects()
{
	int i;

	Sound_ResetAmbientLoudness();

	

        for (i = 0; i < g_NumberSoundEffects; i++)
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
        

		for (i = 0; i < MAX_PLAYING_FX; i++)
		{
			SOUND_SLOT* slot = &m_SFXPlaying[i];

			if (!(slot->flags & SOUND_FLAG_USED))
			{
				continue;
			}

			if (slot->flags & SOUND_FLAG_AMBIENT)
			{

				if (slot->loudness != (uint32_t)SOUND_NOT_AUDIBLE &&
					slot->sound_id != AUDIO_NO_SOUND)
				{
					//SetCurrVolume(slot, (m_MasterVolume * slot->volume) >> 6);
					//SetCurrVolume(slot, (32 * slot->volume) >> 6);
					SetCurrVolume(slot, slot->volume);
				}
				else
				{
					if (slot->sound_id != AUDIO_NO_SOUND)
					{
						sosDIGIStopSample(hDIGIDriver, slot->sound_id);
					}
					Sound_ClearSlot(slot);
				}
				
			}

			else if (S_Audio_SampleSoundIsPlaying(slot->sound_id))
			{
				//доработать этот кусок возможно голоса персонажей
			}

			else
			{
				Sound_ClearSlot(slot);
			}
		}

}

int Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags)
{
	int distance = 0x7FFF;

	int pitch = 0x64;

        if (!m_SoundIsActive)
        {
                return false;
        }


        //если камера под водой то например водопад Natla Mines не звучит
        if (flags != SPM_ALWAYS && (flags & SPM_UNDERWATER) !=
                (g_RoomInfo[g_Camera.pos.room_number].flags & RF_UNDERWATER))
        {
                return false;
        }

		int lut = g_SampleLUT[sfx_num];

		if (lut < 0)
		{
			return 0;
		}

        SOUND_SAMPLE_INFO *sample = &g_SampleInfos[lut];

        //например рычаг под водой Cistern уровень
        //имеет pSI->number = -1
		/*
        if (sample->number < 0)
                return false;

        if (sample->volume < 0)
                return false;
		*/

        if (sample->randomness != 0)
        {
                if (Random_GetControl() > (int32_t)sample->randomness)
                {
                        return false;
                }
        }

		uint32_t soundMode = sample->flags & 3;

		int pan = 0;
		int distSq = 0;

		if (!pos)
		{
			pitch = 8;
			pan = 0;
			distSq = 0;
		}
		else
        //if (pos)
        {

                int32_t dx = pos->x - g_Camera.target.x;
                int32_t dy = pos->y - g_Camera.target.y;
                int32_t dz = pos->z - g_Camera.target.z;

				//SOUND_RADIUS
                if (dx < -8192 || dx > 8192 ||
					dy < -8192 || dy > 8192 ||
					dz < -8192 || dz > 8192)
                {
                        return 0;
                }

				distSq = dx * dx + dy * dy + dz * dz;

				if (distSq == 0)
				{
					pan = 0;
				}
        }

		int dist = phd_sqrt(distSq);

        int volume = sample->volume - dist * 4;

		// Random volume
		if (sample->flags & 0x40)
		{
			int rnd = (Random_GetControl() << 13) >> 15;
				volume -= rnd;
		}

		// No pan
		if (sample->flags & 0x10)
		{
			pan = 0;
		}
		

        if (volume <= 0 && soundMode != SOUND_MODE_AMBIENT)
        {
                        return 0;
        }

		if (pan != 0 && pos != NULL)
		{
			int dx = pos->x - g_LaraItem->pos.x;
			int dz = pos->z - g_LaraItem->pos.z;

			int angle = phd_atan(dz, dx);

			int rot = g_LaraItem->pos.y_rot +
				g_Lara.head_x_rot +
				g_Lara.torso_x_rot;

			pan = (short)(angle - rot);
		}


		if (sample->flags & 0x20)
		{
			int rnd = (Random_GetControl() * 5) * 2;

			pitch = (rnd >> W2V_SHIFT) + 90;
		}

		int32_t variation = (sample->flags >> 2) & 15;

		int sampleId;

		if (variation == 1)
		{
			sampleId = (short)sample->number;
		}
		else
		{
			sampleId = (short)sample->number + ((variation * Random_GetControl()) >> 15);
		}


        if (volume > SOUND_MAX_VOLUME)
        {
                volume = SOUND_MAX_VOLUME;
        }


		//volume = (m_MasterVolume * volume) >> 6;
		volume = (32 * volume) >> 6;


		//if (sosDIGISampleDone(hDIGIDriver, slot->sound_handler))

		

		switch (soundMode)
        {
		case SOUND_MODE_WAIT:
		{

			SOUND_SLOT* slot = Sound_GetSlot(sfx_num, 0, pos, soundMode);

			if (!slot)
			{
				return 0;
			}

			if (slot->flags & SOUND_FLAG_RESTARTED)
			{
				slot->flags &= ~SOUND_FLAG_RESTARTED;
				return 1;
			}

			slot->sound_id = S_SoundPlaySample(sampleId, volume, pitch, 0 );

			if (slot->sound_id == -1)
			{
				return 0;
			}

			for (int i = 0; i < MAX_PLAYING_FX; i++)
			{
				SOUND_SLOT* rslot = &m_SFXPlaying[i];

				if (rslot != slot && rslot->sound_id == slot->sound_id)
				{
					rslot->sound_id = AUDIO_NO_SOUND;
				}
			}

			slot->pos = pos;
			slot->flags |= SOUND_FLAG_USED;
			slot->fxnum = sfx_num;

			return 1;
		}


		case SOUND_MODE_RESTART:
		{
			SOUND_SLOT* slot = Sound_GetSlot(sfx_num, 0, pos, soundMode);

			if (!slot)
			{
				return 0;
			}

			if (slot->flags & SOUND_FLAG_RESTARTED)
			{
				sosDIGIStopSample(hDIGIDriver, slot->sound_id);
				slot->sound_id = S_SoundPlaySample(sampleId, volume, pitch, 0);
				return 1;
			}

			slot->sound_id = S_SoundPlaySample(sampleId, volume, pitch, 0);

			if (slot->sound_id == -1)
			{
				return 0;
			}

			for (int i = 0; i < MAX_PLAYING_FX; i++)
			{
				SOUND_SLOT* rslot = &m_SFXPlaying[i];

				if (rslot != slot && rslot->sound_id == slot->sound_id)
				{
					rslot->sound_id = AUDIO_NO_SOUND;
				}
			}

			slot->pos = pos;
			slot->flags |= SOUND_FLAG_USED;
			slot->fxnum = sfx_num;

			return 1;
		}

		case SOUND_MODE_AMBIENT:
		{

			uint32_t loudness = dist; 

			SOUND_SLOT* slot = Sound_GetSlot(sfx_num, loudness, pos, soundMode);

			if (!slot)
			{
				return 0;
			}

			slot->pos = pos;

			if (slot->flags & SOUND_FLAG_AMBIENT)
			{
				if (volume > 0)
				{
					slot->loudness = loudness;
					slot->pan = pan;
					slot->volume = volume;
					//slot->volume = final_volume;
				}
				else
				{
					slot->loudness = SOUND_NOT_AUDIBLE;
					slot->volume = 0;
				}

				return 1;
			}

		if (volume > 0)
		{
				slot->sound_id = S_SoundPlayAmbientSample(sampleId, volume, pitch, 0);
				

				if (slot->sound_id == -1)
				{
					Sound_ClearSlot(slot);
					return 0;
				}

				for (int i = 0; i < MAX_PLAYING_FX; i++)
				{
					SOUND_SLOT* rslot = &m_SFXPlaying[i];

					if (rslot != slot && rslot->sound_id == slot->sound_id)
					{
						rslot->sound_id = AUDIO_NO_SOUND;
					}
				}



				slot->loudness = loudness;
				slot->fxnum = sfx_num;
				slot->pan = pan;
				slot->volume = volume;
				//slot->volume = final_volume;
				slot->flags |= SOUND_FLAG_AMBIENT | SOUND_FLAG_USED;
				slot->pos = pos;
				return 1;
				}

			slot->loudness = SOUND_NOT_AUDIBLE;
			return 1;
		}

		}

        return true;
}

void Sound_UnInit()
{
        sosEZUnInitSystem();
}

void Sound_ResetEffects()
{
	if (!m_SoundIsActive)
	{
		return;
	}
	//m_MasterVolume = m_MasterVolumeDefault;

	int i;

	for (i = 0; i < MAX_PLAYING_FX; i++)
	{
		Sound_ClearSlot(&m_SFXPlaying[i]);
	}

	Sound_StopAllSamples();

	m_AmbientLookupIdx = 0;

	for (i = 0; i < MAX_SAMPLES; i++)
	{
		if (g_SampleLUT[i] < 0)
		{
			continue;
		}
		SAMPLE_INFO* s = (SAMPLE_INFO*) &g_SampleInfos[g_SampleLUT[i]];

		if (s->volume < 0)
		{
			/*
			Shell_ExitSystemFmt(
				"sample info for effect %d has incorrect volume(%d)", i,
				s->volume);
				*/
		}

		int32_t mode = s->flags & 3;

		if (mode == SOUND_MODE_AMBIENT)
		{
			if (m_AmbientLookupIdx >= MAX_AMBIENT_FX)
			{
				/*
				Shell_ExitSystem("Ran out of ambient fx slots in "
					"Sound_ResetEffects()");
					*/
			}
			m_AmbientLookup[m_AmbientLookupIdx] = i;
			m_AmbientLookupIdx++;
		}
	}
}

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


}



static SOUND_SLOT* Sound_GetSlot(int32_t sfx_num, uint32_t loudness,
	PHD_3DPOS* pos, int16_t mode)
{
	switch (mode)
	{
	case SOUND_MODE_WAIT:
	case SOUND_MODE_RESTART:
	{
		SOUND_SLOT* last_free_slot = NULL;
		for (int i = m_AmbientLookupIdx; i < MAX_PLAYING_FX; i++)
		{
			SOUND_SLOT* result = &m_SFXPlaying[i];
			if ((result->flags & SOUND_FLAG_USED) && result->fxnum == sfx_num &&
				result->pos == pos)
			{
				result->flags |= SOUND_FLAG_RESTARTED;
				return result;
			}
			else if (result->flags == SOUND_FLAG_UNUSED)
			{
				last_free_slot = result;
			}
		}
		return last_free_slot;
	}

	case SOUND_MODE_AMBIENT:

		for (int i = 0; i < MAX_AMBIENT_FX; i++)
		{
			if (m_AmbientLookup[i] == sfx_num)
			{
				SOUND_SLOT* result = &m_SFXPlaying[i];
				if (result->flags != SOUND_FLAG_UNUSED &&
					result->loudness <= loudness)
				{
					return NULL;
				}
				return result;
			}
		}
		break;
	}

	return NULL;
}




void Sound_SetMasterVolume(int8_t volume)
{
        int8_t raw_volume = volume ? 6 * volume + 3 : 0;
        // m_MasterVolumeDefault = raw_volume & 0x3F;
        //m_MasterVolume = raw_volume & 0x3F;
		m_MasterVolume = 32;
}


void Sound_MakeSample(unsigned int i, unsigned char* sample, unsigned int size)
{

	Sound_Buff_Data[i] = (unsigned char*)malloc(size);
	memcpy((void*)Sound_Buff_Data[i], (void*)sample, size);
}


VOID TickerInc(VOID)
{
        Frame_Counter++;
}

//вызвать ZeroSoundBuff() из main() в конце очистка перед return 0
void ZeroSoundBuff()
{
	//вызвать ZeroSoundBuff() из main() в конце очистка перед return 0

        //memset(Sound_Buff_Data, 0, sizeof(Sound_Buff_Data));

		int i;

		for (i = 0; i < 256; i++)
		{
			if(Sound_Buff_Data[i])
			{	
				free(Sound_Buff_Data[i]);
				Sound_Buff_Data[i] = 0;
			}
		}

		memset(Sound_Buff_Data, 0, sizeof(unsigned char) * 256);

        for (i = 0; i < MAX_PLAYING_FX; i++)
        {
                SOUND_SLOT* result = &m_SFXPlaying[i];
                memset(result, 0, sizeof(SOUND_SLOT));
				result->flags = SOUND_FLAG_UNUSED;
                //result->Sound_Buff = NULL;
				result->sound_id = -1;
				result->volume = 0;

        }


		for (int sound_id = 0; sound_id < MAX_ACTIVE_SAMPLES; sound_id++) {
			AUDIO_SAMPLE_SOUND* sound = &m_SampleSounds[sound_id];
			sound->is_used = false;
			sound->is_playing = false;
			sound->volume = 0.0f;
			sound->pitch = 1.0f;
			sound->pan = 0.0f;
			sound->current_sample = 0.0f;
			sound->sample.pSample = NULL;
		}

        

        

}


void Sound_StopAllSamples()
{
	//sosDIGIStopAllSamples(hDIGIDriver);

	SOUND_SLOT* slot;
	/*
	for (int i = 0; i < MAX_PLAYING_FX; i++)
	{
		slot = &m_SFXPlaying[i];

		if (slot->flags == SOUND_FLAG_USED)
		{
			sosDIGIStopSample(hDIGIDriver, slot->sound_handler);

			slot->flags = SOUND_FLAG_UNUSED;
		}
	}
	*/

}


W32 S_SoundPlaySample(int sample_index, int volume, int pitch, int pan)
{
	if (!m_SoundIsActive)
	{
		//return false;
		return -1;
	}

	int adjusted_pan = Adjust_Pan(pan);


	//int final_volume = volume * m_MasterVolume;
	int final_volume = volume * 32;
	final_volume = final_volume >> 6;  // sar eax, 6

	int final_pitch = pitch & 0xFFFF;

	W32 result = Play_Driver_Sound(sample_index, final_volume, adjusted_pan, final_pitch);

	return result;

}


W32 S_SoundPlayAmbientSample(int sample_index, int volume, int pan, int pitch)
{

	if (!m_SoundIsActive)
	{
		//return false;
		return -1;
	}

	int adjusted_pan = Adjust_Pan(pan);


	int final_volume = volume * m_MasterVolume;
	final_volume = final_volume >> 6;  // sar eax, 6

	int final_pitch = pitch & 0xFFFF;

	W32 result = Play_Driver_Sound_Ambient(sample_index, final_volume, adjusted_pan, final_pitch);

	return result;


}

W32 Play_Driver_Sound_Ambient(int sample_index, int volume, int pan, int pitch)
{/*
	if (!Sound_Enabled_Var1 || !Sound_Enabled_Var2) {
		return 0;
	}
	*/

	W32 result;

	for (int sound_id = 0; sound_id < MAX_ACTIVE_SAMPLES; sound_id++)
	{

		unsigned char* curr_sound_buff = (unsigned char*)Sound_Buff_Data[sample_index];

		AUDIO_SAMPLE_SOUND* sound = &m_SampleSounds[sound_id];

		sound->is_used = true;
		sound->is_playing = true;
		sound->volume = volume;
		sound->pitch = pitch;
		sound->pan = pan;
		sound->is_looped = 1;
		sound->current_sample = 0.0f;
		sound->sample.pSample = (char*)(curr_sound_buff + sizeof(_WAVHEADER));



		//_SOS_SAMPLE* sos = &sos_sample[sample_index];

		_WAVHEADER* pWaveHeader;



		pWaveHeader = (_WAVHEADER*)curr_sound_buff;

		//_SOS_SAMPLE sample;

		//sos->pSample = (char*)(curr_sound_buff + sizeof(_WAVHEADER));
		sound->sample.wLength = pWaveHeader->dwDataLength - sizeof(_WAVHEADER);
		sound->sample.wFormat = _PCM_UNSIGNED;
		sound->sample.wRate = 22050;
		sound->sample.wBitsPerSample = pWaveHeader->wBitsPerSample;
		sound->sample.wChannels = pWaveHeader->wChannels;
		sound->sample.wPanPosition = _PAN_CENTER;
		//sound->sample.wVolume = 0xFFFF;

		sound->sample.wVolume = volume;
		//s->wLoopCount = 0x7FFFFFFF;
		//sound->sample.wLoopCount = 0;
		sound->sample.wLoopCount = 0x7FFFFFFF;



		//slot->flags = SOUND_FLAG_USED;

		result = sosDIGIStartSample(hDIGIDriver, &sound->sample);

		break;

	}

	return result;




}


W32 Play_Driver_Sound(int sample_index, int volume, int pan, int pitch)
{/*
	if (!Sound_Enabled_Var1 || !Sound_Enabled_Var2) {
		return 0;
	}
	*/

	W32 result;

	for (int sound_id = 0; sound_id < MAX_ACTIVE_SAMPLES; sound_id++)
	{

		unsigned char* curr_sound_buff = (unsigned char*)Sound_Buff_Data[sample_index];

		AUDIO_SAMPLE_SOUND* sound = &m_SampleSounds[sound_id];

	sound->is_used = true;
	sound->is_playing = true;
	sound->volume = volume;
	sound->pitch = pitch;
	sound->pan = pan;
	sound->is_looped = 0;
	sound->current_sample = 0.0f;
	sound->sample.pSample = (char*)(curr_sound_buff + sizeof(_WAVHEADER));


	
	//_SOS_SAMPLE* sos = &sos_sample[sample_index];

	_WAVHEADER* pWaveHeader;

	

	pWaveHeader = (_WAVHEADER*)curr_sound_buff;

	//_SOS_SAMPLE sample;

	//sos->pSample = (char*)(curr_sound_buff + sizeof(_WAVHEADER));
	sound->sample.wLength = pWaveHeader->dwDataLength - sizeof(_WAVHEADER);
	sound->sample.wFormat = _PCM_UNSIGNED;
	sound->sample.wRate = 22050;
	sound->sample.wBitsPerSample = pWaveHeader->wBitsPerSample;
	sound->sample.wChannels = pWaveHeader->wChannels;
	sound->sample.wPanPosition = _PAN_CENTER;
	//sound->sample.wVolume = 0xFFFF;
	sound->sample.wVolume = volume;

	//slot->sos.wVolume = nVolume;
	//s->wLoopCount = 0x7FFFFFFF;
	
	sound->sample.wLoopCount = 0;
	


	//slot->flags = SOUND_FLAG_USED;

	result =  sosDIGIStartSample(hDIGIDriver, &sound->sample);

	break;

	}

	return result;



}

int Adjust_Pan(int pan)
{
	return 0;
}

static void Sound_ClearSlot(SOUND_SLOT* slot)
{
	slot->sound_id = AUDIO_NO_SOUND;
	slot->pos = NULL;
	slot->flags = 0;
	slot->volume = 0;
	slot->pan = 0;
	slot->loudness = SOUND_NOT_AUDIBLE;
	slot->fxnum = -1;
}



bool S_Audio_SampleSoundIsPlaying(int sound_id)
{

	if (sosDIGISampleDone(hDIGIDriver, sound_id))
	{
		return false;
	}

	return true;
	
}


void Sound_StopAmbientSounds()
{
	if (!m_SoundIsActive)
	{
		return;
	}

	for (int i = 0; i < m_AmbientLookupIdx; i++)
	{
		SOUND_SLOT* slot = &m_SFXPlaying[i];
		if (S_Audio_SampleSoundIsPlaying(slot->sound_id))
		{
			sosDIGIStopSample(hDIGIDriver, slot->sound_id);
			Sound_ClearSlot(slot);
		}
	}
}


