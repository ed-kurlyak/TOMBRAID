#include "winmain.h"
#include "vars.h"
#include "timer.h"
#include "backbuffer.h"
#include "draw.h"
#include "gameflow.h"
#include "init.h"
#include "inv.h"
#include "setup.h"
#include "text.h"
#include "inv.h"
#include "game.h"
#include "screen.h"
#include "input.h"
#include "sound.h"
#include "savegame.h"
#include "drawprimitive.h"

#include "3d_gen.h" //временно для phd_AlterFOV()

#include "cinema.h"

//***************************
// SETUP START
//***************************

// SETUP PART #1
//HARDWARE or SOFTWARE

int Hardware = 1;

int Fullscreen = 0;

int DXNearestTexture = 0;

//---------------------------
// SETUP PART #4 WIDESCREEN/NO
// учет аспекта в matrix.cpp phd_GenerateW2V()
int Widescreen = 0;

//---------------------------
// SETUP PART #5 GAME TYPE - TR1/GOLD
int GameType = VER_TR1;
//int GameType = VER_TR1_GOLD;

//---------------------------
// SETUP PART #6 SCREEN RESOLUTION

/*
int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;
*/


int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;



/*
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
*/


/*
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
*/


//---------------------------
// SETUP PART #7 LARA DIST
// false is original TR1 Lara dist
//влияет на g_PhdPersp
int lara_dist = false;

//---------------------------

// SETUP PART #8 CHEATS MODE
// full ammo/medi/health
int CheatsMode = 1;


//***************************
// SETUP END
//***************************

/*

TOMB ORIGINAL LEVELS LIST
Level 0 - GYM - GYM.PHD
Level 1 - Caves - LEVEL1.PHD
Level 2 - City of Vilcabamba - LEVEL2.PHD
Level 3 - Lost Valley - LEVEL3A.PHD
Level 4 - Tomb of Qualopec - LEVEL3B.PHD
Level 5 - St Francis' Folly - LEVEL4.PHD
Level 6 - Colosseum - LEVEL5.PHD
Level 7 - Palace Midas - LEVEL6.PHD
Level 8 - The Cistern - LEVEL7A.PHD
Level 9 - Tomb of Tihocan - LEVEL7B.PHD
Level 10 - City of Khamoon - LEVEL8A.PHD
Level 11 - Obelisk of Khamoon - LEVEL8B.PHD
Level 12 - Sanctuary of Scion - LEVEL8C.PHD
Level 13 - Natla's Mines - LEVEL10A.PHD
Level 14 - Atlantis - LEVEL10B.PHD
Level 15 - The Great Pyramid - LEVEL10C.PHD

TOMB GOLD LEVELS LIST
Level 0 - GYM - GYM.PHD
Level 1 - Return to Egypt - EGYPT.PHD
Level 2 - Temple of the Cat - CAT.PHD
Level 3 - Atlantean Stronghold - END.PHD
Level 4 - Hive - END2.PHD

*/

//для удаления курсора в полноэкранном режиме
int СursorCounter = 0;

// cтруктура для хранения предыдущего режима
static DEVMODE PreviousMode;
static bool IsPreviousModeStored = false;

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

int g_bWindowClosed = false;
bool g_bFocus = true;

int32_t g_RandControl = 0xD371F947;
int32_t g_RandDraw = 0xD371F947;

//функция для установки режима дисплея
bool SetDisplayMode()
{
	DEVMODE dm = {0};
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = Screen_GetResWidth();
	dm.dmPelsHeight = Screen_GetResHeight();
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if (!IsPreviousModeStored)
	{
		//сохраняем текущий режим
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &PreviousMode))
		{
			IsPreviousModeStored = true;
		}
		else
		{
			//не удалось сохранить предыдущий режим
			return false;
		}
	}

	//устанавливаем новый режим
	LONG result = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		//не удалось установить режим
		return false;
	}

	return true;
}

