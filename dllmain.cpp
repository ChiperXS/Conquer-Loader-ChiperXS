#include "Owner.h"
#include "Common.h"
#include "MemoryShield.h"

#ifdef _WIN32
#include <windows.h>
#else
#error "This code is only supported on Windows platforms."
#endif

extern VOID OwnerInit(HMODULE hModule);
bool IsValidHostApplication()
{
	char modulePath[MAX_PATH];
	GetModuleFileNameA(NULL, modulePath, MAX_PATH);

	std::string currentApp(modulePath);
	return currentApp.find("Conquer.exe") != std::string::npos; 
} 

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
       // AllocConsole(); // Allocate a console
       //FILE* fp;
       //freopen_s(&fp, "CONOUT$", "w", stdout); // Redirect stdout to console
       //freopen_s(&fp, "CONIN$", "r", stdin);   // Redirect stdin if needed

        if (!IsValidHostApplication()) {
            return FALSE;
        }
        OwnerInit(hInstance);
        CreateThread(NULL, 0, CShield::MemoryShield::InitialGameCheck, NULL, 0, NULL);
        DisableThreadLibraryCalls(hInstance);
        break;
    }

    case DLL_PROCESS_DETACH: {
        HANDLE hThread = CreateThread(
            nullptr,
            0,
            reinterpret_cast<LPTHREAD_START_ROUTINE>(FreeLibraryAndExitThread),
            hInstance,
            0,
            nullptr
        );

        if (hThread != nullptr) {
            CloseHandle(hThread);
        }

        break;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

