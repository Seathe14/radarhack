#pragma once

#include <Windows.h>

class cOffsets

{
public:
	DWORD fFlags = 0x104;
	DWORD LocalPlayer = 0xCB3694;
	DWORD dw_ForceJump = 0x51666C0;
	DWORD dwEntityList = 0x4CC3564;
	DWORD m_iGlowIndex = 0xA3F8;
	DWORD dwGlowObjectManager = 0x5203328;
	DWORD m_vecOrigin = 0x138;
	DWORD m_vecViewOffset = 0x108;
	DWORD dwClientState_ViewAngles = 0x4D10;
	DWORD dwClientState = 0x58ACFC;
	DWORD m_iCrosshairId = 0xB390;
	DWORD dwForceAttack = 0x30F4C1C;
	DWORD dwGetAllClasses = 0xC81474;
	DWORD m_bSpotted = 0x93D;
	DWORD m_dwBoneMatrix = 0x2698;
	DWORD dwViewMatrix = 0x4C2DF24;
	DWORD m_iTeamNum = 0xF4;
	DWORD m_bDormant = 0xED;
}Offsets;

struct GlowObj
{
	float GlowTerroristRed;
	float GlowTerroristGreen;
	float GlowTerroristBlue;
	float GlowTerroristAlpha;
};
GlowObj GlowObjectsT{ 1.0f,0.5f,0.0f,1.0f };
GlowObj GlowObjectsCT{ 0.0f,1.f,0.0f,1.0f };

struct Render
{
	bool m_bRenderWhenOccluded = true;
	bool m_bRenderWhenUnoccluded = false;
}m_bRender;

#pragma once
