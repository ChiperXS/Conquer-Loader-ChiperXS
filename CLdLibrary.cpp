#include "CLdLibrary.h"
#include <windows.h>
#include "detours.h"
#include <vector>
#include <string>
#include <algorithm>

namespace CO {
	typedef HMODULE(WINAPI* LoadLibraryA_t)(LPCSTR);
	typedef HMODULE(WINAPI* LoadLibraryW_t)(LPCWSTR);

	LoadLibraryA_t SubLoadLibraryA = nullptr;
	LoadLibraryW_t SubLoadLibraryW = nullptr;

	int count = 0;

	bool IsThreateningModule(const std::string& path) {
		static const std::vector<std::string> banned = {
			"cheatengine", "ollydbg", "x64dbg", "processhacker", "ida", "vehdebug",
			"luaclient", "gameconqueror", "fiddler", "wireshark", "dnspy",
			"reclass", "tsearch", "squalr", "windbg", "mimikatz", "metasploit",
			"burp", "radare2", "scanmem", "cff explorer", "peid", "pointersearcher",
			"artmoney", "immunity", "reflector", "hxd", "procmon", "allochook",
			"pokememory", "inproc", "umstealthedit", "winhook", "am818", "dependency walker"
		};

		std::string lower = path;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

		for (const auto& word : banned) {
			if (lower.find(word) != std::string::npos)
				return true;
		}
		return false;
	}

	bool IsSafeSystemLibrary(const std::string& path) {
		static const std::vector<std::string> allowed = {
			"system", "microsoft", "corporation", "cyberlink", "ac3filter",
			"tqweaesm", "sonic foundry", "totalv", "audiofilter", "tqweaenu",
			"ringz studio", "nero", "tqwealoc", "chrome", "shopstart",
			"gamedata", "tqplat", "tqpdata", "freetime"
		};

		std::string lower = path;
		std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

		for (const auto& ok : allowed) {
			if (lower.find(ok) != std::string::npos)
				return true;
		}
		return false;
	}

	HMODULE WINAPI CLdLibrary::LdLibraryDetourA(LPCSTR str) {
		if (!str) return nullptr;

		char path[MAX_PATH];
		strncpy_s(path, str, MAX_PATH - 1);
		path[MAX_PATH - 1] = '\0';
		CharLowerA(path);

		if (strstr(path, ":\\") != nullptr) {
			count++;

			if (count > 10 && !IsSafeSystemLibrary(path)) {
				API::CFunctions::ExitDetection("Blocked (A): %s", path);
				return nullptr;
			}

			if (IsThreateningModule(path)) {
				const char* filename = strrchr(path, '\\');
				filename = filename ? filename + 1 : path;
				API::CFunctions::ExitDetection("Blocked (A): %s", filename);
				return nullptr;
			}
		}

		return SubLoadLibraryA ? SubLoadLibraryA(str) : nullptr;
	}

	HMODULE WINAPI CLdLibrary::LdLibraryDetourW(LPCWSTR str) {
		if (!str) return nullptr;

		char path[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, str, -1, path, MAX_PATH, NULL, NULL);

		if (strstr(path, ":\\") != nullptr) {
			if (IsThreateningModule(path)) {
				const char* filename = strrchr(path, '\\');
				filename = filename ? filename + 1 : path;
				API::CFunctions::ExitDetection("Blocked (W): %s", filename);
				return nullptr;
			}
		}

		return SubLoadLibraryW ? SubLoadLibraryW(str) : nullptr;
	}

	void CLdLibrary::Hook() {
		HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
		if (!hKernel32) {
			API::CFunctions::ExitDetection("Failed to get kernel32.dll");
			return;
		}

		SubLoadLibraryA = reinterpret_cast<LoadLibraryA_t>(GetProcAddress(hKernel32, "LoadLibraryA"));
		SubLoadLibraryW = reinterpret_cast<LoadLibraryW_t>(GetProcAddress(hKernel32, "LoadLibraryW"));

		if (!SubLoadLibraryA || !SubLoadLibraryW) {
			API::CFunctions::ExitDetection("Failed to get LoadLibrary functions");
			return;
		}

		DetourRestoreAfterWith();
		if (DetourTransactionBegin() != NO_ERROR ||
			DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
			DetourAttach(reinterpret_cast<PVOID*>(&SubLoadLibraryA), reinterpret_cast<PVOID>(LdLibraryDetourA)) != NO_ERROR ||
			DetourAttach(reinterpret_cast<PVOID*>(&SubLoadLibraryW), reinterpret_cast<PVOID>(LdLibraryDetourW)) != NO_ERROR) {
			DetourTransactionAbort();
			API::CFunctions::ExitDetection("Failed to hook LoadLibraryA/W");
			return;
		}

		DetourTransactionCommit();
	}
}