//функция для возврата дисплея в предыдущий режим
bool RestorePreviousDisplayMode()
{
	if (!IsPreviousModeStored)
	{
		//предыдущий режим не был сохранён
		return false;
	}

	//возвращаем предыдущий режим
	LONG result = ChangeDisplaySettings(&PreviousMode, CDS_FULLSCREEN);

	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		//не удалось вернуть предыдущий режим
		return false;
	}

	IsPreviousModeStored = false; //после восстановления сбрасываем флаг

	return true;
}

int Settings_Write()
{
	char KeyBuff[13] = {0};
	char MusicVolume = g_Config.music_volume;
	char SoundVolume = g_Config.sound_volume;

	for (int i = 0; i < INPUT_KEY_NUMBER_OF; i++)
	{
		S_INPUT_KEYCODE Key =
			S_Input_GetAssignedKeyCode(INPUT_LAYOUT_USER, (INPUT_KEY)i);

		KeyBuff[i] = Key;
	}

	FILE *Fp;

	Fp = fopen("settings.dat", "wb");

	fwrite(KeyBuff, sizeof(char) * 13, 1, Fp);

	fwrite(&MusicVolume, sizeof(char), 1, Fp);
	fwrite(&SoundVolume, sizeof(char), 1, Fp);

	fclose(Fp);

	return 1;
}

int Settings_Read()
{
	FILE *Fp;

	Fp = fopen("settings.dat", "rb");

	char KeyBuff;
	char MusicVolume;
	char SoundVolume;

	for (int i = 0; i < INPUT_KEY_NUMBER_OF; i++)
	{
		fread(&KeyBuff, sizeof(char), 1, Fp);

		S_Input_AssignKeyCode(INPUT_LAYOUT_USER, (INPUT_KEY)i, KeyBuff);
	}

	fread(&MusicVolume, sizeof(char), 1, Fp);
	g_Config.music_volume = MusicVolume;

	fread(&SoundVolume, sizeof(char), 1, Fp);
	g_Config.sound_volume = SoundVolume;
	Sound_SetMasterVolume(g_Config.sound_volume);

	fclose(Fp);

	return 1;
}

void S_SeedRandom()
{
	time_t lt = time(0);
	struct tm *tptr = localtime(&lt);
	Random_SeedControl(tptr->tm_sec + 57 * tptr->tm_min + 3543 * tptr->tm_hour);
	Random_SeedDraw(tptr->tm_sec + 43 * tptr->tm_min + 3477 * tptr->tm_hour);
}

void Random_SeedDraw(int32_t seed) { g_RandDraw = seed; }

int32_t Random_GetDraw()
{
	g_RandDraw = 0x41C64E6D * g_RandDraw + 0x3039;
	return (g_RandDraw >> 10) & 0x7FFF;
}

void Random_SeedControl(int32_t seed) { g_RandControl = seed; }

int32_t Random_GetControl()
{
	g_RandControl = 0x41C64E6D * g_RandControl + 0x3039;
	return (g_RandControl >> 10) & 0x7FFF;
}

void phd_InitWindow(int x, int y, int width, int height, int nearz, int farz,
					int view_angle, int scrwidth, int scrheight,
					unsigned char *BackBuff)
{

	// float values screen width/height
	g_SurfaceMinX = 0;
	g_SurfaceMinY = 0;
	g_SurfaceMaxX = (float)Screen_GetResWidth() - 1;
	g_SurfaceMaxY = (float)Screen_GetResHeight() - 1;
	
	// int values screen width/height
	g_PhdWinxmin = 0;
	g_PhdWinymin = 0;
	g_PhdWinxmax = Screen_GetResWidth() - 1;  /* Maximum Window X coord*/
	g_PhdWinymax = Screen_GetResHeight() - 1; /* Maximum Window Y coord*/
	g_PhdScrwidth = Screen_GetResWidth();
	g_PhdWinwidth = Screen_GetResWidth();

	/*
	int16_t c = phd_cos(fov / 2);
	int16_t s = phd_sin(fov / 2);
	g_PhdPersp = ((SCREEN_WIDTH / 2) * c) / s;
	*/
}

int LtWM_QUIT;

int SpinMessageLoop()
{
	MSG msg;

	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else if (g_bFocus)
		{
			return true;
		}
		else
		{
			WaitMessage();
		}
	}

	g_bWindowClosed = true;

	return false;
}

