#include "winmain.h"
#include "timer.h"
#include "backbuffer.h"
#include "draw.h"
#include "vars.h"
#include "gameflow.h"
#include "init.h"

#pragma comment (lib, "legacy_stdio_definitions.lib")

//SETUP START
//SETUP START
//SETUP START
//SETUP START


//игра обычный экран - запускаем на полный широкий
//надо учет аспекта,
//игра широкий экран - запускаем на полный широкий
//не надо учет аспекта,
//1)SETUP FULLSCREEN/NO
//#define FULL_SCREEN

//закоментировать учет аспекта в
//matrix.cpp phd_GenerateW2V()
//2)SETUP WIDESCREEN/NO
int widescreen = 0;

//3)SETUP GAME TYPE - TR1/GOLD
int select_game = VER_TR1;
//int select_game = VER_TR_GOLD;

//4)SETUP LEVEL NUM - TR1/GOLD

	//TOMB ORIGINAL
	//int level_num = 0; //Level 0 - GYM - GYM.PHD 
	int level_num = 1; //Level 1 - Caves - LEVEL1.PHD 
	//int level_num = 2; //Level 2 - City of Vilcabamba - LEVEL2.PHD
	//int level_num = 3; //Level 3 - Lost Valley - LEVEL3A.PHD
	//int level_num = 4; //Level 4 - Tomb of Qualopec - LEVEL3B.PHD
	//int level_num = 5; //Level 5 - St Francis' Folly - LEVEL4.PHD
	//int level_num = 6; //Level 6 - Colosseum - LEVEL5.PHD
	//int level_num = 7; //Level 7 - Palace Midas - LEVEL6.PHD
	//int level_num = 8; //Level 8 - The Cistern - LEVEL7A.PHD
	//int level_num = 9; //Level 9 - Tomb of Tihocan - LEVEL7B.PHD
	//int level_num = 10; //Level 10 - City of Khamoon - LEVEL8A.PHD
	//int level_num = 11; //Level 11 - Obelisk of Khamoon - LEVEL8B.PHD
	//int level_num = 12; //Level 12 - Sanctuary of Scion - LEVEL8C.PHD
	//int level_num = 13; //Level 13 - Natla's Mines - LEVEL10A.PHD
	//int level_num = 14; //Level 14 - Atlantis - LEVEL10B.PHD
	//int level_num = 15; //Level 15 - The Great Pyramid - LEVEL10C.PHD

//TOMB GOLD

//int level_num = 5; //Level 0 - GYM - GYM.PHD 
//int level_num = 1; //Level 1 - Return to Egypt - EGYPT.PHD 
//int level_num = 2; //Level 2 - Temple of the Cat - CAT.PHD 
//int level_num = 3; //Level 3 - Atlantean Stronghold - END.PHD 
//int level_num = 4; //Level 4 - Hive - END2.PHD 


//5)SETUP SCREEN RESOLUTION
/*
int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 900;

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
*/

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;


//6)SETUP LARA DIST
//false is original dist
//влияет на g_PhdPersp
int lara_dist = false;
//int lara_dist = false;


//SETUP END
//SETUP END
//SETUP END
//SETUP END

// Структура для хранения предыдущего режима
static DEVMODE previousMode;
static bool isPreviousModeStored = false;

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

int g_bWindowClosed = false;
bool g_bFocus = true;

int32_t m_RandControl = 0xD371F947;
int32_t m_RandDraw = 0xD371F947;

int16_t		phd_winxmin = 0;
int16_t		phd_winymin = 0;

int16_t		phd_winxmax = SCREEN_WIDTH - 1;        	/* Maximum Window X coord*/
int16_t		phd_winymax = SCREEN_HEIGHT - 1;        	/* Maximum Window Y coord*/
int32_t		phd_scrwidth = SCREEN_WIDTH;

// Функция для установки режима дисплея 800x600
bool SetDisplayMode()
{
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(DEVMODE);
    dm.dmPelsWidth = SCREEN_WIDTH;
    dm.dmPelsHeight = SCREEN_HEIGHT;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    if (!isPreviousModeStored)
    {
        // Сохраняем текущий режим
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &previousMode))
        {
            isPreviousModeStored = true;
        }
        else
        {
            ////std::cerr << "Не удалось сохранить предыдущий режим." << std::endl;
            return false;
        }
    }

    // Устанавливаем новый режим
    LONG result = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
    if (result != DISP_CHANGE_SUCCESSFUL)
    {
        ////std::cerr << "Не удалось установить режим 800x600." << std::endl;
        return false;
    }

    return true;
}

