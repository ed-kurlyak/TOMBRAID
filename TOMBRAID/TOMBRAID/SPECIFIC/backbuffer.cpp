#include "backbuffer.h"
#include "const.h"
#include "vars.h"
//#include "header.h"


MGLDC* windc = NULL;
MGLDC* dibdc = NULL;

void Create_Water_Palette()
{
	    // water color (R, G, B). 1.0 means pass-through, 0.0 means no value at all.
    // [0.6, 0.7, 1.0] is original DOS version filter,
    // [0.45, 1.0, 1.0] is default TombATI filter.

	/*
	//tr1 water color
	unsigned char r = 255 * 0.6f; //153 - 0x99
	unsigned char g = 255 * 0.7f; // 178 - 0xB2
	unsigned char b = 255 * 1.0f; // 255 - 0xFF
	*/

	//tr ati water color
	/*
	unsigned char r = 255 * 0.45f; //114 - 0x72
	unsigned char g = 255 * 1.0f; // 255 - 0xFF
	unsigned char b = 255 * 1.0f; // 255 - 0xFF
	*/
	

	palette_t	pal[256];

	for (int i = 0; i < 256; i++)
	{
		pal[i].red = (unsigned char) (GamePalette[i].r * 0.6f);
		pal[i].green = (unsigned char) (GamePalette[i].g * 0.7f);
		pal[i].blue =  (unsigned char) (GamePalette[i].b * 1.0f);
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);


}

void Create_Normal_Palette()
{
	palette_t	pal[256];

	for (int i = 0; i < 256; i++)
	{
		pal[i].red = GamePalette[i].r;
		pal[i].green = GamePalette[i].g;
		pal[i].blue = GamePalette[i].b;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);

}

void Create_BackBuffer()
{
		pixel_format_t	pf;

	MGL_setAppInstance(g_hInst);

	MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);

	//для оконного режима
	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(g_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(SCREEN_WIDTH, SCREEN_HEIGHT, 8, &pf)) == NULL)
		MGL_fatalError("Unable to create Memory DC!");

	Create_Normal_Palette();

}


void Clear_BackBuffer()
{
	MGL_beginDirectAccess();

	char * phd_winptr_my = NULL;

	phd_winptr_my = (char*)dibdc->surface;

	//очищаем m_BackBuffer (экран)
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			int Index = y * SCREEN_WIDTH + x;

			phd_winptr_my[Index + 0] = 0;
		}
	}

}

void Present_BackBuffer()
{
	MGL_endDirectAccess();

	//MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, MGL_REPLACE_MODE);

	ReleaseDC(g_hWnd, hdcScreen);

}

void Delete_BackBuffer()
{
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
}

