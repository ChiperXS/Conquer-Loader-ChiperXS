#pragma once
#include "Common.h"

namespace CO {
	class CServerDat
	{
	public:
		static VOID Hook();

	private:
		static VOID DatDecrypt(CHAR* Out, int32_t Size);
		static DWORD* WINAPIV LoadFileDetour(CHAR* FileName, CHAR* Mode, int32_t FileHandle);


	};
}