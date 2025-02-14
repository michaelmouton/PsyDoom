#include "o_main.h"

#include "Doom/Base/i_main.h"
#include "Doom/Base/i_misc.h"
#include "Doom/Base/i_texcache.h"
#include "Doom/Base/s_sound.h"
#include "Doom/Base/sounds.h"
#include "Doom/d_main.h"
#include "Doom/Game/g_game.h"
#include "Doom/Game/p_tick.h"
#include "Doom/Renderer/r_data.h"
#include "m_main.h"
#include "PsyDoom/Config/Config.h"
#include "PsyDoom/Game.h"
#include "PsyDoom/MapInfo/MapInfo.h"
#include "PsyDoom/PlayerPrefs.h"
#include "PsyDoom/PsxPadButtons.h"
#include "PsyDoom/Utils.h"
#include "PsyDoom/Video.h"
#include "PsyDoom/Vulkan/VRenderer.h"
#include "saveroot_main.h"
#include "Wess/psxspu.h"
#include "xoptions_main.h"

#include <algorithm>
#include <cmath>

// Available options and their names
enum option_t : int32_t {
    opt_music,
    opt_sound,
// PsyDoom: Removing the psx controller configuration menu and replacing with an 'extra options' menu.
// The previous 'password' slot now also directs to a load and save menu.
#if PSYDOOM_MODS
    opt_load,
    opt_load_save,
    opt_extra_options,
#else
    opt_password,
    opt_config,
#endif
    opt_main_menu,
    opt_restart
};

const char gOptionNames[][16] = {
    { "Music Volume"    },
    { "Sound Volume"    },
// PsyDoom: Removing the psx controller configuration menu and replacing with an 'extra options' menu.
// The previous 'password' menu item now also directs to a load and save menu.
#if PSYDOOM_MODS
    { "Load Game"       },
    { "Load And Save"   },
    { "Extra Options"   },
#else
    { "Password"        },
    { "Configuration"   },
#endif
    { "Main Menu"       },
    { "Restart Level"   }
};

// The layout for the options menu: outside of gameplay, in gameplay (single player) and in gameplay (multiplayer)
struct menuitem_t {
    option_t    option;
    int32_t     x;
    int32_t     y;
};

static const menuitem_t gOptMenuItems_MainMenu[] = {
    { opt_music,            62, 65  },
    { opt_sound,            62, 105 },
// PsyDoom: Removing the psx controller configuration menu and replacing with an 'extra options' menu.
// The previous 'password' menu item now also directs to a load and save menu.
#if PSYDOOM_MODS
    { opt_load,             62, 145 },
    { opt_extra_options,    62, 170 },
#else
    { opt_password,         62, 145 },
    { opt_config,           62, 170 },
#endif
    { opt_main_menu,        62, 195 },
};

static const menuitem_t gOptMenuItems_Single[] = {
    { opt_music,            62, 50  },
    { opt_sound,            62, 90  },
// PsyDoom: Removing the psx controller configuration menu and replacing with an 'extra options' menu.
// The previous 'password' menu item now also directs to a load and save menu.
#if PSYDOOM_MODS
    { opt_load_save,        62, 130 },
    { opt_extra_options,    62, 155 },
#else
    { opt_password,         62, 130 },
    { opt_config,           62, 155 },
#endif
    { opt_main_menu,        62, 180 },
    { opt_restart,          62, 205 },
};

static const menuitem_t gOptMenuItems_NetGame[] = {
    { opt_music,        62, 70  },
    { opt_sound,        62, 110 },
    { opt_main_menu,    62, 150 },
    { opt_restart,      62, 175 },
};

// Currently in-use options menu layout: items list and size
static int32_t              gOptionsMenuSize;
static const menuitem_t*    gpOptionsMenuItems;

// Texture used as a background for the options menu
texture_t gTex_OptionsBg;

// Current options music and sound volume
int32_t gOptionsSndVol = S_SND_DEFAULT_VOL;
int32_t gOptionsMusVol = S_MUS_DEFAULT_VOL;

// PsyDoom: a flag to specify whether we should unpause the game after running the options menu
#if PSYDOOM_MODS
    bool gbUnpauseAfterOptionsMenu;
#endif

