#include "Discord.h"
#include "OwnerState.h"
#include "Constants.h"
#include <chrono>
using namespace std::chrono;

static int64_t cTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

VOID Discord::Initialize() {
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));
    Discord_Initialize(API::Constants::DisocrdAppID.c_str(), &Handle, 1, NULL);
}

VOID Discord::UPDate() {
    DiscordRichPresence Presence;
    memset(&Presence, 0, sizeof(Presence));
    Presence.details = OwnerState::DiscordTitle1;
    Presence.state = OwnerState::DiscordTitle2;
    Presence.startTimestamp = cTime;
    Presence.largeImageKey = "large_image";
    Presence.largeImageText = "Game Logo";
    Presence.smallImageKey = OwnerState::FaceID;
    Presence.smallImageText = "Player Face";
    Presence.button1Label = "Website";
    Presence.button1Url = "https://pyramidsconquer.online/";
    Presence.button2Label = "FaceBook";
    Presence.button2Url = "https://web.facebook.com/profile.php?id=61571920505531";
    Discord_UpdatePresence(&Presence);
}

VOID Discord::Shutdown() {
    Discord_Shutdown();
}
