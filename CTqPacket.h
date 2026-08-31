#pragma once
#include "Common.h"

namespace CO {
	class CTqPacket
	{
	public:
		static VOID Hook();
		static int32_t pGameSocket;
		static DWORD SubTqSend, SubTqRecv;
	private:
		static int32_t __fastcall TqSendDetour(int32_t __pThis, VOID * __Ecx, uint8_t * __PacketBuffer, int32_t __Length);
		static int32_t __fastcall TqRecvDetour(int32_t __pThis, VOID * __Ecx, uint8_t * __PacketBuffer, int32_t __Length);
	};
}