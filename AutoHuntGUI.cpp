#include "AutoHuntGUI.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND AutoHuntGUI::hwnd = nullptr;

HCURSOR AutoHuntGUI::buttonCursor = nullptr;
HCURSOR AutoHuntGUI::defaultCursor = nullptr;
bool AutoHuntGUI::cursorLoaded = false;
bool AutoHuntGUI::isHoveringAutoHuntButton = false;

IDirect3DDevice8* AutoHuntGUI::device = nullptr;
D3DPRESENT_PARAMETERS AutoHuntGUI::d3dpp = {};
WNDPROC AutoHuntGUI::og_wndproc = nullptr;
bool AutoHuntGUI::isInitialized = false;
bool AutoHuntGUI::open = false;
bool AutoHuntGUI::openNewWindow = false;
ImVec2 AutoHuntGUI::mouse_pos = ImVec2(0, 0);
bool AutoHuntGUI::imgui_wants_mouse = false;
bool AutoHuntGUI::texturesLoaded = false;
float AutoHuntGUI::activeTime = 0.0f;
float AutoHuntGUI::lastAutoHuntStartTime = 0.0f;

bool AutoHuntGUI::isAutoHuntRunning = false;
std::vector<int16_t> AutoHuntGUI::RestrictedMaps;
bool AutoHuntGUI::RestrictedMapsReceived = false;
uint8_t AutoHuntGUI::PlayerClass = 0;
uint8_t AutoHuntGUI::PlayerVipLevel = 0;
IDirect3DTexture8* AutoHuntGUI::texture = nullptr;
IDirect3DTexture8* AutoHuntGUI::bgTexture = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStartNormalTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStartHoverTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStartClickTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStopNormalTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStopHoverTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnStopClickTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnExitNormalTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnExitHoverTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnExitClickTex = nullptr;
IDirect3DTexture8* AutoHuntGUI::btnSettingsTex = nullptr;
D3DSURFACE_DESC AutoHuntGUI::bgDesc = {};

AutoHuntGUI::Flip_t AutoHuntGUI::oFlip = nullptr;
AutoHuntGUI::tReset AutoHuntGUI::oReset = nullptr;

bool AutoHuntGUI::showTooltip = false;
float AutoHuntGUI::tooltipTimer = 0.0f;
const float AutoHuntGUI::tooltipDelay = 0.5f;

DWORD StaticAddres = 0;


void AutoHuntGUI::DrawAdvancedTooltip(const char* title, const char* description, ImVec2 mousePos)
{
    ImGui::SetNextWindowPos(ImVec2(mousePos.x - 50, mousePos.y - 50));
    ImGui::SetNextWindowBgAlpha(0.95f);

    ImGui::Begin("##AdvancedTooltip", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoFocusOnAppearing);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.2f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.3f, 1.0f));
    ImGui::Text("%s", title);
    ImGui::PopStyleColor();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    ImGui::End();
}

LPDIRECT3DTEXTURE8 AutoHuntGUI::LoadTextureFromFile(const char* filename, IDirect3DDevice8* device, D3DSURFACE_DESC* outDesc)
{
    LPDIRECT3DTEXTURE8 texture = nullptr;
    HRESULT hr = D3DXCreateTextureFromFileA(device, filename, &texture);
    if (FAILED(hr) || !texture)
        return nullptr;

    if (outDesc)
        texture->GetLevelDesc(0, outDesc);

    return texture;
}

