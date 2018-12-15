// consolebhop.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <sstream>
#include "proc.h"
#include <Windows.h>
#include <thread>
#include <math.h>
#include <cmath>
#include <ctime>
#include "Offsets and structs.h"
#include <objidl.h>
#include <gdiplus.h>
#define	FL_ONGROUND	 (1<<0)
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
using namespace std;
//Globals
DWORD cEntityBase;
DWORD dwMyTeam;
DWORD EntityTeam;
DWORD glowPointer;
DWORD EntityList;
DWORD Entity;
DWORD Engine;
DWORD clientState;
DWORD EntHealth;

//RECT

RECT m_Rect;
HDC HDC_Desktop;
HBRUSH EnemyBrush;
HFONT Font;




// HWND
HWND TargetWnd;
HWND Handle;


COLORREF SnapLineCOLOR;
COLORREF TextCOLOR;


struct W2SMatrix
{
	float flMatrix[4][4];
}W2SMatrix_t;



//Other vars
int crosshairEntityId;
double huy = M_PI;
int  BoneMatrix;
BYTE Red[3] = { 255,127,0 };
DWORD procId = 0;
DWORD clientDLLAddress = 0;
HANDLE procHandle = 0;
DWORD dwLocalBase = 0;
DWORD dwFlags = 0;
int id;
int b = 6;
bool sBhop = false;
bool sGlowHack;
bool sRadarHack;

float enmlocF[3];

float Get3dDistance(float * myCoords, float * enemyCoords)
{
	return sqrt(
		pow(double(enemyCoords[0] - myCoords[0]), 2.0) +
		pow(double(enemyCoords[1] - myCoords[1]), 2.0) +
		pow(double(enemyCoords[2] - myCoords[2]), 2.0));

}


void SetupDrawing(HDC hDesktop, HWND handle)
{
	HDC_Desktop = hDesktop;
	Handle = handle;
	EnemyBrush = CreateSolidBrush(RGB(255, 0, 0));

	SnapLineCOLOR = RGB(0, 0, 255);
	TextCOLOR = RGB(0, 255, 0);
}

bool WorldToScreen(float *from, float *to)
{
	float w = 0.0f;
	to[0] = W2SMatrix_t.flMatrix[0][0] * from[0] + W2SMatrix_t.flMatrix[0][1] * from[1] + W2SMatrix_t.flMatrix[0][2] * from[2] + W2SMatrix_t.flMatrix[0][3];
	to[1] = W2SMatrix_t.flMatrix[1][0] * from[0] + W2SMatrix_t.flMatrix[1][1] * from[1] + W2SMatrix_t.flMatrix[1][2] * from[2] + W2SMatrix_t.flMatrix[1][3];
	w = W2SMatrix_t.flMatrix[3][0] * from[0] + W2SMatrix_t.flMatrix[3][1] * from[1] + W2SMatrix_t.flMatrix[3][2] * from[2] + W2SMatrix_t.flMatrix[3][3];
	if (w < 0.01f)
		return false;
	float invw = 1.0f / w;
	to[0] *= invw;
	to[1] *= invw;

	int width = (int)(m_Rect.right - m_Rect.left);
	int height = (int)(m_Rect.bottom - m_Rect.top);

	float x = width / 2;
	float y = height / 2;

	x += 0.5 * to[0] * width + 0.5;
	y -= 0.5 * to[1] * height + 0.5;

	to[0] = x + m_Rect.left;
	to[1] = y + m_Rect.top;
	return true;

}



void DrawFilledRect(int x, int y, int w, int h)
{
	//We create our rectangle to draw on screen
	RECT rect = { x, y, x + w, y + h };
	//We clear that portion of the screen and display our rectangle
	FillRect(HDC_Desktop, &rect, EnemyBrush);
}

void DrawBoxer(int x, int y, int w, int h, char* string) {
	DrawBoxer(x, y, w, h, string);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
	//Top horiz line
	DrawFilledRect(x, y, w, thickness);
	//Left vertical line
	DrawFilledRect(x, y, thickness, h);
	//right vertical line
	DrawFilledRect((x + w), y, thickness, h);
	//bottom horiz line
	DrawFilledRect(x, y + h, w + thickness, thickness);
}