VOID On_Move(int x, int y)
{
	//DWORD dwWidth = g_RcScreenRect.right - g_RcScreenRect.left;
	//DWORD dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;
	DWORD dwWidth = Screen_GetResWidth();
	DWORD dwHeight = Screen_GetResHeight();
	SetRect(&g_RcScreenRect, x, y, x + dwWidth, y + dwHeight);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE) // Alt+F4 или крестик окна
		{
			PostQuitMessage(0);
			// DestroyWindow(hWnd);
			return 0;
		}
		break;

	case WM_MOVE:
		// Move messages need to be tracked to update the screen rects
		// used for blitting the backbuffer to the primary.
		On_Move((SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam));
		break;

	case WM_SYSKEYDOWN: // Обрабатываем нажатие системной клавиши
		if (wParam == VK_MENU)
			return 0; // Блокируем только одиночный Alt
		break;

	case WM_SYSKEYUP:		   // Обрабатываем отпускание системной клавиши
		if (wParam == VK_MENU) // VK_MENU соответствует клавише Alt
			// Игнорируем клавишу Alt
			return 0;
		break;

	case WM_ACTIVATE:
		switch (LOWORD(wParam))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			g_bFocus = TRUE;
			break;

		case WA_INACTIVE:
			g_bFocus = FALSE;
			break;
		}
		break;

	case WM_DESTROY:
		// WinAppExit();
		PostQuitMessage(0);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

		// default:
		// return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// return 0;
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int LoadTitle()
{
	Text_Init();
	// Output_DisplayPicture(g_GameFlow.main_menu_background_path);
	g_LevelNumTR = g_GameFlow.title_level_num;
	Initialise_Level(g_GameFlow.title_level_num); // title.phd level #20
	//палитру создаем только после загрузки уровня
	if (!Hardware)
		Create_Normal_Palette();

	int ret = Display_Inventory(INV_TITLE_MODE);

	if (ret == GF_START_DEMO)
		return GF_START_DEMO;

	if (ret == GF_EXIT_GAME)
		return GF_EXIT_GAME;

	switch (g_InvChosen)
	{
	case O_PHOTO_OPTION:
		// g_InvExtraData[1] = 0;
		return GF_START_GAME | g_GameFlow.gym_level_num;

	case O_PASSPORT_OPTION:

		// g_InvExtraData[0] - это номер страницы паспорта
		// g_InvExtraData[1] - это номер уровня который будет загружаться

		// страница 0: load game
		if (g_InvExtraData[0] == 0)
		{
			S_LoadGame(&g_SaveGame, g_InvExtraData[1]);
			g_LevelNumTR = 21; // 21 значит saved level
			return GF_START_GAME | g_InvExtraData[1];
		}
		else if (g_InvExtraData[0] == 1)
		{
			// страница 1: new game
			InitialiseStartInfo();
			g_LevelNumTR = g_GameFlow.first_level_num;
			return GF_START_GAME | g_GameFlow.first_level_num;
		}
		else
		{
			// страница 3: exit game
			return GF_EXIT_GAME;
		}
		break;

	default:
		break;
	}

	return 0;
}


bool DD_ClearSurface(IDirectDrawSurface3* pSurf, RECT* pRect = 0, DWORD dwColour = 0);

bool DD_ClearSurface(IDirectDrawSurface3* pSurf, RECT* pRect, DWORD dwColour)
{
	DDBLTFX bltfx;
	ZeroMemory(&bltfx, sizeof(DDBLTFX));
	bltfx.dwFillColor = dwColour;
	pSurf->Blt(pRect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);
		return false;
	return true;
}

bool DD_ClearZBuffer(IDirectDrawSurface3* pSurf, RECT* pRect)
{
	DDBLTFX bltfx;
	ZeroMemory(&bltfx, sizeof(DDBLTFX));
	bltfx.dwFillDepth = 0xffffffff;
	pSurf->Blt(pRect, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &bltfx);
		return false;
	return true;
}