void AutoHuntGUI::LoadTextures()
{
    if (!bgTexture)
        bgTexture = LoadTextureFromFile("GUIAUTO\\center.dds", device, &bgDesc);
    if (!btnStartNormalTex)
        btnStartNormalTex = LoadTextureFromFile("GUIAUTO\\Start1.dds", device);
    if (!btnStartHoverTex)
        btnStartHoverTex = LoadTextureFromFile("GUIAUTO\\Start2.dds", device);
    if (!btnStartClickTex)
        btnStartClickTex = LoadTextureFromFile("GUIAUTO\\Start3.dds", device);
    if (!btnStopNormalTex)
        btnStopNormalTex = LoadTextureFromFile("GUIAUTO\\Stop1.dds", device);
    if (!btnStopHoverTex)
        btnStopHoverTex = LoadTextureFromFile("GUIAUTO\\Stop2.dds", device);
    if (!btnStopClickTex)
        btnStopClickTex = LoadTextureFromFile("GUIAUTO\\Stop3.dds", device);
    if (!btnExitNormalTex)
        btnExitNormalTex = LoadTextureFromFile("GUIAUTO\\close.dds", device);
    if (!btnExitHoverTex)
        btnExitHoverTex = LoadTextureFromFile("GUIAUTO\\close.dds", device);
    if (!btnExitClickTex)
        btnExitClickTex = LoadTextureFromFile("GUIAUTO\\close.dds", device);
    if (!btnSettingsTex)
        btnSettingsTex = LoadTextureFromFile("GUIAUTO\\Settings.dds", device);
}

void AutoHuntGUI::LoadCursor()
{
    if (!cursorLoaded)
    {
        buttonCursor = LoadCursorFromFileA("data\\Cursor\\Button.cur");

        defaultCursor = ::LoadCursor(NULL, IDC_ARROW);

        cursorLoaded = true;
    }
}


void AutoHuntGUI::ReleaseCursor()
{
    if (buttonCursor)
    {
        DestroyCursor(buttonCursor);
        buttonCursor = nullptr;
    }
    cursorLoaded = false;
}

void AutoHuntGUI::ReleaseTextures()
{
    if (bgTexture) { bgTexture->Release(); bgTexture = nullptr; }
    if (btnStartNormalTex) { btnStartNormalTex->Release(); btnStartNormalTex = nullptr; }
    if (btnStartHoverTex) { btnStartHoverTex->Release(); btnStartHoverTex = nullptr; }
    if (btnStartClickTex) { btnStartClickTex->Release(); btnStartClickTex = nullptr; }
    if (btnStopNormalTex) { btnStopNormalTex->Release(); btnStopNormalTex = nullptr; }
    if (btnStopHoverTex) { btnStopHoverTex->Release(); btnStopHoverTex = nullptr; }
    if (btnStopClickTex) { btnStopClickTex->Release(); btnStopClickTex = nullptr; }
    if (btnExitNormalTex) { btnExitNormalTex->Release(); btnExitNormalTex = nullptr; }
    if (btnExitHoverTex) { btnExitHoverTex->Release(); btnExitHoverTex = nullptr; }
    if (btnExitClickTex) { btnExitClickTex->Release(); btnExitClickTex = nullptr; }
    if (btnSettingsTex) { btnSettingsTex->Release(); btnSettingsTex = nullptr; }
    if (texture) { texture->Release(); texture = nullptr; }
    texturesLoaded = false;
}