// Функция для возврата дисплея в предыдущий режим
bool RestorePreviousDisplayMode()
{
    if (!isPreviousModeStored)
    {
        //std::cerr << "Предыдущий режим не был сохранён." << std::endl;
        return false;
    }

    // Возвращаем предыдущий режим
    LONG result = ChangeDisplaySettings(&previousMode, CDS_FULLSCREEN);
    if (result != DISP_CHANGE_SUCCESSFUL)
    {
        ////std::cerr << "Не удалось вернуть предыдущий режим." << std::endl;
        return false;
    }

    isPreviousModeStored = false; // После восстановления сбрасываем флаг
    return true;
}

void S_SeedRandom()
{
    time_t lt = time(0);
    struct tm *tptr = localtime(&lt);
    Random_SeedControl(tptr->tm_sec + 57 * tptr->tm_min + 3543 * tptr->tm_hour);
    Random_SeedDraw(tptr->tm_sec + 43 * tptr->tm_min + 3477 * tptr->tm_hour);
}

void Random_SeedDraw(int32_t seed)
{
    m_RandDraw = seed;
}

int32_t Random_GetDraw()
{
    m_RandDraw = 0x41C64E6D * m_RandDraw + 0x3039;
    return (m_RandDraw >> 10) & 0x7FFF;
}

void Random_SeedControl(int32_t seed)
{
    m_RandControl = seed;
}

int32_t Random_GetControl()
{
    m_RandControl = 0x41C64E6D * m_RandControl + 0x3039;
    return (m_RandControl >> 10) & 0x7FFF;
}

void phd_InitWindow( int  x, 	int y,
					   int  width, int height,
					   int  nearz, int farz,
					   int  view_angle,
					   int  scrwidth,
					   int	 scrheight, unsigned char *BackBuff)
{
	phd_winxmax = SCREEN_WIDTH - 1;
	phd_winymax = SCREEN_HEIGHT - 1;

	g_SurfaceMinX = 0;
	g_SurfaceMinY = 0;
	g_SurfaceMaxX = (float)SCREEN_WIDTH - 1;
	g_SurfaceMaxY = (float)SCREEN_HEIGHT - 1;
}

int SpinMessageLoop()
{
	MSG msg;

	for (;;)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message==WM_QUIT)
				break;

			if (GetKeyState(VK_ESCAPE) & 0xFF00)
				break;
		}
		else if(g_bFocus)
		{
			return true;
		}
		else
		{
			WaitMessage();
		}
	}

	g_bWindowClosed=true;

	return false;
}

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_SYSKEYDOWN: // Обрабатываем нажатие системной клавиши
        if (wParam == VK_MENU) // VK_MENU соответствует клавише Alt
        {
            // Игнорируем клавишу Alt
            return 0;
        }
        break;

    case WM_SYSKEYUP: // Обрабатываем отпускание системной клавиши
        if (wParam == VK_MENU) // VK_MENU соответствует клавише Alt
        {
            // Игнорируем клавишу Alt
            return 0;
        }
        break;

		case WM_ACTIVATE:
			switch(LOWORD(wParam))
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
		 //WinAppExit();
  			PostQuitMessage(0);
			break;
	
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int PASCAL WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	WNDCLASS wcl = {0};
	wcl.lpfnWndProc = WndProc;
	wcl.hInstance = hInstance;
	wcl.lpszClassName = "Sample";
	//wcl.style = CS_HREDRAW | CS_VREDRAW;
	//wcl.cbClsExtra = 0L;
	//wcl.cbWndExtra = 0L;
	//wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wcl.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//wcl.lpszMenuName = NULL;

	if(!RegisterClass(&wcl))
		return 0;

#ifndef FULL_SCREEN
	g_hWnd = CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
					WS_OVERLAPPEDWINDOW,
					0, 0,
					SCREEN_WIDTH, SCREEN_HEIGHT,
					NULL,
					NULL,
					hInstance,
					NULL);

#endif

#ifdef FULL_SCREEN
		g_hWnd = CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
					WS_POPUP | WS_VISIBLE,
					0, 0,
					SCREEN_WIDTH, SCREEN_HEIGHT,
					NULL,
					NULL,
					hInstance,
					NULL);
		
