#include "CHotKey.h"
#include <cstdint> 

namespace CO {
	volatile bool CHotKey::isRunning = false;
	HWND CHotKey::CurrentWindowHandle = nullptr;
	CRITICAL_SECTION CHotKey::SimulationMutex;
	HANDLE CHotKey::SimulationThread = NULL;
	int32_t keyDelay = 100;
	bool CHotKey::keyState = false;

	VOID CHotKey::SimulateSingleKeyPress(WORD key) {
		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = key;
		input.ki.dwFlags = 0;
		input.ki.dwExtraInfo = 0xDEADBEEF;
		SendInput(1, &input, sizeof(INPUT));
	    input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));
	}

	DWORD WINAPI CHotKey::KeyPressTimer(LPVOID lpParam) {
		int32_t DelayMilliseconds = *reinterpret_cast<int32_t*>(lpParam);
		while (isRunning) {
			if (GetForegroundWindow() == CurrentWindowHandle && keyState) {
				SimulateSingleKeyPress(F10_KEY);
				Sleep(keyDelay);
			}
			Sleep(1);
		}
		return 0;
	}

	VOID CHotKey::StartKeyPressSimulation(int32_t DelayMilliseconds) {
		EnterCriticalSection(&SimulationMutex);
		if (isRunning) {
			LeaveCriticalSection(&SimulationMutex);
			return;
		}

		CurrentWindowHandle = GetForegroundWindow();
		if (!CurrentWindowHandle) {
			LeaveCriticalSection(&SimulationMutex);
			return;
		}

		isRunning = true;
		keyState = true;
		int32_t* delayCopy = new int32_t(DelayMilliseconds);
		SimulationThread = CreateThread(NULL, 0, KeyPressTimer, delayCopy, 0, NULL);
		if (SimulationThread) {
			SetThreadPriority(SimulationThread, THREAD_PRIORITY_BELOW_NORMAL);
		}

		LeaveCriticalSection(&SimulationMutex);
	}

	VOID CHotKey::StopKeyPressSimulation() {
		EnterCriticalSection(&SimulationMutex);
		if (!isRunning) {
			LeaveCriticalSection(&SimulationMutex);
			return;
		}
		isRunning = false;
		LeaveCriticalSection(&SimulationMutex);

		if (SimulationThread) {
			WaitForSingleObject(SimulationThread, INFINITE);
			CloseHandle(SimulationThread);
			SimulationThread = NULL;
		}

		CurrentWindowHandle = nullptr;
	}

	VOID CHotKey::Initialize() {
		InitializeCriticalSection(&SimulationMutex);
	}

	VOID CHotKey::Cleanup() {
		DeleteCriticalSection(&SimulationMutex);
	}
}
