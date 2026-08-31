#include "Constants.h"
#include "CWinSocket.h"
#include "OwnerState.h"
#include "Logger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace CO {
    typedef int32_t(WINAPI* CloseSocketFunc)(SOCKET);
    CloseSocketFunc OriginalCloseSocket = nullptr;
    bool IsHooked = false;

    std::string GetLastTwoDigits(uint16_t port) {
        uint16_t lastTwo = port % 100;
        return (lastTwo < 10 ? "0" : "") + std::to_string(lastTwo);
    }

    void LogDisconnection(const std::string& ip, uint16_t port, bool inGame) {
        std::string lastTwoDigits = GetLastTwoDigits(port);
        std::string logMessage = "[SocketClose] IP: " + ip + ", Port: " + lastTwoDigits;

        if (!API::Constants::IsValidPort(port)) {
            // Logger::Log(Debug, logMessage + " -> Unrelated port");
            return;
        }

        if (port == API::Constants::AuthPort) {
            // Logger::Log(Info, logMessage + " -> Auth port disconnected");
        }
        else if (API::Constants::IsGamePort(port)) {
            if (inGame) {
                // Logger::Log(Info, logMessage + " -> Disconnect triggered (Player was in game)");
                OwnerState::Disconnect();
            }
            else {
                // Logger::Log(Info, logMessage + " -> No Disconnect (Player not in game)");
            }
        }
        else {
            // Logger::Log(Info, logMessage + " -> Other valid port disconnected");
        }
    }

    int32_t CALLBACK CWinSocket::_CloseSocketDetour(SOCKET Socket) {
        if (Socket == INVALID_SOCKET || Socket == NULL) {
            // Logger::Log(Warning, "[Socket] Invalid or NULL socket received");
            return SOCKET_ERROR;
        }

        sockaddr_in addr = {};
        int addrSize = sizeof(addr);
        bool isConnected = false;

        std::string clientIP = "unknown";
        uint16_t port = 0;

        if (getpeername(Socket, reinterpret_cast<sockaddr*>(&addr), &addrSize) == 0) {
            isConnected = true;
            port = ntohs(addr.sin_port);

            char ipStr[INET_ADDRSTRLEN] = {};
            inet_ntop(AF_INET, &addr.sin_addr, ipStr, sizeof(ipStr));
            clientIP = ipStr;
        }

        int32_t result = OriginalCloseSocket ? OriginalCloseSocket(Socket) : SOCKET_ERROR;

        if (isConnected) {
            LogDisconnection(clientIP, port, OwnerState::InGame);
        }

        return result;
    }

    VOID CWinSocket::Hook() {
        if (IsHooked)
            return;

        OriginalCloseSocket = reinterpret_cast<CloseSocketFunc>(DetourFindFunction("ws2_32.dll", "closesocket"));
        if (!OriginalCloseSocket) {
            // Logger::Log(Error, "[Socket] Failed to find closesocket in ws2_32.dll");
            return;
        }

        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        if (DetourAttach(reinterpret_cast<PVOID*>(&OriginalCloseSocket), _CloseSocketDetour) != NO_ERROR) {
            // Logger::Log(Error, "[Socket] Failed to attach CloseSocketDetour");
            return;
        }

        if (DetourTransactionCommit() != NO_ERROR) {
            // Logger::Log(Error, "[Socket] Failed to commit Detour transaction");
            return;
        }

        IsHooked = true;
        // Logger::Log(Info, "[Socket] Successfully hooked closesocket");
    }
}
