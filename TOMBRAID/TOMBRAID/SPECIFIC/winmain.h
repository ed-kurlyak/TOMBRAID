#pragma once

#include <windows.h>
#include <windowsx.h>
#include <time.h>

#include "types.h"

extern int lara_dist;

//раскоментировать учет аспекта в
//matrix.cpp phd_GenerateW2V()
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

enum { VER_TR1, VER_TR_GOLD };

extern int select_game;

extern int widescreen;

extern int g_bWindowClosed;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern int16_t		phd_winxmin;
extern int16_t		phd_winymin;
extern int16_t		phd_winxmax;        	/* Maximum Window X coord*/
extern int16_t		phd_winymax;        	/* Maximum Window Y coord*/
extern "C" int32_t		phd_scrwidth;

void S_SeedRandom();
void Random_SeedDraw(int32_t seed);
int32_t Random_GetDraw();
void Random_SeedControl(int32_t seed);
int32_t Random_GetControl();
void	phd_InitWindow( int  x, 	int y,
					   int  width, int height,
					   int  nearz, int farz,
					   int  view_angle,
					   int  scrwidth,
					   int	 scrheight, unsigned char *BackBuff);
int SpinMessageLoop();

