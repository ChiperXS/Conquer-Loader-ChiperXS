#pragma once
#include "Common.h"
#include "Logger.h"
#include <wtsapi32.h>
#include <cstring>
#include <tlhelp32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "crypt32.lib")
using namespace LO;
#define BUFFER_SIZE 1024
#define SALT_SIZE 16
#define KEY_SIZE 32
#define SHA256_HASH_SIZE 32
#define SHA256_HASH_STRING_LENGTH (SHA256_HASH_SIZE * 2)

namespace API {
	class CFunctions
	{
	public:

		static bool CheckIfGameWindowReady()
		{
			struct Param {
				DWORD pid;
				HWND hwnd;
			} param = { GetCurrentProcessId(), nullptr };

			EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
				Param* p = reinterpret_cast<Param*>(lParam);
				DWORD pid;
				GetWindowThreadProcessId(hwnd, &pid);
				if (pid == p->pid && IsWindowVisible(hwnd)) {
					p->hwnd = hwnd;
					return FALSE;
				}
				return TRUE;
				}, reinterpret_cast<LPARAM>(&param));

			return (param.hwnd != nullptr && IsWindow(param.hwnd));
		}

		static HWND _GetWindow(DWORD ProcessID) {
			HWND _HWND;
			for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {
				if (!IsWindowVisible(hwnd)) continue;
				int length = GetWindowTextLength(hwnd);
				if (length == 0) continue;
				DWORD PID;
				GetWindowThreadProcessId(hwnd, &PID);
				if (ProcessID == PID) {
					_HWND = hwnd;
					break;
				}
			}
			return _HWND;
		}
		static HANDLE hAntiCheatMutex;

		static HANDLE CreateSingleInstanceMutex() {
			static HANDLE hAntiCheatMutex = NULL;

			if (hAntiCheatMutex != NULL)
				return hAntiCheatMutex;

			hAntiCheatMutex = CreateMutexA(NULL, TRUE, "Global\\ConquerAntiCheat_Mutex");
			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				CloseHandle(hAntiCheatMutex);
				hAntiCheatMutex = NULL;
			}

			return hAntiCheatMutex;
		}


		static WCHAR* charToWChar(const CHAR* text) {
			const size_t size = strlen(text) + 1;
			WCHAR* wText = new WCHAR[size];
			mbstowcs(wText, text, size);
			return wText;
		}
		static string GetRandomString(int32_t Length) {
			static CHAR alphanum[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

			string random_string;
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dis(0, sizeof(alphanum) / sizeof(alphanum[0]) - 2);

			for (int32_t i = 0; i < Length; ++i) {
				random_string += alphanum[dis(gen)];
			}
			return random_string.c_str();
		}
		static ULONG GetRandomInt(ULONG uMin, ULONG uMax) {
			if (uMax < (ULONG)0xFFFFFFFF) { uMax++; }
			return (rand() % (uMax - uMin)) + uMin;
		}
		static VOID MD5HASH(CHAR* FilePath, CHAR Hash[]) {
			BOOL PResult = false;
			DWORD CBRead, CBHash = NULL;
			HCRYPTPROV HProv, HHash = NULL;
			BYTE RGBFile[1024], RGBHash[16];
			CHAR RGBDigits[] = "0123456789abcdef";
			HANDLE HFile = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

			if (INVALID_HANDLE_VALUE == HFile) return;
			if (!CryptAcquireContextA(&HProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return;
			if (!CryptCreateHash(HProv, CALG_MD5, NULL, NULL, &HHash)) {
				CloseHandle(HFile);
				CryptReleaseContext(HProv, NULL);
				return;
			}
			while (PResult = ReadFile(HFile, RGBFile, 1024, &CBRead, NULL)) {
				if (NULL == CBRead) break;
				if (!CryptHashData(HHash, RGBFile, CBRead, NULL)) {
					CryptReleaseContext(HProv, NULL);
					CryptDestroyHash(HHash);
					CloseHandle(HFile);
					return;
				}
			}
			if (!PResult) {
				CryptReleaseContext(HProv, NULL);
				CryptDestroyHash(HHash);
				CloseHandle(HFile);
				return;
			}
			CBHash = PP_PROVTYPE;
			if (CryptGetHashParam(HHash, HP_HASHVAL, RGBHash, &CBHash, NULL)) {
				INT Count = NULL;
				for (DWORD FL = 0; FL < CBHash; FL++) {
					Hash[Count++] = RGBDigits[RGBHash[FL] >> 4];
					Hash[Count++] = RGBDigits[RGBHash[FL] & 0xF];
				}
				Hash[32] = '\0';
			}
			CryptDestroyHash(HHash);
			CryptReleaseContext(HProv, NULL);
			CloseHandle(HFile);
		}

		static bool IsGameWindowActive() {
			HWND hForeground = GetForegroundWindow();
			if (!hForeground) return false;

			DWORD fgProcessId = 0;
			GetWindowThreadProcessId(hForeground, &fgProcessId);
			DWORD currentProcessId = GetCurrentProcessId();

			return (fgProcessId == currentProcessId);
		}
		static inline BOOL CheckExist(const string& FilePath) {
			ifstream File(FilePath);
			if (!File) {
				File.close();
				return false;
			}
			else {
				File.close();
				return true;
			}
		}
		static string ToString(int64_t Number) {
			stringstream ss;
			ss << Number;
			return ss.str();
		}
		static string ReplaceAll(string Subject, const string& Search, const  string& Replace) {
			size_t pos = 0;
			while ((pos = Subject.find(Search, pos)) != string::npos) {
				Subject.replace(pos, Search.length(), Replace);
				pos += Replace.length();
			}
			return Subject;
		}

		static VOID ExitDetection(CONST CHAR* Format, ...) {
			char buffer[1024];
			va_list args;
			va_start(args, Format);
			vsnprintf(buffer, sizeof(buffer), Format, args);
			va_end(args);
			Logger::Log(Detection, buffer);
			ExitProcess(1);
			TerminateProcess(GetCurrentProcess(), 0);
			TerminateProcess(GetCurrentProcess(), 1);
		}

		static DWORD CALLBACK ExitAfter(LPVOID lpParameter) {
			CloseHandle(GetCurrentProcess());
			Sleep((int8_t)lpParameter * 1000);
			return true;
		}
	};
}