void DrawLine(float StartX, float StartY, float EndX, float EndY, COLORREF Pen)
{
	int a, b = 0;
	HPEN hOPen;
	// penstyle, width, color
	HPEN hNPen = CreatePen(PS_SOLID, 2, Pen);
	hOPen = (HPEN)SelectObject(HDC_Desktop, hNPen);
	// starting point of line
	MoveToEx(HDC_Desktop, StartX, StartY, NULL);
	// ending point of line
	a = LineTo(HDC_Desktop, EndX, EndY);
	DeleteObject(SelectObject(HDC_Desktop, hOPen));
}


void DrawString(int x, int y, COLORREF color, const char* text)
{
	SetTextAlign(HDC_Desktop, TA_CENTER | TA_NOUPDATECP);

	SetBkColor(HDC_Desktop, RGB(0, 0, 0));
	SetBkMode(HDC_Desktop, TRANSPARENT);

	SetTextColor(HDC_Desktop, color);

	//SelectObject(HDC_Desktop, Font);

	TextOutA(HDC_Desktop, x, y, text, strlen(text));

	//DeleteObject(Font);
}

void DrawESP(int x, int y, float distance)
{
	//ESP RECTANGLE

	int width = 18100 / distance;
	int height = 37500 / distance;
	//DrawBoxer();
	DrawBorderBox(x - (width / 2), y - height, width, height, 1);

	//Sandwich ++
	DrawLine((m_Rect.right - m_Rect.left) / 2,
		m_Rect.bottom - m_Rect.top, x, y,
		SnapLineCOLOR);


	std::stringstream ss;
	ss << (int)distance;

	char * distanceInfo = new char[ss.str().size() + 1];
	strcpy(distanceInfo, ss.str().c_str());

	DrawString(x, y, TextCOLOR, distanceInfo);

	delete[] distanceInfo;
}



void OnPaint(HDC hdc, HWND hwnd)
{
	Graphics graphics(hdc);
	Pen pen(Color(255, 0, 0));
	graphics.DrawLine(&pen, 0, 0, 200, 100);
}




struct Vector
{
	float x, y, z;
}; 

Vector ourloc;
float ourlocEyes;
float enmlocEyes;
Vector enmloc;
//Vector angles;
Vector aimAngles;
Vector Bones;
Vector playerViewAngles;
Vector difAngle;
DWORD GetClassId(DWORD entBase)
{
	DWORD vt = Read<DWORD>(procHandle, entBase + 0x8);
	DWORD fn = Read<DWORD>(procHandle, vt + 2 * 0x4);
	DWORD cls = Read<DWORD>(procHandle, fn + 0x1);
	DWORD clsn = Read<DWORD>(procHandle, cls + 0x14);
	//DWORD clsid = Read<DWORD>(procHandle, cls + 20);
	return clsn;
}

//float hypotenuse(Vector vec)
//{
//	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
//}
//Vector subtract(Vector src, Vector enm)
//{
//	Vector temp;
//	temp.x = src.x - enm.x;
//	temp.y = src.y - enm.y;
//	temp.z = src.z - enm.z;
//	return temp;
//}
//
//float Distance(Vector src, Vector enm)
//{
//	Vector temp = subtract(src, enm);
//	return hypotenuse(temp);
//}
Vector Plus(Vector src, Vector dst)
{
	src.x += dst.x;
	src.y += dst.y;
	src.z += dst.z;
	return src;
}

