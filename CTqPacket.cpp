#include "Owner.h"
#include "CTqPacket.h"
#include "CMsgHandler.h"

namespace CO {

	DWORD CTqPacket::SubTqRecv = 0;
	DWORD CTqPacket::SubTqSend = 0;
	int32_t CTqPacket::pGameSocket = 0;

	int32_t __fastcall CTqPacket::TqSendDetour(int32_t __pThis, VOID* __Ecx, uint8_t* __PacketBuffer, int32_t __Length) {
		pGameSocket = __pThis;
		return CMsg::CMsgHandler::SendProcess(__pThis, __PacketBuffer, __Length);
	}

	int32_t __fastcall CTqPacket::TqRecvDetour(int32_t __pThis, VOID* __Ecx, uint8_t* __PacketBuffer, int32_t __Length) {
		return CMsg::CMsgHandler::RecvProcess(__pThis, __PacketBuffer, __Length);
	}

	VOID CTqPacket::Hook() {

		SubTqRecv = 0x536A3A;//done
		SubTqSend = 0x536831;//done

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(LPVOID&)SubTqSend, TqSendDetour);
		if (DetourTransactionCommit() != NO_ERROR) {
			exit(0);
			return;
		}

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(LPVOID&)SubTqRecv, TqRecvDetour);
		if (DetourTransactionCommit() != NO_ERROR) {
			exit(0);
			return;
		}
	}
}