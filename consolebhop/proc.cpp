#include "pch.h"
#include "proc.h"


DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Take snap
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry; // 
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!wcscmp(procEntry.szExeFile, procName)) // compare wide char string
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}

	}
	CloseHandle(hSnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddress = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!wcscmp(modEntry.szModule, modName))
				{
					modBaseAddress = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddress;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

void MemoryWrite(DWORD BaseAddress, char *opcodes, int byteNum)
{
	unsigned long oldProtect=0;
	VirtualProtect((LPVOID)BaseAddress, byteNum, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((LPVOID)BaseAddress, opcodes, byteNum);
	VirtualProtect((LPVOID)BaseAddress, byteNum, oldProtect, NULL);
}

void WriteMemoryProcess(HANDLE hProc, LPVOID dwAddress, LPCVOID ValueToWrite, SIZE_T ofValue, SIZE_T *NumberOfBytes)
{
	WriteProcessMemory(hProc, dwAddress, ValueToWrite, ofValue, NumberOfBytes);
}