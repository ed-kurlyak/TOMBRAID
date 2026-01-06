#include "backbuffer.h"
#include "const.h"
#include "vars.h"
//#include "header.h"
#include "screen.h"

int Screen_Width;
int Screen_Height;

char* phd_winptr_my = NULL;

//LPDIRECTDRAW         g_pDD1 = NULL;
LPDIRECTDRAW2        g_pDD4 = NULL;
//LPDIRECTDRAWSURFACE  g_pDdsPrimary_sw = NULL;
//LPDIRECTDRAWSURFACE  g_pDdsBackBuffer_sw = NULL;
//RECT                 g_RcScreenRect_sw;
//RECT                 g_RcViewportRect_sw;
LPDIRECTDRAWPALETTE g_pPalette = NULL;

//MGLDC *windc = NULL;
//MGLDC *dibdc = NULL;

HRESULT Create_Water_Palette()
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

	// tr ati water color
	/*
	unsigned char r = 255 * 0.45f; //114 - 0x72
	unsigned char g = 255 * 1.0f; // 255 - 0xFF
	unsigned char b = 255 * 1.0f; // 255 - 0xFF
	*/
	/*
	palette_t pal[256];

	for (int i = 0; i < 256; i++)
	{
	*/
		/*
		pal[i].red = (unsigned char) (GameNormalPalette[i].r * 0.6f);
		pal[i].green = (unsigned char) (GameNormalPalette[i].g * 0.7f);
		pal[i].blue =  (unsigned char) (GameNormalPalette[i].b * 1.0f);
		*/

		/*
		pal[i].red = (unsigned char)(GameNormalPalette[i].r * 2 / 3);
		pal[i].green = (unsigned char)(GameNormalPalette[i].g * 2 / 3);
		pal[i].blue = (unsigned char)GameNormalPalette[i].b;
		*/
		/*
		pal[i].red = GameWaterPalette[i].r;
		pal[i].green = GameWaterPalette[i].g;
		pal[i].blue = GameWaterPalette[i].b;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);

	*/




	PALETTEENTRY entries[256];
	for (int i = 0; i < 256; i++)
	{
		entries[i].peRed = GameWaterPalette[i].r;
		entries[i].peGreen = GameWaterPalette[i].g;
		entries[i].peBlue = GameWaterPalette[i].b;
		entries[i].peFlags = 0;
	}

	HRESULT hr;

	hr = g_pDD4->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, entries, &g_pPalette, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_pDdsBackBuffer->SetPalette(g_pPalette);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Create_Normal_Palette()
{
	/*
	palette_t pal[256];

	for (int i = 0; i < 256; i++)
	{
		pal[i].red = GameNormalPalette[i].r;
		pal[i].green = GameNormalPalette[i].g;
		pal[i].blue = GameNormalPalette[i].b;
	}

	MGL_setPalette(dibdc, pal, 256, 0);
	MGL_realizePalette(dibdc, 256, 0, false);
	*/


	PALETTEENTRY entries[256];
	for (int i = 0; i < 256; i++)
	{
		entries[i].peRed = GameNormalPalette[i].r;
		entries[i].peGreen = GameNormalPalette[i].g;
		entries[i].peBlue = GameNormalPalette[i].b;
		entries[i].peFlags = 0;
	}

	HRESULT hr;

	hr = g_pDD4->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, entries, &g_pPalette, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_pDdsBackBuffer->SetPalette(g_pPalette);
	if (FAILED(hr))
		return hr;

	return S_OK;

}

HRESULT Create_BackBuffer()
{

	Screen_Width = Screen_GetResWidth();
	Screen_Height = Screen_GetResHeight();
	
	HRESULT hr;

	hr = DirectDrawCreate(NULL, &g_pDD1, NULL);
	if (FAILED(hr))
		return hr;

	hr = g_pDD1->QueryInterface(IID_IDirectDraw2, (VOID * *)& g_pDD4);
	if (FAILED(hr))
		return hr;


	hr = g_pDD4->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);
	if (FAILED(hr))
		return hr;

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	hr = g_pDD4->CreateSurface(&ddsd, &g_pDdsPrimary1, NULL);
	if (FAILED(hr))
		return hr;

	g_pDdsPrimary1->QueryInterface(IID_IDirectDrawSurface3, (void**)& g_pDdsPrimary);


	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	//ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
	//ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	//ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth = Screen_Width;
	ddsd.dwHeight = Screen_Height;

	GetClientRect(g_hWnd, &g_RcScreenRect);
	GetClientRect(g_hWnd, &g_RcViewportRect);
	ClientToScreen(g_hWnd, (POINT*)& g_RcScreenRect.left);
	ClientToScreen(g_hWnd, (POINT*)& g_RcScreenRect.right);
	//ddsd.dwWidth  = g_RcScreenRect.right - g_RcScreenRect.left;
	//ddsd.dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;


	// 8-битный формат
	ZeroMemory(&ddsd.ddpfPixelFormat, sizeof(DDPIXELFORMAT));
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
	ddsd.ddpfPixelFormat.dwFourCC = 0;
	ddsd.ddpfPixelFormat.dwRGBBitCount = 8;

	ddsd.ddpfPixelFormat.dwRBitMask = 0;
	ddsd.ddpfPixelFormat.dwGBitMask = 0;
	ddsd.ddpfPixelFormat.dwBBitMask = 0;
	ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

	hr = g_pDD4->CreateSurface(&ddsd, &g_pDdsBackBuffer1, NULL);
	if (FAILED(hr))
		return hr;

	g_pDdsBackBuffer1->QueryInterface(IID_IDirectDrawSurface3, (void**)&g_pDdsBackBuffer);

	g_pDdsBackBuffer1->Release();


	return S_OK;



	/*
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

	// Create_Normal_Palette();
	*/
}

