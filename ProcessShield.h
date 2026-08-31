#pragma once
#include "Common.h"
namespace CShield {
	class ProcessShield
	{
	public:
		static VOID WINAPIV OnProcess();
		static BOOL GetWindowTitle(DWORD PID, CHAR Title[]);
	};
}