void AutoHuntGUI::RenderCenteredWindow()
{
    if (!texturesLoaded)
    {
        LoadTextures();
        texturesLoaded = true;
    }

    if (!bgTexture)
        return;

    const float scale = 0.8f;
    const float growFactor = 1.09f;

    ImVec2 windowSize(
        max((float)bgDesc.Width * 0.6f * scale, 160.0f) * growFactor,
        max((float)bgDesc.Height * 0.6f * scale, 160.0f) * growFactor
    );

    ImVec2 center(
        (ImGui::GetIO().DisplaySize.x - windowSize.x) * 0.5f,
        (ImGui::GetIO().DisplaySize.y - windowSize.y) * 0.5f
    );

    ImGui::SetNextWindowPos(center, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize);

    ImGui::Begin("Center Window", &openNewWindow,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar);

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImGui::GetWindowDrawList()->AddImage(
        (ImTextureID)(UINT_PTR)bgTexture,
        windowPos,
        ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y)
    );

    const float exitButtonWidth = windowSize.x * 0.20f;
    const float exitButtonHeight = 60.0f;
    const float exitButtonRightOffset = windowSize.x * 0.005f; 
    const float exitButtonTopOffset = windowSize.y * 0.120f;
    const float settingsButtonTopOffset = exitButtonTopOffset + exitButtonHeight + 2.0f;

    ImVec2 exitButtonPos(windowSize.x - exitButtonWidth - exitButtonRightOffset, exitButtonTopOffset);

    ImGui::SetCursorPos(exitButtonPos);
    ImGui::PushID("ExitButton");

    ImGui::InvisibleButton("exit_btn", ImVec2(exitButtonWidth, exitButtonHeight));
    if (ImGui::IsItemClicked())
    {
        openNewWindow = false;
    }

    ImTextureID exitTex = (ImTextureID)(UINT_PTR)btnExitNormalTex;
    if (ImGui::IsItemActive() && btnExitClickTex)
        exitTex = (ImTextureID)(UINT_PTR)btnExitClickTex;
    else if (ImGui::IsItemHovered() && btnExitHoverTex)
        exitTex = (ImTextureID)(UINT_PTR)btnExitHoverTex;

    ImGui::SetCursorPos(exitButtonPos);
    if (exitTex)
        ImGui::Image(exitTex, ImVec2(exitButtonWidth, exitButtonHeight));

    if (ImGui::IsItemHovered())
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 100, 100, 200), 3.0f, 0, 2.0f);
    }

    ImGui::PopID();

    ImVec2 settingsButtonPos(windowSize.x - exitButtonWidth - exitButtonRightOffset, settingsButtonTopOffset);

    ImGui::SetCursorPos(settingsButtonPos);
    ImGui::PushID("SettingsButton");

    ImGui::InvisibleButton("settings_btn", ImVec2(exitButtonWidth, exitButtonHeight));
    if (ImGui::IsItemClicked())
    {
        CMsg::CMsgHandler::SendStartStopAction(2);
        openNewWindow = false;
    }

    ImTextureID settingsTex = (ImTextureID)(UINT_PTR)btnSettingsTex;

    ImGui::SetCursorPos(settingsButtonPos);
    if (settingsTex)
        ImGui::Image(settingsTex, ImVec2(exitButtonWidth, exitButtonHeight));

    if (ImGui::IsItemHovered())
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(100, 255, 100, 200), 3.0f, 0, 2.0f);
    }

    ImGui::PopID();


    const float buttonWidth = 150.0f * scale;
    const float buttonHeight = 65.0f * scale;
    const float buttonsY = (windowSize.y - buttonHeight) * 0.8f;

    ImVec2 textPos((windowSize.x * (0.62f - 0.01f)), buttonsY - (25.0f * scale));

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();

    auto DrawShadowText = [&](const char* text, ImVec2 pos, ImVec4 color) {
        drawList->AddText(font, fontSize, ImVec2(windowPos.x + pos.x + 1, windowPos.y + pos.y + 1), ImColor(0, 0, 0, 180), text);
        drawList->AddText(font, fontSize, ImVec2(windowPos.x + pos.x, windowPos.y + pos.y), ImColor(color), text);
        };

    DrawShadowText("Auto Hunt Custom System", textPos, ImVec4(1, 1, 1, 1));
    textPos.y += 22.0f * scale;
    if (RestrictedMapsReceived) {
        DrawShadowText("Map Status: Allowed", textPos, ImVec4(0, 1, 0, 1));
    }
    else {
        DrawShadowText("Map Status: Checking...", textPos, ImVec4(1, 1, 0, 1));
    }
    textPos.y += 22.0f * scale;

    DrawShadowText("VIP 6+ Exclusive Feature", textPos, ImVec4(1, 1, 1, 1));
    textPos.y += 20.0f * scale;

    int totalSeconds = static_cast<int>(activeTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char timeBuffer[64];
    snprintf(timeBuffer, sizeof(timeBuffer), "Active Time: %02d:%02d", minutes, seconds);
    DrawShadowText(timeBuffer, textPos, ImVec4(1, 1, 1, 1));
    textPos.y += 20.0f * scale;

    if (isAutoHuntRunning)
        DrawShadowText("Enable", textPos, ImVec4(0, 1, 0, 1));
    else
        DrawShadowText("Disable", textPos, ImVec4(1, 0, 0, 1));

    float buttonX = windowSize.x * 0.5f - buttonWidth - (windowSize.x * 0.05f);

    if (!isAutoHuntRunning)
    {
        ImGui::SetCursorPos(ImVec2(buttonX, buttonsY));
        ImGui::PushID("StartButton");

        ImGui::InvisibleButton("real_start_btn", ImVec2(buttonWidth, buttonHeight));
        if (ImGui::IsItemClicked())
        {
            CMsg::CMsgHandler::SendStartStopAction(1);
            isAutoHuntRunning = true;
            openNewWindow = false;
        }

        ImTextureID startTex = (ImTextureID)(UINT_PTR)btnStartNormalTex;
        if (ImGui::IsItemActive())
            startTex = (ImTextureID)(UINT_PTR)btnStartClickTex;
        else if (ImGui::IsItemHovered())
            startTex = (ImTextureID)(UINT_PTR)btnStartHoverTex;

        ImGui::SetCursorPos(ImVec2(buttonX, buttonsY));
        ImGui::Image(startTex, ImVec2(buttonWidth, buttonHeight));

        if (ImGui::IsItemHovered())
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 255, 0, 180), 8.0f, 0, 3.0f);
        }

        ImGui::PopID();
    }
    else
    {
        ImGui::SetCursorPos(ImVec2(buttonX, buttonsY));
        ImGui::PushID("StopButton");

        ImGui::InvisibleButton("real_stop_btn", ImVec2(buttonWidth, buttonHeight));
        if (ImGui::IsItemClicked())
        {
            CMsg::CMsgHandler::SendStartStopAction(0);
            isAutoHuntRunning = false;
            activeTime = 0.0f;
            openNewWindow = false;
        }
        ImTextureID stopTex = (ImTextureID)(UINT_PTR)btnStopNormalTex;
        if (ImGui::IsItemActive())
            stopTex = (ImTextureID)(UINT_PTR)btnStopClickTex;
        else if (ImGui::IsItemHovered())
            stopTex = (ImTextureID)(UINT_PTR)btnStopHoverTex;

        ImGui::SetCursorPos(ImVec2(buttonX, buttonsY));
        ImGui::Image(stopTex, ImVec2(buttonWidth, buttonHeight));

        if (ImGui::IsItemHovered())
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 255, 0, 180), 8.0f, 0, 3.0f);
        }

        ImGui::PopID();
    }

    ImGui::End();
}

