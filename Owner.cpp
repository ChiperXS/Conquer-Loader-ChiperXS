#include "Owner.h"
#include "RegKey.h"
#include "Memory.h"
#include "CFlashFix.h"
#include "Constants.h"
#include "CSnprintf.h"
#include "CShellApi.h"
#include "CTqPacket.h"
#include "CLdLibrary.h"
#include "CServerDat.h"
#include "CWinSocket.h"
#include "OwnerState.h"
#include "AutoHuntGUI.h"

#include "CFileEditor.h"
#include <wininet.h>
#include "ProtectionThread.h"
#include "CMoveWindow.h"
#include "DetourAddres.h"
#include <tlhelp32.h>
#include <string>
#include <set>
#include <algorithm>
#include <psapi.h>
#include <iostream>
#include <sstream>
#include <vector>
#pragma comment(lib, "psapi.lib")

//HPBAR
CONST LPVOID HP_BAR_ADDR = (LPVOID)(0x0067028D);
//PMCOMMAND
constexpr auto PM_COMMAND_ADDRESS = 0x00514C9A;
//fastLogin
LPVOID fastLogin = (LPVOID)0x005B7AB7;

char PotencyFormat[] = { "d" };

Owner* Owner::sInstance = nullptr;
Owner& Owner::GetInstance() {
	static volatile LONG Protect = 0;
	if (sInstance == nullptr) {
		if (1 == InterlockedIncrement(&Protect)) {
			sInstance = new Owner();
		}
		else {
			while (sInstance == nullptr) {
				Sleep(0);
			}
		}
	}
	return *sInstance;
}

int TrackSpeedValue = 0;
int Speed_Value_POINTER = 0;
int Speed_Value_OLD = 0;
int Speed_Value_NEW = 0;
int CALL_Adress_PATH, OutSafePatchFinding, StopPatchFindingAddr;

void Owner::OnLogin()
{
	TrackSpeedValue = NULL;

}
void __stdcall FuncSpeedCheck()
	{
		auto XOR_Speed_Value_New = Speed_Value_NEW ^ 0x51;
		auto XOR_Speed_Value_OLD = Speed_Value_OLD ^ 0x51;

		DWORD HOOLD = 0x955F8C;

		if (Speed_Value_POINTER == *(int*)HOOLD)
		{
			if (TrackSpeedValue != 0 && TrackSpeedValue != XOR_Speed_Value_OLD)
			{
				OwnerState::Detected(SpeedCClient, "Detected Func");
			}
			TrackSpeedValue = XOR_Speed_Value_New;
		}
	}
void __stdcall FuncSpeedCheckOnFly()
	{

		DWORD HOOLD = 0x955F8C;

		if (Speed_Value_POINTER == *(int*)HOOLD)
		{
			Speed_Value_OLD /= 2;
			Speed_Value_OLD /= 2;
			auto XOR_Speed_Value_OLD = Speed_Value_OLD ^ 0x51;
			TrackSpeedValue = XOR_Speed_Value_OLD;
		}
	}
__declspec(naked) void __stdcall OwnerSpeed()
	{
		__asm
		{
			pushad

			mov Speed_Value_POINTER, esi
			mov edi, [esi + 0x1B8]
			mov Speed_Value_OLD, edi
			mov Speed_Value_NEW, eax

			call FuncSpeedCheck
			popad
			mov[esi + 0x1B8], eax
			ret
		}
	}
__declspec(naked) void __stdcall OwnerSpeedOnFly()
{
	__asm
	{
		pushad
		mov Speed_Value_POINTER, esi
		mov edi, [esi + 0x1B8]
		mov Speed_Value_OLD, edi
		call FuncSpeedCheckOnFly
		popad
		shr byte ptr[esi + 0x1B8], 0x2
		ret
	}
}

DWORD RetJumpAdress;
typedef int(__thiscall* dword1adress)(DWORD);
dword1adress  callAdress;
int helperhp;

