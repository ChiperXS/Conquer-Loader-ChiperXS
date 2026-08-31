#pragma once
#include "Common.h"

namespace CO {
	class CLdLibrary {
	public:
		static void Hook();
		static HMODULE WINAPI LdLibraryDetourA(LPCSTR str);
		static HMODULE WINAPI LdLibraryDetourW(LPCWSTR str);
	};
}