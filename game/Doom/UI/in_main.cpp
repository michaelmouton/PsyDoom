#include "in_main.h"

#include "Doom/Base/i_drawstringex.h"
#include "Doom/Base/i_main.h"
#include "Doom/Base/i_misc.h"
#include "Doom/Base/i_texcache.h"
#include "Doom/Base/s_sound.h"
#include "Doom/Base/sounds.h"
#include "Doom/d_main.h"
#include "Doom/Game/g_game.h"
#include "Doom/Game/p_password.h"
#include "Doom/Game/p_tick.h"
#include "Doom/Renderer/r_data.h"
#include "m_main.h"
#include "PsyDoom/Config/Config.h"
#include "PsyDoom/Game.h"
#include "PsyDoom/Input.h"
#include "PsyDoom/PlayerPrefs.h"
#include "PsyDoom/PsxPadButtons.h"
#include "PsyDoom/Utils.h"
#include "pw_main.h"
#include "SmallString.h"
#include "st_main.h"
#include "Wess/psxcd.h"

struct pstats_t {
    int32_t     killpercent;
    int32_t     itempercent;
    int32_t     secretpercent;
    int32_t     fragcount;
};

// PsyDoom: these are now defined in the 'MapInfo' module and can be overriden for new user maps
#if !PSYDOOM_MODS
    const char gMapNames_Doom[][32] = {
        // Doom 1
        "Hangar",
        "Plant",
        "Toxin Refinery",
        "Command Control",
        "Phobos Lab",
        "Central Processing",
        "Computer Station",
        "Phobos Anomaly",
        "Deimos Anomaly",
        "Containment Area",
        "Refinery",
        "Deimos Lab",
        "Command Center",
        "Halls of the Damned",
        "Spawning Vats",
        "Hell Gate",
        "Hell Keep",
        "Pandemonium",
        "House of Pain",
        "Unholy Cathedral",
        "Mt. Erebus",
        "Limbo",
        "Tower Of Babel",
        "Hell Beneath",
        "Perfect Hatred",
        "Sever The Wicked",
        "Unruly Evil",
        "Unto The Cruel",
        "Twilight Descends",
        "Threshold of Pain",
        // Doom 2
        "Entryway",
        "Underhalls",
        "The Gantlet",
        "The Focus",
        "The Waste Tunnels",
        "The Crusher",
        "Dead Simple",
        "Tricks And Traps",
        "The Pit",
        "Refueling Base",
        "O of Destruction!",
        "The Factory",
        "The Inmost Dens",
        "Suburbs",
        "Tenements",
        "The Courtyard",
        "The Citadel",
        "Nirvana",
        "The Catacombs",
        "Barrels of Fun",
        "Bloodfalls",
        "The Abandoned Mines",
        "Monster Condo",
        "Redemption Denied",
        "Fortress of Mystery",
        "The Military Base",
        "The Marshes",
        "The Mansion",
        "Club Doom"
    };

    const char gMapNames_FinalDoom[][32] = {
        // Master Levels
        "Attack",
        "Virgil",
        "Canyon",
        "Combine",
        "Catwalk",
        "Fistula",
        "Geryon",
        "Minos",
        "Nessus",
        "Paradox",
        "Subspace",
        "Subterra",
        "Vesperas",
        // TNT
        "System Control",
        "Human Barbeque",
        "Wormhole",
        "Crater",
        "Nukage Processing",
        "Deepest Reaches",
        "Processing Area",
        "Lunar Mining Project",
        "Quarry",
        "Ballistyx",
        "Heck",
        // Plutonia
        "Congo",
        "Aztec",
        "Ghost Town",
        "Baron's Lair",
        "The Death Domain",
        "Onslaught"
    };

    static_assert(C_ARRAY_SIZE(gMapNames_Doom) == 59);
    static_assert(C_ARRAY_SIZE(gMapNames_FinalDoom) == 30);

#endif  // #if !PSYDOOM_MODS

// The final stats to display for each player
static pstats_t gPStats[MAXPLAYERS];

// The currently displaying stats for each player, this is ramped up over time
static int32_t  gKillValue[MAXPLAYERS];
static int32_t  gItemValue[MAXPLAYERS];
static int32_t  gSecretValue[MAXPLAYERS];
static int32_t  gFragValue[MAXPLAYERS];

