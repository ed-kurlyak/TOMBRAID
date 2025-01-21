#pragma once

#include "types.h"

extern int8_t * g_GameMemory;
extern int8_t * g_Malloc_Ptr;

extern uint32_t g_Malloc_Size; //1024 * 1024 * 7 / 2 = 3.5 Megs
extern uint32_t g_Malloc_Free;
extern uint32_t g_Malloc_Used;

void Init_Game_Malloc();
void *Game_Alloc(uint32_t MemSize, GAME_BUFFER BufferType);