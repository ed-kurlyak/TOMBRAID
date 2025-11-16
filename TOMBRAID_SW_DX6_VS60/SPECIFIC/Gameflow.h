#ifndef _GAMEFLOW_
#define _GAMEFLOW_

#include "types.h"

extern int g_LevelLoaded;

void Init_Game_Malloc();
void Play_FMV(int v1, int v2);
void Play_FMV_Init_Malloc(int v1, int v2);
int Start_New_Game(int LevelNum);
void Init_GameFlow();

#endif