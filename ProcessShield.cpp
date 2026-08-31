#include "PRW.h"
#include <ws2tcpip.h>
#include <wtsapi32.h>
#include <psapi.h>
#include "CMsgHandler.h"
#include "ProcessShield.h"
#pragma comment (lib, "Wtsapi32.lib")
#pragma comment (lib, "Psapi.lib")

namespace CShield {
    BOOL ProcessShield::GetWindowTitle(DWORD _PID, CHAR* _Title) {
        BOOL Found = false;
        for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {
            if (!IsWindowVisible(hwnd)) continue;
            int32_t Length = GetWindowTextLength(hwnd);
            if (Length == 0) continue;
            DWORD PID;
            WCHAR TitleW[256];
            CHAR Title[256];
            GetWindowThreadProcessId(hwnd, &PID);
            if (_PID == PID) {
                GetWindowTextW(hwnd, TitleW, Length + 1);
                WideCharToMultiByte(CP_ACP, 0, TitleW, -1, Title, 256, NULL, NULL);
                strncpy(_Title, Title, 256);
                Found = true;
                break;
            }
        }
        return Found;
    }

    VOID WINAPIV ProcessShield::OnProcess() {
        DWORD dwProcCount = 0;
        WTS_PROCESS_INFO* pWPIs = NULL;
        if (WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount)) {
            for (DWORD FL = 0; FL < dwProcCount; FL++) {
                CHAR One[1024], Title[256], c_szText[256], Path[MAX_PATH];
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pWPIs[FL].ProcessId);

                wcstombs(c_szText, pWPIs[FL].pProcessName, 256);

                Path[0] = '\0';
                if (hProcess != NULL) {
                    if (GetModuleFileNameExA(hProcess, NULL, Path, MAX_PATH) == 0) {
                        Path[0] = '\0';
                    }
                    CloseHandle(hProcess);
                }

                if (GetWindowTitle(pWPIs[FL].ProcessId, Title)) {
                    sprintf(One, "W|%s|%s|%d|%s", c_szText, Title, pWPIs[FL].SessionId, Path);
                }
                else {
                    sprintf(One, "B|%s|%d|%s", c_szText, pWPIs[FL].SessionId, Path);
                }

                if (FL == 0) {
                    CMsgOpenedProcesses CPacket;
                    CPacket.ActionType = Start;
                    CPacket.ProgramLen = NULL;
                    memcpy(CPacket.OneProgram, "\0", strlen("\0"));
                    OwnerState::SendMsg(&CPacket);
                }
                else if (FL == dwProcCount - 1) {
                    CMsgOpenedProcesses CPacket;
                    CPacket.ActionType = Finish;
                    CPacket.ProgramLen = NULL;
                    memcpy(CPacket.OneProgram, "\0", strlen("\0"));
                    OwnerState::SendMsg(&CPacket);
                }

                CMsgOpenedProcesses CPacket;
                CPacket.ActionType = Insert;
                CPacket.ProgramLen = strlen(One) + 1;
                memcpy(CPacket.OneProgram, One, CPacket.ProgramLen);
                OwnerState::SendMsg(&CPacket);
            }
        }
        if (pWPIs) {
            WTSFreeMemory(pWPIs);
            pWPIs = NULL;
        }
    }
}