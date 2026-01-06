#ifndef _WINMAIN_
#define _WINMAIN_

#include <time.h>
#include <windows.h>
#include <windowsx.h>

#include "types.h"


extern int Hardware;

extern int Fullscreen;

extern int CheatsMode;

extern int lara_dist;

extern int DXNearestTexture;

//раскоментировать учет аспекта в
// matrix.cpp phd_GenerateW2V()
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

enum
{
	VER_TR1,
	VER_TR1_GOLD
};

extern int GameType;

extern int Widescreen;

extern int g_bWindowClosed;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

int Settings_Write();
int Settings_Read();
void S_SeedRandom();
void Random_SeedDraw(int32_t seed);
int32_t Random_GetDraw();
void Random_SeedControl(int32_t seed);
int32_t Random_GetControl();
void phd_InitWindow(int x, int y, int width, int height, int nearz, int farz,
					int view_angle, int scrwidth, int scrheight,
					unsigned char *BackBuff);
int SpinMessageLoop();

//void Create_Shader_Effect();

#endif