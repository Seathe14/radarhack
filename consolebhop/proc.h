#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
//void WriteMemoryProcess(HANDLE hProc, LPVOID dwAddress, LPCVOID ValueToWrite,SIZE_T ofValue,SIZE_T *NumberOfBytes);
void MemoryWrite(DWORD BaseAddress, char *opcodes, int byteNum);


template <class T> 
void Write(HANDLE hProc, DWORD Address,T ValueToWrite)
{
	//T Temp;
	WriteProcessMemory(hProc,(LPVOID)Address, &ValueToWrite, sizeof(ValueToWrite), NULL);
}


template <class P>
void Write(HANDLE hProc, DWORD Address, BYTE ValueToWrite[])
{
	//T Temp;
	WriteProcessMemory(hProc, (LPVOID)Address, ValueToWrite, sizeof(ValueToWrite), NULL);
}


template <class RM>
RM Read(HANDLE hProc, DWORD Address)
{
	RM Temp;
	ReadProcessMemory(hProc, (LPVOID)Address, &Temp, sizeof(Temp), NULL);
	return Temp;
}
