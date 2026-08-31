#pragma once
#include "Common.h"

namespace CO {
    class CFlashFix
    {
    public:
        static VOID Hook();
        static HMODULE WINAPI LoadLibraryDetour(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
    };
}
