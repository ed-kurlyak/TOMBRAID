#include "option.h"

#include "game.h"
#include "gameflow.h"
//#include "game/input.h"
#include "inv.h"
#include "requester.h"
#include "screen.h"
#include "sound.h"
#include "text.h"
#include "vars.h"

#define PASSPORT_PAGE_COUNT 3
#define MAX_GAME_MODES 4
#define MAX_GAME_MODE_LENGTH 20

static int32_t m_PassportMode = 0;
static TEXTSTRING *m_PassportText = NULL;

static char m_NewGameStrings[MAX_GAME_MODES][MAX_GAME_MODE_LENGTH] = {0};
static REQUEST_INFO m_NewGameRequester = {
	MAX_GAME_MODES,			 //.items =
	0,						 //.requested =
	MAX_GAME_MODES,			 //.vis_lines =
	0,						 //.line_offset =
	0,						 //.line_old_offset =
	162,					 //.pix_width =
	TEXT_HEIGHT + 7,		 //.line_height =
	0,						 //.x =
	0,						 //.y =
	0,						 //.z =
	0,						 //.flags =
	NULL,					 //.heading_text =
	&m_NewGameStrings[0][0], //.item_texts =
	MAX_GAME_MODE_LENGTH,	//.item_text_len =
	0,
};

static char m_LoadSaveGameStrings[MAX_SAVE_SLOTS][MAX_LEVEL_NAME_LENGTH] = {0};
REQUEST_INFO g_LoadSaveGameRequester = {
	1,							  //.items =
	0,							  //.requested =
	(unsigned short)-1,			  //.vis_lines =
	0,							  //.line_offset =
	0,							  //.line_old_offset =
	272,						  //.pix_width =
	TEXT_HEIGHT + 7,			  //.line_height =
	0,							  //.x =
	-32,						  //.y =
	0,							  //.z =
	0,							  //.flags =
	NULL,						  //.heading_text =
	&m_LoadSaveGameStrings[0][0], //.item_texts =
	MAX_LEVEL_NAME_LENGTH,		  //.item_text_len =
	0,
};

static void InitNewGameRequester()
{
	REQUEST_INFO *req = &m_NewGameRequester;
	InitRequester(req);
	SetRequesterHeading(req, g_GameFlow.strings[GS_PASSPORT_SELECT_MODE]);
	AddRequesterItem(req, g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME], 0);
	AddRequesterItem(req, g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_PLUS],
					 0);
	AddRequesterItem(req, g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP], 0);
	AddRequesterItem(req, g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP_PLUS],
					 0);
	req->y = -30 * Screen_GetResHeightDownscaled() / 100;
	req->vis_lines = MAX_GAME_MODES;
}

static void InitLoadSaveGameRequester()
{
	REQUEST_INFO *req = &g_LoadSaveGameRequester;
	InitRequester(req);
	// GetSavedGamesList(req);
	SetRequesterHeading(req, g_GameFlow.strings[GS_PASSPORT_SELECT_LEVEL]);

	if (Screen_GetResHeightDownscaled() <= 240)
	{
		req->y = -30;
		req->vis_lines = 5;
	}
	else if (Screen_GetResHeightDownscaled() <= 384)
	{
		req->y = -30;
		req->vis_lines = 8;
	}
	else if (Screen_GetResHeightDownscaled() <= 480)
	{
		req->y = -80;
		req->vis_lines = 10;
	}
	else
	{
		req->y = -120;
		req->vis_lines = 12;
	}

	S_FrontEndCheck();

	//перенес из функции GetSavedGamesList(req);
	//в ней не совпадает req->vis_lines с этим расчетом выше
	if (req->requested >= req->vis_lines)
	{
		req->line_offset = req->requested - req->vis_lines + 1;
	}
	else if (req->line_offset > 0)
	{
		req->line_offset = 0;
	}
}

