#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include "Common.h"

namespace CO {
	class CFileEditor
	{
	public:
		static VOID GameFullScreanClassic();
		static VOID Calculator(int32_t Width, int32_t Height, BOOL IsFull);
	private:
		static CONST DWORD enksh = 0x005D7EAA;
		static CONST DWORD mnksh = 0x0063A15E;
		static CONST DWORD fnksh = 0x005D7EAF;
		static CONST DWORD zorksh = 0x0063A177;
		static VOID CFileEditor::NextStep5517(int32_t Width, int32_t Height);
	};

}