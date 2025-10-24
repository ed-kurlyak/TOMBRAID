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

#include "3d_gen.h" //�������� ��� phd_AlterFOV()

#include "cinema.h"

//***************************
// SETUP START
//***************************

// SETUP PART #1
//HARDWARE or SOFTWARE

int Hardware = 1;

int Fullscreen_Hardware = 0;

//---------------------------
// SETUP PART #9 CLIPPING DX9 or Software
int DX9Clipping = 1;


// SETUP PART #3 FULLSCREEN/NO
int Fullscreen_Software = 0;

//---------------------------
// SETUP PART #4 WIDESCREEN/NO
// ���� ������� � matrix.cpp phd_GenerateW2V()
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
//������ �� g_PhdPersp
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

//��� �������� ������� � ������������� ������
int �ursorCounter = 0;

// c�������� ��� �������� ����������� ������
static DEVMODE PreviousMode;
static bool IsPreviousModeStored = false;

HINSTANCE g_hInst = NULL;
HWND g_hWnd = NULL;

int g_bWindowClosed = false;
bool g_bFocus = true;

int32_t g_RandControl = 0xD371F947;
int32_t g_RandDraw = 0xD371F947;

//������� ��� ��������� ������ �������
bool SetDisplayMode()
{
	DEVMODE dm = {0};
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = Screen_GetResWidth();
	dm.dmPelsHeight = Screen_GetResHeight();
	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	if (!IsPreviousModeStored)
	{
		//��������� ������� �����
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &PreviousMode))
		{
			IsPreviousModeStored = true;
		}
		else
		{
			//�� ������� ��������� ���������� �����
			return false;
		}
	}

	//������������� ����� �����
	LONG result = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		//�� ������� ���������� �����
		return false;
	}

	return true;
}

//������� ��� �������� ������� � ���������� �����
bool RestorePreviousDisplayMode()
{
	if (!IsPreviousModeStored)
	{
		//���������� ����� �� ��� �������
		return false;
	}

	//���������� ���������� �����
	LONG result = ChangeDisplaySettings(&PreviousMode, CDS_FULLSCREEN);

	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		//�� ������� ������� ���������� �����
		return false;
	}

	IsPreviousModeStored = false; //����� �������������� ���������� ����

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE) // Alt+F4 ��� ������� ����
		{
			PostQuitMessage(0);
			// DestroyWindow(hWnd);
			return 0;
		}
		break;

	case WM_SYSKEYDOWN: // ������������ ������� ��������� �������
		if (wParam == VK_MENU)
			return 0; // ��������� ������ ��������� Alt
		break;

	case WM_SYSKEYUP:		   // ������������ ���������� ��������� �������
		if (wParam == VK_MENU) // VK_MENU ������������� ������� Alt
			// ���������� ������� Alt
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
	//������� ������� ������ ����� �������� ������
	if (!Hardware)
		Create_Normal_Palette();

	int ret = Display_Inventory(INV_TITLE_MODE);

	if (ret == GF_START_DEMO)
		return GF_START_DEMO;

	switch (g_InvChosen)
	{
	case O_PHOTO_OPTION:
		// g_InvExtraData[1] = 0;
		return GF_START_GAME | g_GameFlow.gym_level_num;

	case O_PASSPORT_OPTION:

		// g_InvExtraData[0] - ��� ����� �������� ��������
		// g_InvExtraData[1] - ��� ����� ������ ������� ����� �����������

		// �������� 0: load game
		if (g_InvExtraData[0] == 0)
		{
			S_LoadGame(&g_SaveGame, g_InvExtraData[1]);
			g_LevelNumTR = 21; // 21 ������ saved level
			return GF_START_GAME | g_InvExtraData[1];
		}
		else if (g_InvExtraData[0] == 1)
		{
			// �������� 1: new game
			InitialiseStartInfo();
			g_LevelNumTR = g_GameFlow.first_level_num;
			return GF_START_GAME | g_GameFlow.first_level_num;
		}
		else
		{
			// �������� 3: exit game
			return GF_EXIT_GAME;
		}
		break;

	default:
		break;
	}

	return 0;
}

bool Create_Device()
{
	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	if(Fullscreen_Hardware)
	{
		d3dpp.Windowed = FALSE;
	}
	else
	{
		d3dpp.Windowed = TRUE;
	}
	d3dpp.hDeviceWindow = g_hWnd;
	d3dpp.BackBufferWidth = Screen_GetResWidth();
	d3dpp.BackBufferHeight = Screen_GetResHeight();
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;

	if (FAILED(g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_d3d_Device)))
	{
		MessageBox(NULL, "Error create device!", "Info", MB_OK);
		return false;
	}



	HRESULT hr;

	
	hr = g_d3d_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (FAILED(hr)) return false;

	/*
	hr = g_d3d_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	if (FAILED(hr)) return false;

	//�������� ���� �������
	g_d3d_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	//��������� ������ � z-�����
	g_d3d_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//������� ��������� �������: "������ ��� �����"
	g_d3d_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	*/
	//hr = g_d3d_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//if (FAILED(hr)) return false;

	hr = g_d3d_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if (FAILED(hr)) return false;


	//����������� ���������� (��������������� �����)
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	if (FAILED(hr)) return false;

	//����������� ���������� (��������������� ����)
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	if (FAILED(hr)) return false;

	//���������� MIP-�������
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	if (FAILED(hr)) return false;

	/*
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	if (FAILED(hr)) return false;

	
	hr = g_d3d_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	if (FAILED(hr)) return false;
	*/

	if(DX9Clipping)
	{

		hr = g_d3d_Device->SetRenderState(D3DRS_CLIPPING, TRUE);
		if (FAILED(hr)) return false;


	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = Screen_GetResWidth();
	viewport.Height = Screen_GetResHeight();
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	g_d3d_Device->SetViewport(&viewport);
	
	}
	else
	{
		// ��������� �������� ���������
		hr = g_d3d_Device->SetRenderState(D3DRS_CLIPPING, FALSE);
		if (FAILED(hr)) return false;

		// ����� ��������� clip planes
		hr = g_d3d_Device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		if (FAILED(hr)) return false;
	}


	//D3DXCreateTextureFromFile(g_d3d_Device, "texture256.bmp", &g_pTexture);

	return true;

}