#endif

if(!g_hWnd)
		return 0;

#ifndef FULL_SCREEN

	RECT WindowRect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};

	AdjustWindowRectEx(&WindowRect,
		GetWindowStyle(g_hWnd),
		GetMenu(g_hWnd) != NULL,
		GetWindowExStyle(g_hWnd));

	UINT WidthScreen = GetSystemMetrics(SM_CXSCREEN);
	UINT HeightScreen = GetSystemMetrics(SM_CYSCREEN);

	UINT WidthX = WindowRect.right - WindowRect.left;
	UINT WidthY = WindowRect.bottom - WindowRect.top;

	UINT PosX =  (WidthScreen - WidthX)/2;
	UINT PosY =  (HeightScreen - WidthY)/2;
	
	MoveWindow(g_hWnd,
		PosX,
        PosY,
        WidthX,
        WidthY,
        FALSE);
#endif

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	ShowCursor(FALSE);

#ifdef FULL_SCREEN
	if (SetDisplayMode())
    {
        //std::cout << "Режим 800x600 установлен успешно." << std::endl;
    }
#endif

	S_SeedRandom();

	Output_CalcWibbleTable();

	//new for TR1
	Init_Timer();

	
	//параметры ниже widht, height умножаются на screen_sizer
	//x, y, width, height нужны для уменьшения увеличения окна игры
	//scrwidth, scrheight - дисплей width height
	//x, y, width, height, nearz, farz, view_angle, scrwidth, scrheight, backbuffer_memory
	//20480 = 0x5000
	unsigned char *pBackBuff = 0;
	phd_InitWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 10, 20480, 80, SCREEN_WIDTH, SCREEN_HEIGHT, pBackBuff);

	if(select_game == VER_TR1)
	{
	//TOMB ORIGINAL

	Start_New_Game(level_num);

	//Start_New_Game(0); //Level 0 - GYM - GYM.PHD 
	//Start_New_Game(2); //Level 1 - Caves - LEVEL1.PHD 
	//Start_New_Game(2); //Level 2 - City of Vilcabamba - LEVEL2.PHD
	//Start_New_Game(3); //Level 3 - Lost Valley - LEVEL3A.PHD
	//Start_New_Game(4); //Level 4 - Tomb of Qualopec - LEVEL3B.PHD
	//Start_New_Game(5);//Level 5 - St Francis' Folly - LEVEL4.PHD
	//Start_New_Game(6); //Level 6 - Colosseum - LEVEL5.PHD
	//Start_New_Game(7); //Level 7 - Palace Midas - LEVEL6.PHD
	//Start_New_Game(8); //Level 8 - The Cistern - LEVEL7A.PHD
	//Start_New_Game(9); //Level 9 - Tomb of Tihocan - LEVEL7B.PHD
	//Start_New_Game(10); //Level 10 - City of Khamoon - LEVEL8A.PHD
	//Start_New_Game(11); //Level 11 - Obelisk of Khamoon - LEVEL8B.PHD
	//Start_New_Game(12); //Level 12 - Sanctuary of Scion - LEVEL8C.PHD
	//Start_New_Game(13); //Level 13 - Natla's Mines - LEVEL10A.PHD
	//Start_New_Game(14); //Level 14 - Atlantis - LEVEL10B.PHD
	//Start_New_Game(15); //Level 15 - The Great Pyramid - LEVEL10C.PHD
	}

	else if(select_game == VER_TR_GOLD)
	{

	Start_New_Game(level_num);

	//TOMB GOLD
	//Start_New_Game(5); //Level 0 - GYM - GYM.PHD 
	//Start_New_Game(1); //Level 1 - Return to Egypt - EGYPT.PHD 
	//Start_New_Game(2); //Level 2 - Temple of the Cat - CAT.PHD 
	//Start_New_Game(3); //Level 3 - Atlantean Stronghold - END.PHD 
	//Start_New_Game(4); //Level 4 - Hives - END2.PHD 

	}



	free(g_GameMemory);

	Delete_BackBuffer();

#ifdef FULL_SCREEN
	if (RestorePreviousDisplayMode())
    {
        //std::cout << "Предыдущий режим восстановлен успешно." << std::endl;
    }
#endif

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return 0;
}