BOOL AutoHuntGUI::FlipHook(const struct tagRECT* a1, const struct tagRECT* a2, HWND a3)
{
    auto value = (1000 /900);
    DWORD j_timeGetTime = timeGetTime();
    if (j_timeGetTime >= StaticAddres + value)
    {
        StaticAddres = j_timeGetTime;
        if (!device)
            return oFlip(a1, a2, a3);

        HRESULT result = device->TestCooperativeLevel();
        if (result == D3DERR_DEVICELOST)
            return oFlip(a1, a2, a3);
        else if (result == D3DERR_DEVICENOTRESET)
        {
            ImGui_ImplDX8_InvalidateDeviceObjects();
            ReleaseTextures();
            device->Reset(&d3dpp);
            ImGui_ImplDX8_CreateDeviceObjects();
            LoadTextures();
            return oFlip(a1, a2, a3);
        }

        if (open || openNewWindow)
        {
            if (!cursorLoaded)
            {
                LoadCursor();
            }

            ImGui_ImplDX8_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            float x_289 = 0.0f;
            float x_328 = 0.0f;

            if (width == 800 && height == 600)
            {
                float diff = 76.0f * 1 + 38.0f;
                x_289 = (width - 1024) / 2.0f + 132.0f + diff;
                x_328 = (width - 1024) / 2.0f + 223.0f + diff;
            }
            else
            {
                x_289 = (width - 1024) / 2.0f + 132.0f;
                x_328 = (width - 1024) / 2.0f + 223.0f;
            }

            float centerX = (x_289 + x_328) / 2.0f;
            float buttonY = height - 115.0f;

            if (texture == nullptr)
                texture = LoadTextureFromFile("GUIAUTO\\ICO.dds", device);

            ImGuiWindowFlags windowFlags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoNav;

            ImGui::SetNextWindowPos(ImVec2(centerX, buttonY), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(50, 40));

            ImGui::Begin("##AutoHuntButton", nullptr, windowFlags);

            if (texture != nullptr)
            {
                if (ImGui::ImageButton("autohunt_button", texture, ImVec2(50, 40)))
                {
                    openNewWindow = true;
                }

                isHoveringAutoHuntButton = ImGui::IsItemHovered();

                if (isHoveringAutoHuntButton)
                {
                    tooltipTimer += ImGui::GetIO().DeltaTime;
                    if (tooltipTimer >= tooltipDelay)
                    {
                        showTooltip = true;
                    }
                }
                else
                {
                    tooltipTimer = 0.0f;
                    showTooltip = false;
                }
                if (showTooltip && isHoveringAutoHuntButton)
                {
                    const char* title = "Enter The AutoHunt";
                    DrawAdvancedTooltip(title, "", mouse_pos);
                }
            }

            ImGui::End();

            if (openNewWindow)
            {
                if (!texturesLoaded)
                {
                    LoadTextures();
                    texturesLoaded = true;
                }
                RenderCenteredWindow();
            }

            if (isAutoHuntRunning)
            {
                activeTime += ImGui::GetIO().DeltaTime;
            }

            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
        }
    }

    return oFlip(a1, a2, a3);
}

