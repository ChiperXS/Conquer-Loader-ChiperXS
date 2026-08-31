#pragma once
#include "Common.h"
#include "CMsgHandler.h"
#include "CHotKey.h"
#include <windows.h>
#include <queue>
#include <iostream>

namespace CShield {
	class MouseKeybord {
	public:
		static VOID WINAPIV OnProcess();
		static VOID WINAPIV ResetHooks();
		static LRESULT CALLBACK MouseProcces(int32_t NCode, WPARAM WParam, LPARAM LParam);
		static LRESULT CALLBACK KeyboardProcess(int32_t nCode, WPARAM wParam, LPARAM lParam);
	public:
		static HHOOK MMHook, KKHook;
	private:
		static volatile LONG isHandling;
	};
}