// What stage of the intermission we are at.
// 0 = ramping up score, 1 = score fully shown, 2 = begin exiting intermission.
static int32_t  gIntermissionStage;

#if PSYDOOM_MODS
    // PsyDoom: a flag which allows hiding of the 'Entering <MAP_NAME>' message and the password
    bool gbIntermissionHideNextMap;

    // PsyDoom: an override for which network game type is displayed on the intermission screen.
    // If not specified ('gt_none') then the actual network game type will be shown.
    // Used by demo playback so that it can operate the game in single player mode on the intermission screen but still show multiplayer results.
    gametype_t gIntermissionNetGameTypeOverride = gt_none;
#endif

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom helper: encapsulates the logic for getting the network game type to show on the intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
static gametype_t IN_GetNetGameType() noexcept {
    // PsyDoom: demo playback can make the intermission display a different (than real) network game type
    #if PSYDOOM_MODS
        if (gIntermissionNetGameTypeOverride != gt_none)
            return gIntermissionNetGameTypeOverride;
    #endif

    return gNetGame;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Initialization/setup logic for the intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_Start() noexcept {
    // Clear out purgable textures and cache the background for the intermission
    I_PurgeTexCache();
    I_LoadAndCacheTexLump(gTex_BACK, Game::getTexLumpName_Inter_BACK());

    // Initialize final and displayed stats for all players
    for (int32_t playerIdx = 0; playerIdx < MAXPLAYERS; ++playerIdx) {
        const player_t& player = gPlayers[playerIdx];
        pstats_t& pstats = gPStats[playerIdx];

        gKillValue[playerIdx] = 0;
        gItemValue[playerIdx] = 0;
        gSecretValue[playerIdx] = 0;
        gFragValue[playerIdx] = 0;

        if (gTotalKills != 0) {
            pstats.killpercent = (player.killcount * 100) / gTotalKills;
        } else {
            pstats.killpercent = 100;
        }

        if (gTotalItems != 0) {
            pstats.itempercent = (player.itemcount * 100) / gTotalItems;
        } else {
            pstats.itempercent = 100;
        }

        if (gTotalSecret != 0) {
            pstats.secretpercent = (player.secretcount * 100) / gTotalSecret;
        } else {
            pstats.secretpercent = 100;
        }

        if (IN_GetNetGameType() == gt_deathmatch) {
            pstats.fragcount = player.frags;
        }
    }

    // Init menu timeout and intermission stage
    gIntermissionStage = 0;
    gMenuTimeoutStartTicCon = gTicCon;

    // Compute the password for the next map and mark it as entered (so we don't do a pistol start)
    if (gNextMap <= Game::getNumMaps()) {
        P_ComputePassword(gPasswordCharBuffer);
        gNumPasswordCharsEntered = 10;

        // PsyDoom: remember this password in player prefs, so it's restored on relaunch
        #if PSYDOOM_MODS
            PlayerPrefs::pullLastPassword();
        #endif
    }

    // Play the intermission cd track
    psxcd_play_at_andloop(
        gCDTrackNum[cdmusic_intermission],
        gCdMusicVol,
        0,
        0,
        gCDTrackNum[cdmusic_intermission],
        gCdMusicVol,
        0,
        0
    );

    // Wait until some cd audio has been read
    Utils::waitForCdAudioPlaybackStart();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Shutdown logic for the intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_Stop([[maybe_unused]] const gameaction_t exitAction) noexcept {
    // PsyDoom: if quitting the app then exit immediately
    #if PSYDOOM_MODS
        if (Input::isQuitRequested())
            return;
    #endif

    IN_Drawer();
    psxcd_stop();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Update logic for the intermission screen.
// Adavnces the counters and checks for user input to skip to the next intermission stage.
//------------------------------------------------------------------------------------------------------------------------------------------
gameaction_t IN_Ticker() noexcept {
    // PsyDoom: tick only if vblanks are registered as elapsed; this restricts the code to ticking at 30 Hz for NTSC
    #if PSYDOOM_MODS
        if (gPlayersElapsedVBlanks[0] <= 0) {
            gbKeepInputEvents = true;   // Don't consume 'key pressed' etc. events yet, not ticking...
            return ga_nothing;
        }
    #endif

    // Intermission pauses for 1 second (60 vblanks) initially to stop accidental button presses
    if (gTicCon - gMenuTimeoutStartTicCon <= 60)
        return ga_nothing;

    // Checking for inputs from all players to speed up the intermission
    const gametype_t netGameType = IN_GetNetGameType();

    for (int32_t playerIdx = 0; playerIdx < MAXPLAYERS; ++playerIdx) {
        #if PSYDOOM_MODS
            const TickInputs& inputs = gTickInputs[playerIdx];
            const TickInputs& oldInputs = gOldTickInputs[playerIdx];
            const bool bMenuOk = (inputs.fMenuOk() && (!oldInputs.fMenuOk()));
        #else
            const padbuttons_t ticButtons = gTicButtons[playerIdx];
            const padbuttons_t oldTicButtons = gOldTicButtons[playerIdx];
            const bool bMenuOk = ((ticButtons != oldTicButtons) && (ticButtons & PAD_ACTION_BTNS));
        #endif

        // Handle the player trying to goto to the next stage of the intermission when action buttons are pressed
        if (bMenuOk) {
            // Advance to the next stage of the intermission
            gIntermissionStage++;

            // If we are skipping to the stats being fully shown then fully display them now
            if (gIntermissionStage == 1) {
                for (int32_t i = 0; i < MAXPLAYERS; ++i) {
                    const pstats_t& stats = gPStats[i];

                    gKillValue[i] = stats.killpercent;
                    gItemValue[i] = stats.itempercent;
                    gSecretValue[i] = stats.secretpercent;
                    gFragValue[i] = stats.fragcount;
                }

                S_StartSound(nullptr, sfx_barexp);
            }

            // If we are at the stage where the intermission can be exited do that now
            if (gIntermissionStage >= 2) {
                S_StartSound(nullptr, sfx_barexp);
                return ga_died;
            }
        }

        // If a single player game only check the first player for skip inputs
        if (netGameType == gt_single)
            break;
    }

    // If a full 15Hz tick has not yet elapsed then do not advance the count
    if (gGameTic <= gPrevGameTic)
        return ga_nothing;

    // Count up the applicable stats for the current game mode.
    // The step is increments of '2' and if we go past our target then we clamp in each case.
    bool bStillCounting = false;

    for (int32_t playerIdx = 0; playerIdx < MAXPLAYERS; ++playerIdx) {
        const pstats_t& stats = gPStats[playerIdx];

        if (netGameType == gt_deathmatch) {
            // Deathmatch game: note that frag count can count both downwards and upwards
            if (stats.fragcount < 0) {
                if (gFragValue[playerIdx] > stats.fragcount) {
                    gFragValue[playerIdx] -= 2;
                    bStillCounting = true;

                    if (gFragValue[playerIdx] < stats.fragcount) {
                        gFragValue[playerIdx] = stats.fragcount;
                    }
                }
            } else {
                if (gFragValue[playerIdx] < stats.fragcount) {
                    gFragValue[playerIdx] += 2;
                    bStillCounting = true;

                    if (gFragValue[playerIdx] > stats.fragcount) {
                        gFragValue[playerIdx] = stats.fragcount;
                    }
                }
            }
        }
        else {
            // Single player or co-op game
            if (gKillValue[playerIdx] < stats.killpercent) {
                gKillValue[playerIdx] += 2;
                bStillCounting = true;

                if (gKillValue[playerIdx] > stats.killpercent) {
                    gKillValue[playerIdx] = stats.killpercent;
                }
            }

            if (gItemValue[playerIdx] < stats.itempercent) {
                gItemValue[playerIdx] += 2;
                bStillCounting = true;

                if (gItemValue[playerIdx] > stats.itempercent) {
                    gItemValue[playerIdx] = stats.itempercent;
                }
            }

            if (gSecretValue[playerIdx] < stats.secretpercent) {
                gSecretValue[playerIdx] += 2;
                bStillCounting = true;

                if (gSecretValue[playerIdx] > stats.secretpercent) {
                    gSecretValue[playerIdx] = stats.secretpercent;
                }
            }
        }
    }

    // If the ramp up is done and we are on the initial stage then advance to the next and do the explode sfx
    if ((!bStillCounting) && (gIntermissionStage == 0)) {
        gIntermissionStage = 1;
        S_StartSound(nullptr, sfx_barexp);
    }

    // Do periodic gun shot sounds (every 2nd tick) while the count up is happening
    if (bStillCounting && ((gGameTic & 1) == 0)) {
        S_StartSound(nullptr, sfx_pistol);
    }

    return ga_nothing;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_Drawer() noexcept {
    I_IncDrawnFrameCount();

    #if PSYDOOM_MODS
        Utils::onBeginUIDrawing();  // PsyDoom: UI drawing setup for the new Vulkan renderer
    #endif

    const gametype_t netGameType = IN_GetNetGameType();

    if (netGameType == gt_coop) {
        IN_CoopDrawer();
    } else if (netGameType == gt_deathmatch) {
        IN_DeathmatchDrawer();
    } else {
        IN_SingleDrawer();
    }

    // PsyDoom: draw any enabled performance counters
    #if PSYDOOM_MODS
        I_DrawEnabledPerfCounters();
    #endif

    I_SubmitGpuCmds();
    I_DrawPresent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the single player intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_SingleDrawer() noexcept {
    I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_Inter_BACK());

    // PsyDoom: should we show the time taken to complete the level?
    #if PSYDOOM_MODS
        const bool bShowTime = Config::gbEnableSinglePlayerLevelTimer;
    #endif

    // PsyDoom: aligning these slightly better so that their right side is flush with the 'kills' label
    #if PSYDOOM_MODS
        const int32_t itemsX = 54;
        const int32_t secretsX = 28;
    #else
        const int32_t itemsX = 53;
        const int32_t secretsX = 26;
    #endif

    // These are most of the coordinates for the regular/original menu layout
    int32_t percentSignX = 182;
    int32_t percentValueX = 170;
    int32_t mapNameAndFinishedMsgY = 36;
    int32_t killsY = 65;
    int32_t itemsY = 91;
    int32_t secretsY = 117;

    // Use a slightly different menu layout if we are showing the time.
    // Compact a little to make room for the time field...
    #if PSYDOOM_MODS
        if (bShowTime) {
            percentSignX = 202;
            percentValueX = 190;
            mapNameAndFinishedMsgY = 30;
            killsY = 60;
            itemsY = 80;
            secretsY = 100;
        }

        const int32_t timeY = 120;
    #endif

    #if PSYDOOM_MODS
        // PsyDoom: new drawing method that supports multi-line map names with line break commands in them ("\n")
        {
            char mapNameText[256];
            std::snprintf(mapNameText, C_ARRAY_SIZE(mapNameText), "%s\\nFinished", Game::getMapName(gGameMap).c_str().data());

            // Note: nudge the text up a bit if showing 2 lines for the map name (makes it not so close to the stats)
            int32_t textNudgeUp = 0;

            if (I_DrawStringEx_CountNumLines(mapNameText) > 2) {
                textNudgeUp = (bShowTime) ? 2 : 7;
            }

            DrawStringParams drawStrParams = {};
            drawStrParams.xAnchorMode = DrawStringAnchorMode::CENTER;
            drawStrParams.yAnchorMode = DrawStringAnchorMode::CENTER;
            I_DrawStringEx(SCREEN_W / 2, mapNameAndFinishedMsgY - textNudgeUp, drawStrParams, mapNameText);
        }
    #else
        I_DrawString(-1, 20, Game::getMapName(gGameMap).c_str().data());
        I_DrawString(-1, 36, "Finished");
    #endif

    I_DrawString(57, killsY, "Kills");
    I_DrawString(percentSignX, killsY, "%");
    I_DrawNumber(percentValueX, killsY, gKillValue[0]);

    I_DrawString(itemsX, itemsY, "Items");
    I_DrawString(percentSignX, itemsY, "%");
    I_DrawNumber(percentValueX, itemsY, gItemValue[0]);

    I_DrawString(secretsX, secretsY, "Secrets");
    I_DrawString(percentSignX, secretsY, "%");
    I_DrawNumber(percentValueX, secretsY, gSecretValue[0]);

    // Showing the time?
    #if PSYDOOM_MODS
        if (bShowTime) {
            // Get the components of time elapsed and restrict the maximum time display to 999.59.99
            constexpr int64_t MAX_TIME = 5997639;
            int32_t hundreths = (int32_t) std::min(Game::getLevelFinishTimeCentisecs(), MAX_TIME);
            int32_t seconds = hundreths / 100; hundreths %= 100;
            int32_t minutes = seconds / 60; seconds %= 60;

            // Format the time string and figure out how long it is
            char timeString[256];
            std::snprintf(timeString, C_ARRAY_SIZE(timeString), "%d.%02d.%02d", minutes, seconds, hundreths);
            const int32_t timeStringWidth = I_GetStringWidth(timeString);

            // Show the time elapsed
            I_DrawString(64, timeY, "Time");
            I_DrawString(213 - timeStringWidth, timeY, timeString);
        }
    #endif

    // Only draw the next map and password if there is a next map
    if (gNextMap <= Game::getNumMaps()) {
        // PsyDoom: the next map and password can now be hidden via MAPINFO for the last map in the cluster
        #if PSYDOOM_MODS
            const bool bShowNextMap = (!gbIntermissionHideNextMap);
        #else
            const bool bShowNextMap = true;
        #endif

        if (bShowNextMap) {
            I_DrawString(-1, 145, "Entering");

            #if PSYDOOM_MODS
                int32_t passwordYOffset = 0;

                // PsyDoom: new drawing method that supports multi-line map names with line break commands in them ("\n")
                {
                    DrawStringParams drawStrParams = {};
                    drawStrParams.xAnchorMode = DrawStringAnchorMode::CENTER;
                    I_DrawStringEx(SCREEN_W / 2, 161, drawStrParams, Game::getMapName(gNextMap).c_str().data());

                    // Move the password down a bit if the map name is on 2 or more lines instead of 1
                    if (gDrawStringEx_metrics.numLines >= 2) {
                        passwordYOffset += 12;
                    }
                }
            #else
                constexpr int32_t passwordYOffset = 0;
                I_DrawString(-1, 161, Game::getMapName(gNextMap).c_str().data());
            #endif

            I_DrawString(-1, 187 + passwordYOffset, "Password");

            char passwordStr[PW_SEQ_LEN + 1];

            for (int32_t i = 0; i < PW_SEQ_LEN; ++i) {
                passwordStr[i] = gPasswordChars[gPasswordCharBuffer[i]];
            }

            passwordStr[PW_SEQ_LEN] = 0;
            I_DrawString(-1, 203 + passwordYOffset, passwordStr);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the cooperative mode intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_CoopDrawer() noexcept {
    I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_Inter_BACK());

    // PsyDoom: nudge everything up to make room if showing 2 lines or more for the map name
    #if PSYDOOM_MODS
        const int32_t layoutNudgeUp = (I_DrawStringEx_CountNumLines(Game::getMapName(gNextMap).c_str().data()) >= 2) ? BIG_FONT_LINE_HEIGHT : 0;
    #else
        constexpr int32_t layoutNudgeUp = 0;
    #endif

    I_DrawSprite(
        gTex_STATUS.texPageId,
        Game::getTexClut_STATUS(),
        139,
        (int16_t)(20 - layoutNudgeUp),
        // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
        #if PSYDOOM_MODS
            (int16_t)(gTex_STATUS.texPageCoordX + gFaceSprites[0].texU),
            (int16_t)(gTex_STATUS.texPageCoordY + gFaceSprites[0].texV),
        #else
            gFaceSprites[0].texU,
            gFaceSprites[0].texV,
        #endif
        gFaceSprites[0].w,
        gFaceSprites[0].h
    );

    I_DrawString(130, 52 - layoutNudgeUp, "you");

    I_DrawSprite(
        gTex_STATUS.texPageId,
        Game::getTexClut_STATUS(),
        213,
        (int16_t)(20 - layoutNudgeUp),
        // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
        #if PSYDOOM_MODS
            (int16_t)(gTex_STATUS.texPageCoordX + gFaceSprites[0].texU),
            (int16_t)(gTex_STATUS.texPageCoordY + gFaceSprites[0].texV),
        #else
            gFaceSprites[0].texU,
            gFaceSprites[0].texV,
        #endif
        gFaceSprites[0].w,
        gFaceSprites[0].h
    );

    I_DrawString(208, 52 - layoutNudgeUp, "him");

    I_DrawString(57, 79 - layoutNudgeUp, "Kills");
    I_DrawString(155, 79 - layoutNudgeUp, "%");
    I_DrawString(228, 79 - layoutNudgeUp, "%");
    I_DrawNumber(143, 79 - layoutNudgeUp, gKillValue[gCurPlayerIndex]);
    I_DrawNumber(216, 79 - layoutNudgeUp, gKillValue[(gCurPlayerIndex == 0) ? 1 : 0]);

    I_DrawString(53, 101 - layoutNudgeUp, "Items");
    I_DrawString(155, 101 - layoutNudgeUp, "%");
    I_DrawString(228, 101 - layoutNudgeUp, "%");
    I_DrawNumber(143, 101 - layoutNudgeUp, gItemValue[gCurPlayerIndex]);
    I_DrawNumber(216, 101 - layoutNudgeUp, gItemValue[(gCurPlayerIndex == 0) ? 1 : 0]);

    I_DrawString(26, 123 - layoutNudgeUp, "Secrets");
    I_DrawString(155, 123 - layoutNudgeUp, "%");
    I_DrawString(228, 123 - layoutNudgeUp, "%");
    I_DrawNumber(143, 123 - layoutNudgeUp, gSecretValue[gCurPlayerIndex]);
    I_DrawNumber(216, 123 - layoutNudgeUp, gSecretValue[(gCurPlayerIndex == 0) ? 1 : 0]);

    // Only draw the next map and password if there is a next map
    if (gNextMap <= Game::getNumMaps()) {
        // PsyDoom: the next map and password can now be hidden via MAPINFO for the last map in the cluster
        #if PSYDOOM_MODS
            const bool bShowNextMap = (!gbIntermissionHideNextMap);
        #else
            const bool bShowNextMap = true;
        #endif

        if (bShowNextMap) {
            I_DrawString(-1, 149 - layoutNudgeUp, "Entering");

            // PsyDoom: new drawing method that supports multi-line map names with line break commands in them ("\n")
            #if PSYDOOM_MODS
            {
                DrawStringParams drawStrParams = {};
                drawStrParams.xAnchorMode = DrawStringAnchorMode::CENTER;
                I_DrawStringEx(SCREEN_W / 2, 165 - layoutNudgeUp, drawStrParams, Game::getMapName(gNextMap).c_str().data());
            }
            #else
                I_DrawString(-1, 165 - layoutNudgeUp, Game::getMapName(gNextMap).c_str().data());
            #endif

            // Well this is mean! The current player only gets to see a password if not dead :(
            if (gPlayers[gCurPlayerIndex].health > 0) {
                I_DrawString(-1, 191, "Password");

                char passwordStr[PW_SEQ_LEN + 1];

                for (int32_t i = 0; i < PW_SEQ_LEN; ++i) {
                    passwordStr[i] = gPasswordChars[gPasswordCharBuffer[i]];
                }

                passwordStr[PW_SEQ_LEN] = 0;
                I_DrawString(-1, 207, passwordStr);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the deathmatch mode intermission screen
//------------------------------------------------------------------------------------------------------------------------------------------
void IN_DeathmatchDrawer() noexcept {
    I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_Inter_BACK());

    // PsyDoom: new drawing method that supports multi-line map names with line break commands in them ("\n")
    #if PSYDOOM_MODS
    {
        const auto mapName = Game::getMapName(gGameMap).c_str();
        char drawStr[256];
        std::snprintf(drawStr, C_ARRAY_SIZE(drawStr), "%s\\nFinished", mapName.data());

        const int32_t textNudgeUp = (I_DrawStringEx_CountNumLines(mapName.data()) >= 2) ? 5 : 0;
        DrawStringParams drawStrParams = {};
        drawStrParams.xAnchorMode = DrawStringAnchorMode::CENTER;
        drawStrParams.yAnchorMode = DrawStringAnchorMode::CENTER;

        I_DrawStringEx(SCREEN_W / 2, 36 - textNudgeUp, drawStrParams, drawStr);
    }
    #else
        I_DrawString(-1, 20, Game::getMapName(gGameMap).c_str().data());
        I_DrawString(-1, 36, "Finished");
    #endif

    const facesprite_t* pFaceSpriteP1;
    const facesprite_t* pFaceSpriteP2;

    if (gFragValue[0] > gFragValue[1]) {
        if (gCurPlayerIndex == 0) {
            pFaceSpriteP1 = &gFaceSprites[EVILFACE];
            pFaceSpriteP2 = &gFaceSprites[DEADFACE];
        } else {
            pFaceSpriteP1 = &gFaceSprites[DEADFACE];
            pFaceSpriteP2 = &gFaceSprites[EVILFACE];
        }
    }
    else if (gFragValue[0] < gFragValue[1]) {
        if (gCurPlayerIndex == 0) {
            pFaceSpriteP1 = &gFaceSprites[DEADFACE];
            pFaceSpriteP2 = &gFaceSprites[EVILFACE];
        } else {
            pFaceSpriteP1 = &gFaceSprites[EVILFACE];
            pFaceSpriteP2 = &gFaceSprites[DEADFACE];
        }
    }
// PsyDoom: This check will always evaluate to 'true' so help compilers which might warn we are using unitialized vars...
#if PSYDOOM_MODS
    else {
#else
    else if (gFragValue[0] == gFragValue[1]) {
#endif
        pFaceSpriteP1 = &gFaceSprites[0];
        pFaceSpriteP2 = &gFaceSprites[0];
    }

    I_DrawSprite(
        gTex_STATUS.texPageId,
        Game::getTexClut_STATUS(),
        127,
        70,
        // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
        #if PSYDOOM_MODS
            (int16_t)(gTex_STATUS.texPageCoordX + pFaceSpriteP1->texU),
            (int16_t)(gTex_STATUS.texPageCoordY + pFaceSpriteP1->texV),
        #else
            pFaceSpriteP1->texU,
            pFaceSpriteP1->texV,
        #endif
        pFaceSpriteP1->w,
        pFaceSpriteP1->h
    );

    I_DrawString(118, 102, "you");

    I_DrawSprite(
        gTex_STATUS.texPageId,
        Game::getTexClut_STATUS(),
        200,
        70,
        // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
        #if PSYDOOM_MODS
            (int16_t)(gTex_STATUS.texPageCoordX + pFaceSpriteP2->texU),
            (int16_t)(gTex_STATUS.texPageCoordY + pFaceSpriteP2->texV),
        #else
            pFaceSpriteP2->texU,
            pFaceSpriteP2->texV,
        #endif
        pFaceSpriteP2->w,
        pFaceSpriteP2->h
    );

    I_DrawString(195, 102, "him");

    I_DrawString(35, 138, "Frags");
    I_DrawNumber(133, 138, gFragValue[gCurPlayerIndex]);
    I_DrawNumber(206, 138, gFragValue[(gCurPlayerIndex == 0) ? 1 : 0]);

    // Only draw the next map if there is one
    if (gNextMap <= Game::getNumMaps()) {
        // PsyDoom: the next map can now be hidden via MAPINFO for the last map in the cluster
        #if PSYDOOM_MODS
            const bool bShowNextMap = (!gbIntermissionHideNextMap);
        #else
            const bool bShowNextMap = true;
        #endif

        if (bShowNextMap) {
            // PsyDoom: new drawing method that supports multi-line map names with line break commands in them ("\n")
            #if PSYDOOM_MODS
                const auto mapName = Game::getMapName(gNextMap).c_str();
                char drawStr[256];
                std::snprintf(drawStr, C_ARRAY_SIZE(drawStr), "Entering\\n%s", mapName.data());

                const int32_t textNudgeUp = (I_DrawStringEx_CountNumLines(mapName.data()) >= 2) ? 7 : 0;
                DrawStringParams drawStrParams = {};
                drawStrParams.xAnchorMode = DrawStringAnchorMode::CENTER;
                drawStrParams.yAnchorMode = DrawStringAnchorMode::CENTER;

                I_DrawStringEx(SCREEN_W / 2, 206 - textNudgeUp, drawStrParams, drawStr);
            #else
                I_DrawString(-1, 190, "Entering");
                I_DrawString(-1, 206, Game::getMapName(gNextMap).c_str().data());
            #endif
        }
    }
}
