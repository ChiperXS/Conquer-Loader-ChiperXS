#pragma once
#include "Common.h"

namespace Crypt {
	class CipherPacket
	{
	public:
	    static VOID GameEncrypt(uint8_t* pBuf, int16_t Length);
		static VOID GameDecrypt(uint8_t* pBuf, int16_t Length);
	};
}