__declspec(naked) void __stdcall FixJump()
{
	__asm
	{
		cmp     dword ptr[edi + 0x88], 0xF423F
		jbe label2
		push    ecx
		push    eax
		mov     ecx, edi
		call    callAdress
		mov     helperhp, eax
		cmp     helperhp, 0
		jle label1
		mov     eax, helperhp
		add     eax, 0x26
		mov    helperhp, eax

		label1 :
		mov     ecx, helperhp
			pop     eax
			sub     eax, ecx
			pop     ecx

			label2 :
		mov     dword ptr[ebp - 0x28], ecx
			mov     dword ptr[ebp - 0x14], eax
			mov     dword ptr[ebp - 0x1C], eax
			add     eax, 4
			jmp     RetJumpAdress
	}
}

VOID CheckEnvironmentFolder() {
	CHAR szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	string FilePath(szFileName);
	size_t LastSlashidx = FilePath.rfind('\\');
	if (LastSlashidx != string::npos) {
		string directoryPath = FilePath.substr(0, LastSlashidx);
		string searchPath = directoryPath + "\\*.exe";
		WIN32_FIND_DATAA findData;
		HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
		if (hFind != INVALID_HANDLE_VALUE) {
			set<string> allowedExeNames = {
				"play.exe",
				"Play4.0.exe",
				"conquer.exe",
				"5-removerrosasss.exe",
				"lagfix.exe",
				"timedelay.exe",
				"updaterplay.exe"
			};

			do {
				string exeName = findData.cFileName;
				string lowerExeName = exeName;
				transform(lowerExeName.begin(), lowerExeName.end(), lowerExeName.begin(), ::tolower);

				if (allowedExeNames.find(lowerExeName) == allowedExeNames.end()) {
					string fullExePath = directoryPath + "\\" + exeName;

					bool processTerminated = false;
					HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
					PROCESSENTRY32 pEntry;
					pEntry.dwSize = sizeof(pEntry);
					BOOL hRes = Process32First(hSnapShot, &pEntry);
					while (hRes) {
						char szAnsiExeFile[MAX_PATH];
						WideCharToMultiByte(CP_ACP, 0, pEntry.szExeFile, -1, szAnsiExeFile, MAX_PATH, NULL, NULL);
						string processName = szAnsiExeFile;
						string lowerProcessName = processName;
						transform(lowerProcessName.begin(), lowerProcessName.end(), lowerProcessName.begin(), ::tolower);

						if (lowerProcessName == lowerExeName) {
							HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pEntry.th32ProcessID);
							if (hProcess != NULL) {
								if (TerminateProcess(hProcess, 9)) {
									processTerminated = true;
								}
								CloseHandle(hProcess);
							}
						}
						hRes = Process32Next(hSnapShot, &pEntry);
					}
					CloseHandle(hSnapShot);

					if (processTerminated) {
						Sleep(500);
					}

					SetFileAttributesA(fullExePath.c_str(), FILE_ATTRIBUTE_NORMAL);
					for (int i = 0; i < 3; i++) {
						if (DeleteFileA(fullExePath.c_str())) {
							break;
						}
						Sleep(500);
					}
				}
			} while (FindNextFileA(hFind, &findData));
			FindClose(hFind);
		}
	}
}
VOID OwnerInit(HMODULE hModule){
	Owner& Owner = Owner::GetInstance();
}

void Trim(std::string& str) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));

	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), str.end());
}

int GetConfigValue(const char* section, const char* key, int defaultValue, const char* iniFile) {
	char value[256] = { 0 };
	GetPrivateProfileStringA(section, key, "", value, sizeof(value), iniFile);

	std::string strValue(value);
	Trim(strValue);
	try {
		return strValue.empty() ? defaultValue : std::stoi(strValue);
	}
	catch (...) {
		return defaultValue;
	}
}
DWORD FindStringAddress(const char* search)
{
	MODULEINFO modInfo = {};
	HMODULE hMod = GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), hMod, &modInfo, sizeof(MODULEINFO));
	DWORD base = (DWORD)modInfo.lpBaseOfDll;
	DWORD size = (DWORD)modInfo.SizeOfImage;
	for (DWORD i = base; i < base + size - strlen(search); ++i)
	{
		if (memcmp((void*)i, search, strlen(search)) == 0)
			return i;
	}
	return 0;
}