LRESULT AutoHuntGUI::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_MOUSEMOVE)
    {
        mouse_pos.x = (float)GET_X_LPARAM(lparam);
        mouse_pos.y = (float)GET_Y_LPARAM(lparam);
    }

    if (msg == WM_SETCURSOR)
    {
        if (isHoveringAutoHuntButton && buttonCursor)
        {
            SetCursor(buttonCursor);
            return TRUE; 
        }
    }

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        if (ImGui::GetIO().WantCaptureMouse)
            return 0;
        break;

    case WM_KEYDOWN:
        if (wparam == VK_RETURN && (GetAsyncKeyState(VK_MENU) & 0x8000))
        {
            bool isFullscreen = (GetWindowLong(hwnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW) == 0;
            if (isFullscreen)
            {
                SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowPos(hwnd, HWND_TOP, 100, 100, 800, 600, SWP_FRAMECHANGED);
            }
            else
            {
                SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);
                SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
            }
            PostMessage(hwnd, WM_SIZE, SIZE_RESTORED, 0);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return CallWindowProcW(og_wndproc, hwnd, msg, wparam, lparam);
}

HRESULT WINAPI AutoHuntGUI::hkReset(LPDIRECT3DDEVICE8 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    ImGui_ImplDX8_InvalidateDeviceObjects();
    HRESULT result = oReset(pDevice, pPresentationParameters);
    if (result == D3D_OK)
        ImGui_ImplDX8_CreateDeviceObjects();
    return result;
}

