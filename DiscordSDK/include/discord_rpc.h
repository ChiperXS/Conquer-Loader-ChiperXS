#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

// clang-format off

#if defined(DISCORD_DYNAMIC_LIB)
#  if defined(_WIN32)
#    if defined(DISCORD_BUILDING_SDK)
#      define DISCORD_EXPORT __declspec(dllexport)
#    else
#      define DISCORD_EXPORT __declspec(dllimport)
#    endif
#  else
#    define DISCORD_EXPORT __attribute__((visibility("default")))
#  endif
#else
#  define DISCORD_EXPORT
#endif

// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DiscordRichPresence {
	const char* state;        /* The user's current status, such as "Playing" or "Idle". Maximum 128 bytes. */
	const char* details;      /* Additional details about the user's current activity. Maximum 128 bytes. */
	int64_t startTimestamp;   /* Unix timestamp for the start of the user's activity. */
	int64_t endTimestamp;     /* Unix timestamp for the end of the user's activity. */
	const char* largeImageKey;   /* The key for the large image asset to display. Maximum 32 bytes. */
	const char* largeImageText;  /* Text to display when hovering over the large image asset. Maximum 128 bytes. */
	const char* smallImageKey;   /* The key for the small image asset to display. Maximum 32 bytes. */
	const char* smallImageText;  /* Text to display when hovering over the small image asset. Maximum 128 bytes. */
	const char* partyId;         /* A unique identifier for the user's party or group. Maximum 128 bytes. */
	int partySize;               /* The current size of the user's party or group. */
	int partyMax;                /* The maximum size of the user's party or group. */
	const char* matchSecret;     /* A secret value used for matchmaking. Maximum 128 bytes. */
	const char* joinSecret;      /* A secret value used for joining an activity. Maximum 128 bytes. */
	const char* spectateSecret;  /* A secret value used for spectating an activity. Maximum 128 bytes. */
	int8_t instance;             /* Whether the user's activity is an "instance" session. */

	const char* button1Label;    /* The label for the first button. Maximum 32 bytes. */
	const char* button1Url;      /* The URL to open when the first button is clicked. */

	const char* button2Label;    /* The label for the second button. Maximum 32 bytes. */
	const char* button2Url;      /* The URL to open when the second button is clicked. */
} DiscordRichPresence;

typedef struct DiscordUser {
    const char* userId;
    const char* username;
    const char* discriminator;
    const char* avatar;
} DiscordUser;

typedef struct DiscordEventHandlers {
    void (*ready)(const DiscordUser* request);
    void (*disconnected)(int errorCode, const char* message);
    void (*errored)(int errorCode, const char* message);
    void (*joinGame)(const char* joinSecret);
    void (*spectateGame)(const char* spectateSecret);
    void (*joinRequest)(const DiscordUser* request);
} DiscordEventHandlers;

#define DISCORD_REPLY_NO 0
#define DISCORD_REPLY_YES 1
#define DISCORD_REPLY_IGNORE 2

DISCORD_EXPORT void Discord_Initialize(const char* applicationId,
                                       DiscordEventHandlers* handlers,
                                       int autoRegister,
                                       const char* optionalSteamId);
DISCORD_EXPORT void Discord_Shutdown(void);

/* checks for incoming messages, dispatches callbacks */
DISCORD_EXPORT void Discord_RunCallbacks(void);

/* If you disable the lib starting its own io thread, you'll need to call this from your own */
#ifdef DISCORD_DISABLE_IO_THREAD
DISCORD_EXPORT void Discord_UpdateConnection(void);
#endif

DISCORD_EXPORT void Discord_UpdatePresence(const DiscordRichPresence* presence);
DISCORD_EXPORT void Discord_ClearPresence(void);

DISCORD_EXPORT void Discord_Respond(const char* userid, /* DISCORD_REPLY_ */ int reply);

DISCORD_EXPORT void Discord_UpdateHandlers(DiscordEventHandlers* handlers);

#ifdef __cplusplus
} /* extern "C" */
#endif