/*

void Clear_BackBuffer()
{
		MGL_beginDirectAccess();

		char * phd_winptr_my = NULL;

		phd_winptr_my = (char*)dibdc->surface;

		//очищаем m_BackBuffer (экран)
		for (int x = 0; x < Screen_GetResWidth(); x++)
		{
				for (int y = 0; y < Screen_GetResHeight(); y++)
				{
						int Index = y * Screen_GetResWidth() + x;

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

		MGL_bitBltCoord(windc, dibdc, 0, 0, Screen_GetResWidth(),
Screen_GetResHeight(), 0, 0, MGL_REPLACE_MODE);

		ReleaseDC(g_hWnd, hdcScreen);

}

*/

HRESULT Clear_BackBuffer()
{
	/*
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
	*/


	DDSURFACEDESC Desc;
	ZeroMemory(&Desc, sizeof(DDSURFACEDESC));
	Desc.dwSize = sizeof(DDSURFACEDESC);

	HRESULT hr = g_pDdsBackBuffer->Lock((RECT*)& g_RcViewportRect, &Desc, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	//char* phd_winptr_my = NULL;

	phd_winptr_my = (char*)Desc.lpSurface;

	for (int y = 0; y < Screen_Height; y++)
	{
		for (int x = 0; x < Screen_Width; x++)
		{
			//int Index =  y * 4 * WINDOW_WIDTH + x * 4;
			int Index = y * Screen_Width + x;

			phd_winptr_my[Index] = 0;
			/*
			g_Data[Index + 0] = 127;	//B
			g_Data[Index + 1] = 255;	//G
			g_Data[Index + 2] = 255;	//R
			g_Data[Index + 3] = 255;	//A
			*/

		}
	}

	return S_OK;

}

HRESULT Present_BackBuffer()
{
	/*
	MGL_endDirectAccess();

	// MGL present back buffer
	HDC hdcScreen = GetDC(g_hWnd);
	MGL_setWinDC(windc, hdcScreen);

	MGL_bitBltCoord(windc, dibdc, 0, 0, Screen_Width, Screen_Height, 0, 0,
					MGL_REPLACE_MODE);

	ReleaseDC(g_hWnd, hdcScreen);
	*/

	HRESULT hr;

	hr = g_pDdsBackBuffer->Unlock(NULL);

	hr = g_pDdsPrimary->Blt(&g_RcScreenRect, g_pDdsBackBuffer,
		&g_RcViewportRect, DDBLT_WAIT, NULL);

	return S_OK;

}

HRESULT Delete_BackBuffer()
{
	/*
	if (windc)
		MGL_destroyDC(windc);
	if (dibdc)
		MGL_destroyDC(dibdc);

	windc = dibdc = NULL;
	*/

	//освобождаем палитру
		if (g_pPalette)
		{
			g_pPalette->Release();
			g_pPalette = NULL;
		}

		//освобождаем back buffer
		if (g_pDdsBackBuffer)
		{
			g_pDdsBackBuffer->Release();
			g_pDdsBackBuffer = NULL;
		}

		//освобождаем primary surface
		if (g_pDdsPrimary)
		{
			g_pDdsPrimary->Release();
			g_pDdsPrimary = NULL;
		}

		//освобождаем DirectDraw4
		if (g_pDD4)
		{
			g_pDD4->Release();
			g_pDD4 = NULL;
		}

		//освобождаем DirectDraw1 (если используется)
		if (g_pDD1)
		{
			g_pDD1->Release();
			g_pDD1 = NULL;
		}

		return S_OK;
}
