#ifndef CHOTKEY_H
#define CHOTKEY_H
#include "OwnerState.h"
#include <cstdint> 
namespace CO {
	class CHotKey {
	public:
		static VOID StartKeyPressSimulation(int32_t DelayMilliseconds);
		static VOID StopKeyPressSimulation();
		static VOID Initialize();
		static VOID Cleanup();
		static bool keyState;

	private:
		static volatile bool isRunning;
		static HWND CurrentWindowHandle;
		static CRITICAL_SECTION SimulationMutex;
		static HANDLE SimulationThread;

		static DWORD WINAPI KeyPressTimer(LPVOID lpParam);
		static VOID SimulateSingleKeyPress(WORD key);

		static const WORD F10_KEY = VK_F10;
	};
}

#endif
