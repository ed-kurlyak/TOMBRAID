#include "backbuffer.h"
#include "const.h"
#include "vars.h"
#include "screen.h"

int Screen_Width;
int Screen_Height;

MGLDC *windc = NULL;
MGLDC *dibdc = NULL;

void Create_Water_Palette()
{
	// origianl DOS filter r = g = 2.0f/3.0f, b = 1.0f
	// [0.6666667, 0.6666667, 1.0] is original DOS version filter
	// [0.45, 1.0, 1.0] is default TombATI filter.

	palette_t pal[256];

	for (int i = 0; i < 256; i++)
	{
		pal[i].red = GameWaterPalette[i].r;
		pal[i].green = GameWaterPalette[i].g;
		pal[i].blue = GameWaterPalette[i].b;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);
}

void Create_Normal_Palette()
{
	palette_t pal[256];

	for (int i = 0; i < 256; i++)
	{
		pal[i].red = GameNormalPalette[i].r;
		pal[i].green = GameNormalPalette[i].g;
		pal[i].blue = GameNormalPalette[i].b;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);
}

void Create_BackBuffer()
{

	Screen_Width = Screen_GetResWidth();
	Screen_Height = Screen_GetResHeight();

	pixel_format_t pf;

	MGL_setAppInstance(g_hInst);

	MGL_registerDriver(MGL_PACKED8NAME, PACKED8_driver);

	//для оконного режима
	MGL_initWindowed("");

	if (!MGL_changeDisplayMode(grWINDOWED))
		MGL_fatalError("Unable to use window mode!");

	if ((windc = MGL_createWindowedDC(g_hWnd)) == NULL)
		MGL_fatalError("Unable to create Windowed DC!");

	MGL_getPixelFormat(windc, &pf);

	if ((dibdc = MGL_createMemoryDC(Screen_Width, Screen_Height, 8, &pf)) ==
		NULL)
		MGL_fatalError("Unable to create Memory DC!");
}

void Clear_BackBuffer()
{
	MGL_beginDirectAccess();

	char *phd_winptr_my = NULL;

	phd_winptr_my = (char *)dibdc->surface;

	//очищаем m_BackBuffer (экран)
	for (int x = 0; x < Screen_Width; x++)
	{
		for (int y = 0; y < Screen_Height; y++)
		{
			int Index = y * Screen_Width + x;

			phd_winptr_my[Index + 0] = 0;
		}
	}
}

void Present_BackBuffer()
{
	MGL_endDirectAccess();

	// MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, Screen_Width, Screen_Height, 0, 0,
					MGL_REPLACE_MODE);

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
