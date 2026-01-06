#include <windows.h>

#include "init.h"

int8_t *g_GameMemory = NULL;
int8_t *g_Malloc_Ptr = NULL;

// uint32_t g_Malloc_Size = 0x380000; //1024 * 1024 * 7 / 2 = 3.5 Megs
uint32_t g_Malloc_Size = 1024 * 1024 * 7; // 7 Megs
uint32_t g_Malloc_Free;
uint32_t g_Malloc_Used;

void Init_Game_Malloc()
{
	g_Malloc_Ptr = g_GameMemory;
	g_Malloc_Free = g_Malloc_Size;
	g_Malloc_Used = 0;
}

void *Game_Alloc(uint32_t MemSize, GAME_BUFFER BufferType)
{
	uint32_t AlignedSize = (MemSize + 3) & ~3;

	if (AlignedSize > g_Malloc_Free)
	{
		MessageBox(NULL, "GameBuf_Alloc(): OUT OF MEMORY", "Tomb Raider",
				   MB_OK);
	}

	void *Ptr = g_Malloc_Ptr;

	g_Malloc_Free -= AlignedSize;
	g_Malloc_Used += AlignedSize;
	g_Malloc_Ptr += AlignedSize;

	return Ptr;
}