void* AutoHuntGUI::GetVTableFunction(void* pInterface, size_t index)
{
    void** vTable = *(void***)pInterface;
    return vTable[index];
}
DWORD callst = 0x050656C + 0x16;
DWORD SetItemColer = 0x0FFFF00;
char* SetItemName;
unsigned short ItemPosition(unsigned int ID)
{
    unsigned int iType = ID / 1000;

    if (iType == 622 || iType == 624 || iType == 626)
        return static_cast<unsigned short>(4);
    if (iType == 620)
        return static_cast<unsigned short>(4);
    if (iType == 619)
        return static_cast<unsigned short>(5);
    if (iType == 617)
        return static_cast<unsigned short>(4);
    if (iType == 148)
        return static_cast<unsigned short>(1);
    if (iType == 614)
        return static_cast<unsigned short>(4);
    if (iType == 615 || iType == 616)
        return static_cast<unsigned short>(4);
    if ((iType >= 111 && iType <= 118) || iType == 123 || (iType >= 141 && iType <= 144) || iType == 145 || iType == 170)
        return static_cast<unsigned short>(1);
    else if (iType >= 120 && iType <= 121)
        return static_cast<unsigned short>(2);
    else if ((iType >= 130 && iType <= 139) || (iType == 101))
        return static_cast<unsigned short>(3);
    else if (iType >= 150 && iType <= 152)
        return static_cast<unsigned short>(6);
    else if (iType == 160)
        return static_cast<unsigned short>(8);
    else if (iType == 1050 || iType == 900)
        return static_cast<unsigned short>(5);
    else if ((iType >= 410 && iType <= 490) || (iType >= 500 && iType <= 580) || (iType >= 601 && iType <= 613))
        return static_cast<unsigned short>(4);

    return 0;
}
void AddGradientText(ImDrawList* dl, ImVec2 pos, const char* text)
{
    float x = pos.x;
    float y = pos.y;

    const char* p = text;
    int len = strlen(text);

    for (int i = 0; i < len; i++)
    {
        char c[2] = { p[i], 0 };
        ImVec2 charSize = ImGui::CalcTextSize(c);

        ImU32 col;
        float t = (float)i / (float)len;
        if (t < 0.5f)
            col = IM_COL32(255, (int)(t * 2 * 165), 0, 255);
        else
            col = IM_COL32(255, 165 + (int)((t - 0.5f) * 2 * 90), 0, 255);

        dl->AddText(ImVec2(x, y), col, c);
        x += charSize.x;
    }
}
void __stdcall FuncItemCheck(int ID, char* a4, int a2, int a3, int a5)
{
    ImGui_ImplDX8_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hidden", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
    ImGui::Dummy(ImVec2(0, 0));
    ImGui::End();

    ImDrawList* dl = ImGui::GetForegroundDrawList();

    ImVec2 textSize = ImGui::CalcTextSize(a4);
    float padX = 6.0f;
    float padY = 4.0f;

    ImVec2 bgMin = ImVec2((float)a2 - padX, (float)a3 - padY);
    ImVec2 bgMax = ImVec2((float)a2 + textSize.x + padX, (float)a3 + textSize.y + padY);

    dl->AddRectFilled(ImVec2(bgMin.x + 2, bgMin.y + 2),
        ImVec2(bgMax.x + 2, bgMax.y + 2),
        IM_COL32(0, 0, 0, 100), 4.0f);

    dl->AddRectFilled(bgMin, bgMax, IM_COL32(0, 0, 0, 80), 4.0f);

    ImU32 col = IM_COL32(255, 255, 255, 255);

    switch (ID)
    {
    case 1050000:
        col = IM_COL32(255, 255, 255, 255);
        break;
    case 1050001:
        col = IM_COL32(0, 200, 200, 255);  
        break;
    case 1050002:
        col = IM_COL32(200, 100, 0, 255);  
        break;
    case 1088001:
        col = IM_COL32(255, 0, 0, 255);    
        break;
    case 1088000:
        AddGradientText(dl, ImVec2((float)a2, (float)a3), a4);
        goto skip_default_draw;
    default:
        if (ItemPosition(ID) > 0)
        {
            col = IM_COL32(255, 255, 0, 255);
            if (ID / 0x186A0 < 0xA && (ID < 0xAAE60 || ID > 0xC3500))
            {
                switch (ID % 0xA)
                {
                case 6: col = IM_COL32(0, 255, 0, 255); break;
                case 7: col = IM_COL32(0, 128, 255, 255); break;
                case 8: col = IM_COL32(200, 0, 200, 255); break;
                case 9: col = IM_COL32(255, 255, 0, 255); break;
                default: col = IM_COL32(255, 255, 255, 255); break;
                }
            }
        }
        break;
    }

    SetItemColer = col;

    dl->AddText(ImVec2((float)a2, (float)a3), SetItemColer, a4);

skip_default_draw:;

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
}
__declspec(naked) void __stdcall MapFlore()
{
    __asm
    {
        pushad
        PUSH 0x0FFFF00
        MOV ECX, 0x690758
        PUSH DWORD PTR SS : [EBP - 0x18]
        PUSH DWORD PTR SS : [EBP - 0x1c]
        PUSH DWORD PTR SS : [EBP - 0xb4]
        push    dword ptr[esi + 0x20]
        call FuncItemCheck
        popad
        PUSH SetItemColer
        MOV ECX, 0x690758
        PUSH 0
        PUSH DWORD PTR SS : [EBP - 0x1c]
        PUSH DWORD PTR SS : [EBP - 0xb4]
        jmp callst
    }
}
void AutoHuntGUI::InitializeOnce()
{
    if (isInitialized)
        return;
    auto fun = (DWORD)PtrToUlong(MapFlore) - ((DWORD)(0x050656C) + 5);
    DWORD oldProtect = 0;
    VirtualProtect((void*)0x050656C, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset((void*)0x050656C, 0x90, 0x16);
    *(BYTE*)0x050656C = 0xE9;
    *((DWORD*)((DWORD)0x050656C + 1)) = fun;

    auto ss = API::CMemory::AOBScanByName("C3_CORE_DLL.dll", "\xC3\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B\x54\x24\x0C", "xxxxxxxxxxxxxxx") + 0xB;

    auto flip = (Flip_t)ss; 

    auto d1 = API::CMemory::AOBScanByName("C3_CORE_DLL.dll", "\x8B\x08\x53\x68\x92\x00\x00\x00\x50\xFF\x91", "xxxxxxxxxxx") + 0xF;

    device = **(IDirect3DDevice8***)(d1 + 1);

    hwnd = *(HWND*)0x0691C94;

    og_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProc)));

    void* pReset = GetVTableFunction(device, 14);

    oReset = (tReset)pReset;


    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oReset, hkReset);
    DetourTransactionCommit();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX8_Init(device);

    MH_Initialize();
    MH_CreateHook(flip, &FlipHook, (PVOID*)&oFlip);
    MH_EnableHook(flip);

    isInitialized = true;
}
void AutoHuntGUI::Cleanup()
{
    if (!isInitialized)
        return;

    showTooltip = false;
    tooltipTimer = 0.0f;

    ReleaseTextures();
    ReleaseCursor();
    ImGui_ImplDX8_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    if (hwnd && og_wndproc)
        SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(og_wndproc));

    open = false;
    openNewWindow = false;
    isInitialized = false;
}

void AutoHuntGUI::SetDevice(IDirect3DDevice8* dev, HWND window)
{
    device = dev;
    hwnd = window;
}
void AutoHuntGUI::SetRestrictedMaps(const std::vector<int16_t>& maps)
{
    RestrictedMaps = maps;
    RestrictedMapsReceived = true;
}

bool AutoHuntGUI::IsMapRestrictedForAutoHunt(int16_t mapID)
{
    if (!RestrictedMapsReceived) return false;
    return std::find(RestrictedMaps.begin(), RestrictedMaps.end(), mapID) != RestrictedMaps.end();
}

void AutoHuntGUI::SetRestrictedMapsReceived(bool received)
{
    RestrictedMapsReceived = received;
}

void AutoHuntGUI::Show()
{
    open = true;
}

void AutoHuntGUI::Hide()
{
    open = false;
    openNewWindow = false;
}

void AutoHuntGUI::Initialize()
{
    Show();
}
