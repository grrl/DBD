#ifndef HDIRECTX_H
#define HDIRECTX_H

#include <iostream>
#include <Windows.h>
#include <algorithm>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

extern IDirect3D9Ex* dx_Object;
extern IDirect3DDevice9Ex* dx_Device;
extern D3DPRESENT_PARAMETERS dx_Params;
extern ID3DXLine* dx_Line;
extern ID3DXFont* dx_Font;
extern ID3DXFont* dx_FontCalibri;
extern LPDIRECT3DVERTEXBUFFER9 g_pVB;    // Buffer to hold vertices
extern LPDIRECT3DINDEXBUFFER9  g_pIB;    // Buffer to hold indice
extern HWND hWnd, TargetWnd, ohwnd;
extern RECT WindowRect, ClientRect;
extern bool show_menu;
extern float clientHeight, clientWidth;
extern int windowWidth, windowHeight;
extern int borderWidth, borderHeight;
extern int windowstate;
extern float fov;
int D3D9Init(HWND hWnd);
#endif