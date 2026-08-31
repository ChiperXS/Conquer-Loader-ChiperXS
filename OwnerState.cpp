#include <unordered_map>
#include "DetourAddres.h"
#include "Discord.h"
#include "CMsgHandler.h"
#include "OwnerState.h"
#include "Logger.h"

using namespace LO;

ThreadBool* OwnerState::pThread = NULL;
CHAR* OwnerState::FaceID = NULL;
BOOL OwnerState::InGame = false;
BOOL OwnerState::FileScan = false;
CHAR* OwnerState::DiscordTitle1 = NULL;
CHAR* OwnerState::DiscordTitle2 = NULL;
int32_t OwnerState::Identifier = 0;
int16_t OwnerState::ClientVersion = 0;
CHAR OwnerState::Password[32] = { 0 };
CHAR OwnerState::PlayerName[16] = { 0 };
int16_t OwnerState::FrameDelayMs = 0;
int OwnerState::Width = 1024;
int OwnerState::Height = 768;

VOID OwnerState::SendMsg(VOID* __pBuffer) {
	int16_t __Size = *((int16_t*)__pBuffer);
	auto __PacketBuffer = reinterpret_cast<uint8_t*>(__pBuffer);
	if (InGame) {
		if (CO::CTqPacket::pGameSocket != NULL) {
			if (__Size > 0 && __Size < 1024) {
				Crypt::CipherPacket::GameEncrypt(__PacketBuffer, __Size);
				reinterpret_cast<VOID(__thiscall*)(int32_t, uint8_t*, int32_t)>(CO::CTqPacket::SubTqSend)(
					CO::CTqPacket::pGameSocket, __PacketBuffer, __Size);
				delete[] __PacketBuffer;
				free(__pBuffer);
			}
		}
		else {
			API::CFunctions::ExitDetection("Error in Send GameSocket = null");
		}
	}
	else {
		API::CFunctions::ExitDetection("Error in Send GameServer = null");
	}
}
VOID OwnerState::MapScale(int32_t _pIn) {
	if (_pIn >= 120 && _pIn <= 512)
	{
		reinterpret_cast<VOID(__thiscall*)(DWORD, int32_t)>(CO::DetourAddres::SubTqScale)(CO::DetourAddres::pTempScale, _pIn);
	}
	else
	{
		_pIn = 256;
		reinterpret_cast<VOID(__thiscall*)(DWORD, int32_t)>(CO::DetourAddres::SubTqScale)(CO::DetourAddres::pTempScale, _pIn);
	}
}
VOID OwnerState::Disconnect() {
	FaceID = 0;
	Identifier = 0;
	InGame = false;
	FileScan = false;
	OwnerState::DiscordTitle1 = NULL;
	OwnerState::DiscordTitle2 = NULL;
	memset(Password, 0, 32);
	memset(PlayerName, 0, 16);
	Discord::Shutdown();
	CMsg::CMsgHandler::Cleanup();
}

DWORD OwnerState::dwLoginProtectionThreadId = 0;
HANDLE OwnerState::hLoginProtectionThread = NULL;

VOID CALLBACK OwnerState::_DoLoginProtection() {
	pThread = new ThreadBool();
	pThread->Protection();
}

void OwnerState::CheckAndRestartLoginProtection() {
	if (hLoginProtectionThread == NULL) { 
		hLoginProtectionThread = CreateThread( nullptr, 0, LPTHREAD_START_ROUTINE(OwnerState::_DoLoginProtection), nullptr, 0, &dwLoginProtectionThreadId);
	}
}
VOID OwnerState::Detected(int32_t Reason, CHAR* Str) {
	static std::tr1::unordered_map<int16_t, std::string> ReasonNames;
	if (ReasonNames.empty()) {
		ReasonNames.insert(std::make_pair(0, "None"));
		ReasonNames.insert(std::make_pair(2, "Program"));
		ReasonNames.insert(std::make_pair(4, "AutoClicker"));
		ReasonNames.insert(std::make_pair(6, "AutoKeyboard"));
		ReasonNames.insert(std::make_pair(8, "SpeedClient"));
		ReasonNames.insert(std::make_pair(10, "Debugger"));
		ReasonNames.insert(std::make_pair(12, "InjectDll"));
		ReasonNames.insert(std::make_pair(14, "ClosingThread"));
		ReasonNames.insert(std::make_pair(16, "ChangeConquerFile"));
		ReasonNames.insert(std::make_pair(18, "dbg"));
		ReasonNames.insert(std::make_pair(20, "GamingMouse"));
		ReasonNames.insert(std::make_pair(22, "ExternalMouse"));
	}

	CMsgCheat* CPacket = new CMsgCheat();

	if (Str && Str[0] != '\0') {
		size_t len = strlen(Str);
		if (len >= sizeof(CPacket->ReaosnString)) {
			len = sizeof(CPacket->ReaosnString) - 1;
		}

		memcpy(CPacket->ReaosnString, Str, len);
		CPacket->ReaosnString[len] = '\0';
		CPacket->CheatFlags = Reason;
		CPacket->lReaosnString = static_cast<int32_t>(len); 
	}
	else {
		CPacket->CheatFlags = Reason;
		CPacket->lReaosnString = 0;
		CPacket->ReaosnString[0] = '\0';
	}

	std::string reasonName = "Unknown";
	auto it = ReasonNames.find(static_cast<int16_t>(Reason));
	if (it != ReasonNames.end()) {
		reasonName = it->second;
	}

	printf("[DETECTED] Reason: %d (%s), Text: %s, Length: %d\n",
		Reason, reasonName.c_str(), CPacket->ReaosnString, CPacket->lReaosnString);

	if (OwnerState::InGame) {
		OwnerState::SendMsg(reinterpret_cast<uint8_t*>(CPacket));
	}
}
