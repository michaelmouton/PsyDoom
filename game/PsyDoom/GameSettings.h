#pragma once

#include "Macros.h"

#include <cstdint>

//------------------------------------------------------------------------------------------------------------------------------------------
// Settings for the game, which in most cases are configurable by the user.
// These must be synchronized in multiplayer games and set appropriately for correct demo playback.
//------------------------------------------------------------------------------------------------------------------------------------------
struct GameSettings {
    // The current version of this struct: this can be incremented for future PsyDoom releases if the format changes
    static constexpr uint32_t VERSION = 2;

    uint8_t     bUsePalTimings;                         // Use 50 Hz vblanks and other various timing adjustments for the PAL version of the game?
    uint8_t     bUseDemoTimings;                        // Force player logic to run at a consistent, but slower rate used by demos? (15 Hz for NTSC)
    uint8_t     bFixKillCount;                          // If enabled then increment the total number of kills whenever a new enemy is spawned or respawned
    uint8_t     bFixLineActivation;                     // Use the fix for line activation logic, to make it more reliable and prevent exploits?
    uint8_t     bUseExtendedPlayerShootRange;           // Extend player max shoot distance from '2048' to '8192' and auto aim distance from '1024' to '8192'? (similar range to ZDoom)
    uint8_t     bFixMultiLineSpecialCrossing;           // Allow the player to cross multiple special lines in the same frame?
    uint8_t     bUsePlayerRocketBlastFix;               // Apply the fix for the player sometimes not receiving splash damage from rocket blasts?
    uint8_t     bUseSuperShotgunDelayTweak;             // Whether to apply the gameplay tweak that reduces the initial firing delay of the super shotgun
    uint8_t     bUseMoveInputLatencyTweak;              // Use a tweak to player movement which tries to reduce input latency? This affects movement slightly.
    uint8_t     bUseItemPickupFix;                      // Fix an original bug where sometimes valid items can't be picked up due to other items nearby that can't be picked up?
    uint8_t     bUseFinalDoomPlayerMovement;            // Whether to use the Final Doom way of doing player movement & turning
    uint8_t     bAllowMovementCancellation;             // Digital movement only: whether opposite move inputs can cancel each other out
    uint8_t     bAllowTurningCancellation;              // Digital turning only: whether opposite turn inputs can cancel each other out
    uint8_t     bFixViewBobStrength;                    // Fix the strength of the view bobbing when running at 30 FPS so it's as intense as 15 FPS
    uint8_t     bFixGravityStrength;                    // Fix the strength of gravity so it applies consistently regardless of framerate? (weakens overly strong gravity at 30 FPS)
    uint8_t     bNoMonsters;                            // Is the '-nomonsters' command line cheat activated?
    uint8_t     bNoMonstersBossFixup;                   // Is the '-nmbossfixup' command line switch specified?
    uint8_t     bPistolStart;                           // Is the '-pistolstart' command line switch specified?
    uint8_t     bTurboMode;                             // Is the '-turbo' command line cheat specified?
    uint8_t     bUseLostSoulSpawnFix;                   // If true then apply a fix to prevent Lost Souls from spawning outside of the level
    uint8_t     bUseLineOfSightOverflowFix;             // If true then apply a fix to prevent numeric overflows in the enemy line of sight code
    uint8_t     bRemoveMaxCrossLinesLimit;              // If true then the player can cross an unlimited number of line specials per tick rather than '8'
    uint8_t     bFixOutdoorBulletPuffs;                 // Fix a Doom engine bug where bullet puffs don't appear sometimes after shooting outdoor walls?
    uint8_t     bFixBlockingGibsBug;                    // Fix an original bug where monster gibs can block the player if the enemy is crushed during it's death animation
    uint8_t     bFixSoundPropagation;                   // Fix an original bug where sometimes sound propagates through closed doors that should block it?
    uint8_t     bFixSpriteVerticalWarp;                 // Fix an original bug where some sprites on the edge of ledges can alternate suddenly between the top and bottom of the ledge?
    uint8_t     bAllowMultiMapPickup;                   // If true then allow players to pickup more than one 'Computer Area Map'
    uint8_t     bEnableMapPatches_GamePlay;             // Enable patches for original game maps that affect gameplay?
    uint8_t     bCoopNoFriendlyFire;                    // If true then players are not able to damage each other in co-op.
    uint8_t     bCoopForceSpawnDeathmatchThings;        // Enable multiplayer-only things in co-op?
    uint8_t     bDmExitDisabled;                        // If true while playing deathmatch exits will display a 'disabled' message and do nothing.
    uint8_t     bCoopPreserveKeys;                      // If true while playing co-op a player will spawn with previously collected keys.
    uint8_t     bDmActivateBossSpecialSectors;          // If true while playing deathmatch all boss-related special sectors will active automatcally upon entering a map.
    int32_t     lostSoulSpawnLimit;                     // How many lost souls to limit a level to when Pain Elementals try to spawn one. -1 means no limit.
    int32_t     viewBobbingStrengthFixed;               // 16.16 multiplier for view bobbing strength
    int32_t     dmFragLimit;                            // If playing deathmatch, level will exit when this number of frags is reached. <0 = infinite.
    int32_t     coopPreserveAmmoFactor;                 // How much ammo a player keeps after dying in co-op. 0 = none | 1 = all | 2 = half
    uint8_t     bSinglePlayerForceSpawnDmThings;        // Enable multiplayer-only things in single player?

    void byteSwap() noexcept;
    void endianCorrect() noexcept;
};

BEGIN_NAMESPACE(GameSettingUtils)

int32_t getGameSettingsVersionForDemoFileVersion(const int32_t demoFileVersion) noexcept;
int32_t getGameSettingsSize(const int32_t gameSettingsVersion) noexcept;
bool readAndMigrateGameSettings(const int32_t gameSettingsVersion, const void* const pSrcBuffer, GameSettings& dstSettings) noexcept;

END_NAMESPACE(GameSettingUtils)
