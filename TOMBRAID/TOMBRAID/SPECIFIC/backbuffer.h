#pragma once

#include "..\scitech\INCLUDE\MGRAPH.H"
//#pragma comment (lib, "./scitech/LIB/WIN32/VC/MGLLT.LIB")
#pragma comment (lib, ".\\scitech\\LIB\\WIN32\\VC\\MGLLT.LIB")
#pragma comment(lib, "winmm.lib")


extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern MGLDC* windc;
extern MGLDC* dibdc;

void Create_Water_Palette();
void Create_Normal_Palette();
void Create_BackBuffer();
void Clear_BackBuffer();
void Present_BackBuffer();
void Delete_BackBuffer();
