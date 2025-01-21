#include <windows.h>
#include <malloc.h>

#include "gameflow.h"
#include "init.h"
#include "backbuffer.h"
#include "setup.h"
#include "game.h"

uint32_t g_CurrentLevel;
int g_bLevel_Loaded;


void Play_FMV(int v1, int v2)
{

}

void Play_FMV_Init_Malloc(int v1, int v2)
{
	if(g_GameMemory)
		free(g_GameMemory);

	Play_FMV(v1, v2);

	g_GameMemory = (int8_t*)malloc(g_Malloc_Size);
	
	if( !g_GameMemory )
	{
		MessageBox(NULL, "ERROR: Could not allocate enough memory", "INFO", MB_OK);
	}

	Init_Game_Malloc();

}

int Start_New_Game(int LevelNum)
{

	int32_t result = 0;
	
	switch(LevelNum)
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

	g_bLevel_Loaded = 0;

	//level types
	/*
	typedef enum GAMEFLOW_LEVEL_TYPE {
    GFL_TITLE = 0,
    GFL_NORMAL = 1,
    GFL_SAVED = 2,
    GFL_DEMO = 3,
    GFL_CUTSCENE = 4,
    GFL_GYM = 5,
    GFL_CURRENT = 6, // legacy level type for reading TombATI's savegames
		} GAMEFLOW_LEVEL_TYPE;
		*/
	//if saved level 0x15 type
	if(g_CurrentLevel != 0x15) //21d GFL_SAVED level type
	{
		Initialise_Level_Flags();
	}

	
	if(!Initialise_Level(LevelNum))
	{
		g_CurrentLevel = 0;

		return 256;
	}

	//new for tr1
	//создаем палитру после загрузки уровня
	//т.е. данных о палитре из файла уровня
	Create_BackBuffer();

	result = Game_Loop(0);

	if(result == 256)
		return result;

	return result;
}