void Option_Passport(INVENTORY_ITEM *inv_item)
{
	Text_Remove(g_InvItemText[0]);
	g_InvItemText[IT_NAME] = NULL;

	int16_t page = (inv_item->goal_frame - inv_item->open_frame) / 5;

	if ((inv_item->goal_frame - inv_item->open_frame) % 5)
	{
		page = -1;
	}

	if (g_InvMode == INV_LOAD_MODE || g_InvMode == INV_SAVE_MODE)
	{
		g_InputDB.left = 0;
		g_InputDB.right = 0;
	}

	switch (page)
	{
	// load game если находимся в титлах и игре
	case 0:
		//находимся в титлах и игре
		if (m_PassportMode == 1)
		{
			int32_t select = DisplayRequester(&g_LoadSaveGameRequester);

			if (select)
			{

				if (select > 0)
				{
					//если выбран уровень загрузки записываем его минус 1
					// g_InvExtraData[1] хранит номер уровня загрузки
					g_InvExtraData[1] = select - 1;
				}
				else if (g_InvMode != INV_SAVE_MODE &&
						 g_InvMode != INV_LOAD_MODE)
				{
					g_Input.any = 0;
					g_InputDB.any = 0;
				}
				m_PassportMode = 0;
			}
			else
			{
				g_Input.any = 0;
				g_InputDB.any = 0;
			}
		}
		else if (m_PassportMode == 0)
		{
			if (!g_SavedGamesCount || g_InvMode == INV_SAVE_MODE)
			{
				g_InputDB.any = 0;
				g_InputDB.right = 1;
			}
			else
			{
				if (!m_PassportText)
				{
					m_PassportText = Text_Create(
						0, -16, g_GameFlow.strings[GS_PASSPORT_LOAD_GAME]);
					Text_AlignBottom(m_PassportText, 1);
					Text_CentreH(m_PassportText, 1);
				}

				if (g_InputDB.select || g_InvMode == INV_LOAD_MODE)
				{
					Text_Remove(g_InvRingText);
					g_InvRingText = NULL;
					Text_Remove(g_InvItemText[IT_NAME]);
					g_InvItemText[IT_NAME] = NULL;

					g_LoadSaveGameRequester.flags |= RIF_BLOCKABLE;
					InitLoadSaveGameRequester();
					m_PassportMode = 1;

					g_Input.any = 0;
					g_InputDB.any = 0;
				}
			}
		}
		break;
	// new game если в титлах, save game если в игре
	case 1:
		if (m_PassportMode == 2)
		{
			int32_t select = DisplayRequester(&m_NewGameRequester);
			if (select)
			{
				if (select > 0)
				{
					g_InvExtraData[1] = select - 1;
				}
				else if (g_InvMode != INV_GAME_MODE)
				{

					g_Input.any = 0;
					g_InputDB.any = 0;
				}
				m_PassportMode = 0;
			}
			else
			{
				g_Input.any = 0;
				g_InputDB.any = 0;
			}
		}
		else if (m_PassportMode == 1)
		{
			int32_t select = DisplayRequester(&g_LoadSaveGameRequester);
			if (select)
			{
				if (select > 0)
				{
					m_PassportMode = 0;
					g_InvExtraData[1] = select - 1;
				}
				else
				{
					if (g_InvMode != INV_SAVE_MODE &&
						g_InvMode != INV_SAVE_CRYSTAL_MODE &&
						g_InvMode != INV_LOAD_MODE)
					{
						g_Input.any = 0;
						g_InputDB.any = 0;
					}
					m_PassportMode = 0;
				}
			}
			else
			{
				g_Input.any = 0;
				g_InputDB.any = 0;
			}
		}
		else if (m_PassportMode == 0)
		{
			if (g_InvMode == INV_DEATH_MODE)
			{
				if (inv_item->anim_direction == -1)
				{
					g_InputDB.any = 0;
					g_InputDB.left = 1;
				}
				else
				{
					g_InputDB.any = 0;
					g_InputDB.right = 1;
				}
			}
			if (!m_PassportText)
			{
				if (g_InvMode == INV_TITLE_MODE ||
					g_CurrentLevel == g_GameFlow.gym_level_num)
				{
					m_PassportText = Text_Create(
						0, -16, g_GameFlow.strings[GS_PASSPORT_NEW_GAME]);
				}
				else
				{
					m_PassportText = Text_Create(
						0, -16, g_GameFlow.strings[GS_PASSPORT_SAVE_GAME]);
				}

				Text_AlignBottom(m_PassportText, 1);
				Text_CentreH(m_PassportText, 1);
			}
			if (g_InputDB.select || g_InvMode == INV_SAVE_MODE)
			{
				if (g_InvMode == INV_TITLE_MODE ||
					g_CurrentLevel == g_GameFlow.gym_level_num)
				{
					Text_Remove(g_InvRingText);
					g_InvRingText = NULL;
					Text_Remove(g_InvItemText[IT_NAME]);
					g_InvItemText[IT_NAME] = NULL;
				}
				else
				{
					Text_Remove(g_InvRingText);
					g_InvRingText = NULL;
					Text_Remove(g_InvItemText[IT_NAME]);
					g_InvItemText[IT_NAME] = NULL;

					g_LoadSaveGameRequester.flags &= ~RIF_BLOCKABLE;
					InitLoadSaveGameRequester();
					m_PassportMode = 1;

					g_Input.any = 0;
					g_InputDB.any = 0;
				}
			}
		}
		break;
	// exit game в титлах, exit to tile в игре
	case 2:
		if (!m_PassportText)
		{
			if (g_InvMode == INV_TITLE_MODE)
			{
				m_PassportText = Text_Create(
					0, -16, g_GameFlow.strings[GS_PASSPORT_EXIT_GAME]);
			}
			else
			{
				m_PassportText = Text_Create(
					0, -16, g_GameFlow.strings[GS_PASSPORT_EXIT_TO_TITLE]);
			}

			Text_AlignBottom(m_PassportText, 1);
			Text_CentreH(m_PassportText, 1);
		}
		break;
	}

	/*
	bool pages_available[3] = {
		g_SavedGamesCount > 0,
		g_InvMode == INV_TITLE_MODE || g_InvMode == INV_SAVE_CRYSTAL_MODE
			|| !g_GameFlow.enable_save_crystals,
		true,
	};
	*/

	bool pages_available[3] = {
		g_SavedGamesCount > 0,
		true,
		true,
	};

	if (g_InputDB.left && (g_InvMode != INV_DEATH_MODE || g_SavedGamesCount))
	{
		while (--page >= 0)
		{
			if (pages_available[page])
			{
				break;
			}
		}

		if (page >= 0)
		{
			inv_item->anim_direction = -1;
			inv_item->goal_frame = inv_item->open_frame + 5 * page;
			Sound_Effect(SFX_MENU_PASSPORT, NULL, SPM_ALWAYS);

			if (m_PassportText)
			{
				Text_Remove(m_PassportText);
				m_PassportText = NULL;
			}
		}

		g_Input.any = 0;
		g_InputDB.any = 0;
	}

	if (g_InputDB.right)
	{
		g_Input.any = 0;
		g_InputDB.any = 0;

		while (++page < PASSPORT_PAGE_COUNT)
		{
			if (pages_available[page])
			{
				break;
			}
		}

		if (page < PASSPORT_PAGE_COUNT)
		{
			inv_item->anim_direction = 1;
			inv_item->goal_frame = inv_item->open_frame + 5 * page;
			Sound_Effect(SFX_MENU_PASSPORT, NULL, SPM_ALWAYS);

			if (m_PassportText)
			{
				Text_Remove(m_PassportText);
				m_PassportText = NULL;
			}
		}
	}

	if (g_InputDB.deselect)
	{
		if (g_InvMode == INV_DEATH_MODE)
		{
			g_Input.any = 0;
			g_InputDB.any = 0;
		}
		else
		{
			if (page == 2)
			{
				inv_item->anim_direction = 1;
				inv_item->goal_frame = inv_item->frames_total - 1;
			}
			else
			{
				inv_item->goal_frame = 0;
				inv_item->anim_direction = -1;
			}

			if (m_PassportText)
			{
				Text_Remove(m_PassportText);
				m_PassportText = NULL;
			}
		}
	}

	if (g_InputDB.select)
	{
		g_InvExtraData[0] = page;

		if (page == 2)
		{
			inv_item->anim_direction = 1;
			inv_item->goal_frame = inv_item->frames_total - 1;
		}
		else
		{
			inv_item->goal_frame = 0;
			inv_item->anim_direction = -1;
		}

		if (m_PassportText)
		{
			Text_Remove(m_PassportText);
			m_PassportText = NULL;
		}
	}
}
