#include "ProtectionThread.h"
#include "OwnerState.h"
#include <windows.h>
#include <winternl.h>
#include <vector>
#include <iostream>
#include <Psapi.h>
#include <intrin.h>
#include <iphlpapi.h>
#include <TlHelp32.h>

#pragma comment(lib, "iphlpapi.lib")

namespace CShield
{
#ifndef _PEB_DEFINED
	typedef struct _PEB {
		BYTE Reserved1[2];
		BYTE BeingDebugged;
		BYTE Reserved2[1];
		PVOID Reserved3[2];
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID Reserved4[3];
		PVOID AtlThunkSListPtr;
		PVOID Reserved5;
		ULONG Reserved6;
		PVOID Reserved7;
		ULONG Reserved8;
		ULONG AtlThunkSListPtr32;
		PVOID Reserved9[45];
		BYTE Reserved10[96];
		PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
		BYTE Reserved11[128];
		PVOID Reserved12[1];
		ULONG SessionId;
		ULONG NtGlobalFlag;
	} PEB, * PPEB;
#endif

	typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
	typedef NTSTATUS(WINAPI* pNtSetInformationThread)(HANDLE, ULONG, PVOID, ULONG);

	bool IsRunningInVM() {
		int cpuInfo[4] = { -1 };
		__cpuid(cpuInfo, 0);
		char vendor[13];
		memcpy(vendor, &cpuInfo[1], 4);
		memcpy(vendor + 4, &cpuInfo[3], 4);
		memcpy(vendor + 8, &cpuInfo[2], 4);
		vendor[12] = 0;

		std::string vendorStr(vendor);
		if (vendorStr == "VMwareVMware" || vendorStr == "VBoxVBoxVBox") {
			return true;
		}
		return false;
	}

	bool CheckVBoxDevice() {
		HANDLE hDevice = CreateFile(L"\\\\.\\VBoxMiniRdrDN", GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (hDevice != INVALID_HANDLE_VALUE) {
			CloseHandle(hDevice);
			return true;
		}

		return false;
	}

	bool CheckMacAddress() {
		IP_ADAPTER_INFO AdapterInfo[16];
		DWORD bufLen = sizeof(AdapterInfo);
		if (GetAdaptersInfo(AdapterInfo, &bufLen) != ERROR_SUCCESS)
			return false;

		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

		while (pAdapterInfo) {
			BYTE* mac = pAdapterInfo->Address;
			if ((mac[0] == 0x00 && mac[1] == 0x05 && mac[2] == 0x69) ||
				(mac[0] == 0x08 && mac[1] == 0x00 && mac[2] == 0x27)) {
				return true;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}

		return false;
	}

	bool CheckVirtualDrivers() {
		const std::vector<std::wstring> drivers = {
			L"C:\\Windows\\System32\\drivers\\VBoxGuest.sys",
			L"C:\\Windows\\System32\\drivers\\vm3dmp.sys"
		};

		for (const auto& driver : drivers) {
			DWORD attr = GetFileAttributesW(driver.c_str());
			if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
				return true;
			}
		}
		return false;
	}

	bool CheckGPUName() {
		DISPLAY_DEVICE dd;
		dd.cb = sizeof(dd);
		for (int i = 0; EnumDisplayDevices(0, i, &dd, 0); i++) {
			std::wstring gpuName(dd.DeviceString);
			if (gpuName.find(L"VMware") != std::wstring::npos || gpuName.find(L"VirtualBox") != std::wstring::npos) {
				return true;
			}
		}
		return false;
	}

	bool IsDebuggerPresentAPI() {
		return IsDebuggerPresent() != 0;
	}

	void HideThreadFromDebugger() {
		HANDLE hThread = GetCurrentThread();
		pNtSetInformationThread NtSetInformationThread = (pNtSetInformationThread)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetInformationThread");
		if (NtSetInformationThread) {
			NtSetInformationThread(hThread, 0x11, 0, 0);
		}
	}

	bool CheckPEB() {
		PPEB pPeb = (PPEB)__readfsdword(0x30);
		if (pPeb->BeingDebugged) {
			return true;
		}
		if (pPeb->NtGlobalFlag & 0x70) {
			return true;
		}
		return false;
	}

	bool CheckNtQueryInformationProcess() {
		pNtQueryInformationProcess NtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(
			GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");

		if (!NtQueryInformationProcess) return false;

		DWORD DebugPort = 0;
		if (NtQueryInformationProcess(GetCurrentProcess(), 7, &DebugPort, sizeof(DebugPort), NULL) == 0 && DebugPort != 0) {
			return true;
		}
		return false;
	}

	bool CheckRemoteDebugger() {
		BOOL isDebuggerPresent = FALSE;
		CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent);
		return isDebuggerPresent != 0;
	}
	__declspec(naked) void AntiAttach() {
		__asm
		{
			jmp ExitProcess
		}
	}

	VOID ProtectionThread::VMwareDetection() {

		HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
		FARPROC func_DbgUiRemoteBreakin = GetProcAddress(hMod, "DbgUiRemoteBreakin");
		WriteProcessMemory(GetCurrentProcess(), func_DbgUiRemoteBreakin, AntiAttach, 6, NULL);

		if (IsRunningInVM()) {
			API::CFunctions::ExitDetection("Game cannot run inside a Virtual Machine (Detected CPU Vendor ID: VMware or VirtualBox).");
			return;
		}

		if (CheckVBoxDevice()) {
			API::CFunctions::ExitDetection("Game cannot run inside a Virtual Machine (Detected VBox device).");
			return;
		}

		if (CheckMacAddress()) {
			API::CFunctions::ExitDetection("Game cannot run inside a Virtual Machine (Detected Virtual MAC Address).");
			return;
		}

		if (CheckVirtualDrivers()) {
			API::CFunctions::ExitDetection("Game cannot run inside a Virtual Machine (Detected Virtual Machine Driver Files).");
			return;
		}

		if (CheckGPUName()) {
			API::CFunctions::ExitDetection("Game cannot run inside a Virtual Machine (Detected Virtual GPU Device).");
			return;
		}

		HideThreadFromDebugger();
	}
	VOID ProtectionThread::AntiDebugProtection() {
		while (true) {
			if (API::CFunctions::IsGameWindowActive()) {
				if (CheckPEB() || CheckNtQueryInformationProcess() || CheckRemoteDebugger() ||
					IsDebuggerPresentAPI()) {
					API::CFunctions::ExitDetection("Debugger or Hacking Tool Detected");
				}
			}
			Sleep(5000);
		}
	}

}