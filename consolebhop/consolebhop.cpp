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

#define	FL_ONGROUND	 (1<<0)
using namespace std;
//Globals
DWORD cEntityBase;
DWORD dwMyTeam;
DWORD glowPointer;
DWORD EntityList;
DWORD Entity;
DWORD Engine;

//Other vars
DWORD procId = 0;
DWORD clientDLLAddress = 0;
HANDLE procHandle = 0;
DWORD dwLocalBase = 0;
DWORD dwFlags = 0;
int b = 6;
bool sBhop = false;
bool sGlowHack;
bool sRadarHack;

void ReadInfo()
{
	// Basic Stuff
	procId = GetProcId(L"csgo.exe");
	clientDLLAddress = GetModuleBaseAddress(procId, L"client_panorama.dll");
	Engine = GetModuleBaseAddress(procId, L"engine.dll");
	procHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	dwLocalBase = Read<DWORD>(procHandle, Offsets.LocalPlayer + clientDLLAddress);
	cEntityBase = clientDLLAddress + Offsets.dwEntityList;
	dwMyTeam = Read<DWORD>(procHandle, dwLocalBase + Offsets.m_iTeamNum);
	glowPointer = Read<DWORD>(procHandle, clientDLLAddress + Offsets.dwGlowObjectManager);
}

void bhop()
{
		ReadProcessMemory(procHandle, (LPVOID)(dwLocalBase + Offsets.fFlags), &dwFlags, sizeof(dwFlags), NULL);
		if (dwFlags & FL_ONGROUND && GetAsyncKeyState(VK_SPACE))
		{
			Write<int>(procHandle, clientDLLAddress + Offsets.dw_ForceJump, 6);
		}
		this_thread::sleep_for(chrono::milliseconds(1));
}

void GlowHack()
{
	for (int i = 0; i < 32; i++)
	{
		int glow_currentPlayer = Read<int>(procHandle, cEntityBase + (i * 0x10)); // Loop through entitylist for every entity
		bool isDormant = Read<bool>(procHandle, glow_currentPlayer + Offsets.m_bDormant); // check if it is dormant
		int glowcurrentPlayerGlowIndex = Read<int>(procHandle, glow_currentPlayer + Offsets.m_iGlowIndex);
		int EntityBaseTeamID = Read<int>(procHandle, glow_currentPlayer + Offsets.m_iTeamNum);
		if (isDormant == 1 || EntityBaseTeamID == 0)
			continue;
	    else
		{
			if (dwMyTeam != EntityBaseTeamID) // if not in my team so glow with color defined in GlowObjects structure
			{
				switch (EntityBaseTeamID) {
				case 2:
					Write <GlowObj>(procHandle, (glowPointer + (glowcurrentPlayerGlowIndex * 0x38) + 0x4), GlowObjectsT);
					Write <Render>(procHandle, (glowPointer + ((glowcurrentPlayerGlowIndex * 0x38) + 0x24)), m_bRender);
					break;
				case 3:
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
	if(status == 1)
	for (int i = 0; i < 32; i++)
	{
		Entity = Read<DWORD>(procHandle, cEntityBase + i * 0x10); // Loop through entitylist for every entity
		bool isDormant = Read<DWORD>(procHandle, Entity + Offsets.m_bDormant); // check if it is dormant
			if (isDormant == 1) continue;
			else 
			{
				int NumberTeam = Read<DWORD>(procHandle, Entity + 0xF4);
				if (dwMyTeam != NumberTeam)
					Write<DWORD>(procHandle, Entity + Offsets.m_bSpotted, status); // if not in my team then mark as spotted
			}
	}

}

int main()
{
	do{
		ReadInfo();
			if (GetAsyncKeyState(VK_F1))
			{
				sBhop = !sBhop;
				Sleep(200);
			}
			if (sBhop)
			{
				bhop();
			}
			if (GetAsyncKeyState(VK_F2))
			{
				sGlowHack = !sGlowHack;
				Sleep(200);
			}
			if (sGlowHack)
			{
				GlowHack();
			}
			if (GetAsyncKeyState(VK_F3))
			{
				sRadarHack = !sRadarHack;
				Sleep(200);
			}
			if (sRadarHack)
			{
				RadarHack(1);
			}
			else RadarHack(0);
		Sleep(1);
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
