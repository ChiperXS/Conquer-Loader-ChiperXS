#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  // Windows XP
#endif
#include "Common.h"


namespace CO {
	class CMoveWindow {
	public:
		static void Hook();
	private:
		static PVOID SubMoveWindow;
		static int __stdcall TqMoveWindow(HWND hWnd, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, BOOL bRepaint);
	};
}
