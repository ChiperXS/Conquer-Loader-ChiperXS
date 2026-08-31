#pragma once
#include "Common.h"
#include "CTqPacket.h"
#include "ThreadBool.h"
#include "CipherPacket.h"
#include "Constants.h"
#include <unordered_map>
#include <windows.h>
#include <string>
#include <vector>

class OwnerState
{
public:
	static BOOL InGame, FileScan;
	static CHAR* FaceID;
	static CHAR Password[32];
	static CHAR PlayerName[16];
	static CHAR* DiscordTitle1;
	static CHAR* DiscordTitle2;
	static int32_t Identifier, Width, Height;
	static int16_t FrameDelayMs;
	static int16_t ClientVersion;
public:
	static ThreadBool* pThread;
	static VOID Disconnect();
	static HANDLE hLoginProtectionThread;
	static DWORD dwLoginProtectionThreadId;
	static VOID CALLBACK _DoLoginProtection();
	static void CheckAndRestartLoginProtection();

	static VOID Detected(int32_t Reason, CHAR* Str);
	static VOID SendMsg(VOID* __pBuffer);
	static VOID MapScale(int32_t _pIn);

};
