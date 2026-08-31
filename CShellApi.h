#pragma once
#include "Common.h"

namespace CO {
	class CShellApi
	{
	public:
		static VOID Hook();
	private:
		static int32_t CALLBACK ShellexecDetour(HWND hWnd, CHAR* lpOperation, CHAR* lpFile, CHAR* lpParams, CHAR* lpDir, int32_t nCmd);
	};
}