void Create_Shader_Effect()
{
	HRESULT hr;
	ID3DXBuffer* errorBuffer = 0;

	hr = D3DXCreateEffectFromFile(g_d3d_Device, ".\\shader\\textured.fx", 0, 0, 0, 0, &pEffectTexColor, &errorBuffer);
	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}
	if (FAILED(hr)) return;

	g_hTechTexColor = pEffectTexColor->GetTechniqueByName("TexturedTech");
	//mhWVP  = pEffectTexColor->GetParameterByName(0, "gWVP");



	/*
	D3DVERTEXELEMENT9 decl[]=
   {
	   {0,	0, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
	   {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	   D3DDECL_END()
   };
   */


	D3DVERTEXELEMENT9 decl_tex_color[] =
	{
		{ 0,  0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // x, y, z, w
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, // tu, tv
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 }, // diffuse
		D3DDECL_END()
	};


	hr = g_d3d_Device->CreateVertexDeclaration(decl_tex_color, &g_pVertDeclTexColor);
	if (FAILED(hr)) return;




	D3DVERTEXELEMENT9 decl_color[] =
	{
		{ 0,  0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // x, y, z, w
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,    0 }, // diffuse
		D3DDECL_END()
	};


	hr = g_d3d_Device->CreateVertexDeclaration(decl_color, &g_pVertDeclColor);
	if (FAILED(hr)) return;


	D3DVERTEXELEMENT9 declLine[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	hr = g_d3d_Device->CreateVertexDeclaration(decl_color, &g_pVertDeclLines);
	if (FAILED(hr)) return;



}


void FreeGameMemory()
{

	if (m_pLevelTile)
	{
		for (UINT i = 0; i < TexturePageCount; i++)
		{
			if (m_pLevelTile[i])
			{
				m_pLevelTile[i]->Release();
				m_pLevelTile[i] = nullptr;
			}
		}

		delete[] m_pLevelTile;
		m_pLevelTile = nullptr;
	}

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
		S_InitialisePolyList = S_InitialisePolyList_HW;
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
		S_InitialisePolyList = S_InitialisePolyList_SW;
		S_OutputPolyList = S_OutputPolyList_SW;
		
	}

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	WNDCLASS wcl = {0};
	wcl.style = CS_CLASSDC;
	wcl.lpfnWndProc = WndProc;
	wcl.hInstance = hInstance;
	wcl.lpszClassName = "Sample";
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClass(&wcl))
		return 0;

	if (!Fullscreen_Software)
	{
		g_hWnd =
			CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
						 WS_OVERLAPPEDWINDOW, 0, 0, Screen_GetResWidth(),
						 Screen_GetResHeight(), NULL, NULL, hInstance, NULL);
	}
	else if (Fullscreen_Software)
	{
		g_hWnd =
			CreateWindow("Sample", "Tomb Raider 1 Ed Kurlyak",
						 WS_POPUP | WS_VISIBLE, 0, 0, Screen_GetResWidth(),
						 Screen_GetResHeight(), NULL, NULL, hInstance, NULL);
	}

	if (!g_hWnd)
		return 0;


	if (!Fullscreen_Software)
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
		Create_Shader_Effect();
	}


	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	if (Fullscreen_Software)
	{
		if (SetDisplayMode())
		{
			//���������� ���������� �������
		}

		//������� ������ (�������� � ������������� ����������� Windows)
		while (�ursorCounter >= 0)
		{
			�ursorCounter = ShowCursor(FALSE);
		}
	}
	else
	{
		ShowCursor(TRUE);
	}

	//����� ��� ��1
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

	//����� ��� ��1
	Init_Timer();

	//��������� ���� widht, height ���������� �� screen_sizer
	// x, y, width, height ����� ��� ���������� ���������� ���� ����
	// scrwidth, scrheight - ������� �������
	//��������� phd_InitWindow ��������� x, y, width, height,
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
			//� ������� ���� ���� ���� ��� �� ������ ���� �������� ������� �
			//�����
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

	if (Fullscreen_Software)
	{
		if (RestorePreviousDisplayMode())
		{
			//���������� ����� ������������ �������
		}
	}

	if(Hardware)
		FreeGameMemory();

	DestroyWindow(g_hWnd);
	UnregisterClass(wcl.lpszClassName, wcl.hInstance);

	return 0;
}
