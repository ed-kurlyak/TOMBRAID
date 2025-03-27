#pragma once

#include "types.h"

//extern uint32_t g_CurrentLevel;
extern int g_bLevel_Loaded;

void Init_Game_Malloc();
void Play_FMV(int v1, int v2);
void Play_FMV_Init_Malloc(int v1, int v2);
int Start_New_Game(int LevelNum);
//int Start_New_Game(int LevelNum);
void Init_GameFlow();