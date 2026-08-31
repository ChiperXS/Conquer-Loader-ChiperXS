#pragma once
#include <string>
#include <fstream>
#include <windows.h>

namespace LO {
	enum LogType {
		Error,
		Detection,
		Info,
		Debug,
		Warning
	};

	class Logger {
	public:
		static void Log(LogType type, const std::string& message);
	};
}