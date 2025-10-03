#include "option.h"

//#include "config.h"
#include "gameflow.h"
//#include "game/input.h"
//#include "game/music.h"
//#include "game/settings.h"
#include "sound.h"
#include "text.h"
#include "vars.h"

#include <stdio.h>

typedef enum SOUND_TEXT
{
	TEXT_MUSIC_VOLUME = 0,
	TEXT_SOUND_VOLUME = 1,
	TEXT_TITLE = 2,
	TEXT_TITLE_BORDER = 3,
	TEXT_NUMBER_OF = 4,
	TEXT_OPTION_MIN = TEXT_MUSIC_VOLUME,
	TEXT_OPTION_MAX = TEXT_SOUND_VOLUME,
} SOUND_TEXT;

static TEXTSTRING *m_Text[4] = {0};

static void Option_SoundInitText();

static void Option_SoundInitText()
{
	char buf[20];

	if (g_Config.music_volume > 10)
	{
		g_Config.music_volume = 10;
	}
	sprintf(buf, "| %2d", g_Config.music_volume);
	m_Text[TEXT_MUSIC_VOLUME] = Text_Create(0, 0, buf);

	if (g_Config.sound_volume > 10)
	{
		g_Config.sound_volume = 10;
	}
	sprintf(buf, "} %2d", g_Config.sound_volume);
	m_Text[TEXT_SOUND_VOLUME] = Text_Create(0, 25, buf);

	m_Text[TEXT_TITLE] =
		Text_Create(0, -30, g_GameFlow.strings[GS_SOUND_SET_VOLUMES]);
	m_Text[TEXT_TITLE_BORDER] = Text_Create(0, -32, " ");

	Text_AddBackground(m_Text[g_OptionSelected], 128, 0, 0, 0);
	Text_AddOutline(m_Text[g_OptionSelected], 1);
	Text_AddBackground(m_Text[TEXT_TITLE], 136, 0, 0, 0);
	Text_AddOutline(m_Text[TEXT_TITLE], 1);
	Text_AddBackground(m_Text[TEXT_TITLE_BORDER], 140, 85, 0, 0);
	Text_AddOutline(m_Text[TEXT_TITLE_BORDER], 1);

	for (int i = 0; i < TEXT_NUMBER_OF; i++)
	{
		Text_CentreH(m_Text[i], 1);
		Text_CentreV(m_Text[i], 1);
	}
}

void Option_Sound(INVENTORY_ITEM *inv_item)
{
	char buf[20];

	if (!m_Text[0])
	{
		Option_SoundInitText();
	}

	if (g_InputDB.forward && g_OptionSelected > TEXT_OPTION_MIN)
	{
		Text_RemoveOutline(m_Text[g_OptionSelected]);
		Text_RemoveBackground(m_Text[g_OptionSelected]);
		Text_AddBackground(m_Text[--g_OptionSelected], 128, 0, 0, 0);
		Text_AddOutline(m_Text[g_OptionSelected], 1);
	}

	if (g_InputDB.back && g_OptionSelected < TEXT_OPTION_MAX)
	{
		Text_RemoveOutline(m_Text[g_OptionSelected]);
		Text_RemoveBackground(m_Text[g_OptionSelected]);
		Text_AddBackground(m_Text[++g_OptionSelected], 128, 0, 0, 0);
		Text_AddOutline(m_Text[g_OptionSelected], 1);
	}

	switch (g_OptionSelected)
	{
	case TEXT_MUSIC_VOLUME:
		if (g_Input.left && g_Config.music_volume > 0)
		{
			g_Config.music_volume--;
			g_IDelay = true;
			g_IDCount = 10;
			sprintf(buf, "| %2d", g_Config.music_volume);
			Text_ChangeText(m_Text[TEXT_MUSIC_VOLUME], buf);
			Settings_Write();
		}
		else if (g_Input.right && g_Config.music_volume < 10)
		{
			g_Config.music_volume++;
			g_IDelay = true;
			g_IDCount = 10;
			sprintf(buf, "| %2d", g_Config.music_volume);
			Text_ChangeText(m_Text[TEXT_MUSIC_VOLUME], buf);
			Settings_Write();
		}

		if (g_Input.left || g_Input.right)
		{
			// Music_SetVolume(g_Config.music_volume);
			Sound_Effect(SFX_MENU_PASSPORT, NULL, SPM_ALWAYS);
		}
		break;

	case TEXT_SOUND_VOLUME:
		if (g_Input.left && g_Config.sound_volume > 0)
		{
			g_Config.sound_volume--;
			g_IDelay = true;
			g_IDCount = 10;
			sprintf(buf, "} %2d", g_Config.sound_volume);
			Text_ChangeText(m_Text[TEXT_SOUND_VOLUME], buf);
			Settings_Write();
		}
		else if (g_Input.right && g_Config.sound_volume < 10)
		{
			g_Config.sound_volume++;
			g_IDelay = true;
			g_IDCount = 10;
			sprintf(buf, "} %2d", g_Config.sound_volume);
			Text_ChangeText(m_Text[TEXT_SOUND_VOLUME], buf);
			Settings_Write();
		}

		if (g_Input.left || g_Input.right)
		{
			Sound_SetMasterVolume(g_Config.sound_volume);
			Sound_Effect(SFX_MENU_PASSPORT, NULL, SPM_ALWAYS);
		}
		break;
	}

	if (g_InputDB.deselect || g_InputDB.select)
	{
		for (int i = 0; i < TEXT_NUMBER_OF; i++)
		{
			Text_Remove(m_Text[i]);
			m_Text[i] = NULL;
		}
	}
}
