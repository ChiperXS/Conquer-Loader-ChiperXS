#include "CShellApi.h"

namespace CO {

	typedef int32_t(CALLBACK* ShellExecuteA_t)(HWND, CHAR*, CHAR*, CHAR*, CHAR*, int32_t);
	ShellExecuteA_t SubShellApi = NULL;

	int32_t CALLBACK CShellApi::ShellexecDetour(HWND hWnd, CHAR* lpOperation, CHAR* lpFile, CHAR* lpParams, CHAR* lpDir, int32_t nCmd) {
		return SubShellApi(hWnd, lpOperation, lpFile, lpParams, lpDir, nCmd);
	}

	VOID CShellApi::Hook() {
		SubShellApi = (ShellExecuteA_t)DetourFindFunction("Shell32.dll", "ShellExecuteA");
		if (!SubShellApi) {
			MessageBoxA(NULL, "Failed to find ShellExecuteA", "Hook Error", MB_OK | MB_ICONERROR);
			return;
		}

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(reinterpret_cast<PVOID*>(&SubShellApi), ShellexecDetour);

		if (DetourTransactionCommit() != NO_ERROR) {
			MessageBoxA(NULL, "Failed to hook ShellExecuteA", "Hook Error", MB_OK | MB_ICONERROR);
			return;
		}
	}
}
