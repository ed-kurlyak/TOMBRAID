#include <malloc.h>
#include <windows.h>
#include <stdio.h>
#include "game.h"
#include "gameflow.h"
#include "init.h"
#include "setup.h"
#include "vars.h"

int g_LevelLoaded;

void Play_FMV(int v1, int v2) {}

void Play_FMV_Init_Malloc(int v1, int v2)
{
	if (g_GameMemory)
		free(g_GameMemory);

	Play_FMV(v1, v2);

	g_GameMemory = (int8_t *)malloc(g_Malloc_Size);

	if (!g_GameMemory)
	{
		MessageBox(NULL, "ERROR: Could not allocate enough memory", "INFO",
				   MB_OK);
	}

	Init_Game_Malloc();
}

void Init_GameFlow()
{
	sprintf(&g_GameFlow.strings[GS_HEADING_INVENTORY][0], "%s", "INVENTORY");
	sprintf(&g_GameFlow.strings[GS_HEADING_GAME_OVER][0], "%s", "GAME OVER");
	sprintf(&g_GameFlow.strings[GS_HEADING_OPTION][0], "%s", "OPTION");
	sprintf(&g_GameFlow.strings[GS_HEADING_ITEMS][0], "%s", "ITEMS");

	sprintf(&g_GameFlow.strings[GS_PASSPORT_SELECT_LEVEL][0], "%s", "Select Level");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_SELECT_MODE][0], "%s", "Select Mode");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME][0], "%s", "New Game");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_PLUS][0], "%s", "New Game+");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP][0], "%s", "Japanese NG");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP_PLUS][0], "%s", "Japanese NG+");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_NEW_GAME][0], "%s", "New Game");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_LOAD_GAME][0], "%s", "Load Game");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_SAVE_GAME][0], "%s", "Save Game");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_EXIT_GAME][0], "%s", "Exit Game");
	sprintf(&g_GameFlow.strings[GS_PASSPORT_EXIT_TO_TITLE][0], "%s", "Exit to Title");
	sprintf(&g_GameFlow.strings[GS_DETAIL_SELECT_DETAIL][0], "%s", "Select Detail");
	sprintf(&g_GameFlow.strings[GS_DETAIL_LEVEL_HIGH][0], "%s", "High");
	sprintf(&g_GameFlow.strings[GS_DETAIL_LEVEL_MEDIUM][0], "%s", "Medium");
	sprintf(&g_GameFlow.strings[GS_DETAIL_LEVEL_LOW][0], "%s", "Low");
	sprintf(&g_GameFlow.strings[GS_DETAIL_PERSPECTIVE_FMT][0], "%s", "Perspective     %s");
	sprintf(&g_GameFlow.strings[GS_DETAIL_BILINEAR_FMT][0], "%s", "Bilinear        %s");
	sprintf(&g_GameFlow.strings[GS_DETAIL_BRIGHTNESS_FMT][0], "%s", "Brightness      %.1f");
	sprintf(&g_GameFlow.strings[GS_DETAIL_UI_TEXT_SCALE_FMT][0], "%s", "UI text scale   %.1f");
	sprintf(&g_GameFlow.strings[GS_DETAIL_UI_BAR_SCALE_FMT][0], "%s", "UI bar scale    %.1f");
	sprintf(&g_GameFlow.strings[GS_DETAIL_VIDEO_MODE_FMT][0], "%s", "Game Video Mode %s");
	sprintf(&g_GameFlow.strings[GS_SOUND_SET_VOLUMES][0], "%s", "Set Volumes");
	sprintf(&g_GameFlow.strings[GS_CONTROL_DEFAULT_KEYS][0], "%s", "Default Keys");
	sprintf(&g_GameFlow.strings[GS_CONTROL_USER_KEYS][0], "%s", "User Keys");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_RUN][0], "%s", "Run");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_BACK][0], "%s", "Back");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_LEFT][0], "%s", "Left");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_RIGHT][0], "%s", "Right");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_STEP_LEFT][0], "%s", "Step Left");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_STEP_RIGHT][0], "%s", "Step Right");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_WALK][0], "%s", "Walk");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_JUMP][0], "%s", "Jump");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_ACTION][0], "%s", "Action");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_DRAW_WEAPON][0], "%s", "Draw Weapon");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_LOOK][0], "%s", "Look");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_ROLL][0], "%s", "Roll");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_INVENTORY][0], "%s", "Inventory");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_PAUSE][0], "%s", "Pause");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_FLY_CHEAT][0], "%s", "Fly cheat");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_ITEM_CHEAT][0], "%s", "Item cheat");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_LEVEL_SKIP_CHEAT][0], "%s", "Level skip");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_CAMERA_UP][0], "%s", "Camera Up");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_CAMERA_DOWN][0], "%s", "Camera Down");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_CAMERA_LEFT][0], "%s", "Camera Left");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_CAMERA_RIGHT][0], "%s", "Camera Right");
	sprintf(&g_GameFlow.strings[GS_KEYMAP_CAMERA_RESET][0], "%s", "Reset Camera");
	sprintf(&g_GameFlow.strings[GS_STATS_TIME_TAKEN_FMT][0], "%s", "TIME TAKEN %s");
	sprintf(&g_GameFlow.strings[GS_STATS_SECRETS_FMT][0], "%s", "SECRETS %d OF %d");
	sprintf(&g_GameFlow.strings[GS_STATS_PICKUPS_FMT][0], "%s", "PICKUPS %d");
	sprintf(&g_GameFlow.strings[GS_STATS_KILLS_FMT][0], "%s", "KILLS %d");
	sprintf(&g_GameFlow.strings[GS_PAUSE_PAUSED][0], "%s", "Paused");
	sprintf(&g_GameFlow.strings[GS_PAUSE_EXIT_TO_TITLE][0], "%s", "Exit to title?");
	sprintf(&g_GameFlow.strings[GS_PAUSE_CONTINUE][0], "%s", "Continue");
	sprintf(&g_GameFlow.strings[GS_PAUSE_QUIT][0], "%s", "Quit");
	sprintf(&g_GameFlow.strings[GS_PAUSE_ARE_YOU_SURE][0], "%s", "Are you sure?");
	sprintf(&g_GameFlow.strings[GS_PAUSE_YES][0], "%s", "Yes");
	sprintf(&g_GameFlow.strings[GS_PAUSE_NO][0], "%s", "No");
	sprintf(&g_GameFlow.strings[GS_MISC_ON][0], "%s", "On");
	sprintf(&g_GameFlow.strings[GS_MISC_OFF][0], "%s", "Off");
	sprintf(&g_GameFlow.strings[GS_MISC_EMPTY_SLOT_FMT][0], "%s", "- EMPTY SLOT %d -");
	sprintf(&g_GameFlow.strings[GS_MISC_DEMO_MODE][0], "%s", "Demo Mode");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_MEDI][0], "%s", "Small Medi Pack");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_BIG_MEDI][0], "%s", "Large Medi Pack");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PUZZLE1][0], "%s", "Puzzle");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PUZZLE2][0], "%s", "Puzzle");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PUZZLE3][0], "%s", "Puzzle");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PUZZLE4][0], "%s", "Puzzle");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_KEY1][0], "%s", "Key");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_KEY2][0], "%s", "Key");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_KEY3][0], "%s", "Key");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_KEY4][0], "%s", "Key");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PICKUP1][0], "%s", "Pickup");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PICKUP2][0], "%s", "Pickup");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_LEADBAR][0], "%s", "Lead Bar");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_SCION][0], "%s", "Scion");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PISTOLS][0], "%s", "Pistols");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_SHOTGUN][0], "%s", "Shotgun");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_MAGNUM][0], "%s", "Magnums");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_UZI][0], "%s", "Uzis");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_GRENADE][0], "%s", "Grenade");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_PISTOL_AMMO][0], "%s", "Pistol Clips");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_SHOTGUN_AMMO][0], "%s", "Shotgun Shells");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_MAGNUM_AMMO][0], "%s", "Magnum Clips");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_UZI_AMMO][0], "%s", "Uzi Clips");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_COMPASS][0], "%s", "Compass");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_GAME][0], "%s", "Game");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_DETAILS][0], "%s", "Detail Levels");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_SOUND][0], "%s", "Sound");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_CONTROLS][0], "%s", "Controls");
	sprintf(&g_GameFlow.strings[GS_INV_ITEM_LARAS_HOME][0], "%s", "Lara's Home");

	// g_GameFlow.levels[0].level_type
	// g_GameFlow.levels[0].music = 0;
	sprintf(g_GameFlow.levels[0].level_title, "%s", "Lara's Home");
	sprintf(g_GameFlow.levels[0].level_file, "%s", "data\\gym.phd");
	g_GameFlow.levels[0].key1[0] = 0;
	g_GameFlow.levels[0].key2[0] = 0;
	g_GameFlow.levels[0].key3[0] = 0;
	g_GameFlow.levels[0].key4[0] = 0;
	g_GameFlow.levels[0].pickup1[0] = 0;
	g_GameFlow.levels[0].pickup2[0] = 0;
	g_GameFlow.levels[0].puzzle1[0] = 0;
	g_GameFlow.levels[0].puzzle2[0] = 0;
	g_GameFlow.levels[0].puzzle3[0] = 0;
	g_GameFlow.levels[0].puzzle4[0] = 0;
	// g_GameFlow.levels[0].demo = 0
	g_GameFlow.levels[0].secrets = 0;

	if (GameType == VER_TR1)
	{
		sprintf(g_GameFlow.save_game_fmt, "%s", "saveati.%d");

		// g_GameFlow.levels[1].level_type
		// g_GameFlow.levels[1].music = 0;
		sprintf(g_GameFlow.levels[1].level_title, "%s", "Caves");
		sprintf(g_GameFlow.levels[1].level_file, "%s", "data\\level1.phd");
		g_GameFlow.levels[1].key1[0] = 0;
		g_GameFlow.levels[1].key2[0] = 0;
		g_GameFlow.levels[1].key3[0] = 0;
		g_GameFlow.levels[1].key4[0] = 0;
		g_GameFlow.levels[1].pickup1[0] = 0;
		g_GameFlow.levels[1].pickup2[0] = 0;
		g_GameFlow.levels[1].puzzle1[0] = 0;
		g_GameFlow.levels[1].puzzle2[0] = 0;
		g_GameFlow.levels[1].puzzle3[0] = 0;
		g_GameFlow.levels[1].puzzle4[0] = 0;
		// g_GameFlow.levels[1].demo = 0;
		g_GameFlow.levels[1].secrets = 0;

		// g_GameFlow.levels[2].level_type
		// g_GameFlow.levels[2].music = 0;
		sprintf(g_GameFlow.levels[2].level_title, "%s", "City of Vilcabamba");
		sprintf(g_GameFlow.levels[2].level_file, "%s", "data\\level2.phd");
		sprintf(g_GameFlow.levels[2].key1, "%s", "Silver Key");
		g_GameFlow.levels[2].key2[0] = 0;
		g_GameFlow.levels[2].key3[0] = 0;
		g_GameFlow.levels[2].key4[0] = 0;
		g_GameFlow.levels[2].pickup1[0] = 0;
		g_GameFlow.levels[2].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[2].puzzle1, "%s", "Gold Idol");
		g_GameFlow.levels[2].puzzle2[0] = 0;
		g_GameFlow.levels[2].puzzle3[0] = 0;
		g_GameFlow.levels[2].puzzle4[0] = 0;
		// g_GameFlow.levels[2].demo = 0;
		g_GameFlow.levels[2].secrets = 0;

		// g_GameFlow.levels[3].level_type
		// g_GameFlow.levels[3].music = 0;
		sprintf(g_GameFlow.levels[3].level_title, "%s", "Lost Valley");
		sprintf(g_GameFlow.levels[3].level_file, "%s", "data\\level3a.phd");
		g_GameFlow.levels[3].key1[0] = 0;
		g_GameFlow.levels[3].key2[0] = 0;
		g_GameFlow.levels[3].key3[0] = 0;
		g_GameFlow.levels[3].key4[0] = 0;
		g_GameFlow.levels[3].pickup1[0] = 0;
		g_GameFlow.levels[3].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[3].puzzle1, "%s", "Machine Cog");
		g_GameFlow.levels[3].puzzle2[0] = 0;
		g_GameFlow.levels[3].puzzle3[0] = 0;
		g_GameFlow.levels[3].puzzle4[0] = 0;
		// g_GameFlow.levels[3].demo[0] = 0;
		g_GameFlow.levels[3].secrets = 0;

		// g_GameFlow.levels[4].level_type
		// g_GameFlow.levels[4].music[0] = 0;
		sprintf(g_GameFlow.levels[4].level_title, "%s", "Tomb of Qualopec");
		sprintf(g_GameFlow.levels[4].level_file, "%s", "data\\level3b.phd");
		g_GameFlow.levels[4].key1[0] = 0;
		g_GameFlow.levels[4].key2[0] = 0;
		g_GameFlow.levels[4].key3[0] = 0;
		g_GameFlow.levels[4].key4[0] = 0;
		g_GameFlow.levels[4].pickup1[0] = 0;
		g_GameFlow.levels[4].pickup2[0] = 0;
		g_GameFlow.levels[4].puzzle1[0] = 0;
		g_GameFlow.levels[4].puzzle2[0] = 0;
		g_GameFlow.levels[4].puzzle3[0] = 0;
		g_GameFlow.levels[4].puzzle4[0] = 0;
		// g_GameFlow.levels[4].demo[0] = 0;
		g_GameFlow.levels[4].secrets = 0;

		// g_GameFlow.levels[5].level_type
		// g_GameFlow.levels[5].music[0] = 0;
		sprintf(g_GameFlow.levels[5].level_title, "%s", "St. Francis' Folly");
		sprintf(g_GameFlow.levels[5].level_file, "%s", "data\\level4.phd");
		sprintf(g_GameFlow.levels[5].key1, "%s", "Neptune Key");
		sprintf(g_GameFlow.levels[5].key2, "%s", "Atlas Key");
		sprintf(g_GameFlow.levels[5].key3, "%s", "Damocles Key");
		sprintf(g_GameFlow.levels[5].key4, "%s", "Thor Key");
		g_GameFlow.levels[5].pickup1[0] = 0;
		g_GameFlow.levels[5].pickup2[0] = 0;
		g_GameFlow.levels[5].puzzle1[0] = 0;
		g_GameFlow.levels[5].puzzle2[0] = 0;
		g_GameFlow.levels[5].puzzle3[0] = 0;
		g_GameFlow.levels[5].puzzle4[0] = 0;
		// g_GameFlow.levels[5].demo[0] = 0;
		g_GameFlow.levels[5].secrets = 0;

		// g_GameFlow.levels[6].level_type
		// g_GameFlow.levels[6].music[0] = 0;
		sprintf(g_GameFlow.levels[6].level_title, "%s", "Colosseum");
		sprintf(g_GameFlow.levels[6].level_file, "%s", "data\\level5.phd");
		sprintf(g_GameFlow.levels[6].key1, "%s", "Rusty Key");
		g_GameFlow.levels[6].key2[0] = 0;
		g_GameFlow.levels[6].key3[0] = 0;
		g_GameFlow.levels[6].key4[0] = 0;
		g_GameFlow.levels[6].pickup1[0] = 0;
		g_GameFlow.levels[6].pickup2[0] = 0;
		g_GameFlow.levels[6].puzzle1[0] = 0;
		g_GameFlow.levels[6].puzzle2[0] = 0;
		g_GameFlow.levels[6].puzzle3[0] = 0;
		g_GameFlow.levels[6].puzzle4[0] = 0;
		// g_GameFlow.levels[6].demo[0] = 0;
		g_GameFlow.levels[6].secrets = 0;

		// g_GameFlow.levels[7].level_type
		// g_GameFlow.levels[7].music[0] = 0;
		sprintf(g_GameFlow.levels[7].level_title, "%s", "Palace Midas");
		sprintf(g_GameFlow.levels[7].level_file, "%s", "data\\level6.phd");
		g_GameFlow.levels[7].key1[0] = 0;
		g_GameFlow.levels[7].key2[0] = 0;
		g_GameFlow.levels[7].key3[0] = 0;
		g_GameFlow.levels[7].key4[0] = 0;
		g_GameFlow.levels[7].pickup1[0] = 0;
		g_GameFlow.levels[7].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[7].puzzle1, "%s", "Gold Bar");
		g_GameFlow.levels[7].puzzle2[0] = 0;
		g_GameFlow.levels[7].puzzle3[0] = 0;
		g_GameFlow.levels[7].puzzle4[0] = 0;
		// g_GameFlow.levels[7].demo[0] = 0;
		g_GameFlow.levels[7].secrets = 0;

		// g_GameFlow.levels[8].level_type
		// g_GameFlow.levels[8].music[0] = 0;
		sprintf(g_GameFlow.levels[8].level_title, "%s", "The Cistern");
		sprintf(g_GameFlow.levels[8].level_file, "%s", "data\\level7a.phd");
		sprintf(g_GameFlow.levels[8].key1, "%s", "Gold Key");
		sprintf(g_GameFlow.levels[8].key2, "%s", "Silver Key");
		sprintf(g_GameFlow.levels[8].key3, "%s", "Rusty Key");
		g_GameFlow.levels[8].key4[0] = 0;
		g_GameFlow.levels[8].pickup1[0] = 0;
		g_GameFlow.levels[8].pickup2[0] = 0;
		g_GameFlow.levels[8].puzzle1[0] = 0;
		g_GameFlow.levels[8].puzzle2[0] = 0;
		g_GameFlow.levels[8].puzzle3[0] = 0;
		g_GameFlow.levels[8].puzzle4[0] = 0;
		// g_GameFlow.levels[8].demo[0] = 0;
		g_GameFlow.levels[8].secrets = 0;

		// g_GameFlow.levels[9].level_type
		// g_GameFlow.levels[9].music[0] = 0;
		sprintf(g_GameFlow.levels[9].level_title, "%s", "Tomb of Tihocan");
		sprintf(g_GameFlow.levels[9].level_file, "%s", "data\\level7b.phd");
		sprintf(g_GameFlow.levels[9].key1, "%s", "Gold Key");
		sprintf(g_GameFlow.levels[9].key2, "%s", "Rusty Key");
		sprintf(g_GameFlow.levels[9].key3, "%s", "Rusty Key");
		g_GameFlow.levels[9].key4[0] = 0;
		g_GameFlow.levels[9].pickup1[0] = 0;
		g_GameFlow.levels[9].pickup2[0] = 0;
		g_GameFlow.levels[9].puzzle1[0] = 0;
		g_GameFlow.levels[9].puzzle2[0] = 0;
		g_GameFlow.levels[9].puzzle3[0] = 0;
		g_GameFlow.levels[9].puzzle4[0] = 0;
		// g_GameFlow.levels[9].demo[0] = 0;
		g_GameFlow.levels[9].secrets = 0;

		// g_GameFlow.levels[10].level_type
		// g_GameFlow.levels[10].music[0] = 0;
		sprintf(g_GameFlow.levels[10].level_title, "%s", "City of Khamoon");
		sprintf(g_GameFlow.levels[10].level_file, "%s", "data\\level8a.phd");
		sprintf(g_GameFlow.levels[10].key1, "%s", "Sapphire Key");
		g_GameFlow.levels[10].key2[0] = 0;
		g_GameFlow.levels[10].key3[0] = 0;
		g_GameFlow.levels[10].key4[0] = 0;
		g_GameFlow.levels[10].pickup1[0] = 0;
		g_GameFlow.levels[10].pickup2[0] = 0;
		g_GameFlow.levels[10].puzzle1[0] = 0;
		g_GameFlow.levels[10].puzzle2[0] = 0;
		g_GameFlow.levels[10].puzzle3[0] = 0;
		g_GameFlow.levels[10].puzzle4[0] = 0;
		// g_GameFlow.levels[10].demo[0] = 0;
		g_GameFlow.levels[10].secrets = 0;

		// g_GameFlow.levels[11].level_type
		// g_GameFlow.levels[11].music[0] = 0;
		sprintf(g_GameFlow.levels[11].level_title, "%s", "Obelisk of Khamoon");
		sprintf(g_GameFlow.levels[11].level_file, "%s", "data\\level8b.phd");
		sprintf(g_GameFlow.levels[11].key1, "%s", "Sapphire Key");
		g_GameFlow.levels[11].key2[0] = 0;
		g_GameFlow.levels[11].key3[0] = 0;
		g_GameFlow.levels[11].key4[0] = 0;
		g_GameFlow.levels[11].pickup1[0] = 0;
		g_GameFlow.levels[11].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[11].puzzle1, "%s", "Eye of Horus");
		sprintf(g_GameFlow.levels[11].puzzle2, "%s", "Scarab");
		sprintf(g_GameFlow.levels[11].puzzle3, "%s", "Seal of Anubis");
		sprintf(g_GameFlow.levels[11].puzzle4, "%s", "Ankh");
		// g_GameFlow.levels[11].demo[0] = 0;
		g_GameFlow.levels[11].secrets = 0;

		// g_GameFlow.levels[12].level_type
		// g_GameFlow.levels[12].music[0] = 0;
		sprintf(g_GameFlow.levels[12].level_title, "%s", "Sanctuary of the Scion");
		sprintf(g_GameFlow.levels[12].level_file, "%s", "data\\level8c.phd");
		sprintf(g_GameFlow.levels[12].key1, "%s", "Gold Key");
		g_GameFlow.levels[12].key2[0] = 0;
		g_GameFlow.levels[12].key3[0] = 0;
		g_GameFlow.levels[12].key4[0] = 0;
		g_GameFlow.levels[12].pickup1[0] = 0;
		g_GameFlow.levels[12].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[12].puzzle1, "%s", "Ankh");
		sprintf(g_GameFlow.levels[12].puzzle2, "%s", "Scarab");
		g_GameFlow.levels[12].puzzle3[0] = 0;
		g_GameFlow.levels[12].puzzle4[0] = 0;
		// g_GameFlow.levels[12].demo[0] = 0;
		g_GameFlow.levels[12].secrets = 0;

		// g_GameFlow.levels[13].level_type
		// g_GameFlow.levels[13].music[0] = 0;
		sprintf(g_GameFlow.levels[13].level_title, "%s", "Natla's Mines");
		sprintf(g_GameFlow.levels[13].level_file, "%s", "data\\level10a.phd");
		g_GameFlow.levels[13].key1[0] = 0;
		g_GameFlow.levels[13].key2[0] = 0;
		g_GameFlow.levels[13].key3[0] = 0;
		g_GameFlow.levels[13].key4[0] = 0;
		g_GameFlow.levels[13].pickup1[0] = 0;
		g_GameFlow.levels[13].pickup2[0] = 0;
		sprintf(g_GameFlow.levels[13].puzzle1, "%s", "Fuse");
		sprintf(g_GameFlow.levels[13].puzzle2, "%s", "Pyramid Key");
		g_GameFlow.levels[13].puzzle3[0] = 0;
		g_GameFlow.levels[13].puzzle4[0] = 0;
		// g_GameFlow.levels[13].demo[0] = 0;
		g_GameFlow.levels[13].secrets = 0;

		// g_GameFlow.levels[14].level_type
		// g_GameFlow.levels[14].music[0] = 0;
		sprintf(g_GameFlow.levels[14].level_title, "%s", "Atlantis");
		sprintf(g_GameFlow.levels[14].level_file, "%s", "data\\level10b.phd");
		g_GameFlow.levels[14].key1[0] = 0;
		g_GameFlow.levels[14].key2[0] = 0;
		g_GameFlow.levels[14].key3[0] = 0;
		g_GameFlow.levels[14].key4[0] = 0;
		g_GameFlow.levels[14].pickup1[0] = 0;
		g_GameFlow.levels[14].pickup2[0] = 0;
		g_GameFlow.levels[14].puzzle1[0] = 0;
		g_GameFlow.levels[14].puzzle2[0] = 0;
		g_GameFlow.levels[14].puzzle3[0] = 0;
		g_GameFlow.levels[14].puzzle4[0] = 0;
		// g_GameFlow.levels[14].demo[0] = 0;
		g_GameFlow.levels[14].secrets = 0;

		// g_GameFlow.levels[15].level_type
		// g_GameFlow.levels[15].music[0] = 0;
		sprintf(g_GameFlow.levels[15].level_title, "%s", "The Great Pyramid");
		sprintf(g_GameFlow.levels[15].level_file, "%s", "data\\level10c.phd");
		g_GameFlow.levels[15].key1[0] = 0;
		g_GameFlow.levels[15].key2[0] = 0;
		g_GameFlow.levels[15].key3[0] = 0;
		g_GameFlow.levels[15].key4[0] = 0;
		g_GameFlow.levels[15].pickup1[0] = 0;
		g_GameFlow.levels[15].pickup2[0] = 0;
		g_GameFlow.levels[15].puzzle1[0] = 0;
		g_GameFlow.levels[15].puzzle2[0] = 0;
		g_GameFlow.levels[15].puzzle3[0] = 0;
		g_GameFlow.levels[15].puzzle4[0] = 0;
		// g_GameFlow.levels[15].demo[0] = 0;
		g_GameFlow.levels[15].secrets = 0;

		// g_GameFlow.levels[16].level_type
		// g_GameFlow.levels[16].music[0] = 0;
		sprintf(g_GameFlow.levels[16].level_title, "%s", "Cut Scene 1");
		sprintf(g_GameFlow.levels[16].level_file, "%s", "data\\cut1.phd");
		g_GameFlow.levels[16].key1[0] = 0;
		g_GameFlow.levels[16].key2[0] = 0;
		g_GameFlow.levels[16].key3[0] = 0;
		g_GameFlow.levels[16].key4[0] = 0;
		g_GameFlow.levels[16].pickup1[0] = 0;
		g_GameFlow.levels[16].pickup2[0] = 0;
		g_GameFlow.levels[16].puzzle1[0] = 0;
		g_GameFlow.levels[16].puzzle2[0] = 0;
		g_GameFlow.levels[16].puzzle3[0] = 0;
		g_GameFlow.levels[16].puzzle4[0] = 0;
		// g_GameFlow.levels[16].demo[0] = 0;
		g_GameFlow.levels[16].secrets = 0;

		// g_GameFlow.levels[17].level_type
		// g_GameFlow.levels[17].music[0] = 0;
		sprintf(g_GameFlow.levels[17].level_title, "%s", "Cut Scene 2");
		sprintf(g_GameFlow.levels[17].level_file, "%s", "data\\cut2.phd");
		g_GameFlow.levels[17].key1[0] = 0;
		g_GameFlow.levels[17].key2[0] = 0;
		g_GameFlow.levels[17].key3[0] = 0;
		g_GameFlow.levels[17].key4[0] = 0;
		g_GameFlow.levels[17].pickup1[0] = 0;
		g_GameFlow.levels[17].pickup2[0] = 0;
		g_GameFlow.levels[17].puzzle1[0] = 0;
		g_GameFlow.levels[17].puzzle2[0] = 0;
		g_GameFlow.levels[17].puzzle3[0] = 0;
		g_GameFlow.levels[17].puzzle4[0] = 0;
		// g_GameFlow.levels[17].demo[0] = 0;
		g_GameFlow.levels[17].secrets = 0;

		// g_GameFlow.levels[18].level_type
		// g_GameFlow.levels[18].music[0] = 0;
		sprintf(g_GameFlow.levels[18].level_title, "%s", "Cut Scene 3");
		sprintf(g_GameFlow.levels[18].level_file, "%s", "data\\cut3.phd");
		g_GameFlow.levels[18].key1[0] = 0;
		g_GameFlow.levels[18].key2[0] = 0;
		g_GameFlow.levels[18].key3[0] = 0;
		g_GameFlow.levels[18].key4[0] = 0;
		g_GameFlow.levels[18].pickup1[0] = 0;
		g_GameFlow.levels[18].pickup2[0] = 0;
		g_GameFlow.levels[18].puzzle1[0] = 0;
		g_GameFlow.levels[18].puzzle2[0] = 0;
		g_GameFlow.levels[18].puzzle3[0] = 0;
		g_GameFlow.levels[18].puzzle4[0] = 0;
		// g_GameFlow.levels[18].demo[0] = 0;
		g_GameFlow.levels[18].secrets = 0;

		// g_GameFlow.levels[19].level_type
		// g_GameFlow.levels[19].music[0] = 0;
		sprintf(g_GameFlow.levels[19].level_title, "%s", "Cut Scene 4");
		sprintf(g_GameFlow.levels[19].level_file, "%s", "data\\cut4.phd");
		g_GameFlow.levels[19].key1[0] = 0;
		g_GameFlow.levels[19].key2[0] = 0;
		g_GameFlow.levels[19].key3[0] = 0;
		g_GameFlow.levels[19].key4[0] = 0;
		g_GameFlow.levels[19].pickup1[0] = 0;
		g_GameFlow.levels[19].pickup2[0] = 0;
		g_GameFlow.levels[19].puzzle1[0] = 0;
		g_GameFlow.levels[19].puzzle2[0] = 0;
		g_GameFlow.levels[19].puzzle3[0] = 0;
		g_GameFlow.levels[19].puzzle4[0] = 0;
		// g_GameFlow.levels[19].demo[0] = 0;
		g_GameFlow.levels[19].secrets = 0;

		// g_GameFlow.levels[20].level_type
		// g_GameFlow.levels[20].music[0] = 0;
		sprintf(g_GameFlow.levels[20].level_title, "%s", "Title");
		sprintf(g_GameFlow.levels[20].level_file, "%s", "data\\title.phd");
		g_GameFlow.levels[20].key1[0] = 0;
		g_GameFlow.levels[20].key2[0] = 0;
		g_GameFlow.levels[20].key3[0] = 0;
		g_GameFlow.levels[20].key4[0] = 0;
		g_GameFlow.levels[20].pickup1[0] = 0;
		g_GameFlow.levels[20].pickup2[0] = 0;
		g_GameFlow.levels[20].puzzle1[0] = 0;
		g_GameFlow.levels[20].puzzle2[0] = 0;
		g_GameFlow.levels[20].puzzle3[0] = 0;
		g_GameFlow.levels[20].puzzle4[0] = 0;
		// g_GameFlow.levels[20].demo[0] = 0;
		g_GameFlow.levels[20].secrets = 0;

		// уровень 21
		// Ётот уровень необходим дл€ чтени€ файлов сохранений TombATI!
		// ¬ оригинальной игре (OG) есть специальный уровень под названием
		// LV_CURRENT дл€ обработки логики сохранени€/загрузки. ¬ T1M этот трюк
		// больше не используетс€. ќднако существующие сохранЄнные игры ожидают,
		// что количество уровней будет совпадать, "%s", иначе игра вылетит. ѕоэтому
		// добавлен этот фиктивный уровень.

		// g_GameFlow.levels[21].level_type
		// g_GameFlow.levels[21].music[0] = 0;
		sprintf(g_GameFlow.levels[21].level_title, "%s", "Current Position");
		sprintf(g_GameFlow.levels[21].level_file, "%s", "data\\current.phd");
		g_GameFlow.levels[21].key1[0] = 0;
		g_GameFlow.levels[21].key2[0] = 0;
		g_GameFlow.levels[21].key3[0] = 0;
		g_GameFlow.levels[21].key4[0] = 0;
		g_GameFlow.levels[21].pickup1[0] = 0;
		g_GameFlow.levels[21].pickup2[0] = 0;
		g_GameFlow.levels[21].puzzle1[0] = 0;
		g_GameFlow.levels[21].puzzle2[0] = 0;
		g_GameFlow.levels[21].puzzle3[0] = 0;
		g_GameFlow.levels[21].puzzle4[0] = 0;
		// g_GameFlow.levels[21].demo[0] = 0;
		g_GameFlow.levels[21].secrets = 0;
	}
	else
	{
		sprintf(g_GameFlow.save_game_fmt, "%s", "saveuba.%d");

		// g_GameFlow.levels[1].level_type
		// g_GameFlow.levels[1].music[0] = 0;
		sprintf(g_GameFlow.levels[1].level_title, "%s", "Return to Egypt");
		sprintf(g_GameFlow.levels[1].level_file, "%s", "data\\egypt.phd");
		sprintf(g_GameFlow.levels[1].key1, "%s", "Gold Key");
		g_GameFlow.levels[1].key2[0] = 0;
		g_GameFlow.levels[1].key3[0] = 0;
		g_GameFlow.levels[1].key4[0] = 0;
		g_GameFlow.levels[1].pickup1[0] = 0;
		g_GameFlow.levels[1].pickup2[0] = 0;
		g_GameFlow.levels[1].puzzle1[0] = 0;
		g_GameFlow.levels[1].puzzle2[0] = 0;
		g_GameFlow.levels[1].puzzle3[0] = 0;
		g_GameFlow.levels[1].puzzle4[0] = 0;
		// g_GameFlow.levels[1].demo[0] = 0;
		g_GameFlow.levels[1].secrets = 0;

		// g_GameFlow.levels[2].level_type
		// g_GameFlow.levels[2].music[0] = 0;
		sprintf(g_GameFlow.levels[2].level_title, "%s", "Temple of the Cat");
		sprintf(g_GameFlow.levels[2].level_file, "%s", "data\\cat.phd");
		sprintf(g_GameFlow.levels[2].key1, "%s", "Ornate Key");
		g_GameFlow.levels[2].key2[0] = 0;
		g_GameFlow.levels[2].key3[0] = 0;
		g_GameFlow.levels[2].key4[0] = 0;
		g_GameFlow.levels[2].pickup1[0] = 0;
		g_GameFlow.levels[2].pickup2[0] = 0;
		g_GameFlow.levels[2].puzzle1[0] = 0;
		g_GameFlow.levels[2].puzzle2[0] = 0;
		g_GameFlow.levels[2].puzzle3[0] = 0;
		g_GameFlow.levels[2].puzzle4[0] = 0;
		// g_GameFlow.levels[2].demo[0] = 0;
		g_GameFlow.levels[2].secrets = 0;

		// g_GameFlow.levels[3].level_type
		// g_GameFlow.levels[3].music[0] = 0;
		sprintf(g_GameFlow.levels[3].level_title, "%s", "Atlantean Stronghold");
		sprintf(g_GameFlow.levels[3].level_file, "%s", "data\\end.phd");
		g_GameFlow.levels[3].key1[0] = 0;
		g_GameFlow.levels[3].key2[0] = 0;
		g_GameFlow.levels[3].key3[0] = 0;
		g_GameFlow.levels[3].key4[0] = 0;
		g_GameFlow.levels[3].pickup1[0] = 0;
		g_GameFlow.levels[3].pickup2[0] = 0;
		g_GameFlow.levels[3].puzzle1[0] = 0;
		g_GameFlow.levels[3].puzzle2[0] = 0;
		g_GameFlow.levels[3].puzzle3[0] = 0;
		g_GameFlow.levels[3].puzzle4[0] = 0;
		// g_GameFlow.levels[3].demo[0] = 0;
		g_GameFlow.levels[3].secrets = 0;

		// g_GameFlow.levels[4].level_type
		// g_GameFlow.levels[4].music[0] = 0;
		sprintf(g_GameFlow.levels[4].level_title, "%s", "The Hive");
		sprintf(g_GameFlow.levels[4].level_file, "%s", "data\\end2.phd");
		g_GameFlow.levels[4].key1[0] = 0;
		g_GameFlow.levels[4].key2[0] = 0;
		g_GameFlow.levels[4].key3[0] = 0;
		g_GameFlow.levels[4].key4[0] = 0;
		g_GameFlow.levels[4].pickup1[0] = 0;
		g_GameFlow.levels[4].pickup2[0] = 0;
		g_GameFlow.levels[4].puzzle1[0] = 0;
		g_GameFlow.levels[4].puzzle2[0] = 0;
		g_GameFlow.levels[4].puzzle3[0] = 0;
		g_GameFlow.levels[4].puzzle4[0] = 0;
		// g_GameFlow.levels[4].demo[0] = 0;
		g_GameFlow.levels[4].secrets = 0;

		// g_GameFlow.levels[5].level_type
		// g_GameFlow.levels[5].music[0] = 0;
		sprintf(g_GameFlow.levels[5].level_title, "%s", "Title");
		sprintf(g_GameFlow.levels[5].level_file, "%s", "data\\title.phd");
		g_GameFlow.levels[5].key1[0] = 0;
		g_GameFlow.levels[5].key2[0] = 0;
		g_GameFlow.levels[5].key3[0] = 0;
		g_GameFlow.levels[5].key4[0] = 0;
		g_GameFlow.levels[5].pickup1[0] = 0;
		g_GameFlow.levels[5].pickup2[0] = 0;
		g_GameFlow.levels[5].puzzle1[0] = 0;
		g_GameFlow.levels[5].puzzle2[0] = 0;
		g_GameFlow.levels[5].puzzle3[0] = 0;
		g_GameFlow.levels[5].puzzle4[0] = 0;
		// g_GameFlow.levels[5].demo[0] = 0;
		g_GameFlow.levels[5].secrets = 0;
	}
}

