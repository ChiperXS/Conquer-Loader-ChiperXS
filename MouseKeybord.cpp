#include "MouseKeybord.h"
#include "CHotKey.h"
#include "ProtectionThread.h"
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <deque>
#include <unordered_map>
#include <chrono>

namespace CShield {

    HHOOK MouseKeybord::MMHook = NULL;
    HHOOK MouseKeybord::KKHook = NULL;
    static const int DETECTION_COOLDOWN = 15000;
    __int64 lastDetectionTime = 0;

    __int64 GetHighResolutionTime() {
        LARGE_INTEGER frequency, time;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&time);
        return (time.QuadPart * 1000) / frequency.QuadPart;
    }

    LRESULT CALLBACK MouseKeybord::KeyboardProcess(int32_t nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode >= HC_ACTION) {
            if (API::CFunctions::IsGameWindowActive()) {
                auto pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

                if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                    BOOL InjectedKey = (pKeyBoard->flags & LLKHF_INJECTED) == LLKHF_INJECTED;
                    bool isFunctionKey = (pKeyBoard->vkCode >= VK_F1 && pKeyBoard->vkCode <= VK_F10);

                    if (InjectedKey && isFunctionKey) {
                        if (OwnerState::InGame) {
                            bool isLegitimate = (pKeyBoard->dwExtraInfo == 0xDEADBEEF);

                            if (!isLegitimate) {
                                __int64 currentTime = GetHighResolutionTime();
                                if (currentTime - lastDetectionTime > DETECTION_COOLDOWN) {
                                    OwnerState::Detected(AutoKeyboard, "Detected");
                                    lastDetectionTime = currentTime;
                                }
                            }
                        }
                    }
                }
            }
        }
        return CallNextHookEx(KKHook, nCode, wParam, lParam);
    }

    LRESULT CALLBACK MouseKeybord::MouseProcces(int32_t nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && wParam >= WM_LBUTTONDOWN && wParam <= WM_MBUTTONDBLCLK) {
            auto click = reinterpret_cast<PMSLLHOOKSTRUCT>(lParam);
            if (click && API::CFunctions::IsGameWindowActive()) {
                if (OwnerState::InGame) {
                    if (click->flags & LLMHF_INJECTED) {
                        OwnerState::Detected(AutoClicker, "Detected");
                    }
                }
            }
        }
        return CallNextHookEx(MMHook, nCode, wParam, lParam);
    }

    void MouseKeybord::ResetHooks() {
        if (MMHook)
            UnhookWindowsHookEx(MMHook);
        if (KKHook)
            UnhookWindowsHookEx(KKHook);

        MMHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProcces, GetModuleHandle(NULL), NULL);
        KKHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProcess, GetModuleHandle(NULL), NULL);

    }

    VOID WINAPIV MouseKeybord::OnProcess() {
        while (true) {
            if (API::CFunctions::IsGameWindowActive()) {
                ResetHooks();
                MSG msg;
                while (GetMessage(&msg, NULL, 0, 0)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            Sleep(1000);
        }
    }
}
