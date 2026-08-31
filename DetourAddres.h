#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include "Common.h"
#include "OwnerState.h"

namespace CO {
	class DetourAddres
	{
	public:
		static VOID SetRoleScale(int16_t Size);
		static int WINAPI SetFps(int Value);
		static int __fastcall CPlayerThread(int Addr, int a0);
		static int __fastcall ShiftClick(int Addr, int a0);

		static std::string g_iniFile1Path;
		static std::string g_iniFile2Path;

		static DWORD RoleScale, ScreanFrame, SubTqScale, pTempScale, pFps, pCPlayerThread;
		static int FLSValue;
	public:
		static VOID Hook();
		static VOID _fastcall ScaleDetour(DWORD _pThis, VOID* EDX, int32_t _pIn);
	};
}