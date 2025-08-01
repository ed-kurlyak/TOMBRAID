#include <malloc.h>
#include <windows.h>

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
	g_GameFlow.strings[GS_HEADING_INVENTORY] = "INVENTORY";
	g_GameFlow.strings[GS_HEADING_GAME_OVER] = "GAME OVER";
	g_GameFlow.strings[GS_HEADING_OPTION] = "OPTION";
	g_GameFlow.strings[GS_HEADING_ITEMS] = "ITEMS";

	g_GameFlow.strings[GS_PASSPORT_SELECT_LEVEL] = "Select Level";
	g_GameFlow.strings[GS_PASSPORT_SELECT_MODE] = "Select Mode";
	g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME] = "New Game";
	g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_PLUS] = "New Game+";
	g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP] = "Japanese NG";
	g_GameFlow.strings[GS_PASSPORT_MODE_NEW_GAME_JP_PLUS] = "Japanese NG+";
	g_GameFlow.strings[GS_PASSPORT_NEW_GAME] = "New Game";
	g_GameFlow.strings[GS_PASSPORT_LOAD_GAME] = "Load Game";
	g_GameFlow.strings[GS_PASSPORT_SAVE_GAME] = "Save Game";
	g_GameFlow.strings[GS_PASSPORT_EXIT_GAME] = "Exit Game";
	g_GameFlow.strings[GS_PASSPORT_EXIT_TO_TITLE] = "Exit to Title";
	g_GameFlow.strings[GS_DETAIL_SELECT_DETAIL] = "Select Detail";
	g_GameFlow.strings[GS_DETAIL_LEVEL_HIGH] = "High";
	g_GameFlow.strings[GS_DETAIL_LEVEL_MEDIUM] = "Medium";
	g_GameFlow.strings[GS_DETAIL_LEVEL_LOW] = "Low";
	g_GameFlow.strings[GS_DETAIL_PERSPECTIVE_FMT] = "Perspective     %s";
	g_GameFlow.strings[GS_DETAIL_BILINEAR_FMT] = "Bilinear        %s";
	g_GameFlow.strings[GS_DETAIL_BRIGHTNESS_FMT] = "Brightness      %.1f";
	g_GameFlow.strings[GS_DETAIL_UI_TEXT_SCALE_FMT] = "UI text scale   %.1f";
	g_GameFlow.strings[GS_DETAIL_UI_BAR_SCALE_FMT] = "UI bar scale    %.1f";
	g_GameFlow.strings[GS_DETAIL_VIDEO_MODE_FMT] = "Game Video Mode %s";
	g_GameFlow.strings[GS_SOUND_SET_VOLUMES] = "Set Volumes";
	g_GameFlow.strings[GS_CONTROL_DEFAULT_KEYS] = "Default Keys";
	g_GameFlow.strings[GS_CONTROL_USER_KEYS] = "User Keys";
	g_GameFlow.strings[GS_KEYMAP_RUN] = "Run";
	g_GameFlow.strings[GS_KEYMAP_BACK] = "Back";
	g_GameFlow.strings[GS_KEYMAP_LEFT] = "Left";
	g_GameFlow.strings[GS_KEYMAP_RIGHT] = "Right";
	g_GameFlow.strings[GS_KEYMAP_STEP_LEFT] = "Step Left";
	g_GameFlow.strings[GS_KEYMAP_STEP_RIGHT] = "Step Right";
	g_GameFlow.strings[GS_KEYMAP_WALK] = "Walk";
	g_GameFlow.strings[GS_KEYMAP_JUMP] = "Jump";
	g_GameFlow.strings[GS_KEYMAP_ACTION] = "Action";
	g_GameFlow.strings[GS_KEYMAP_DRAW_WEAPON] = "Draw Weapon";
	g_GameFlow.strings[GS_KEYMAP_LOOK] = "Look";
	g_GameFlow.strings[GS_KEYMAP_ROLL] = "Roll";
	g_GameFlow.strings[GS_KEYMAP_INVENTORY] = "Inventory";
	g_GameFlow.strings[GS_KEYMAP_PAUSE] = "Pause";
	g_GameFlow.strings[GS_KEYMAP_FLY_CHEAT] = "Fly cheat";
	g_GameFlow.strings[GS_KEYMAP_ITEM_CHEAT] = "Item cheat";
	g_GameFlow.strings[GS_KEYMAP_LEVEL_SKIP_CHEAT] = "Level skip";
	g_GameFlow.strings[GS_KEYMAP_CAMERA_UP] = "Camera Up";
	g_GameFlow.strings[GS_KEYMAP_CAMERA_DOWN] = "Camera Down";
	g_GameFlow.strings[GS_KEYMAP_CAMERA_LEFT] = "Camera Left";
	g_GameFlow.strings[GS_KEYMAP_CAMERA_RIGHT] = "Camera Right";
	g_GameFlow.strings[GS_KEYMAP_CAMERA_RESET] = "Reset Camera";
	g_GameFlow.strings[GS_STATS_TIME_TAKEN_FMT] = "TIME TAKEN %s";
	g_GameFlow.strings[GS_STATS_SECRETS_FMT] = "SECRETS %d OF %d";
	g_GameFlow.strings[GS_STATS_PICKUPS_FMT] = "PICKUPS %d";
	g_GameFlow.strings[GS_STATS_KILLS_FMT] = "KILLS %d";
	g_GameFlow.strings[GS_PAUSE_PAUSED] = "Paused";
	g_GameFlow.strings[GS_PAUSE_EXIT_TO_TITLE] = "Exit to title?";
	g_GameFlow.strings[GS_PAUSE_CONTINUE] = "Continue";
	g_GameFlow.strings[GS_PAUSE_QUIT] = "Quit";
	g_GameFlow.strings[GS_PAUSE_ARE_YOU_SURE] = "Are you sure?";
	g_GameFlow.strings[GS_PAUSE_YES] = "Yes";
	g_GameFlow.strings[GS_PAUSE_NO] = "No";
	g_GameFlow.strings[GS_MISC_ON] = "On";
	g_GameFlow.strings[GS_MISC_OFF] = "Off";
	g_GameFlow.strings[GS_MISC_EMPTY_SLOT_FMT] = "- EMPTY SLOT %d -";
	g_GameFlow.strings[GS_MISC_DEMO_MODE] = "Demo Mode";
	g_GameFlow.strings[GS_INV_ITEM_MEDI] = "Small Medi Pack";
	g_GameFlow.strings[GS_INV_ITEM_BIG_MEDI] = "Large Medi Pack";
	g_GameFlow.strings[GS_INV_ITEM_PUZZLE1] = "Puzzle";
	g_GameFlow.strings[GS_INV_ITEM_PUZZLE2] = "Puzzle";
	g_GameFlow.strings[GS_INV_ITEM_PUZZLE3] = "Puzzle";
	g_GameFlow.strings[GS_INV_ITEM_PUZZLE4] = "Puzzle";
	g_GameFlow.strings[GS_INV_ITEM_KEY1] = "Key";
	g_GameFlow.strings[GS_INV_ITEM_KEY2] = "Key";
	g_GameFlow.strings[GS_INV_ITEM_KEY3] = "Key";
	g_GameFlow.strings[GS_INV_ITEM_KEY4] = "Key";
	g_GameFlow.strings[GS_INV_ITEM_PICKUP1] = "Pickup";
	g_GameFlow.strings[GS_INV_ITEM_PICKUP2] = "Pickup";
	g_GameFlow.strings[GS_INV_ITEM_LEADBAR] = "Lead Bar";
	g_GameFlow.strings[GS_INV_ITEM_SCION] = "Scion";
	g_GameFlow.strings[GS_INV_ITEM_PISTOLS] = "Pistols";
	g_GameFlow.strings[GS_INV_ITEM_SHOTGUN] = "Shotgun";
	g_GameFlow.strings[GS_INV_ITEM_MAGNUM] = "Magnums";
	g_GameFlow.strings[GS_INV_ITEM_UZI] = "Uzis";
	g_GameFlow.strings[GS_INV_ITEM_GRENADE] = "Grenade";
	g_GameFlow.strings[GS_INV_ITEM_PISTOL_AMMO] = "Pistol Clips";
	g_GameFlow.strings[GS_INV_ITEM_SHOTGUN_AMMO] = "Shotgun Shells";
	g_GameFlow.strings[GS_INV_ITEM_MAGNUM_AMMO] = "Magnum Clips";
	g_GameFlow.strings[GS_INV_ITEM_UZI_AMMO] = "Uzi Clips";
	g_GameFlow.strings[GS_INV_ITEM_COMPASS] = "Compass";
	g_GameFlow.strings[GS_INV_ITEM_GAME] = "Game";
	g_GameFlow.strings[GS_INV_ITEM_DETAILS] = "Detail Levels";
	g_GameFlow.strings[GS_INV_ITEM_SOUND] = "Sound";
	g_GameFlow.strings[GS_INV_ITEM_CONTROLS] = "Controls";
	g_GameFlow.strings[GS_INV_ITEM_LARAS_HOME] = "Lara's Home";

	// g_GameFlow.levels[0].level_type
	// g_GameFlow.levels[0].music = 0;
	g_GameFlow.levels[0].level_title = "Lara's Home";
	g_GameFlow.levels[0].level_file = "data\\gym.phd";
	g_GameFlow.levels[0].key1 = 0;
	g_GameFlow.levels[0].key2 = 0;
	g_GameFlow.levels[0].key3 = 0;
	g_GameFlow.levels[0].key4 = 0;
	g_GameFlow.levels[0].pickup1 = 0;
	g_GameFlow.levels[0].pickup2 = 0;
	g_GameFlow.levels[0].puzzle1 = 0;
	g_GameFlow.levels[0].puzzle2 = 0;
	g_GameFlow.levels[0].puzzle3 = 0;
	g_GameFlow.levels[0].puzzle4 = 0;
	// g_GameFlow.levels[0].demo = 0
	g_GameFlow.levels[0].secrets = 0;

	if (GameType == VER_TR1)
	{
		g_GameFlow.save_game_fmt = "saveati.%d";

		// g_GameFlow.levels[1].level_type
		// g_GameFlow.levels[1].music = 0;
		g_GameFlow.levels[1].level_title = "Caves";
		g_GameFlow.levels[1].level_file = "data\\level1.phd";
		g_GameFlow.levels[1].key1 = 0;
		g_GameFlow.levels[1].key2 = 0;
		g_GameFlow.levels[1].key3 = 0;
		g_GameFlow.levels[1].key4 = 0;
		g_GameFlow.levels[1].pickup1 = 0;
		g_GameFlow.levels[1].pickup2 = 0;
		g_GameFlow.levels[1].puzzle1 = 0;
		g_GameFlow.levels[1].puzzle2 = 0;
		g_GameFlow.levels[1].puzzle3 = 0;
		g_GameFlow.levels[1].puzzle4 = 0;
		// g_GameFlow.levels[1].demo = 0;
		g_GameFlow.levels[1].secrets = 0;

		// g_GameFlow.levels[2].level_type
		// g_GameFlow.levels[2].music = 0;
		g_GameFlow.levels[2].level_title = "City of Vilcabamba";
		g_GameFlow.levels[2].level_file = "data\\level2.phd";
		g_GameFlow.levels[2].key1 = "Silver Key";
		g_GameFlow.levels[2].key2 = 0;
		g_GameFlow.levels[2].key3 = 0;
		g_GameFlow.levels[2].key4 = 0;
		g_GameFlow.levels[2].pickup1 = 0;
		g_GameFlow.levels[2].pickup2 = 0;
		g_GameFlow.levels[2].puzzle1 = "Gold Idol";
		g_GameFlow.levels[2].puzzle2 = 0;
		g_GameFlow.levels[2].puzzle3 = 0;
		g_GameFlow.levels[2].puzzle4 = 0;
		// g_GameFlow.levels[2].demo = 0;
		g_GameFlow.levels[2].secrets = 0;

		// g_GameFlow.levels[3].level_type
		// g_GameFlow.levels[3].music = 0;
		g_GameFlow.levels[3].level_title = "Lost Valley";
		g_GameFlow.levels[3].level_file = "data\\level3a.phd";
		g_GameFlow.levels[3].key1 = 0;
		g_GameFlow.levels[3].key2 = 0;
		g_GameFlow.levels[3].key3 = 0;
		g_GameFlow.levels[3].key4 = 0;
		g_GameFlow.levels[3].pickup1 = 0;
		g_GameFlow.levels[3].pickup2 = 0;
		g_GameFlow.levels[3].puzzle1 = "Machine Cog";
		g_GameFlow.levels[3].puzzle2 = 0;
		g_GameFlow.levels[3].puzzle3 = 0;
		g_GameFlow.levels[3].puzzle4 = 0;
		// g_GameFlow.levels[3].demo = 0;
		g_GameFlow.levels[3].secrets = 0;

		// g_GameFlow.levels[4].level_type
		// g_GameFlow.levels[4].music = 0;
		g_GameFlow.levels[4].level_title = "Tomb of Qualopec";
		g_GameFlow.levels[4].level_file = "data\\level3b.phd";
		g_GameFlow.levels[4].key1 = 0;
		g_GameFlow.levels[4].key2 = 0;
		g_GameFlow.levels[4].key3 = 0;
		g_GameFlow.levels[4].key4 = 0;
		g_GameFlow.levels[4].pickup1 = 0;
		g_GameFlow.levels[4].pickup2 = 0;
		g_GameFlow.levels[4].puzzle1 = 0;
		g_GameFlow.levels[4].puzzle2 = 0;
		g_GameFlow.levels[4].puzzle3 = 0;
		g_GameFlow.levels[4].puzzle4 = 0;
		// g_GameFlow.levels[4].demo = 0;
		g_GameFlow.levels[4].secrets = 0;

		// g_GameFlow.levels[5].level_type
		// g_GameFlow.levels[5].music = 0;
		g_GameFlow.levels[5].level_title = "St. Francis' Folly";
		g_GameFlow.levels[5].level_file = "data\\level4.phd";
		g_GameFlow.levels[5].key1 = "Neptune Key";
		g_GameFlow.levels[5].key2 = "Atlas Key";
		g_GameFlow.levels[5].key3 = "Damocles Key";
		g_GameFlow.levels[5].key4 = "Thor Key";
		g_GameFlow.levels[5].pickup1 = 0;
		g_GameFlow.levels[5].pickup2 = 0;
		g_GameFlow.levels[5].puzzle1 = 0;
		g_GameFlow.levels[5].puzzle2 = 0;
		g_GameFlow.levels[5].puzzle3 = 0;
		g_GameFlow.levels[5].puzzle4 = 0;
		// g_GameFlow.levels[5].demo = 0;
		g_GameFlow.levels[5].secrets = 0;

		// g_GameFlow.levels[6].level_type
		// g_GameFlow.levels[6].music = 0;
		g_GameFlow.levels[6].level_title = "Colosseum";
		g_GameFlow.levels[6].level_file = "data\\level5.phd";
		g_GameFlow.levels[6].key1 = "Rusty Key";
		g_GameFlow.levels[6].key2 = 0;
		g_GameFlow.levels[6].key3 = 0;
		g_GameFlow.levels[6].key4 = 0;
		g_GameFlow.levels[6].pickup1 = 0;
		g_GameFlow.levels[6].pickup2 = 0;
		g_GameFlow.levels[6].puzzle1 = 0;
		g_GameFlow.levels[6].puzzle2 = 0;
		g_GameFlow.levels[6].puzzle3 = 0;
		g_GameFlow.levels[6].puzzle4 = 0;
		// g_GameFlow.levels[6].demo = 0;
		g_GameFlow.levels[6].secrets = 0;

		// g_GameFlow.levels[7].level_type
		// g_GameFlow.levels[7].music = 0;
		g_GameFlow.levels[7].level_title = "Palace Midas";
		g_GameFlow.levels[7].level_file = "data\\level6.phd";
		g_GameFlow.levels[7].key1 = 0;
		g_GameFlow.levels[7].key2 = 0;
		g_GameFlow.levels[7].key3 = 0;
		g_GameFlow.levels[7].key4 = 0;
		g_GameFlow.levels[7].pickup1 = 0;
		g_GameFlow.levels[7].pickup2 = 0;
		g_GameFlow.levels[7].puzzle1 = "Gold Bar";
		g_GameFlow.levels[7].puzzle2 = 0;
		g_GameFlow.levels[7].puzzle3 = 0;
		g_GameFlow.levels[7].puzzle4 = 0;
		// g_GameFlow.levels[7].demo = 0;
		g_GameFlow.levels[7].secrets = 0;

		// g_GameFlow.levels[8].level_type
		// g_GameFlow.levels[8].music = 0;
		g_GameFlow.levels[8].level_title = "The Cistern";
		g_GameFlow.levels[8].level_file = "data\\level7a.phd";
		g_GameFlow.levels[8].key1 = "Gold Key";
		g_GameFlow.levels[8].key2 = "Silver Key";
		g_GameFlow.levels[8].key3 = "Rusty Key";
		g_GameFlow.levels[8].key4 = 0;
		g_GameFlow.levels[8].pickup1 = 0;
		g_GameFlow.levels[8].pickup2 = 0;
		g_GameFlow.levels[8].puzzle1 = 0;
		g_GameFlow.levels[8].puzzle2 = 0;
		g_GameFlow.levels[8].puzzle3 = 0;
		g_GameFlow.levels[8].puzzle4 = 0;
		// g_GameFlow.levels[8].demo = 0;
		g_GameFlow.levels[8].secrets = 0;

		// g_GameFlow.levels[9].level_type
		// g_GameFlow.levels[9].music = 0;
		g_GameFlow.levels[9].level_title = "Tomb of Tihocan";
		g_GameFlow.levels[9].level_file = "data\\level7b.phd";
		g_GameFlow.levels[9].key1 = "Gold Key";
		g_GameFlow.levels[9].key2 = "Rusty Key";
		g_GameFlow.levels[9].key3 = "Rusty Key";
		g_GameFlow.levels[9].key4 = 0;
		g_GameFlow.levels[9].pickup1 = 0;
		g_GameFlow.levels[9].pickup2 = 0;
		g_GameFlow.levels[9].puzzle1 = 0;
		g_GameFlow.levels[9].puzzle2 = 0;
		g_GameFlow.levels[9].puzzle3 = 0;
		g_GameFlow.levels[9].puzzle4 = 0;
		// g_GameFlow.levels[9].demo = 0;
		g_GameFlow.levels[9].secrets = 0;

		// g_GameFlow.levels[10].level_type
		// g_GameFlow.levels[10].music = 0;
		g_GameFlow.levels[10].level_title = "City of Khamoon";
		g_GameFlow.levels[10].level_file = "data\\level8a.phd";
		g_GameFlow.levels[10].key1 = "Sapphire Key";
		g_GameFlow.levels[10].key2 = 0;
		g_GameFlow.levels[10].key3 = 0;
		g_GameFlow.levels[10].key4 = 0;
		g_GameFlow.levels[10].pickup1 = 0;
		g_GameFlow.levels[10].pickup2 = 0;
		g_GameFlow.levels[10].puzzle1 = 0;
		g_GameFlow.levels[10].puzzle2 = 0;
		g_GameFlow.levels[10].puzzle3 = 0;
		g_GameFlow.levels[10].puzzle4 = 0;
		// g_GameFlow.levels[10].demo = 0;
		g_GameFlow.levels[10].secrets = 0;

		// g_GameFlow.levels[11].level_type
		// g_GameFlow.levels[11].music = 0;
		g_GameFlow.levels[11].level_title = "Obelisk of Khamoon";
		g_GameFlow.levels[11].level_file = "data\\level8b.phd";
		g_GameFlow.levels[11].key1 = "Sapphire Key";
		g_GameFlow.levels[11].key2 = 0;
		g_GameFlow.levels[11].key3 = 0;
		g_GameFlow.levels[11].key4 = 0;
		g_GameFlow.levels[11].pickup1 = 0;
		g_GameFlow.levels[11].pickup2 = 0;
		g_GameFlow.levels[11].puzzle1 = "Eye of Horus";
		g_GameFlow.levels[11].puzzle2 = "Scarab";
		g_GameFlow.levels[11].puzzle3 = "Seal of Anubis";
		g_GameFlow.levels[11].puzzle4 = "Ankh";
		// g_GameFlow.levels[11].demo = 0;
		g_GameFlow.levels[11].secrets = 0;

		// g_GameFlow.levels[12].level_type
		// g_GameFlow.levels[12].music = 0;
		g_GameFlow.levels[12].level_title = "Sanctuary of the Scion";
		g_GameFlow.levels[12].level_file = "data\\level8c.phd";
		g_GameFlow.levels[12].key1 = "Gold Key";
		g_GameFlow.levels[12].key2 = 0;
		g_GameFlow.levels[12].key3 = 0;
		g_GameFlow.levels[12].key4 = 0;
		g_GameFlow.levels[12].pickup1 = 0;
		g_GameFlow.levels[12].pickup2 = 0;
		g_GameFlow.levels[12].puzzle1 = "Ankh";
		g_GameFlow.levels[12].puzzle2 = "Scarab";
		g_GameFlow.levels[12].puzzle3 = 0;
		g_GameFlow.levels[12].puzzle4 = 0;
		// g_GameFlow.levels[12].demo = 0;
		g_GameFlow.levels[12].secrets = 0;

		// g_GameFlow.levels[13].level_type
		// g_GameFlow.levels[13].music = 0;
		g_GameFlow.levels[13].level_title = "Natla's Mines";
		g_GameFlow.levels[13].level_file = "data\\level10a.phd";
		g_GameFlow.levels[13].key1 = 0;
		g_GameFlow.levels[13].key2 = 0;
		g_GameFlow.levels[13].key3 = 0;
		g_GameFlow.levels[13].key4 = 0;
		g_GameFlow.levels[13].pickup1 = 0;
		g_GameFlow.levels[13].pickup2 = 0;
		g_GameFlow.levels[13].puzzle1 = "Fuse";
		g_GameFlow.levels[13].puzzle2 = "Pyramid Key";
		g_GameFlow.levels[13].puzzle3 = 0;
		g_GameFlow.levels[13].puzzle4 = 0;
		// g_GameFlow.levels[13].demo = 0;
		g_GameFlow.levels[13].secrets = 0;

		// g_GameFlow.levels[14].level_type
		// g_GameFlow.levels[14].music = 0;
		g_GameFlow.levels[14].level_title = "Atlantis";
		g_GameFlow.levels[14].level_file = "data\\level10b.phd";
		g_GameFlow.levels[14].key1 = 0;
		g_GameFlow.levels[14].key2 = 0;
		g_GameFlow.levels[14].key3 = 0;
		g_GameFlow.levels[14].key4 = 0;
		g_GameFlow.levels[14].pickup1 = 0;
		g_GameFlow.levels[14].pickup2 = 0;
		g_GameFlow.levels[14].puzzle1 = 0;
		g_GameFlow.levels[14].puzzle2 = 0;
		g_GameFlow.levels[14].puzzle3 = 0;
		g_GameFlow.levels[14].puzzle4 = 0;
		// g_GameFlow.levels[14].demo = 0;
		g_GameFlow.levels[14].secrets = 0;

		// g_GameFlow.levels[15].level_type
		// g_GameFlow.levels[15].music = 0;
		g_GameFlow.levels[15].level_title = "The Great Pyramid";
		g_GameFlow.levels[15].level_file = "data\\level10c.phd";
		g_GameFlow.levels[15].key1 = 0;
		g_GameFlow.levels[15].key2 = 0;
		g_GameFlow.levels[15].key3 = 0;
		g_GameFlow.levels[15].key4 = 0;
		g_GameFlow.levels[15].pickup1 = 0;
		g_GameFlow.levels[15].pickup2 = 0;
		g_GameFlow.levels[15].puzzle1 = 0;
		g_GameFlow.levels[15].puzzle2 = 0;
		g_GameFlow.levels[15].puzzle3 = 0;
		g_GameFlow.levels[15].puzzle4 = 0;
		// g_GameFlow.levels[15].demo = 0;
		g_GameFlow.levels[15].secrets = 0;

		// g_GameFlow.levels[16].level_type
		// g_GameFlow.levels[16].music = 0;
		g_GameFlow.levels[16].level_title = "Cut Scene 1";
		g_GameFlow.levels[16].level_file = "data\\cut1.phd";
		g_GameFlow.levels[16].key1 = 0;
		g_GameFlow.levels[16].key2 = 0;
		g_GameFlow.levels[16].key3 = 0;
		g_GameFlow.levels[16].key4 = 0;
		g_GameFlow.levels[16].pickup1 = 0;
		g_GameFlow.levels[16].pickup2 = 0;
		g_GameFlow.levels[16].puzzle1 = 0;
		g_GameFlow.levels[16].puzzle2 = 0;
		g_GameFlow.levels[16].puzzle3 = 0;
		g_GameFlow.levels[16].puzzle4 = 0;
		// g_GameFlow.levels[16].demo = 0;
		g_GameFlow.levels[16].secrets = 0;

		// g_GameFlow.levels[17].level_type
		// g_GameFlow.levels[17].music = 0;
		g_GameFlow.levels[17].level_title = "Cut Scene 2";
		g_GameFlow.levels[17].level_file = "data\\cut2.phd";
		g_GameFlow.levels[17].key1 = 0;
		g_GameFlow.levels[17].key2 = 0;
		g_GameFlow.levels[17].key3 = 0;
		g_GameFlow.levels[17].key4 = 0;
		g_GameFlow.levels[17].pickup1 = 0;
		g_GameFlow.levels[17].pickup2 = 0;
		g_GameFlow.levels[17].puzzle1 = 0;
		g_GameFlow.levels[17].puzzle2 = 0;
		g_GameFlow.levels[17].puzzle3 = 0;
		g_GameFlow.levels[17].puzzle4 = 0;
		// g_GameFlow.levels[17].demo = 0;
		g_GameFlow.levels[17].secrets = 0;

		// g_GameFlow.levels[18].level_type
		// g_GameFlow.levels[18].music = 0;
		g_GameFlow.levels[18].level_title = "Cut Scene 3";
		g_GameFlow.levels[18].level_file = "data\\cut3.phd";
		g_GameFlow.levels[18].key1 = 0;
		g_GameFlow.levels[18].key2 = 0;
		g_GameFlow.levels[18].key3 = 0;
		g_GameFlow.levels[18].key4 = 0;
		g_GameFlow.levels[18].pickup1 = 0;
		g_GameFlow.levels[18].pickup2 = 0;
		g_GameFlow.levels[18].puzzle1 = 0;
		g_GameFlow.levels[18].puzzle2 = 0;
		g_GameFlow.levels[18].puzzle3 = 0;
		g_GameFlow.levels[18].puzzle4 = 0;
		// g_GameFlow.levels[18].demo = 0;
		g_GameFlow.levels[18].secrets = 0;

		// g_GameFlow.levels[19].level_type
		// g_GameFlow.levels[19].music = 0;
		g_GameFlow.levels[19].level_title = "Cut Scene 4";
		g_GameFlow.levels[19].level_file = "data\\cut4.phd";
		g_GameFlow.levels[19].key1 = 0;
		g_GameFlow.levels[19].key2 = 0;
		g_GameFlow.levels[19].key3 = 0;
		g_GameFlow.levels[19].key4 = 0;
		g_GameFlow.levels[19].pickup1 = 0;
		g_GameFlow.levels[19].pickup2 = 0;
		g_GameFlow.levels[19].puzzle1 = 0;
		g_GameFlow.levels[19].puzzle2 = 0;
		g_GameFlow.levels[19].puzzle3 = 0;
		g_GameFlow.levels[19].puzzle4 = 0;
		// g_GameFlow.levels[19].demo = 0;
		g_GameFlow.levels[19].secrets = 0;

		// g_GameFlow.levels[20].level_type
		// g_GameFlow.levels[20].music = 0;
		g_GameFlow.levels[20].level_title = "Title";
		g_GameFlow.levels[20].level_file = "data\\title.phd";
		g_GameFlow.levels[20].key1 = 0;
		g_GameFlow.levels[20].key2 = 0;
		g_GameFlow.levels[20].key3 = 0;
		g_GameFlow.levels[20].key4 = 0;
		g_GameFlow.levels[20].pickup1 = 0;
		g_GameFlow.levels[20].pickup2 = 0;
		g_GameFlow.levels[20].puzzle1 = 0;
		g_GameFlow.levels[20].puzzle2 = 0;
		g_GameFlow.levels[20].puzzle3 = 0;
		g_GameFlow.levels[20].puzzle4 = 0;
		// g_GameFlow.levels[20].demo = 0;
		g_GameFlow.levels[20].secrets = 0;

		// уровень 21
		// Ётот уровень необходим дл€ чтени€ файлов сохранений TombATI!
		// ¬ оригинальной игре (OG) есть специальный уровень под названием
		// LV_CURRENT дл€ обработки логики сохранени€/загрузки. ¬ T1M этот трюк
		// больше не используетс€. ќднако существующие сохранЄнные игры ожидают,
		// что количество уровней будет совпадать, иначе игра вылетит. ѕоэтому
		// добавлен этот фиктивный уровень.

		// g_GameFlow.levels[21].level_type
		// g_GameFlow.levels[21].music = 0;
		g_GameFlow.levels[21].level_title = "Current Position";
		g_GameFlow.levels[21].level_file = "data\\current.phd";
		g_GameFlow.levels[21].key1 = 0;
		g_GameFlow.levels[21].key2 = 0;
		g_GameFlow.levels[21].key3 = 0;
		g_GameFlow.levels[21].key4 = 0;
		g_GameFlow.levels[21].pickup1 = 0;
		g_GameFlow.levels[21].pickup2 = 0;
		g_GameFlow.levels[21].puzzle1 = 0;
		g_GameFlow.levels[21].puzzle2 = 0;
		g_GameFlow.levels[21].puzzle3 = 0;
		g_GameFlow.levels[21].puzzle4 = 0;
		// g_GameFlow.levels[21].demo = 0;
		g_GameFlow.levels[21].secrets = 0;
	}
	else
	{
		g_GameFlow.save_game_fmt = "saveuba.%d";

		// g_GameFlow.levels[1].level_type
		// g_GameFlow.levels[1].music = 0;
		g_GameFlow.levels[1].level_title = "Return to Egypt";
		g_GameFlow.levels[1].level_file = "data\\egypt.phd";
		g_GameFlow.levels[1].key1 = "Gold Key";
		g_GameFlow.levels[1].key2 = 0;
		g_GameFlow.levels[1].key3 = 0;
		g_GameFlow.levels[1].key4 = 0;
		g_GameFlow.levels[1].pickup1 = 0;
		g_GameFlow.levels[1].pickup2 = 0;
		g_GameFlow.levels[1].puzzle1 = 0;
		g_GameFlow.levels[1].puzzle2 = 0;
		g_GameFlow.levels[1].puzzle3 = 0;
		g_GameFlow.levels[1].puzzle4 = 0;
		// g_GameFlow.levels[1].demo = 0;
		g_GameFlow.levels[1].secrets = 0;

		// g_GameFlow.levels[2].level_type
		// g_GameFlow.levels[2].music = 0;
		g_GameFlow.levels[2].level_title = "Temple of the Cat";
		g_GameFlow.levels[2].level_file = "data\\cat.phd";
		g_GameFlow.levels[2].key1 = "Ornate Key";
		g_GameFlow.levels[2].key2 = 0;
		g_GameFlow.levels[2].key3 = 0;
		g_GameFlow.levels[2].key4 = 0;
		g_GameFlow.levels[2].pickup1 = 0;
		g_GameFlow.levels[2].pickup2 = 0;
		g_GameFlow.levels[2].puzzle1 = 0;
		g_GameFlow.levels[2].puzzle2 = 0;
		g_GameFlow.levels[2].puzzle3 = 0;
		g_GameFlow.levels[2].puzzle4 = 0;
		// g_GameFlow.levels[2].demo = 0;
		g_GameFlow.levels[2].secrets = 0;

		// g_GameFlow.levels[3].level_type
		// g_GameFlow.levels[3].music = 0;
		g_GameFlow.levels[3].level_title = "Atlantean Stronghold";
		g_GameFlow.levels[3].level_file = "data\\end.phd";
		g_GameFlow.levels[3].key1 = 0;
		g_GameFlow.levels[3].key2 = 0;
		g_GameFlow.levels[3].key3 = 0;
		g_GameFlow.levels[3].key4 = 0;
		g_GameFlow.levels[3].pickup1 = 0;
		g_GameFlow.levels[3].pickup2 = 0;
		g_GameFlow.levels[3].puzzle1 = 0;
		g_GameFlow.levels[3].puzzle2 = 0;
		g_GameFlow.levels[3].puzzle3 = 0;
		g_GameFlow.levels[3].puzzle4 = 0;
		// g_GameFlow.levels[3].demo = 0;
		g_GameFlow.levels[3].secrets = 0;

		// g_GameFlow.levels[4].level_type
		// g_GameFlow.levels[4].music = 0;
		g_GameFlow.levels[4].level_title = "The Hive";
		g_GameFlow.levels[4].level_file = "data\\end2.phd";
		g_GameFlow.levels[4].key1 = 0;
		g_GameFlow.levels[4].key2 = 0;
		g_GameFlow.levels[4].key3 = 0;
		g_GameFlow.levels[4].key4 = 0;
		g_GameFlow.levels[4].pickup1 = 0;
		g_GameFlow.levels[4].pickup2 = 0;
		g_GameFlow.levels[4].puzzle1 = 0;
		g_GameFlow.levels[4].puzzle2 = 0;
		g_GameFlow.levels[4].puzzle3 = 0;
		g_GameFlow.levels[4].puzzle4 = 0;
		// g_GameFlow.levels[4].demo = 0;
		g_GameFlow.levels[4].secrets = 0;

		// g_GameFlow.levels[5].level_type
		// g_GameFlow.levels[5].music = 0;
		g_GameFlow.levels[5].level_title = "Title";
		g_GameFlow.levels[5].level_file = "data\\title.phd";
		g_GameFlow.levels[5].key1 = 0;
		g_GameFlow.levels[5].key2 = 0;
		g_GameFlow.levels[5].key3 = 0;
		g_GameFlow.levels[5].key4 = 0;
		g_GameFlow.levels[5].pickup1 = 0;
		g_GameFlow.levels[5].pickup2 = 0;
		g_GameFlow.levels[5].puzzle1 = 0;
		g_GameFlow.levels[5].puzzle2 = 0;
		g_GameFlow.levels[5].puzzle3 = 0;
		g_GameFlow.levels[5].puzzle4 = 0;
		// g_GameFlow.levels[5].demo = 0;
		g_GameFlow.levels[5].secrets = 0;
	}
}

int Start_New_Game(int LevelNum)
{

	int32_t result = 0;

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
		if (g_CurrentLevel == 0)
		{
			return GF_EXIT_TO_TITLE;
		}
		else
		{
			return GF_LEVEL_COMPLETE;
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