Vector Subtract(Vector src, Vector dst)
{
	Vector diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

float* Subtract(float* src, float* dst)
{
	float diff[3];
	diff[0] = src[0] - dst[0];
	diff[1] = src[1] - dst[1];
	diff[2] = src[2] - dst[2];
	return diff;
}

float Magnitude(Vector vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

float Magnitude(float* vec)
{
	return sqrtf(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}


float Distance(Vector src, Vector dst)
{
	Vector diff = Subtract(src, dst);
	return Magnitude(diff);
}

float Distance(float* src, float* dst)
{
	float* diff = Subtract(src, dst);
	cout << diff[0] << " " << diff[1] << " " << diff[2] << endl;
	Sleep(500);
	return Magnitude(diff);
}




Vector calAngle(Vector src, Vector dir)
{
	Vector angle;
	Vector newAngle;
	angle.x = dir.x - src.x;
	angle.y = dir.y - src.y;
	angle.z = dir.z - src.z;

	float Magnitude = sqrt(angle.x * angle.x + angle.y * angle.y + angle.z * angle.z);
	newAngle.x = (atan2(angle.y , angle.x)  * 180 / 3.14);
	newAngle.y = (-asinf(angle.z / Magnitude)  * 180 / 3.14);
	newAngle.z = 0;
	return newAngle;

}
//Vector calculateAngle(Vector src, Vector enm)
//{
//	Vector angles;
//	angles.x = atan(enm.y - src.y / enm.x - src.x) * 180.0f / M_PI;//((float)atan2(enm.y - src.y, enm.x - src.x))  * 180.0f / 3.14;
//	angles.y = acosf(enm.z - src.z / Distance(src, enm)) * 180.0f / M_PI; // (-atan2(enm.z - src.z, Distance(src, enm))) * 180.0f / 3.14;
//	angles.z = 0.0f;
//	return angles;
//}

void ReadInfo()
{
	// Basic Stuff
	procId = GetProcId(L"csgo.exe");
	clientDLLAddress = GetModuleBaseAddress(procId, L"client_panorama.dll");
	Engine = GetModuleBaseAddress(procId, L"engine.dll");
	procHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	dwLocalBase = Read<DWORD>(procHandle, Offsets.LocalPlayer + clientDLLAddress);
	cEntityBase = clientDLLAddress + Offsets.dwEntityList;
	dwMyTeam = Read<DWORD>(procHandle, dwLocalBase + 0xF0);
	glowPointer = Read<DWORD>(procHandle, clientDLLAddress + Offsets.dwGlowObjectManager);
	//W2SMatrix_t = Read<W2SMatrix>(procHandle, clientDLLAddress + Offsets.dwViewMatrix);
	ReadProcessMemory(procHandle, (PBYTE*)(clientDLLAddress + Offsets.dwViewMatrix), &W2SMatrix_t, sizeof(W2SMatrix_t), 0);

	//ReadProcessMemory(procHandle, (BYTE*)dwLocalBase + Offsets.m_vecOrigin, &ourloc, sizeof(ourloc), NULL);

}

//void smoothing(float i, Vector angles)
//{
//	for (float k = i; k >= 1;k-=0.05f)
//	{
//		Write<float>(procHandle, clientState + Offsets.dwClientState_ViewAngles + 0x4, angles.x / k);
//		//Sleep(3);
//	}
//}
// bhop useless func.
void bhop()
{
	//cout << sBhop << "F1 worked " << endl;
		//cout << " huy" << endl;
		ReadProcessMemory(procHandle, (LPVOID)(dwLocalBase + Offsets.fFlags), &dwFlags, sizeof(dwFlags), NULL);
	//	cout << "dw flags = " << dwFlags << endl;
		if (dwFlags & FL_ONGROUND && GetAsyncKeyState(VK_SPACE))
		{
			WriteProcessMemory(procHandle, (BYTE*)clientDLLAddress + Offsets.dw_ForceJump, &b, sizeof(b), NULL);
		}
		this_thread::sleep_for(chrono::milliseconds(1));
}

void GlowHack()
{
	for (int i = 0; i < 32; i++)
	{
		int glow_currentPlayer = Read<int>(procHandle, cEntityBase + (i * 0x10));
		bool isDormant = Read<bool>(procHandle, glow_currentPlayer + 0xE9);
		int glowcurrentPlayerGlowIndex = Read<int>(procHandle, glow_currentPlayer + 0xA330);
		int EntityBaseTeamID = Read<int>(procHandle, glow_currentPlayer + 0xF0);
		if (isDormant == 1 || EntityBaseTeamID == 0)
			continue;
	    else
		{
			if (dwMyTeam != EntityBaseTeamID)
			{
				switch (EntityBaseTeamID) {
				case 2:
			//		Write<int>(procHandle, glow_currentPlayer + 0x70, Red);
					Write <GlowObj>(procHandle, (glowPointer + (glowcurrentPlayerGlowIndex * 0x38) + 0x4), GlowObjectsT);
					Write <Render>(procHandle, (glowPointer + ((glowcurrentPlayerGlowIndex * 0x38) + 0x24)), m_bRender);
					break;
				case 3:
				//	Write<int>(procHandle, glow_currentPlayer + 0x70, Red);
					Write <GlowObj>(procHandle, (glowPointer + (glowcurrentPlayerGlowIndex * 0x38) + 0x4), GlowObjectsCT);
					Write <Render>(procHandle, (glowPointer + ((glowcurrentPlayerGlowIndex * 0x38) + 0x24)), m_bRender);
					break;
				}
			}
		}
		this_thread::sleep_for(chrono::nanoseconds(15));
	}
}
void RadarHack(bool status)
{
	if (status == 1)
	for (int i = 0; i < 32; i++)
	{
		Entity = Read<DWORD>(procHandle, cEntityBase + i * 0x10);
		
		//cout << Entity << endl;
		//Sleep(100);

		//bool isDormant = Read<DWORD>(procHandle, Entity + 0xE9);
		//	if (isDormant == 1)
		//		continue;
		//	else {
				int NumberTeam = Read<DWORD>(procHandle, Entity + 0xF4);
				if (dwMyTeam != NumberTeam)
					Write<DWORD>(procHandle, Entity + Offsets.m_bSpotted, status);
			//}
	}
}
void Aim()
{
	for (int i = 0; i < 32; i++)
	{
		//
		Entity = Read<DWORD>(procHandle, cEntityBase +  0x40);
		clientState = Read<DWORD>(procHandle, Engine + Offsets.dwClientState);
		//ourlocEyes = Read<float>(procHandle, clientState + Offsets.m_vecViewOffset);
		//cout << Entity << endl;
		int NumberTeam = Read<DWORD>(procHandle, Entity + 0xF0);
		enmloc = Read<Vector>(procHandle, Entity + Offsets.m_vecOrigin);
		//
		cout << enmloc.x << " " << enmloc.z << endl;
		BoneMatrix = Read<DWORD>(procHandle, Entity + Offsets.m_dwBoneMatrix);
		Bones.x = Read<float>(procHandle, BoneMatrix +0x30 * 8+ 0x0C);
		Bones.y = Read<float>(procHandle, BoneMatrix +0x30 * 8+ 0x1C);
		Bones.z = Read<float>(procHandle, BoneMatrix +0x30 + 0x2C);
		bool isDormant = Read<DWORD>(procHandle, Entity + 0xE9);
		if (isDormant == 1)
			continue;
		
		//cout << ourlocEyes << endl;
		if (dwMyTeam != NumberTeam)
		{
			
			aimAngles = calAngle(ourloc, Bones);
			difAngle = Subtract(playerViewAngles, aimAngles);
			playerViewAngles.x = playerViewAngles.x + difAngle.x / 4;
			playerViewAngles.y = playerViewAngles.y + difAngle.y / 4;
			////difAngle.y /= 4;
		//	aimAngles.x = aimAngles.x + difAngle.x;
			//aimAngles.y = aimAngles.y + difAngle.y;
			playerViewAngles = Read<Vector>(procHandle, clientState + Offsets.dwClientState_ViewAngles);

			//cout << playerViewAngles.x <<" " << playerViewAngles.y << endl;
		//	Sleep(500);

			Write<float>(procHandle, clientState + Offsets.dwClientState_ViewAngles, aimAngles.y);
			Write <float>(procHandle, clientState + Offsets.dwClientState_ViewAngles + 0x4, aimAngles.x);
			//Write<float>(procHandle, clientState + Offsets.dwClientState_ViewAngles + 0x4, aimAngles.x);
			//smoothing(16.0f, aimAngles);
			break;
			
			//Write<float>(procHandle, clientState + Offsets.dwClientState_ViewAngles + 0x4, aimAngles.x/1.1);
			//Sleep(1000);
			//Write<float>(procHandle, clientState + Offsets.dwClientState_ViewAngles + 0x4, aimAngles.x);
			Sleep(500);
		}

	}
}
void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
} 


void ESP()
{
	GetWindowRect(FindWindowA("Valve001", 0), &m_Rect);
	for (int i = 0; i < 64; i++)
	{
		Entity = Read<DWORD>(procHandle, cEntityBase + i * 0x10);
		EntHealth = Read<DWORD>(procHandle, Entity + 0xFC);
		//enmlocF = Read<float>(procHandle, Entity + Offsets.m_vecOrigin);
		ReadProcessMemory(procHandle, (PBYTE*)(Entity + Offsets.m_vecOrigin), &enmlocF, sizeof(float[3]), 0);
		float ourlocF[3];
		ReadProcessMemory(procHandle, (PBYTE*)(dwLocalBase + Offsets.m_vecOrigin), &ourlocF, sizeof(float[3]), 0);
		//cout << enmlocF[1] << endl;
		//Sleep(100);
		int NumberTeam = Read<DWORD>(procHandle, Entity + 0xF0);
		bool isDormant = Read<DWORD>(procHandle, Entity + 0xE9);
		if (isDormant == 1)
			continue;
		if (EntHealth < 2)
			continue;
		if (dwMyTeam == NumberTeam)
			continue;
		float EnemyXY[3];
		if (WorldToScreen(ourlocF, EnemyXY))
		{
		//	cout << Distance(enmlocF, ourlocF) << endl;
			//Sleep(1000);
			DrawESP(EnemyXY[0] - m_Rect.left, EnemyXY[1] - m_Rect.top, Get3dDistance(ourlocF, enmlocF));
		}
	}
}

int main()
{
	
	//ShowWindow(FindWindow(L"ConsoleWindowClass", NULL), true);
	//TargetWnd = FindWindow(NULL, L"Counter-Strike: Global Offensive");
	//LPWSTR lpClass;
	//GetClassName(TargetWnd, lpClass, sizeof(lpClass));
	//TargetWnd = FindWindow(0, L"Counter-Strike: Global Offensive");
	//HDC HDC_Desktop = GetDC(TargetWnd);

	//HDC_Desktop = HDC_Desktop1;
	//Handle = TargetWnd;
	//EnemyBrush = CreateSolidBrush(RGB(255, 0, 0));

	//SnapLineCOLOR = RGB(0, 0, 255);
	//TextCOLOR = RGB(0, 255, 0);
	//SetupDrawing(HDC_Desktop, TargetWnd);
	//cout << M_PI << endl;

	

	do{
			// Trying GlowHack
		//cout << angles.x << " " << angles.y << " " << angles.z << endl;
		//Sleep(15);
		ReadInfo();
		
		/*ESP();*/
			if (GetAsyncKeyState(VK_F1))
			{
				
				sBhop = !sBhop;
				Sleep(200);
			}
			if (sBhop)
			{
				bhop();
			//	cout << "Bhop is ON" << endl;
				//this_thread::sleep_for(chrono::milliseconds(5));
			//	clear();
	
			}
		//	else 
			//{
			//	cout << "Bhop is OFF" << endl;
			//	this_thread::sleep_for(chrono::milliseconds(5));
			//	clear();

			//}
			RadarHack(1);
			if (GetAsyncKeyState(VK_F2))
			{
				sGlowHack = !sGlowHack;
				Sleep(200);
			}
			if(sGlowHack)
				GlowHack();

			/*if (GetAsyncKeyState(VK_F3))
			{
				sRadarHack = !sRadarHack;
				Sleep(200);
			}
			if (sRadarHack)
			{
				RadarHack(1);
			}
			else RadarHack(0);*/

		id = Read<int>(procHandle, dwLocalBase + Offsets.m_iCrosshairId);
		//cout << id << endl;
		Sleep(1);
		//cout << aimAngles.x << " - x, y -  " << aimAngles.y << endl;
	} while (true);
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
