#include "CMoveWindow.h"
#include "OwnerState.h"


namespace CO {
	PVOID CMoveWindow::SubMoveWindow = NULL;


	BOOL WINAPI CMoveWindow::TqMoveWindow(HWND hWnd, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, BOOL bRepaint) {

		if (Y == 627 + (OwnerState::Height - 768) && nWidth == OwnerState::Width && X == 0) {
			X = ((OwnerState::Width - 1024) / 2);
		}

		if (X == 365 && Y == 300 && nWidth == 300 && nHeight == 108)
			X = OwnerState::Width / 2 - nWidth / 2;

		//x=10 y=315 Width=45 height=325Control0
		if (X == 10 && Y == 315 && nWidth == 45 && nHeight == 325 && GetDlgCtrlID(hWnd) == 0)//chat dialog
			Y = OwnerState::Height - 453;

		//x=320 y=100 Width=384 height=380Control0
		if (X == 320 && Y == 100 && nWidth == 384 && nHeight == 380 && GetDlgCtrlID(hWnd) == 0)//offline TG
		{
			X = OwnerState::Width / 2 - nWidth / 2;
			// Y = Height / 2 - nHeight / 2;
		}
		if (X == 380 && Y == 0 && nWidth == 290 && nHeight == 114 && GetDlgCtrlID(hWnd) == 0)//msg
			X = OwnerState::Width / 2 - nWidth / 2;
		if (X == 182 && Y == 231 && nWidth == 660 && nHeight == 306 && GetDlgCtrlID(hWnd) == 0)//msg
		{
			X = OwnerState::Width / 2 - nWidth / 2;
			Y = OwnerState::Height / 2 - nHeight / 2;
		}

		if (X == 388 && Y == 280 && nWidth == 248 && nHeight == 208 && GetDlgCtrlID(hWnd) == 0)//chat gui
		{
			X = OwnerState::Width / 2 - nWidth / 2;
			Y = OwnerState::Height / 2 - nHeight / 2;
		}

		if (X == 280 && Y == 180 && nWidth == 426 && GetDlgCtrlID(hWnd) == 0)//merchant
			X = OwnerState::Width / 2 - nWidth / 2;

		if (X == 223 && Y == 654)//item lock
		{
			X = (OwnerState::Width - 1024) / 2 + 250;
			Y = OwnerState::Height - 119;
		}
		if (X == 165 && Y == 639)//mentor reward
		{
			X = (OwnerState::Width - 1024) / 2 + 185;
			Y -= 25;
			Y = 5000;
			//   Y = 793;//Height - 129;
		}
		if (X == 276 && Y == 650)//vip icon
		{
			X = (OwnerState::Width - 1024) / 2 + 312;
			Y = OwnerState::Height - 119;//(Height - 768) + 650;
			//  X = Width - 892;
			//  Y = Height - 123;

		}
		//Width=226 height=118
		if (X == 574 && nWidth == 226 && nHeight == 118 && GetDlgCtrlID(hWnd) == 0)
		{
			X += (OwnerState::Width - 1024) / 2;
		}



		if (X == 117 && nWidth == 110 && nHeight == 300 && GetDlgCtrlID(hWnd) == 0)//Enemy Bar
			X += (OwnerState::Width - 1024) / 2;



		if (X == 110 && Y > 500 && nWidth == 15 && nHeight == 32)
			//if(X == 110&& Y == 795 || X == 110 && Y == 975)//help
			X += (OwnerState::Width - 1024) / 2 + 0;

		if (X == 0 && OwnerState::Width == nWidth && nHeight == 141)
			// if(X == 0 && Y == 759 || X == 0 && Y == 939)//bara de jos
			X += (OwnerState::Width - 1024) / 2;



		SetWindowPos(hWnd, NULL, X, Y, nWidth, nHeight, SWP_NOZORDER | (bool)bRepaint * SWP_NOREDRAW);
		return true;
	}

	VOID CMoveWindow::Hook() {
		HMODULE hUser32 = GetModuleHandleA("user32.dll");
		if (!hUser32) return;

		SubMoveWindow = DetourFindFunction("user32.dll", "MoveWindow");
		if (!SubMoveWindow) return;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(LPVOID&)SubMoveWindow, TqMoveWindow);

		if (DetourTransactionCommit() != NO_ERROR) {
			MessageBoxA(NULL, "Failed to hook MoveWindow!", "Error", MB_OK | MB_ICONERROR);
		}
	}
}