//------------------------------------------------------------------------------------------------------------------------------------------
// Initializes the options menu
//------------------------------------------------------------------------------------------------------------------------------------------
void O_Init() noexcept {
    S_StartSound(nullptr, sfx_pistol);      // Bam!

    // PsyDoom: initially assume we don't unpause the game after the options menu is done
    #if PSYDOOM_MODS
        gbUnpauseAfterOptionsMenu = false;
    #endif

    // Initialize cursor position and vblanks until move for all players
    gCursorFrame = 0;

    for (int32_t playerIdx = 0; playerIdx < MAXPLAYERS; ++playerIdx) {
        gCursorPos[playerIdx] = 0;
        gVBlanksUntilMenuMove[playerIdx] = 0;
    }

    // Set what menu layout to use
    if (gNetGame != gt_single) {
        gpOptionsMenuItems = gOptMenuItems_NetGame;
        gOptionsMenuSize = C_ARRAY_SIZE(gOptMenuItems_NetGame);
    }
    else if (gbGamePaused) {
        gpOptionsMenuItems = gOptMenuItems_Single;
        gOptionsMenuSize = C_ARRAY_SIZE(gOptMenuItems_Single);
    }
    else {
        gpOptionsMenuItems = gOptMenuItems_MainMenu;
        gOptionsMenuSize = C_ARRAY_SIZE(gOptMenuItems_MainMenu);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Shuts down the options menu
//------------------------------------------------------------------------------------------------------------------------------------------
void O_Shutdown([[maybe_unused]] const gameaction_t exitAction) noexcept {
    // Reset the cursor position for all players
    for (int32_t playerIdx = 0; playerIdx < MAXPLAYERS; ++playerIdx) {
        gCursorPos[playerIdx] = 0;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Runs update logic for the options menu: does menu controls
//------------------------------------------------------------------------------------------------------------------------------------------
gameaction_t O_Control() noexcept {
    // PsyDoom: tick only if vblanks are registered as elapsed; this restricts the code to ticking at 30 Hz for NTSC
    #if PSYDOOM_MODS
        if (gPlayersElapsedVBlanks[0] <= 0) {
            gbKeepInputEvents = true;   // Don't consume 'key pressed' etc. events yet, not ticking...
            return ga_nothing;
        }
    #endif

    // Animate the skull cursor
    if ((gGameTic > gPrevGameTic) && ((gGameTic & 3) == 0)) {
        gCursorFrame ^= 1;
    }

    // Do options menu controls for all players
    for (int32_t playerIdx = MAXPLAYERS - 1; playerIdx >= 0; --playerIdx) {
        // Ignore this player if not in the game
        if (!gbPlayerInGame[playerIdx])
            continue;

        // Exit the menu if start or select is pressed.
        // Note: checking for buttons newly pressed (as opposed to just pressed) here is a change incorporated from Final Doom.
        // It's required due to some changes made in other UIs like the password screen.
        #if PSYDOOM_MODS
            const TickInputs& inputs = gTickInputs[playerIdx];
            const TickInputs& oldInputs = gOldTickInputs[playerIdx];

            const bool bMenuStart = (inputs.fMenuStart() && (!oldInputs.fMenuStart()));
            const bool bMenuBack = (inputs.fMenuBack() && (!oldInputs.fMenuBack()));
            const bool bMenuOk = (inputs.fMenuOk() && (!oldInputs.fMenuOk()));
            const bool bMenuUp = inputs.fMenuUp();
            const bool bMenuDown = inputs.fMenuDown();
            const bool bMenuLeft = inputs.fMenuLeft();
            const bool bMenuRight = inputs.fMenuRight();
            const bool bMenuMove = (bMenuUp || bMenuDown || bMenuLeft || bMenuRight);
        #else
            const padbuttons_t ticButtons = gTicButtons[playerIdx];
            const padbuttons_t oldTicButtons = gOldTicButtons[playerIdx];

            const bool bMenuStart = ((ticButtons != oldTicButtons) && (ticButtons & PAD_START));
            const bool bMenuBack = ((ticButtons != oldTicButtons) && (ticButtons & PAD_SELECT));
            const bool bMenuOk = (ticButtons & PAD_ACTION_BTNS);
            const bool bMenuUp = (ticButtons & PAD_UP);
            const bool bMenuDown = (ticButtons & PAD_DOWN);
            const bool bMenuLeft = (ticButtons & PAD_LEFT);
            const bool bMenuRight = (ticButtons & PAD_RIGHT);
            const bool bMenuMove = (ticButtons & PAD_DIRECTION_BTNS);
        #endif

        // Allow the start or select buttons to close the menu
        if (bMenuStart || bMenuBack) {
            S_StartSound(nullptr, sfx_pistol);

            // PsyDoom: should we unpause the game after closing the options menu?
            // Input events will be wiped by the screen transition, so we need to remember if an unpause was requested too here.
            // This code basically allows the 'Esc' key (in the default bindings) to function as both a 'pause' and 'menu back' key which toggles
            // directly between the options menu and the game, bypassing the intermediate step of the pause screen:
            #if PSYDOOM_MODS
                gbUnpauseAfterOptionsMenu = (inputs.fTogglePause() && (!oldInputs.fTogglePause()));
            #endif

            return ga_exit;
        }

        // Check for up/down movement
        if (!bMenuMove) {
            // If there are no direction buttons pressed then the next move is allowed instantly
            gVBlanksUntilMenuMove[playerIdx] = 0;
        } else {
            // Direction buttons pressed or held down, check to see if we can an up/down move now
            gVBlanksUntilMenuMove[playerIdx] -= gPlayersElapsedVBlanks[0];

            if (gVBlanksUntilMenuMove[playerIdx] <= 0) {
                gVBlanksUntilMenuMove[playerIdx] = 15;

                if (bMenuDown) {
                    gCursorPos[playerIdx]++;

                    if (gCursorPos[playerIdx] >= gOptionsMenuSize) {
                        gCursorPos[playerIdx] = 0;
                    }

                    // Note: only play sound for this user's player!
                    if (playerIdx == gCurPlayerIndex) {
                        S_StartSound(nullptr, sfx_pstop);
                    }
                }
                else if (bMenuUp) {
                    gCursorPos[playerIdx]--;

                    if (gCursorPos[playerIdx] < 0) {
                        gCursorPos[playerIdx] = gOptionsMenuSize - 1;
                    }

                    // Note: only play sound for this user's player!
                    if (playerIdx == gCurPlayerIndex) {
                        S_StartSound(nullptr, sfx_pstop);
                    }
                }
            }
        }

        // Handle option actions and adjustment
        const option_t option = gpOptionsMenuItems[gCursorPos[playerIdx]].option;

        switch (option) {
            // Change music volume
            case opt_music: {
                // Only process audio updates for this player
                if (playerIdx == gCurPlayerIndex) {
                    if (bMenuRight) {
                        gOptionsMusVol++;

                        if (gOptionsMusVol > S_MAX_VOL) {
                            gOptionsMusVol = S_MAX_VOL;
                        } else {
                            S_SetMusicVolume(doomToWessVol(gOptionsMusVol));

                            if (gOptionsMusVol & 1) {
                                S_StartSound(nullptr, sfx_stnmov);
                            }
                        }

                        gCdMusicVol = (gOptionsMusVol * PSXSPU_MAX_CD_VOL) / S_MAX_VOL;
                    }
                    else if (bMenuLeft) {
                        gOptionsMusVol--;

                        if (gOptionsMusVol < 0) {
                            gOptionsMusVol = 0;
                        } else {
                            S_SetMusicVolume(doomToWessVol(gOptionsMusVol));

                            if (gOptionsMusVol & 1) {
                                S_StartSound(nullptr, sfx_stnmov);
                            }
                        }

                        gCdMusicVol = (gOptionsMusVol * PSXSPU_MAX_CD_VOL) / S_MAX_VOL;
                    }
                }

                // PsyDoom: update the volume levels in the preferences module also, whenever they are edited in the options menu.
                // Want to save the correct values upon exiting the game.
                #if PSYDOOM_MODS
                    PlayerPrefs::pullSoundAndMusicPrefs();
                #endif
            }   break;

            // Change sound volume
            case opt_sound: {
                // Only process audio updates for this player
                if (playerIdx == gCurPlayerIndex) {
                    if (bMenuRight) {
                        gOptionsSndVol++;

                        if (gOptionsSndVol > S_MAX_VOL) {
                            gOptionsSndVol = S_MAX_VOL;
                        } else {
                            S_SetSfxVolume(doomToWessVol(gOptionsSndVol));

                            if (gOptionsSndVol & 1) {
                                S_StartSound(nullptr, sfx_stnmov);
                            }
                        }
                    }
                    else if (bMenuLeft) {
                        gOptionsSndVol--;

                        if (gOptionsSndVol < 0) {
                            gOptionsSndVol = 0;
                        } else {
                            S_SetSfxVolume(doomToWessVol(gOptionsSndVol));

                            if (gOptionsSndVol & 1) {
                                S_StartSound(nullptr, sfx_stnmov);
                            }
                        }
                    }
                }

                // PsyDoom: update the volume levels in the preferences module also, whenever they are edited in the options menu.
                // Want to save the correct values upon exiting the game.
                #if PSYDOOM_MODS
                    PlayerPrefs::pullSoundAndMusicPrefs();
                #endif
            }   break;

        // Password entry.
        // PsyDoom: this is now replaced by the 'load game' or 'load and save' menu option.
        #if PSYDOOM_MODS
            case opt_load:
            case opt_load_save: {
                // Note: disable this menu for the demo version of the game
                if (bMenuOk && (!Game::gbIsDemoVersion)) {
                    const gameaction_t action = MiniLoop(SaveRoot_Init, SaveRoot_Shutdown, SaveRoot_Update, SaveRoot_Draw);

                    if ((action == ga_warped) || (action == ga_exitmenus) || (action == ga_restart))
                        return action;
                }
            }   break;
        #else
            case opt_password: {
                if (bMenuOk) {
                    if (MiniLoop(START_PasswordScreen, STOP_PasswordScreen, TIC_PasswordScreen, DRAW_PasswordScreen) == ga_warped)
                        return ga_warped;
                }
            }   break;
        #endif

            // PsyDoom: Adding a new 'extra options' menu
            case opt_extra_options: {
                if (bMenuOk) {
                    MiniLoop(XOptions_Init, XOptions_Shutdown, XOptions_Update, XOptions_Draw);
                }
            }   break;

        // PsyDoom: Removing the psx controller configuration menu
        #if !PSYDOOM_MODS
            // Controller configuration
            case opt_config: {
                if (bMenuOk) {
                    MiniLoop(START_ControlsScreen, STOP_ControlsScreen, TIC_ControlsScreen, DRAW_ControlsScreen);
                }
            }   break;
        #endif

            // Main menu option
            case opt_main_menu: {
                if (bMenuOk) {
                    S_StartSound(nullptr, sfx_pistol);
                    return ga_exitdemo;
                }
            }   break;

            // Restart option
            case opt_restart: {
                if (bMenuOk) {
                    S_StartSound(nullptr, sfx_pistol);
                    return ga_restart;
                }
            }   break;
        }
    }

    return ga_nothing;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the options menu
//------------------------------------------------------------------------------------------------------------------------------------------
void O_Drawer() noexcept {
    // Increment the frame count for the texture cache and draw the background using the 'MARB01' sprite
    I_IncDrawnFrameCount();

    #if PSYDOOM_MODS
        Utils::onBeginUIDrawing();  // PsyDoom: UI drawing setup for the new Vulkan renderer
    #endif

    // Draw the options screen background
    O_DrawBackground(gTex_OptionsBg, Game::getTexClut_OptionsBg(), 128, 128, 128);

    // Don't do any rendering if we are about to exit the menu
    if (gGameAction == ga_nothing) {
        // Menu title
        I_DrawString(-1, 20, "Options");

        // Draw each menu item for the current options screen layout.
        // The available options will vary depending on game mode.
        const menuitem_t* pMenuItem = gpOptionsMenuItems;

        for (int32_t optIdx = 0; optIdx < gOptionsMenuSize; ++optIdx, ++pMenuItem) {
            // Draw the option label
            I_DrawString(pMenuItem->x, pMenuItem->y, gOptionNames[pMenuItem->option]);

            // If the option has a slider associated with it, draw that too
            if (pMenuItem->option <= opt_sound) {
                // Draw the slider backing/container
                I_DrawSprite(
                    gTex_STATUS.texPageId,
                    Game::getTexClut_STATUS(),
                    (int16_t)(pMenuItem->x + 13),
                    (int16_t)(pMenuItem->y + 20),
                    // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
                    #if PSYDOOM_MODS
                        (int16_t)(gTex_STATUS.texPageCoordX + 0),
                        (int16_t)(gTex_STATUS.texPageCoordY + 184),
                    #else
                        0,
                        184,
                    #endif
                    108,
                    11
                );

                // Draw the slider handle
                const int32_t sliderVal = (pMenuItem->option == opt_sound) ? gOptionsSndVol : gOptionsMusVol;

                I_DrawSprite(
                    gTex_STATUS.texPageId,
                    Game::getTexClut_STATUS(),
                    (int16_t)(pMenuItem->x + 14 + sliderVal),
                    (int16_t)(pMenuItem->y + 20),
                    // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
                    #if PSYDOOM_MODS
                        (int16_t)(gTex_STATUS.texPageCoordX + 108),
                        (int16_t)(gTex_STATUS.texPageCoordY + 184),
                    #else
                        108,
                        184,
                    #endif
                    6,
                    11
                );
            }
        }

        // Draw the skull cursor
        const int32_t cursorPos = gCursorPos[gCurPlayerIndex];
        const menuitem_t& menuItem = gpOptionsMenuItems[cursorPos];

        I_DrawSprite(
            gTex_STATUS.texPageId,
            Game::getTexClut_STATUS(),
            (int16_t) menuItem.x - 24,
            (int16_t) menuItem.y - 2,
            // PsyDoom: the STATUS texture atlas might not be at UV 0,0 anymore! (if limit removing, but always offset to be safe)
            #if PSYDOOM_MODS
                (int16_t)(gTex_STATUS.texPageCoordX + M_SKULL_TEX_U + (uint8_t) gCursorFrame * M_SKULL_W),
                (int16_t)(gTex_STATUS.texPageCoordY + M_SKULL_TEX_V),
            #else
                M_SKULL_TEX_U + (uint8_t) gCursorFrame * M_SKULL_W,
                M_SKULL_TEX_V,
            #endif
            M_SKULL_W,
            M_SKULL_H
        );
    }

    // PsyDoom: draw any enabled performance counters
    #if PSYDOOM_MODS
        I_DrawEnabledPerfCounters();
    #endif

    // Finish up the frame
    I_SubmitGpuCmds();
    I_DrawPresent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
// A helper that encapsulates the logic for drawing the background in the options menu and other menus
//------------------------------------------------------------------------------------------------------------------------------------------
void O_DrawBackground(
    texture_t& bgTex,
    const uint16_t bgTexClutId,
    const uint8_t colR,
    const uint8_t colG,
    const uint8_t colB
) noexcept {
    // PsyDoom: some additional enhancements over the original background drawing code.
    // 
    // (1) Add support for color tinting the background.
    // (2) For the Vulkan renderer add support for widescreen tiling if the current game or mod has this feature enabled.
    // (3) Fix slight seams when MSAA is enabled for the Vulkan Renderer by restricting the drawing area to powers of two.
    //     The Final Doom background tile unfortunately has some black/empty rows and columns at the edges of the texture (beyond the intended 64x64 area).
    //     When MSAA is enabled these bogus rows and columns tend to work their way into the rendered image as seams.
    //     Restricting the draw area to powers of two fixes this particular issue for Final Doom while avoiding hard-coding the tile size to 64x64 (useful for user mods).
    //
    #if PSYDOOM_MODS
        // Make sure the texture is cached first
        I_CacheTex(bgTex);

        // Abort if the background texture is junk!
        if ((bgTex.width <= 0) || (bgTex.height <= 0))
            return;

        // Helper to floor the specified dimension down to a power of two
        const auto pow2Floor = [](uint16_t value) noexcept {
            uint32_t highestSetBit = 0;
            value >>= 1;

            while (value > 0) {
                value >>= 1;
                highestSetBit++;
            }

            return (uint16_t)(1u << highestSetBit);
        };

        // Figure out how many tiles we need
        const uint16_t tileW = pow2Floor(bgTex.width);
        const uint16_t tileH = pow2Floor(bgTex.height);
        int32_t numTilesX = (SCREEN_W + tileW - 1) / tileW;
        int32_t numTilesY = (SCREEN_H + tileH - 1) / tileH;

        // Support widescreen if widescreen options menu tiling is enabled.
        // This allows user mods or new game types to implement menus that fully support widescreen.
        if (Video::isUsingVulkanRenderPath() && Config::gbVulkanWidescreenEnabled && MapInfo::getGameInfo().bAllowWideOptionsBg) {
            const int32_t extraSpaceAtSides = (int32_t) std::max(std::floor(VRenderer::gPsxCoordsFbX), 0.0f);
            const int32_t numExtraTilesAtSides = (extraSpaceAtSides + tileW - 1) / tileW;
            numTilesX += numExtraTilesAtSides * 2;
        }

        // Don't let this get too out of hand!
        numTilesX = std::min(numTilesX, 64);
        numTilesY = std::min(numTilesX, 16);

        // Figure out the x position to center all these tiles
        const int32_t startX = (SCREEN_W - numTilesX * tileW) / 2;
        const int32_t startY = 0;

        // Draw all the tiles!
        for (int tileY = 0; tileY < numTilesY; ++tileY) {
            for (int tileX = 0; tileX < numTilesX; ++tileX) {
                I_DrawColoredSprite(
                    bgTex.texPageId,
                    bgTexClutId,
                    (int16_t)(startX + tileX * tileW),
                    (int16_t)(startY + tileY * tileH),
                    bgTex.texPageCoordX,
                    bgTex.texPageCoordY,
                    tileW,
                    tileH,
                    colR,
                    colG,
                    colB,
                    false
                );
            }
        }
    #else
        // This is the original background drawing code
        for (int16_t y = 0; y < 4; ++y) {
            for (int16_t x = 0; x < 4; ++x) {
                I_CacheAndDrawSprite(bgTex, x * 64, y * 64, bgTexClutId);
            }
        }
    #endif
}