HRESULT WINAPI D3DEnumDeviceCallback(GUID* pGUID,
	char* pDeviceDescription,
	char* pDeviceName,
	D3DDEVICEDESC* pHWDesc,
	D3DDEVICEDESC* pHELDesc,
	void* pContext)
{

	DWORD* dwZBuffBitDetpth = (DWORD*)pContext;
	//D3DPRIMCAPS& rPC=pHWDesc->dpcTriCaps;
	if (IsEqualIID(*pGUID, IID_IDirect3DHALDevice))
	{
		DWORD dwZBuffBit = pHWDesc->dwDeviceZBufferBitDepth;

		//dwZBuffBit = 0x600;

		if (dwZBuffBit & DDBD_8)	//0x800
			* dwZBuffBitDetpth = 8;
		if (dwZBuffBit & DDBD_16)	//0x400
			* dwZBuffBitDetpth = 16;
		if (dwZBuffBit & DDBD_24) //0x200
			* dwZBuffBitDetpth = 24;
		if (dwZBuffBit & DDBD_32)	//0x100
			* dwZBuffBitDetpth = 32;

		return D3DENUMRET_CANCEL;
	}

	return D3DENUMRET_OK;
}

void Hide_Cursor()
{

	//удаляем курсор (проблема в полноэкранных приложениях Windows)
	while (СursorCounter >= 0)
	{
		СursorCounter = ShowCursor(FALSE);
	}
}
bool Create_Device()
{
	HRESULT hr;

	//-------------------------------------------------------------------------
	// Step 1: Create DirectDraw and set the coop level
	//-------------------------------------------------------------------------

	// Create the IDirectDraw interface. The first parameter is the GUID,
	// which is allowed to be NULL. If there are more than one DirectDraw
	// drivers on the system, a NULL guid requests the primary driver. For 
	// non-GDI hardware cards like the 3DFX and PowerVR, the guid would need
	// to be explicity specified . (Note: these guids are normally obtained
	// from enumeration, which is convered in a subsequent tutorial.)
	hr = DirectDrawCreate(NULL, &g_pDD1, NULL);
	if (FAILED(hr))
		return hr;

	// Get a ptr to an IDirectDraw4 interface. This interface to DirectDraw
	// represents the DX6 version of the API.
	hr = g_pDD1->QueryInterface(IID_IDirectDraw2, (VOID * *)& g_pDD2);
	if (FAILED(hr))
		return hr;

	// Set the Windows cooperative level. This is where we tell the system
	// whether wew will be rendering in fullscreen mode or in a window. Note
	// that some hardware (non-GDI) may not be able to render into a window.
	// The flag DDSCL_NORMAL specifies windowed mode. Using fullscreen mode
	// is the topic of a subsequent tutorial. The DDSCL_FPUSETUP flag is a 
	// hint to DirectX to optomize floating points calculations. See the docs
	// for more info on this. Note: this call could fail if another application
	// already controls a fullscreen, exclusive mode.
	
	if (Fullscreen)
	{
		//hr = g_pDD2->SetCooperativeLevel(g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT | DDSCL_NOWINDOWCHANGES);
		hr = g_pDD2->SetCooperativeLevel(g_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT);
		if (FAILED(hr))
			return hr;

		hr = g_pDD2->SetDisplayMode(Screen_GetResWidth(),
			Screen_GetResHeight(),
			32,	//bpp
			0,
			0);

		if (FAILED(hr))
			return hr;


	}
	else
	{
		hr = g_pDD2->SetCooperativeLevel(g_hWnd, DDSCL_NORMAL);
		if (FAILED(hr))
			return hr;
	}

	//-------------------------------------------------------------------------
	// Step 2: Create DirectDraw surfaces used for rendering
	//-------------------------------------------------------------------------

	// Initialize a surface description structure for the primary surface. The
	// primary surface represents the entire display, with dimensions and a
	// pixel format of the display. Therefore, none of that information needs
	// to be specified in order to create the primary surface.
	

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));


	GetClientRect(g_hWnd, &g_RcScreenRect);
	GetClientRect(g_hWnd, &g_RcViewportRect);
	ClientToScreen(g_hWnd, (POINT*)& g_RcScreenRect.left);
	ClientToScreen(g_hWnd, (POINT*)& g_RcScreenRect.right);


	if (Fullscreen)
	{

		
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
		ddsd.dwBackBufferCount = 1;

		// Create the primary surface.
		hr = g_pDD2->CreateSurface(&ddsd, &g_pDdsPrimary1, NULL);
		if (FAILED(hr))
			return hr;

		g_pDdsPrimary1->QueryInterface(IID_IDirectDrawSurface3, (void**)& g_pDdsPrimary);

		DD_ClearSurface(g_pDdsPrimary);


		DDSCAPS ddscaps;
		ZeroMemory(&ddscaps, sizeof(DDSCAPS));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		hr = g_pDdsPrimary->GetAttachedSurface(&ddscaps, &g_pDdsBackBuffer);
		if (FAILED(hr))
			return hr;

		DD_ClearSurface(g_pDdsBackBuffer);
	}
	else
	{
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		hr = g_pDD2->CreateSurface(&ddsd, &g_pDdsPrimary1, NULL);
		if (FAILED(hr))
			return hr;

		g_pDdsPrimary1->QueryInterface(IID_IDirectDrawSurface3, (void**)& g_pDdsPrimary);


	// Setup a surface description to create a backbuffer. This is an
	// offscreen plain surface with dimensions equal to our window size.
	// The DDSCAPS_3DDEVICE is needed so we can later query this surface
	// for an IDirect3DDevice interface.

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;

	// Set the dimensions of the backbuffer. Note that if our window changes
	// size, we need to destroy this surface and create a new one.
	//ddsd.dwWidth = g_RcScreenRect.right - g_RcScreenRect.left;
	//ddsd.dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;
	ddsd.dwWidth = Screen_GetResWidth();
	ddsd.dwHeight = Screen_GetResHeight();

	// Create the backbuffer. The most likely reason for failure is running
	// out of video memory. (A more sophisticated app should handle this.)
	hr = g_pDD2->CreateSurface(&ddsd, &g_pDdsBackBuffer1, NULL);
	if (FAILED(hr))
		return hr;


	g_pDdsBackBuffer1->QueryInterface(IID_IDirectDrawSurface3, (void**)&g_pDdsBackBuffer);

	g_pDdsBackBuffer1->Release();
	
	}

	// Note: if using a z-buffer, the zbuffer surface creation would go around
	// here. However, z-buffer usage is the topic of a subsequent tutorial.


	// Before creating the device, check that we are NOT in a palettized
	// display. That case will cause CreateDevice() to fail, since this simple 
	// tutorial does not bother with palettes.
	/*
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	g_pDD2->GetDisplayMode(&ddsd);
	if (ddsd.ddpfPixelFormat.dwRGBBitCount <= 8)
		return DDERR_INVALIDMODE;
	*/

	// Query DirectDraw for access to Direct3D
	g_pDD2->QueryInterface(IID_IDirect3D2, (VOID**)& g_pD3D);
	if (FAILED(hr))
		return hr;


	//узнаем глубину Z buffer
	DWORD dwZBuffBitDetpth = 0;
	g_pD3D->EnumDevices(D3DEnumDeviceCallback, &dwZBuffBitDetpth);
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_ZBUFFERBITDEPTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
		//ddsd.dwWidth = g_RcScreenRect.right - g_RcScreenRect.left;
		//ddsd.dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;
		ddsd.dwWidth = Screen_GetResWidth();
		ddsd.dwHeight = Screen_GetResHeight();
		
		//ddsd.dwZBufferBitDepth = 16;
		ddsd.dwZBufferBitDepth = dwZBuffBitDetpth;

		/*
		// Настраиваем формат пикселей для Z-буфера
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
		ddsd.ddpfPixelFormat.dwZBufferBitDepth = 16;  // 16-битный Z-буфер
		*/


		if (FAILED(hr = g_pDD2->CreateSurface(&ddsd, &g_pDdsZBuffer1, NULL)))
		{
			return hr;

		}
		

	g_pDdsZBuffer1->QueryInterface(IID_IDirectDrawSurface3, (void**)&g_pDdsZBuffer);

	g_pDdsZBuffer1->Release();

	

	// Attach the z-buffer to the back buffer.
	
	hr = g_pDdsBackBuffer->AddAttachedSurface(g_pDdsZBuffer);
	if (FAILED(hr))
	{
		
		return hr;
	}

	// Create a clipper object which handles all our clipping for cases when
