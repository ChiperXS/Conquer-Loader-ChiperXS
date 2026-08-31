#pragma once
#include "PRW.h"
#include "Owner.h"
#include "Common.h"
#include "RegKey.h"
#include "CTqPacket.h"
#include "OwnerState.h"
#include "MsgFileHash.h"
#include "MemoryShield.h"
#include "ProcessShield.h"

namespace CMsg {
	class CMsgHandler
	{
	public:
		static string OwnerVersion() {
			string Temp;
			fstream File(".\\version.dat", fstream::in);
			if (File) {
				getline(File, Temp, '\0');
				File.close();
				return Temp;
			}
			else {
				File.close();
				return "\0";
			}
		}
	public:
		static VOID SendStartStopAction(int16_t actionType);
		static void Cleanup();
		static int32_t SendProcess(int32_t __pThis, uint8_t * __PacketBuffer, int32_t __Length);
		static int32_t RecvProcess(int32_t __pThis, uint8_t * __PacketBuffer, int32_t __Length);
	};
}