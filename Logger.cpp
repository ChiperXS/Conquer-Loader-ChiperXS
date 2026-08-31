#include "Logger.h"

namespace LO {
	void Logger::Log(LogType type, const std::string& message) {
	/*	return;*/
		CreateDirectoryA("SilentLog", NULL);

		std::ofstream logFile("SilentLog\\SilentLog.log", std::ios::app);
		if (logFile.is_open()) {
			std::string msg_with_errorcode;

			switch (type) {
			case Error:
				msg_with_errorcode = "[ERROR] " + message;
				break;
			case Detection:
				msg_with_errorcode = "[DETECTION] " + message;
				break;
			case Info:
				msg_with_errorcode = "[INFO] " + message;
				break;
			case Warning:
				msg_with_errorcode = "[WARNING] " + message;
				break;
			default:
				msg_with_errorcode = "[ERROR] " + message;
				break;
			}

			logFile << msg_with_errorcode << std::endl;
			logFile.close();
		}
	}
}