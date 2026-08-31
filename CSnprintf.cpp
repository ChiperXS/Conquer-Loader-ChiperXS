#include "Owner.h"
#include "CSnprintf.h"
#include "OwnerState.h"

namespace CO {

    static PVOID OriginalSnprintf = nullptr;
    constexpr const char* PingFormatOriginal = "[%s] (%03d,%03d) Ping: %04d";
    constexpr const char* PingFormatModified = "[%s] (%03d,%03d) Ping: %04d; Fps: %02d; %02d/%02d/%d %s %02d:%02d %s";

    int CreateFormattedString(char* buffer, int length, const char* format, ...) {
        va_list args;
        va_start(args, format);
        int result = vsnprintf(buffer, length, format, args);
        va_end(args);
        return result;
    }

    int32_t CSnprintf::SnprintfDetour(char* buffer, int32_t length, const char* format, ...) {
        va_list args;
        va_start(args, format);

        if (OwnerState::InGame && strstr(format, PingFormatOriginal)) {
            va_list copiedArgs;
            va_copy(copiedArgs, args);

            const char* playerName = va_arg(copiedArgs, const char*);
            int x = va_arg(copiedArgs, int);
            int y = va_arg(copiedArgs, int);
            int ping = va_arg(copiedArgs, int);
            int fps = va_arg(copiedArgs, int);
            int day = va_arg(copiedArgs, int);
            int month = va_arg(copiedArgs, int);
            int year = va_arg(copiedArgs, int);
            const char* weekday = va_arg(copiedArgs, const char*);
            int hour = va_arg(copiedArgs, int);
            int minute = va_arg(copiedArgs, int);

            va_end(copiedArgs);

            int finalPing = (ping > 500) ? 100 : std::max(ping / 4, 10);

            int result = CreateFormattedString(
                buffer,
                length,
                PingFormatModified,
                playerName,
                x, y,
                finalPing,
                fps,
                day, month, year,
                weekday,
                hour, minute,
                API::Constants::ServerName.c_str()
            );

            va_end(args);
            return result;
        }

        int32_t result = vsnprintf(buffer, length, format, args);
        va_end(args);
        return result;
    }

    void CSnprintf::Hook() {
        OriginalSnprintf = DetourFindFunction("msvcrt.dll", "_snprintf");

        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach(&(PVOID&)OriginalSnprintf, SnprintfDetour);

        if (DetourTransactionCommit() != NO_ERROR) {
            exit(EXIT_FAILURE);
        }
    }
}
