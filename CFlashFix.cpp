#include "CFlashFix.h"
#pragma comment(lib, "detours.lib")

namespace CO {
    PVOID SubLoadLibraryExW = NULL;
    typedef HMODULE(WINAPI* pLoadLibraryExw)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);

    HMODULE WINAPI CFlashFix::LoadLibraryDetour(_In_ LPCWSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags) {
        if (wcsstr(lpLibFileName, L"Flash") && wcsstr(lpLibFileName, L"ocx")) {
            lpLibFileName = L"./Flash.ocx";
        }
        return ((pLoadLibraryExw)SubLoadLibraryExW)(lpLibFileName, hFile, dwFlags);
    }

    VOID CFlashFix::Hook() {
        SubLoadLibraryExW = DetourFindFunction("kernelbase.dll", "LoadLibraryExW");
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(LPVOID&)SubLoadLibraryExW, LoadLibraryDetour);
        if (DetourTransactionCommit() != NO_ERROR) {
            exit(0);
            return;
        }
    }
}