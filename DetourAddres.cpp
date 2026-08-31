#include "DetourAddres.h"
#include "TitleProtection.h"
#include "CheatEngine.h"
#include "Constants.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace CO {

	DWORD DetourAddres::RoleScale = 0;
	DWORD DetourAddres::SubTqScale = 0;
	DWORD DetourAddres::pTempScale = 0;
	DWORD DetourAddres::pCPlayerThread = 0;
	DWORD DetourAddres::pFps = 0;
	int DetourAddres::FLSValue = 0;
	VOID DetourAddres::SetRoleScale(int16_t Size) {
		float __Size = (Size > 0 && Size <= 200) ? static_cast<float>(Size) / 100.0f : 0.75f;
		API::CMemory::WriteFloatMemory(reinterpret_cast<LPVOID>(RoleScale), __Size);
	}

	VOID _fastcall DetourAddres::ScaleDetour(DWORD _pThis, VOID* EDX, int32_t _pIn) {
		pTempScale = _pThis;
		reinterpret_cast<VOID(__thiscall*)(DWORD, int32_t)>(SubTqScale)(_pThis, _pIn);
	}

	DWORD StaticAddres = 0;
	int WINAPI DetourAddres::SetFps(int Value)
	{
		auto value = (1000 / FLSValue);
		DWORD j_timeGetTime = timeGetTime();
		if (j_timeGetTime >= StaticAddres + value)
		{
			StaticAddres = j_timeGetTime;
			typedef int(__thiscall* CallFps)(int*, int a2);

			auto Address = 0x95807C;
			DWORD hAddress = 0x0;
			hAddress = *(int*)Address + 0x40;

			((CallFps)0x5AFD4C)((int*)hAddress, 1);

			typedef int(__thiscall* CallFps2)(VOID);
			((CallFps2)0x5AA327)();
		}
		else
		{
			Sleep(StaticAddres - j_timeGetTime + value);
		}
		return 1;
	}
	int IntervalThread1 = 0;
	int IntervalThread2 = 0;
	int IntervalThread3 = 0;
	HANDLE hThread_CheatEngine = NULL;
	HANDLE hThread_TitleProtection = NULL;
	bool IsThreadRunning(HANDLE hThread)
	{
		if (hThread == NULL)
		{
			return false;
		}
		DWORD exitCode = 0;
		if (GetExitCodeThread(hThread, &exitCode))
			return (exitCode == STILL_ACTIVE);
		return false;
	}
	int __fastcall DetourAddres::CPlayerThread(int Addr, int a0)
	{
		static bool isCheatChecker = false;
		static bool isInitialized = false;

		if (!isInitialized)
		{
			isCheatChecker = (API::CFunctions::CreateSingleInstanceMutex() != NULL);
			isInitialized = true;
		}

		if (isCheatChecker)
		{
			if (timeGetTime() - IntervalThread1 > 60000)
			{
				if (!IsThreadRunning(hThread_CheatEngine))
					hThread_CheatEngine = CreateThread(NULL, 0, CheatEngine::OnProcess, NULL, 0, NULL);

				IntervalThread1 = timeGetTime();
			}

			if (timeGetTime() - IntervalThread2 > 3000)
			{
				if (!IsThreadRunning(hThread_TitleProtection))
					hThread_TitleProtection = CreateThread(NULL, 0, TitleProtection::CheckWindowTitles, NULL, 0, NULL);
				IntervalThread2 = timeGetTime();
			}
		}
		return reinterpret_cast<int(__thiscall*)(DWORD)>(pCPlayerThread)(Addr);
	}


	typedef int(__thiscall* ShiftClickFunc)(DWORD);
	ShiftClickFunc pShiftClick = (ShiftClickFunc)0x480;
	int __fastcall DetourAddres::ShiftClick(int Addr, int a0)
	{
		int ID = 0;
		HWND hwnd = nullptr;

		__asm {
			mov ecx, Addr
			call dword ptr[pShiftClick]
			mov ID, eax
		}
		if (!(GetKeyState(VK_MENU) & 0x8000)) // VK_MENU alt // VK_CONTROL ctrl // VK_SHIFT Shift
			return ID;
		auto GetWnd = [](int code) -> HWND {
			typedef int(WINAPIV* GetBaseFunc)(int);
			GetBaseFunc fn = (GetBaseFunc)0x597FC3;
			int base = fn(code);
			HWND* hwndPtr = (HWND*)(base + 0x20);
			return *hwndPtr;
			};

		auto Click = [](HWND parent, int controlId) {
			HWND h = GetDlgItem(parent, controlId);
			PostMessageA(h, WM_LBUTTONDOWN, 2, 0);
			PostMessageA(h, WM_LBUTTONUP, 2, 0);
			};

		//WareHouse
		Click(GetWnd(161), 1625);
		////Improve
		//Click(GetWnd(211), 1628);
		//Click(GetWnd(211), 1629);
		////Trade
		//Click(GetWnd(137), 1141);
		////Sell Item
		//Click(GetWnd(152), 1552);
		////Sash Packet
		//// Click(GetWnd(689), 5521);
		////VIP WareHouse
		//Click(GetWnd(341), 1625);
		////Sash Item
		//Click(GetWnd(277), 1723); 
		////Compose Items
		//Click(GetWnd(335), 1536);
		//Click(GetWnd(335), 1537);
		////Socket A Gem
		//Click(GetWnd(229), 1688);
		//Click(GetWnd(229), 1689);
		//Click(GetWnd(229), 1690);
		////Equipment Socket
		//Click(GetWnd(425), 1669);
		//Click(GetWnd(425), 1670);
		//Click(GetWnd(425), 1671);
		////Enchant
		//Click(GetWnd(105), 1444);
		//Click(GetWnd(105), 1445);
		////Heaven Blessing Attribute
		//Click(GetWnd(426), 2749);
		//Click(GetWnd(426), 2757);
		////Tortoise Gem Composing
		//Click(GetWnd(438), 2787);
		//Click(GetWnd(438), 2788);
		//Click(GetWnd(438), 2789);
		//Click(GetWnd(438), 2790);
		//Click(GetWnd(438), 2791);
		//Click(GetWnd(438), 2792);
		////Degrade Equipment
		//Click(GetWnd(506), 1669);
		//Click(GetWnd(506), 1670);

		return ID;
	}
	VOID DetourAddres::Hook() {



		RoleScale = 0x00E90C;
		SubTqScale = 0x0061ED;
		pFps = 0xAB53;
		pCPlayerThread = 0x6DFC;
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//DetourAttach(&(LPVOID&)SubTqScale, ScaleDetour);
		//DetourAttach(&(PVOID&)pFps, SetFps);
		//DetourAttach(&(PVOID&)pCPlayerThread, CPlayerThread);
		//DetourAttach(&(PVOID&)pShiftClick, ShiftClick);

		if (DetourTransactionCommit() != NO_ERROR) {
			exit(0);
		}
	}
}