int Start_New_Game(int LevelNum)
{

	int32_t result = 0;

	if (GameType == VER_TR1)
	{

		switch (LevelNum)
		{
		case 0:
			Play_FMV_Init_Malloc(1, 1);
			break;

		case 1:
			Play_FMV_Init_Malloc(2, 1);
			break;

		case 5:
			Play_FMV_Init_Malloc(3, 1);
			break;

		case 10:
			Play_FMV_Init_Malloc(4, 1);
			break;

		case 13:
			Play_FMV_Init_Malloc(5, 1);
			break;

		case 14:
			Play_FMV_Init_Malloc(6, 1);
			break;

		default:
			Play_FMV_Init_Malloc(0, 0);
			break;
		}

	}

	g_CurrentLevel = LevelNum;
	g_LevelLoaded = 0;

	//если не загруженый уровень 0x15 type 21d
	if (g_LevelNumTR != 21)
	{
		Initialise_Level_Flags();
	}

	if (!Initialise_Level(LevelNum))
	{
		g_CurrentLevel = 0;

		return 256;
	}

	result = Game_Loop(0);

	// level complete
	if (g_LevelComplete)
	{
		/*
		if (g_CurrentLevel == 0)
		{
			return GF_EXIT_TO_TITLE;
		}
		else
		*/
		{
			return g_CurrentLevel | GF_LEVEL_COMPLETE;
		}
	}

	// if (result == 1)
	{
		switch (g_InvChosen)
		{
		case O_PASSPORT_OPTION:

			if (g_InvExtraData[0] == 0)
			{
				// page 1: load game
				// InitialiseStartInfo();
				S_LoadGame(&g_SaveGame, g_InvExtraData[1]);
				g_LevelNumTR = 21; // 21 значит saved level
				return GF_START_GAME | g_InvExtraData[1];
			}
			else if (g_InvExtraData[0] == 1)
			{
				// page 1: save game, or new game in gym
				// if (g_CurrentLevel == g_GameFlow.gym_level_num)
				{
					// InitialiseStartInfo();
					return GF_START_GAME | g_GameFlow.first_level_num;
					// return 1;
				}
			}
			else
			{
				// page 3: exit to title
				return GF_EXIT_TO_TITLE;
			}
		}
	}

	if (result == 256)
		return result;

	return result;
}