// our window is partially obscured by other windows. This is not needed
// for apps running in fullscreen mode.
	LPDIRECTDRAWCLIPPER pcClipper;
	hr = g_pDD2->CreateClipper(0, &pcClipper, NULL);
	if (FAILED(hr))
		return hr;

	// Associate the clipper with our window. Note that, afterwards, the
	// clipper is internally referenced by the primary surface, so it is safe
	// to release our local reference to it.
	pcClipper->SetHWnd(0, g_hWnd);
	g_pDdsPrimary->SetClipper(pcClipper);
	pcClipper->Release();

	//-------------------------------------------------------------------------
	// Step 3: Create the Direct3D interfaces
	//-------------------------------------------------------------------------


	// Create the device. The GUID is hardcoded for now, but should come from
// device enumeration, which is the topic of a future tutorial. The device
// is created off of our back buffer, which becomes the render target for
// the newly created device.
	hr = g_pD3D->CreateDevice(IID_IDirect3DHALDevice, (IDirectDrawSurface*)g_pDdsBackBuffer,
		&g_pD3dDevice);

	if (FAILED(hr))
	{
		MessageBox(NULL, "3", "3", 0);
		return hr;
	}

	//-------------------------------------------------------------------------
	// Step 4: Create the viewport
	//-------------------------------------------------------------------------

	// Set up the viewport data parameters
	D3DVIEWPORT2 vdData;
	ZeroMemory(&vdData, sizeof(D3DVIEWPORT2));
	vdData.dwSize = sizeof(D3DVIEWPORT2);
	//vdData.dwWidth = g_RcScreenRect.right - g_RcScreenRect.left;
	//vdData.dwHeight = g_RcScreenRect.bottom - g_RcScreenRect.top;
	vdData.dwWidth = Screen_GetResWidth();
	vdData.dwHeight = Screen_GetResHeight();
	vdData.dvClipX = -1.0f;
	vdData.dvClipWidth = 2.0f;
	vdData.dvClipY = 1.0f;
	vdData.dvClipHeight = 2.0f;
	vdData.dvMaxZ = 1.0f;

	// Create the viewport
	hr = g_pD3D->CreateViewport(&g_pViewport, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, "4", "4", 0);
		return hr;
	}
		

	// Associate the viewport with the D3DDEVICE object
	g_pD3dDevice->AddViewport(g_pViewport);

	// Set the parameters to the new viewport
	g_pViewport->SetViewport2(&vdData);

	// Set the viewport as current for the device
	g_pD3dDevice->SetCurrentViewport(g_pViewport);

	return true;

}


