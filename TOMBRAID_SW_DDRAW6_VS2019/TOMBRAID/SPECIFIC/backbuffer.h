#pragma once

#include <ddraw.h>
#include <d3d.h>
#include <d3dtypes.h>
#include <d3dcaps.h>

#pragma comment (lib, "ddraw.lib")
#pragma comment (lib, "dxguid.lib")

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

//extern MGLDC *windc;
//extern MGLDC *dibdc;

extern char* phd_winptr_my;

HRESULT Create_Water_Palette();
HRESULT Create_Normal_Palette();
HRESULT Create_BackBuffer();
HRESULT Clear_BackBuffer();
HRESULT Present_BackBuffer();
HRESULT Delete_BackBuffer();