void InitializeSpeedHackAddresses()
{
	HMODULE hModSpeedCheat = GetModuleHandleW(L"Kernel32.dll");
	HMODULE hModSpeedOther = GetModuleHandleW(L"WINMM.dll");

	if (hModSpeedCheat)
		API::Constants::Adress_SpeedCheat = GetProcAddress(hModSpeedCheat, "GetTickCount");

	if (hModSpeedOther)
		API::Constants::Adress_SpeedOther = GetProcAddress(hModSpeedOther, "timeGetTime");

	if (API::Constants::Adress_SpeedCheat)
		memcpy(API::Constants::pBytesSpeedCheat, API::Constants::Adress_SpeedCheat, 8);

	if (API::Constants::Adress_SpeedOther)
		memcpy(API::Constants::pBytesSpeedOther, API::Constants::Adress_SpeedOther, 8);
}
bool GetModuleSize(HANDLE hProcess, LPVOID ImageBase, DWORD& Size)
{
	bool bFound = false;
	MEMORY_BASIC_INFORMATION mbi;
	BYTE* QueryAddress = (BYTE*)ImageBase;
	while (!bFound)
	{
		if (VirtualQueryEx(hProcess, QueryAddress, &mbi, sizeof(mbi)) != sizeof(mbi))
			break;

		if (mbi.AllocationBase != ImageBase)
		{
			// Found, calculate the module size
			Size = QueryAddress - (BYTE*)ImageBase;
			bFound = true;
			break;
		}
		QueryAddress += mbi.RegionSize;
	}
	// Complete
	return bFound;
}

LPVOID FindMemoryPattern(PBYTE pattern, bool* wildCards, int len)
{
	HANDLE mod = GetModuleHandle(NULL);
	DWORD size;
	if (!GetModuleSize(GetCurrentProcess(), mod, size))
		return NULL;
	BYTE* buffer = new BYTE[size];
	ReadProcessMemory(GetCurrentProcess(), mod, buffer, size, NULL);
	LPVOID found = NULL;
	for (DWORD i = 0; i < size; i++)
	{
		bool match = true;
		for (int j = 0; j < len; j++)
		{
			if ((buffer[i + j] != pattern[j]) && !wildCards[j])
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			found = (PBYTE)mod + i;
			break;
		}
	}
	delete[] buffer;
	return found;
}



