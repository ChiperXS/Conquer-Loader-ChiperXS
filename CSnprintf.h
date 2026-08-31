#pragma once
#include "Common.h"

namespace CO {
	class CSnprintf
	{
	public:
		static VOID Hook();

	private:
		static int32_t SnprintfDetour(CHAR* Data, int32_t Length, const CHAR* Format, ...);
		static int32_t CreateFormat(CHAR * Data, int32_t Length, const CHAR* Format, ...) {
			va_list ArgPtr;
			va_start(ArgPtr, Format);
			return vsnprintf(Data, Length, Format, ArgPtr);
		}
	};
}