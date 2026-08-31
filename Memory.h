#pragma once
#include <Windows.h>
#include <Psapi.h>
#include "Common.h"

#define NOP_OPCODE 0x90
#define RET_OPCODE 0xC3
#define JMP_OPCODE 0xE9
#define JNZ_OPCODE 0x85
namespace API {
	class CMemory
	{
	public:
		static VOID Write(LPVOID IpBaseAddress, LPCVOID IpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
			WriteProcessMemory(GetCurrentProcess(), IpBaseAddress, IpBuffer, nSize, lpNumberOfBytesWritten);
		}
		static BOOL Read(LPVOID IpBaseAddress, LPVOID IpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten) {
			/*DWORD Protect;
			VirtualProtect(IpBaseAddress, nSize, PAGE_EXECUTE_READWRITE, &Protect);*/
			return ReadProcessMemory(GetCurrentProcess(), IpBaseAddress, IpBuffer, nSize, lpNumberOfBytesWritten);
		}
		static VOID WriteSafe(LPVOID IpBaseAddress, LPCVOID IpBuffer, SIZE_T Size) {
			DWORD Protect;
			VirtualProtect(IpBaseAddress, Size, PAGE_EXECUTE_READWRITE, &Protect);
			WriteProcessMemory(GetCurrentProcess(), IpBaseAddress, IpBuffer, Size, NULL);
			VirtualProtect(IpBaseAddress, Size, Protect, &Protect);
		}
		static VOID WriteUInt8Memory(LPVOID Address, BYTE value) {
			DWORD dwOld;
			PBYTE pAddr = (PBYTE)Address;
			VirtualProtect(&pAddr[0], 1, PAGE_EXECUTE_READWRITE, &dwOld);
			*pAddr = value;
		}
		static VOID WriteUInt16Memory(LPVOID Address, uint16_t value) {
			DWORD dwOld;
			PBYTE pAddr = (PBYTE)Address;
			VirtualProtect(&pAddr[0], 4, PAGE_EXECUTE_READWRITE, &dwOld);
			*(uint16_t*)(pAddr + 0) = value;
		}
		static VOID WriteFloatMemory(LPVOID Address, float value) {
			DWORD dwOld;
			PBYTE pAddr = (PBYTE)Address;
			VirtualProtect(pAddr, sizeof(float), PAGE_EXECUTE_READWRITE, &dwOld);
			*(float*)(pAddr) = value;
			VirtualProtect(pAddr, sizeof(float), dwOld, &dwOld);
		}
		static void WriteToMemory(DWORD pAddress, void* pBuffer, int iSize)
		{
			DWORD Protect = NULL;
			VirtualProtect((void*)pAddress, iSize, PAGE_EXECUTE_READWRITE, &Protect);
			WriteProcessMemory(GetCurrentProcess(), (void*)pAddress, pBuffer, iSize, NULL);
			VirtualProtect((void*)pAddress, iSize, Protect, &Protect);
		}
		static VOID WriteUInt32Memory(LPVOID Address, uint32_t value) {
			DWORD dwOld;
			PBYTE pAddr = (PBYTE)Address;
			VirtualProtect(&pAddr[0], 4, PAGE_EXECUTE_READWRITE, &dwOld);
			*(uint32_t*)(pAddr + 0) = value;
		}
		static VOID WriteARRAY8Memory(LPVOID Address, BYTE* Data) {
			DWORD dwOld;
			PBYTE pAddr = (PBYTE)Address;
			VirtualProtect(&pAddr[0], 4, PAGE_EXECUTE_READWRITE, &dwOld);
			for (int32_t x = 0; x < 4; x++)
				pAddr[x] = Data[x];
		}
		static DWORD AOBScan(CHAR* Pattern, CHAR* Mask) {
			MODULEINFO ModuleInfo = { NULL };
			GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &ModuleInfo, sizeof(MODULEINFO));
			DWORD Base = (DWORD)ModuleInfo.lpBaseOfDll;
			DWORD Size = (DWORD)ModuleInfo.SizeOfImage;
			for (DWORD I = 0; I < Size - strlen(Mask); I++) {
				BOOL Found = true;
				for (DWORD J = 0; J < strlen(Mask); J++) {
					Found &= Mask[J] == '?' || Pattern[J] == *(CHAR*)(Base + I + J);
				}
				if (Found) { return Base + I; }
			}
			return NULL;
		}
		static DWORD AOBScanByName(LPCSTR lpModuleName, CHAR* Pattern, CHAR* Mask) {
			MODULEINFO ModuleInfo = { NULL };
			HMODULE ModuleName = GetModuleHandleA(lpModuleName);
			if (ModuleName == NULL) {
				return NULL;
			}

			if (!GetModuleInformation(GetCurrentProcess(), ModuleName, &ModuleInfo, sizeof(MODULEINFO))) {
				return NULL;
			}

			DWORD Base = (DWORD)ModuleInfo.lpBaseOfDll;
			DWORD Size = (DWORD)ModuleInfo.SizeOfImage;

			for (DWORD I = 0; I < Size - strlen(Mask); I++) {
				BOOL Found = true;
				for (DWORD J = 0; J < strlen(Mask); J++) {
					Found &= Mask[J] == '?' || Pattern[J] == *(CHAR*)(Base + I + J);
				}
				if (Found) {
					return Base + I;
				}
			}
			return NULL;
		}

	};
}