Owner::Owner() {

	int16_t FlashFix = GetConfigValue("Start", "FixWhiteScreen", 0, ".\\Settings.ini");
	API::Constants::DiscordStatus = GetConfigValue("DiscordStatus", "Value", 0, ".\\Settings.ini");
	int16_t HpBar = GetConfigValue("Player", "HPBar", 0, ".\\Settings.ini");

	//int16_t GmCmmand = GetConfigValue("GM", "Value", 0, ".\\Settings.ini");

	CheckEnvironmentFolder();
	RegKey::InitializeMutex();
	RegKey::Read();
	CO::CFileEditor::GameFullScreanClassic();
	CO::DetourAddres::Hook();
	CO::CSnprintf::Hook();
	CO::CShellApi::Hook();
	CO::CTqPacket::Hook();
	CO::CLdLibrary::Hook();
	CO::CWinSocket::Hook();
	CO::CMoveWindow::Hook();
	InitializeSpeedHackAddresses();

	CShield::ProtectionThread::VMwareDetection();

	OwnerState::pThread = new ThreadBool();
	OwnerState::pThread->ThreadCreation();

	if (FlashFix != 0) {
		CO::CFlashFix::Hook();
	}

	/*if (GmCmmand != 0) {
		uint8_t skipPmCheck_5165[] = { 0x90, 0x90 };

		API::CMemory::WriteSafe(
			reinterpret_cast<LPVOID>(PM_COMMAND_ADDRESS),
			skipPmCheck_5165,
			sizeof(skipPmCheck_5165)
		);
	}*/
	LPVOID SkipMore = (LPVOID)0x004DC3C1;
	unsigned char patch[] = { 0x33, 0xC0, 0x90, 0x90, 0x90, 0x90 };
	API::CMemory::WriteSafe(SkipMore, patch, sizeof(patch));

	BYTE donationPatch[] = { 0xEB, 0x39 };
	API::CMemory::WriteSafe(reinterpret_cast<LPVOID>(0x00422C57), donationPatch, sizeof(donationPatch));

	
	BYTE loginlater[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	API::CMemory::WriteSafe((LPVOID)0x0044B07E, loginlater, sizeof(loginlater));


	BYTE FastLogin[] = { 0xE9, 0x80, 0x02, 0x00, 0x00, 0x90 ,0x8B, 0x4C, 0x24 };
	API::CMemory::WriteSafe(fastLogin, FastLogin, 9);


	//Screen 30
	API::CMemory::WriteUInt8Memory((VOID*)(0x004E8079 + 2), 30);
	API::CMemory::WriteUInt8Memory((VOID*)(0x004E8065 + 2), 30);

	//jump

	API::CMemory::WriteUInt8Memory((void*)(0x0051485E + 2), 30);
	API::CMemory::WriteUInt8Memory((void*)(0x00514855 + 2), 30);

	//fps
	API::CMemory::WriteUInt8Memory((void*)(0x004B9F07 + 2), 4);
	API::CMemory::WriteUInt8Memory((void*)(0x004B9F10 + 2), 4);

	//Arrow
	API::CMemory::WriteUInt32Memory((LPVOID)(0x0051B03A + 3), (((UINT16)OwnerState::Width - 1024) / 2 + 230));//design

	//CleanBattlePower
	API::CMemory::WriteUInt32Memory((LPVOID)(0x0047FCE2 + 1), (UINT32)&PotencyFormat);//my self
	API::CMemory::WriteUInt32Memory((LPVOID)(0x0046D004 + 1), (UINT32)&PotencyFormat);//others

	// ClientRes
	API::CMemory::WriteUInt16Memory((void*)(0x004BC096 + 6), (UINT16)OwnerState::Width);
	API::CMemory::WriteUInt16Memory((void*)(0x004BC0A3 + 6), (UINT16)OwnerState::Height);
	// PuzzleRes
	BYTE pattern[] = { 0xB9,0x00,0x00,0x00,0x00,0x89,0x45,0xC8,0x89,0x7D,0xE8,0xE8,0x00,0x00,0x00,0x00,0x0F,0xAF,0xC7,0x99,0xF7,0x3D,0x00,0x00,0x00,0x00,0xB9,0x00,0x00,0x00,0x00,0x89,0x45,0xFC,0x8B,0xC1,0x99,0xF7,0x7D,0xFC,0x8B,0xF0 };
	bool wildcards[] = { 0x0,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x0,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
	PBYTE match = (PBYTE)FindMemoryPattern(pattern, wildcards, sizeof(pattern));

	match += 26;

	API::CMemory::WriteUInt16Memory((void*)(match + 1), (UINT16)OwnerState::Width);
	match += 23;
	API::CMemory::WriteUInt16Memory((void*)(match + 1), (UINT16)OwnerState::Height);


	if (HpBar >= 1) {
		API::CMemory::WriteUInt32Memory((LPVOID)0x0060C6B2, 45178139);//hpbar
		API::CMemory::WriteUInt32Memory((LPVOID)(0x005277E4 + 6), 2000000);//hpbar
		API::CMemory::WriteUInt8Memory((void*)(0x00527A77 + 1), 0x4D);//Name Y //4D 
		API::CMemory::WriteUInt8Memory((void*)(0x00528465 + 1), 0x52);//Guild Y //52
		API::CMemory::WriteUInt8Memory((void*)(0x00528493 + 1), 0x52);//Guild Y Jump //52
		API::CMemory::WriteUInt8Memory((void*)(0x0052781D + 1), 0x51);//HP Bar Y //7F
		API::CMemory::WriteUInt8Memory((void*)(0x00527AA5 + 1), 0x51);//HP Bar Y Jump

		callAdress = (dword1adress)0x5114B7;
		RetJumpAdress = 0x527845 + 12;

		DWORD curProtection;
		VirtualProtect((void*)0x527845, 12, PAGE_EXECUTE_READWRITE, &curProtection);
		memset((void*)0x527845, 0x90, 12);
		DWORD relativeAddress = ((DWORD)FixJump - (DWORD)(void*)0x527845) - 5;
		*(BYTE*)(void*)0x527845 = 0xE9;
		*(DWORD*)((DWORD)(void*)0x527845 + 1) = relativeAddress;
		DWORD temp;
		VirtualProtect((void*)0x527845, 12, curProtection, &temp);
	}
}