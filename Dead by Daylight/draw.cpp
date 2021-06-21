#include <iostream>
#include <vector>
#include "draw.h"
#include "vector3.h"

#define PI 3.14159265

LPDIRECT3DVERTEXBUFFER9 g_pVB;    // Buffer to hold vertices
LPDIRECT3DINDEXBUFFER9  g_pIB;    // Buffer to hold indices

int DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT ShadowPos;
	ShadowPos.left = x + 1;
	ShadowPos.top = y + 1;
	RECT FontPos;
	FontPos.left = x;
	FontPos.top = y;
	ifont->DrawTextA(0, String, strlen(String), &ShadowPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r / 3, g / 3, b / 3));
	ifont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}
int DrawShadowString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont)
{
	RECT Font;
	Font.left = x;
	Font.top = y;
	RECT Fonts;
	Fonts.left = x + 1;
	Fonts.top = y;
	RECT Fonts1;
	Fonts1.left = x - 1;
	Fonts1.top = y;
	RECT Fonts2;
	Fonts2.left = x;
	Fonts2.top = y + 1;
	RECT Fonts3;
	Fonts3.left = x;
	Fonts3.top = y - 1;
	ifont->DrawTextA(0, String, strlen(String), &Fonts3, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts2, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts1, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Fonts, DT_NOCLIP, D3DCOLOR_ARGB(255, 1, 1, 1));
	ifont->DrawTextA(0, String, strlen(String), &Font, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
	return 0;
}

void GradientFunc(int x, int y, int w, int h, int r, int g, int b, int a)
{
	int iColorr, iColorg, iColorb;
	for (int i = 1; i < h; i++)
	{
		iColorr = (int)((float)i / h * r);
		iColorg = (int)((float)i / h * g);
		iColorb = (int)((float)i / h * b);
		DrawFilled(x, y + i, w, 1, r - iColorr, g - iColorg, b - iColorb, a);
	}
}
void DrawLine(float x, float y, float xx, float yy, int r, int g, int b, int a)
{
	D3DXVECTOR2 dLine[2];

	dx_Line->SetWidth(1);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	dx_Line->Draw(dLine, 2, D3DCOLOR_ARGB(a, r, g, b));

}
void DrawFilled(float x, float y, float w, float h, int r, int g, int b, int a)
{
	D3DXVECTOR2 vLine[2];

	dx_Line->SetWidth(w);

	vLine[0].x = x + w / 2;
	vLine[0].y = y;
	vLine[1].x = x + w / 2;
	vLine[1].y = y + h;

	dx_Line->Begin();
	dx_Line->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
	dx_Line->End();
}
void DrawBox(float x, float y, float width, float height, float px, int r, int g, int b, int a)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	dx_Line->SetWidth(px);
	dx_Line->Draw(points, 5, D3DCOLOR_RGBA(r, g, b, a));
}
void DrawGUIBox(float x, float y, float w, float h, int r, int g, int b, int a, int rr, int gg, int bb, int aa)
{
	DrawBox(x, y, w, h, 1, r, g, b, a);
	DrawFilled(x, y, w, h, rr, gg, bb, a);
}
void DrawHealthBar(float x, float y, float w, float h, int r, int g, int b, int a)
{
	DrawFilled(x, y, w, h, r, g, b, a);
}
void DrawHealthBarBack(float x, float y, float w, float h, int a)
{
	DrawFilled(x, y, w, h, 0, 0, 0, a);
}
void DrawCenterLine(float x, float y, int width, int r, int g, int b)
{
	D3DXVECTOR2 dPoints[2];
	dPoints[0] = D3DXVECTOR2(x, y);
	dPoints[1] = D3DXVECTOR2(windowWidth / 2, windowHeight);
	dx_Line->SetWidth(width);
	dx_Line->Draw(dPoints, 2, D3DCOLOR_RGBA(r, g, b, 255));
}

void DrawCircle(float x, float y, float r, float s, DWORD color)
{
	float Step = PI * 2.0 / s;
	for (float a = 0; a < (PI * 2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		//DrawLine(x1, y1, x2, y2, color);
	}
}

void Circle(int X, int Y, int radius, int numSides, DWORD Color)
{
	D3DXVECTOR2 Line[128];
	float Step = PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < PI*2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	dx_Line->Begin();
	dx_Line->Draw(Line, Count, Color);
	dx_Line->End();
}


void Circle(float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, DWORD color)
{
	std::vector<vertex> circle(resolution + 2);
	float angle = rotate * D3DX_PI / 180;
	float pi;

	if (type == full) pi = D3DX_PI;        // Full circle
	if (type == half) pi = D3DX_PI / 2;      // 1/2 circle
	if (type == quarter) pi = D3DX_PI / 4;   // 1/4 circle

	for (int i = 0; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(i*(2 * pi / resolution)));
		circle[i].y = (float)(y - radius * sin(i*(2 * pi / resolution)));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
		circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
	}

	dx_Device->CreateVertexBuffer((resolution + 2) * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);

	VOID* pVertices;
	g_pVB->Lock(0, (resolution + 2) * sizeof(vertex), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(vertex));
	g_pVB->Unlock();


	dx_Device->SetTexture(0, NULL);
	dx_Device->SetPixelShader(NULL);
	if (smoothing)
	{
		dx_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		dx_Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	}
	dx_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dx_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dx_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	dx_Device->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
	dx_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	dx_Device->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
	if (g_pVB != NULL)
		g_pVB->Release();
}