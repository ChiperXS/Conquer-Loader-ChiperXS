#pragma once
#include "Common.h"
#include "OwnerState.h"

namespace CO {
	class CWinSocket
	{
	public:
		static VOID Hook();	
	private:
		static int32_t CALLBACK _CloseSocketDetour(SOCKET Socket);
	};
}