void Create_Shader_Effect()
{
}


void FreeGameMemory()
{

	if (m_pLevelTile)
	{
		for (UINT i = 0; i < TexturePageCount; i++)
		{
			/*
			if (m_pLevelTile[i])
			{
				m_pLevelTile[i]->Release();
				m_pLevelTile[i] = nullptr;
			}
			*/
		}

		delete[] m_pLevelTile;
		m_pLevelTile = nullptr;
	}

	//освобождаем viewport
	if (g_pViewport)
	{
		g_pViewport->Release();
		g_pViewport = NULL;
	}

	//освобождаем D3D-устройство
	if (g_pD3dDevice)
	{
		g_pD3dDevice->Release();
		g_pD3dDevice = NULL;
	}

	//освобождаем Z-буфер
	if (g_pDdsZBuffer)
	{
		g_pDdsZBuffer->Release();
		g_pDdsZBuffer = NULL;
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

	//освобождаем интерфейс Direct3D
	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	//освобождаем интерфейс DirectDraw2
	if (g_pDD2)
	{
		g_pDD2->Release();
		g_pDD2 = NULL;
	}

	//освобождаем интерфейс DirectDraw1
	//ошибка acces violation release
	/*
	if (g_pDD1)
	{
		g_pDD1->Release();
		g_pDD1 = NULL;
	}
	*/


}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	//switch hardware dx9 or software
	if (Hardware)
	{
		S_DrawObjectGT4 = S_DrawObjectGT4_HW;
		S_DrawObjectGT3 = S_DrawObjectGT3_HW;
		S_DrawObjectG4 = S_DrawObjectG4_HW;
		S_DrawObjectG3 = S_DrawObjectG3_HW;
		S_Output_DrawShadow = S_Output_DrawShadow_HW;
		S_Output_DrawSprite = S_Output_DrawSprite_HW;
		S_Output_DrawScreenFBox = S_Output_DrawScreenFBox_HW;
		S_Output_DrawScreenFlatQuad = S_Output_DrawScreenFlatQuad_HW;
		S_Output_DrawTriangle = S_Output_DrawTriangle_HW;
		S_Output_DrawLine = S_Output_DrawLine_HW;
		//S_InitialisePolyList = S_InitialisePolyList_HW;
		S_OutputPolyList = S_OutputPolyList_HW;
	}
	else
	{
		S_DrawObjectGT4 = S_DrawObjectGT4_SW;
		S_DrawObjectGT3 = S_DrawObjectGT3_SW;
		S_DrawObjectG4 = S_DrawObjectG4_SW;
		S_DrawObjectG3 = S_DrawObjectG3_SW;
		S_Output_DrawShadow = S_Output_DrawShadow_SW;
		S_Output_DrawSprite = S_Output_DrawSprite_SW;
		S_Output_DrawScreenFBox = S_Output_DrawScreenFBox_SW;
		S_Output_DrawScreenFlatQuad = S_Output_DrawScreenFlatQuad_SW;
		S_Output_DrawTriangle = S_Output_DrawTriangle_SW;
		S_Output_DrawLine = S_Output_DrawLine_SW;
		//S_InitialisePolyList = S_InitialisePolyList_SW;
		S_OutputPolyList = S_OutputPolyList_SW;
		
	}

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	WNDCLASS wcl = {0};
	ZeroMemory(&wcl, sizeof(WNDCLASS));
	//wcl.style = CS_CLASSDC;
	wcl.style = 0;
	wcl.lpfnWndProc = WndProc;
	wcl.hInstance = hInstance;
	wcl.lpszClassName = "Sample";
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wcl))
		return 0;

	if (!Fullscreen)
	{
		g_hWnd =
			CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
						 WS_OVERLAPPEDWINDOW, 0, 0, Screen_GetResWidth(),
						 Screen_GetResHeight(), NULL, NULL, hInstance, NULL);
	}
	else if (Fullscreen)
	{
		g_hWnd =
			CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
						 WS_POPUP | WS_VISIBLE, 0, 0, Screen_GetResWidth(),
						 Screen_GetResHeight(), NULL, NULL, hInstance, NULL);
	}

	if (!g_hWnd)
		return 0;


	if (!Fullscreen)
	{
		RECT WindowRect = {0, 0, Screen_GetResWidth(), Screen_GetResHeight()};

		AdjustWindowRectEx(&WindowRect, GetWindowStyle(g_hWnd),
						   GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

		UINT WidthScreen = GetSystemMetrics(SM_CXSCREEN);
		UINT HeightScreen = GetSystemMetrics(SM_CYSCREEN);

		UINT WidthX = WindowRect.right - WindowRect.left;
		UINT WidthY = WindowRect.bottom - WindowRect.top;

		UINT PosX = (WidthScreen - WidthX) / 2;
		UINT PosY = (HeightScreen - WidthY) / 2;

		MoveWindow(g_hWnd, PosX, PosY, WidthX, WidthY, FALSE);
	}
	
	if (Hardware)
	{
		Create_Device();
		//Create_Shader_Effect();
	}


	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	if (!Hardware && Fullscreen)
	{
		if (SetDisplayMode())
		{
			//видеорежим установлен успешно
		}

		Hide_Cursor();
	}
	else
	{
		ShowCursor(TRUE);
	}

	if (Fullscreen)
		Hide_Cursor();

	//новое для ТР1
	Init_GameFlow();

	// GS_CONTROL_DEFAULT_KEYS
	g_Config.input.layout = INPUT_LAYOUT_DEFAULT;
	g_GameFlow.gym_level_num = 0;
	g_GameFlow.first_level_num = 1;
	g_GameFlow.last_level_num = 15;

	if (GameType == VER_TR1)
	{
		g_GameFlow.title_level_num = 20;
		g_GameFlow.level_count = 22;
	}
	else if (GameType == VER_TR1_GOLD)
	{
		g_GameFlow.title_level_num = 5;
		g_GameFlow.level_count = 4;
	}

	g_Config.render_flags.fps_counter = 1;
	g_Config.enable_enemy_healthbar = 1;
	g_Config.enemy_healthbar_location = T1M_BL_BOTTOM_LEFT;
	g_Config.enemy_healthbar_color = 2;
	g_Config.ui.bar_scale = 1.0f;
	g_Config.ui.text_scale = 1.0f;
	g_Config.healthbar_location = T1M_BL_TOP_LEFT;
	g_Config.healthbar_color = 3;
	g_Config.airbar_location = T1M_BL_TOP_RIGHT;
	g_Config.airbar_color = 1;

	S_SeedRandom();

	Output_CalcWibbleTable();

	//новое для ТР1
	Init_Timer();

	//параметры ниже widht, height умножаются на screen_sizer
	// x, y, width, height нужны для уменьшения увеличения окна игры
	// scrwidth, scrheight - дисплей размеры
	//параметры phd_InitWindow следующие x, y, width, height,
	// nearz, farz, view_angle, scrwidth, scrheight, backbuffer_memory
	// 20480 = 0x5000
	unsigned char *pBackBuff = 0;
	phd_InitWindow(0, 0, Screen_GetResWidth(), Screen_GetResHeight(), 10, 20480,
				   80, Screen_GetResWidth(), Screen_GetResHeight(), pBackBuff);

	if (g_GameMemory)
		free(g_GameMemory);

	g_GameMemory = (int8_t *)malloc(g_Malloc_Size);

	if (!g_GameMemory)
	{
		MessageBox(NULL, "ERROR: Could not allocate enough memory", "INFO",
				   MB_OK);
	}

	Init_Game_Malloc();
	S_FrontEndCheck();
	Settings_Read();
	
	if(!Hardware)
		Create_BackBuffer();
	
	InitialiseStartInfo();

	bool intro_played = false;
	bool loop_continue = true;

	int32_t gf_option = LoadTitle();

	phd_AlterFOV(80 * PHD_DEGREE);

	while (loop_continue)
	{
		int32_t gf_direction = gf_option & ~((1 << 6) - 1);
		int32_t gf_param = gf_option & ((1 << 6) - 1);

		switch (gf_direction)
		{

		case GF_EXIT_TO_TITLE:
			gf_option = LoadTitle();
			break;

		case GF_START_GAME:
			gf_option = Start_New_Game(gf_param);
			break;

		case GF_START_CINE:
			
			gf_option = Play_Cinematic(gf_param);
			
			break;

		case GF_START_DEMO:
			// gf_option = StartDemo();
			//я добавил пока нету демо что бы вместо демо выходило обратно в
			//титлы
			g_NoInputCount = 0;
			g_ResetFlag = 0;
			gf_option = GF_EXIT_TO_TITLE;
			break;

		case GF_LEVEL_COMPLETE:
			gf_option = Print_Level_Stats(gf_param);
			g_LevelNumTR = gf_param;
			break;

		case GF_EXIT_GAME:
			loop_continue = false;
			break;

		default:
			break;
		}
	}

	free(g_GameMemory);
	
	if (!Hardware)
		Delete_BackBuffer();

	if (!Hardware && Fullscreen)
	{
		if (RestorePreviousDisplayMode())
		{
			//предыдущий режим восстановлен успешно
		}
	}

	if(Hardware)
		FreeGameMemory();

	ShowCursor(TRUE);

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return 0;
}
