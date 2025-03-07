#include "m_main.h"

#include "Doom/Base/i_crossfade.h"
#include "Doom/Base/i_main.h"
#include "Doom/Base/i_misc.h"
#include "Doom/Base/i_texcache.h"
#include "Doom/Base/s_sound.h"
#include "Doom/Base/sounds.h"
#include "Doom/d_main.h"
#include "Doom/Game/g_game.h"
#include "Doom/Game/p_tick.h"
#include "Doom/Renderer/r_data.h"
#include "o_main.h"
#include "PsyDoom/Game.h"
#include "PsyDoom/Input.h"
#include "PsyDoom/MapInfo/GecMapInfo.h"
#include "PsyDoom/MapInfo/MapInfo.h"
#include "PsyDoom/Network.h"
#include "PsyDoom/PsxPadButtons.h"
#include "PsyDoom/Utils.h"
#include "PsyQ/LIBGPU.h"
#include "SmallString.h"
#include "Wess/psxcd.h"

#include <algorithm>

texture_t   gTex_BACK;                  // The background texture for the main menu (and intermission)
texture_t   gTex_DOOM;                  // The texture for the 'DOOM' logo (PsyDoom: this is now only used for the GEC ME Beta 3 title screen, usage here is replaced by the episode logo list)
int32_t     gCursorPos[MAXPLAYERS];     // Which of the menu options each player's cursor is over (see 'menu_t')
int32_t     gCursorFrame;               // Current frame that the menu cursor is displaying
int32_t     gMenuTimeoutStartTicCon;    // Tick that we start checking for menu timeouts from

#if PSYDOOM_MODS
    texture_t gTex_DATA;     // GEC Master Edition (Beta 3): sprite atlas for the title screen containing the text 'MASTER EDITION'
    texture_t gTex_MELOGO;   // GEC Master Edition (Beta 4 and later): sprite logo saying "MASTER EDITION", which is shown on the main menu
#endif

// Main menu options
enum menu_t : int32_t {
    gamemode,
    level,
    difficulty,
    options,
#if PSYDOOM_MODS        // PsyDoom: add a quit option to the main menu
    menu_quit,
#endif
    NUMMENUITEMS
};

#if PSYDOOM_MODS
// PsyDoom: represents a menu element for dynamic layout purposes.
// Defines the y position and height of the element.
struct MenuElem {
    int32_t height;
    int32_t yPos;
};

// What types of menu elements there are for dynamic layout
enum MenuElemType : uint32_t {
    MenuElem_MasterEditionSpritePrePad = 0,
    MenuElem_MasterEditionSprite,
    MenuElem_LogoPrePad,
    MenuElem_Logo,
    MenuElem_TextPrePad,
    MenuElem_GameMode_Line1,
    MenuElem_GameMode_Line2,
    MenuElem_Level,
    MenuElem_Difficulty_Line1,
    MenuElem_Difficulty_Line2,
    MenuElem_Options,
    MenuElem_Quit,
    MenuElem_TextPostPad,
    NUM_MENU_ELEM_TYPES
};

// Layout data for all the menu elements
static MenuElem gMenuElems[NUM_MENU_ELEM_TYPES] = {};

// The minimum and maximum spacing between options
static constexpr int32_t MAX_OPTION_SPACING = 22;
static constexpr int32_t MIN_OPTION_SPACING = 17;

// PsyDoom: holds info for an episode logo (lump name and texture)
struct EpisodeLogo {
    String8     lumpName;
    texture_t   tex;
};

// PsyDoom: a list of all episode logos (only 1 entry per unique lump name)
static std::vector<EpisodeLogo> gEpisodeLogos;

#endif  // #if PSYDOOM_MODS

// The position of each main menu option.
// PsyDoom: this is replaced by the 'gMenuElems' array and dynamic layout.
#if !PSYDOOM_MODS
    static const int16_t gMenuYPos[NUMMENUITEMS] = {
        91,     // gamemode
        133,    // level
        158,    // difficulty
        200     // options
    };
#endif

// Game mode names and skill names
static const char gGameTypeNames[NUMGAMETYPES][16] = {
    "Single Player",
    "Cooperative",
    "Deathmatch"
};

static const char gSkillNames[NUMSKILLS][16] = {
    "I am a Wimp",
    "Not Too Rough",
    "Hurt Me Plenty",
    "Ultra Violence",
#if PSYDOOM_MODS        // PsyDoom: exposing the unused 'Nightmare' mode
    "Nightmare!"
#endif
};

// Restricts what maps or episodes the player can pick
static int32_t gMaxStartEpisodeOrMap;

#if PSYDOOM_MODS
//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom: performs layout for the menu using the specified spacing between the options.
// If the layout doesn't fit then tries a more compact layout, if possible.
//------------------------------------------------------------------------------------------------------------------------------------------
static void M_DoMenuLayout(const int32_t optionSpacing) noexcept {
    // Default init all menu elements
    for (MenuElem& menuElem : gMenuElems) {
        menuElem = {};
    }

    // The size of menu elements that are always present
    gMenuElems[MenuElem_LogoPrePad].height = 8;
    gMenuElems[MenuElem_Logo].height = 70;  // Allow a fixed 70px maximum height for the logo (stays constant no matter the content)
    gMenuElems[MenuElem_TextPrePad].height = 4;
    gMenuElems[MenuElem_GameMode_Line1].height = optionSpacing;
    gMenuElems[MenuElem_GameMode_Line2].height = std::min(optionSpacing, 20);
    gMenuElems[MenuElem_Level].height = optionSpacing;
    gMenuElems[MenuElem_Difficulty_Line1].height = optionSpacing;
    gMenuElems[MenuElem_Difficulty_Line2].height = std::min(optionSpacing, 20);
    gMenuElems[MenuElem_Options].height = optionSpacing;
    gMenuElems[MenuElem_Quit].height = optionSpacing;
    gMenuElems[MenuElem_TextPostPad].height = 8;

    // Allocate room for the 'Master Edition' sprite logo for the GEC ME (8px maximum height).
    // Also reduce the padding before the logo to make more room for it.
    if (Game::isGameTypeGecMe()) {
        const int32_t spriteH = (GecMapInfo::shouldUseGecMapInfo()) ? gTex_MELOGO.height : 8;   // For GEC ME Beta 3 we have to hard code the height of this sprite!
        gMenuElems[MenuElem_MasterEditionSpritePrePad].height = 2;
        gMenuElems[MenuElem_MasterEditionSprite].height = spriteH;
        gMenuElems[MenuElem_LogoPrePad].height = 2;
    }

    // Determine the height of all the elements
    int32_t allElemsH = 0;

    for (const MenuElem& menuElem : gMenuElems) {
        allElemsH += menuElem.height;
    }

    // If it doesn't all fit then try and do tighter spacing (if possible)
    if (allElemsH > SCREEN_H) {
        if (optionSpacing > MIN_OPTION_SPACING) {
            M_DoMenuLayout(optionSpacing - 1);
            return;
        }
    }

    // Center all the content in the middle of the screen and begin positioning the elements
    int32_t y = (SCREEN_H - allElemsH) / 2;

    for (MenuElem& menuElem : gMenuElems) {
        menuElem.yPos = y;
        y += menuElem.height;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom utility: loads and caches all important/required menu and UI related texture resources.
// This needs to be called when we warp directly to a map on startup, since we miss important textures being cached via the skipped menus.
// If these textures are not cached then there might be crashes later when we try to draw them...
//------------------------------------------------------------------------------------------------------------------------------------------
static void M_LoadAndCacheRequiredUITextures() noexcept {
    I_PurgeTexCache();
    I_LoadAndCache_LOADING_TexLump(gTex_LOADING);
    I_LoadAndCacheTexLump(gTex_NETERR, "NETERR", 0);
    I_LoadAndCacheTexLump(gTex_PAUSE, "PAUSE", 0);
    I_LoadAndCacheTexLump(gTex_BACK, Game::getTexLumpName_BACK());

    #if PSYDOOM_MODS
        // PsyDoom: there is now a list of episode logos instead of just 1
        for (EpisodeLogo& episodeLogo : gEpisodeLogos) {
            I_LoadAndCacheTexLump(episodeLogo.tex, episodeLogo.lumpName);
        }
    #else
        I_LoadAndCacheTexLump(gTex_DOOM, "DOOM", 0);
    #endif

    I_LoadAndCacheTexLump(gTex_OptionsBg, Game::getTexLumpName_OptionsBg());

    // Assets specific to the GEC Master Edition
    if (Game::isGameTypeGecMe()) {
        if (GecMapInfo::shouldUseGecMapInfo()) {
            // GEC ME Beta 4 and later
            I_LoadAndCacheTexLump(gTex_MELOGO, GecMapInfo::getTitleLogo().lumpName);
        } else {
            // GEC ME Beta 3
            I_LoadAndCacheTexLump(gTex_DATA, "DATA");
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom utility: attempts to find the episode logo with the specified lump name
//------------------------------------------------------------------------------------------------------------------------------------------
static EpisodeLogo* M_FindEpisodeLogo(const String8& lumpName) noexcept {
    for (EpisodeLogo& logo : gEpisodeLogos) {
        if (logo.lumpName == lumpName)
            return &logo;
    }

    return nullptr;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom: builds a list of all the episode logos, made unique by their lump name.
// Note: this is only done once since the list must live for the duration of the app's lifetime (due to texture cache back references).
//------------------------------------------------------------------------------------------------------------------------------------------
static void M_BuildOnceEpisodeLogoList() noexcept {
    // Already built the logo list?
    // Can't touch the list once it's built, since the texture cache may reference entries within the list.
    if (!gEpisodeLogos.empty())
        return;

    for (const MapInfo::Episode& episode : MapInfo::allEpisodes()) {
        // Skip this logo if we already added it to the list, otherwise add an entry for it:
        if (M_FindEpisodeLogo(episode.logoPic))
            continue;

        EpisodeLogo& logo = gEpisodeLogos.emplace_back();
        logo.lumpName = episode.logoPic;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom: returns which episode is currently selected on the main menu
//------------------------------------------------------------------------------------------------------------------------------------------
static const MapInfo::Episode* M_GetSelectedEpisode() noexcept {
    const int32_t episodeNum = (gStartGameType == gt_single) ? gStartMapOrEpisode : Game::getMapEpisode(gStartMapOrEpisode);
    return MapInfo::getEpisode(episodeNum);
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom helper: searches for the next non-hidden episode number after the specified one.
// Returns '-1' if there is no such episode.
//------------------------------------------------------------------------------------------------------------------------------------------
static int32_t const M_NextNonHiddenEpisodeNum(const int32_t afterEpisodeNum) noexcept {
    for (const MapInfo::Episode& episode : MapInfo::allEpisodes()) {
        // Note: the episode list is in order, so stop the search once we find the first episode number which is greater:
        if ((episode.episodeNum > afterEpisodeNum) && (!episode.bIsHidden))
            return episode.episodeNum;
    }

    return -1;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom helper: searches for the previous non-hidden episode number before the specified one.
// Returns '-1' if there is no such episode.
//------------------------------------------------------------------------------------------------------------------------------------------
static int32_t const M_PrevNonHiddenEpisodeNum(const int32_t beforeEpisodeNum) noexcept {
    int32_t prevEpisodeNum = -1;

    for (const MapInfo::Episode& episode : MapInfo::allEpisodes()) {
        // Note: the episode list is in order, so stop the search once we find the first episode number which is greater than or equal:
        if (episode.episodeNum >= beforeEpisodeNum)
            break;

        if (!episode.bIsHidden) {
            prevEpisodeNum = episode.episodeNum;
        }
    }

    return prevEpisodeNum;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// PsyDoom helper: fixes up the start episode number to point to a valid and non hidden episode.
// Used to skip past 'hidden' secret episodes after making an episode selection, or after initializing the menu.
// Will either fix up by chosing a valid next episode (forward direction) or chosing a valid previous episode (reverse direction).
// Will also loop around to the beginning if required.
//------------------------------------------------------------------------------------------------------------------------------------------
static void M_FixupStartEpisodeNumber(const bool bForwardDir) noexcept {
    // Only do this when we are selecting episodes and not maps (single player mode only)
    if (gStartGameType != gt_single)
        return;

    const MapInfo::Episode* const pCurEpisode = MapInfo::getEpisode(gStartMapOrEpisode);
    const bool bCurEpisodeValid = (pCurEpisode && (!pCurEpisode->bIsHidden));

    if (!bCurEpisodeValid) {
        if (bForwardDir) {
            gStartMapOrEpisode = M_NextNonHiddenEpisodeNum(gStartMapOrEpisode);

            if (gStartMapOrEpisode < 0) {
                gStartMapOrEpisode = M_NextNonHiddenEpisodeNum(0);

                if (gStartMapOrEpisode < 0) {
                    I_Error("M_FixupStartEpisodeNumber: unable to find an episode which isn't hidden!");
                }
            }
        }
        else {
            gStartMapOrEpisode = M_PrevNonHiddenEpisodeNum(gStartMapOrEpisode);

            if (gStartMapOrEpisode < 0) {
                gStartMapOrEpisode = M_PrevNonHiddenEpisodeNum(gMaxStartEpisodeOrMap + 1);

                if (gStartMapOrEpisode < 0) {
                    I_Error("M_FixupStartEpisodeNumber: unable to find an episode which isn't hidden!");
                }
            }
        }
    }
}
#endif  // #if PSYDOOM_MODS

//------------------------------------------------------------------------------------------------------------------------------------------
// Encapsulates the logic for drawing the 'DOOM' logo.
// Also draws any additional elements, like 'MASTER EDITION' text for the GEC Master Edition.
//------------------------------------------------------------------------------------------------------------------------------------------
static void M_DrawDoomLogo() noexcept {
    // Draw the 'DOOM' or episode logo
    #if PSYDOOM_MODS
        const int32_t logoYPos = gMenuElems[MenuElem_Logo].yPos;
    #else
        constexpr int32_t logoYPos = 20;
    #endif

    // PsyDoom: the episode logo can now change dynamically depending on which episode is selected
    #if PSYDOOM_MODS
        if (const MapInfo::Episode* const pEpisode = M_GetSelectedEpisode()) {
            if (EpisodeLogo* const pEpisodeLogo = M_FindEpisodeLogo(pEpisode->logoPic)) {
                const uint16_t logoPaletteClut = (pEpisode->logoPal >= 0) ? R_GetPaletteClutId(pEpisode->logoPal) : Game::getTexClut_DOOM();
                I_CacheAndDrawSprite(pEpisodeLogo->tex, pEpisode->logoX, (int16_t)(logoYPos + pEpisode->logoYOffset), logoPaletteClut);
            }
        }
    #else
        I_CacheAndDrawSprite(gTex_DOOM, 75, (int16_t) logoYPos, Game::getTexClut_DOOM());
    #endif

    // PsyDoom: cache and draw the 'MASTER EDITION' text if we're playing the GEC Master Edition
    #if PSYDOOM_MODS
        if (Game::isGameTypeGecMe()) {
            if (GecMapInfo::shouldUseGecMapInfo()) {
                // Beta 4 (and later): this text is stored as a full sprite pointed to by GEC MAPINFO.
                // Note: the y position in the MAPINFO is converted to an offset to PsyDoom's own automatic layout here.
                const MapInfo::MenuSprite& sprite = GecMapInfo::getTitleLogo();
                const int32_t yOffset = sprite.yPos - 10;
                I_CacheAndDrawSprite(gTex_MELOGO, sprite.xPos, (int16_t)(gMenuElems[MenuElem_MasterEditionSprite].yPos + yOffset), R_GetPaletteClutId(sprite.paletteIdx));
            }
            else {
                // Beta 3 'MASTER EDITION' text is in this sprite atlas
                I_CacheTex(gTex_DATA);
                I_DrawSprite(
                    gTex_DATA.texPageId,
                    Game::getTexClut_DATA(),
                    48,
                    (int16_t) gMenuElems[MenuElem_MasterEditionSprite].yPos,
                    gTex_DATA.texPageCoordX + 1,
                    gTex_DATA.texPageCoordY + 1,
                    157,
                    8
                );
            }
        }
    #endif
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Starts up the main menu and returns the action to do on exit
//------------------------------------------------------------------------------------------------------------------------------------------
gameaction_t RunMenu() noexcept {
    do {
        // Run the menu: abort to the title screen & demos if the menu timed out.
        // PsyDoom: also quit if app quit is requested, and skip doing menus if we are warping directly to a map.
        #if PSYDOOM_MODS
            const gameaction_t menuResult = (gbStartupWarpToMap) ? ga_exit : MiniLoop(M_Start, M_Stop, M_Ticker, M_Drawer);

            // If warping directly to a map then we need to still load some resources that would have been loaded had we ran the menus
            if (gbStartupWarpToMap) {
                M_LoadAndCacheRequiredUITextures();
            }

            // If the '-warp' command line argument was specified then only warp straight to a map (bypassing menus) once.
            // After that the player has full control over the game flow:
            gbStartupWarpToMap = false;
        #else
            const gameaction_t menuResult = MiniLoop(M_Start, M_Stop, M_Ticker, M_Drawer);
        #endif

        #if PSYDOOM_MODS
            if ((menuResult == ga_timeout) || (menuResult == ga_quitapp))
                return menuResult;
        #else
            if (menuResult == ga_timeout)
                return menuResult;
        #endif

        // If we're not timing out draw the background and DOOM logo to prep for a 'loading' or 'connecting' plaque being drawn
        #if PSYDOOM_MODS
            Utils::onBeginUIDrawing();  // PsyDoom: UI drawing setup for the new Vulkan renderer
        #endif

        I_IncDrawnFrameCount();
        I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_BACK());
        M_DrawDoomLogo();
        I_SubmitGpuCmds();
        I_DrawPresent();

        // If the game type is singleplayer then we can just go straight to initializing and running the game.
        // Otherwise for a net game, we need to establish a connection and show the 'connecting' plaque...
        if (gStartGameType == gt_single)
            break;

        I_DrawLoadingPlaque(gTex_CONNECT, 54, 103, Game::getTexClut_CONNECT());
        I_NetSetup();

        // PsyDoom: abort if app quit was requested
        #if PSYDOOM_MODS
            if (Input::isQuitRequested())
                break;
        #endif

        // Once the net connection has been established, re-draw the background in prep for a loading or error plaque
        #if PSYDOOM_MODS
            Utils::onBeginUIDrawing();  // PsyDoom: UI drawing setup for the new Vulkan renderer
        #endif

        I_IncDrawnFrameCount();
        I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_BACK());
        M_DrawDoomLogo();
        I_SubmitGpuCmds();
        I_DrawPresent();

        // Play a sound to acknowledge that the connecting process has ended
        S_StartSound(nullptr, sfx_pistol);

    } while (gbDidAbortGame);

    // Startup the game!
    // PsyDoom: don't do this if app quit was requested.
    {
        #if PSYDOOM_MODS
            const bool bStartGame = (!Input::isQuitRequested());
        #else 
            const bool bStartGame = true;
        #endif

        if (bStartGame) {
            G_InitNew(gStartSkill, gStartMapOrEpisode, gStartGameType);
            G_RunGame();
        }
    }

    // PsyDoom: cleanup any network connections if we were doing a net game
    #if PSYDOOM_MODS
        Network::shutdown();
    #endif

    return ga_nothing;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Setup/init logic for the main menu
//------------------------------------------------------------------------------------------------------------------------------------------
void M_Start() noexcept {
    // PsyDoom: build (once only) a list of all the episode logos
    #if PSYDOOM_MODS
        M_BuildOnceEpisodeLogoList();
    #endif

    // Assume no networked game initially
    gNetGame = gt_single;
    gCurPlayerIndex = 0;
    gbPlayerInGame[0] = true;
    gbPlayerInGame[1] = false;

    // Clear out any textures that can be unloaded
    I_PurgeTexCache();

    // Show the loading plaque
    I_LoadAndCache_LOADING_TexLump(gTex_LOADING);
    I_DrawLoadingPlaque(gTex_LOADING, 95, 109, Game::getTexClut_LOADING());

    // Load sounds for the menu
    S_LoadMapSoundAndMusic(0);

    // Load and cache some commonly used UI textures
    I_LoadAndCacheTexLump(gTex_BACK, Game::getTexLumpName_BACK());

    #if PSYDOOM_MODS
        // PsyDoom: there is now a list of episode logos instead of just 1
        for (EpisodeLogo& episodeLogo : gEpisodeLogos) {
            I_LoadAndCacheTexLump(episodeLogo.tex, episodeLogo.lumpName);
        }
    #else
        I_LoadAndCacheTexLump(gTex_DOOM, "DOOM", 0);
    #endif

    I_LoadAndCacheTexLump(gTex_CONNECT, "CONNECT", 0);

    #if PSYDOOM_MODS
        // Load and cache assets specific to the GEC Master Edition
        if (Game::isGameTypeGecMe()) {
            if (GecMapInfo::shouldUseGecMapInfo()) {
                // GEC ME Beta 4 and later
                I_LoadAndCacheTexLump(gTex_MELOGO, GecMapInfo::getTitleLogo().lumpName);
            } else {
                // GEC ME Beta 3
                I_LoadAndCacheTexLump(gTex_DATA, "DATA");
            }
        }

        // PsyDoom: layout the menu and try to space it out as much as possible initially
        M_DoMenuLayout(MAX_OPTION_SPACING);
    #endif

    // Some basic menu setup
    gCursorFrame = 0;
    gCursorPos[0] = 0;
    gVBlanksUntilMenuMove[0] = 0;

    if (gStartGameType == gt_single) {
        gMaxStartEpisodeOrMap = Game::getNumEpisodes();
    } else {
        #if PSYDOOM_MODS
            gMaxStartEpisodeOrMap = Game::getNumMaps();         // For multiplayer any of the maps (including secret maps) can be selected
        #else
            gMaxStartEpisodeOrMap = Game::getNumRegularMaps();  // For multiplayer any of the normal (non secret) maps can be selected
        #endif
    }

    if (gStartMapOrEpisode > gMaxStartEpisodeOrMap) {
        // Wrap back around if we have to...
        gStartMapOrEpisode = 1;
    }
    else if (gStartMapOrEpisode < 0) {
        // Start map or episode will be set to '-NextEpisodeNum' when an episode like 'Ultimate Doom' is finished, which has a next episode.
        // This means we want to point the user to the next episode automatically, upon entering the main menu.
        gStartMapOrEpisode = -gStartMapOrEpisode;

        // PsyDoom: an added check, just in case...
        #if PSYDOOM_MODS
            gStartMapOrEpisode = std::min(gStartMapOrEpisode, gMaxStartEpisodeOrMap);
        #endif
    }

    // PsyDoom: make sure the start episode is valid in single player and skip past hidden ones
    #if PSYDOOM_MODS
        M_FixupStartEpisodeNumber(true);
    #endif

    // Play the main menu music
    psxcd_play_at_andloop(
        gCDTrackNum[cdmusic_main_menu],
        gCdMusicVol,
        0,
        0,
        gCDTrackNum[cdmusic_main_menu],
        gCdMusicVol,
        0,
        0
    );

    // Wait until some cd audio has been read
    Utils::waitForCdAudioPlaybackStart();

    // Don't clear the screen when setting a new draw environment.
    // Need to preserve the screen contents for the cross fade:
    gDrawEnvs[0].isbg = false;
    gDrawEnvs[1].isbg = false;

    // Draw the menu to the other framebuffer and do the cross fade
    gCurDispBufferIdx ^= 1;

    #if PSYDOOM_MODS
        I_PreCrossfade();
    #endif

    M_Drawer();
    I_CrossfadeFrameBuffers();

    // Restore background clearing for both draw envs
    gDrawEnvs[0].isbg = true;
    gDrawEnvs[1].isbg = true;

    // Begin counting for menu timeouts
    gMenuTimeoutStartTicCon = gTicCon;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Teardown/cleanup logic for the main menu
//------------------------------------------------------------------------------------------------------------------------------------------
void M_Stop(const gameaction_t exitAction) noexcept {
    // PsyDoom: if quitting the app then exit immediately, don't play any sounds and fade cd audio etc.
    #if PSYDOOM_MODS
        if (Input::isQuitRequested() || (exitAction == ga_quitapp))
            return;
    #endif

    // Play the pistol sound and stop the current cd music track
    S_StartSound(nullptr, sfx_pistol);

    #if PSYDOOM_MODS
        // PsyDoom: update sounds so that the barrel explosion plays while the music fades out.
        // Need to do this now since sounds don't play immediately and are queued.
        S_UpdateSounds();
    #endif

    psxcd_stop();

    // Single player: adjust the start map for the episode that was selected
    if ((exitAction == ga_exit) && (gStartGameType == gt_single)) {
        gStartMapOrEpisode = Game::getEpisodeStartMap(gStartMapOrEpisode);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Update/ticker logic for the main menu
//------------------------------------------------------------------------------------------------------------------------------------------
gameaction_t M_Ticker() noexcept {
    // PsyDoom: tick only if vblanks are registered as elapsed; this restricts the code to ticking at 30 Hz for NTSC
    #if PSYDOOM_MODS
        if (gPlayersElapsedVBlanks[0] <= 0) {
            gbKeepInputEvents = true;   // Don't consume 'key pressed' etc. events yet, not ticking...
            return ga_nothing;
        }
    #endif

    // Reset the menu timeout if buttons are being pressed.
    // PsyDoom: just restrict that to valid menu inputs only.
    #if PSYDOOM_MODS
        const TickInputs& inputs = gTickInputs[0];
        const TickInputs& oldInputs = gOldTickInputs[0];

        const bool bMenuStart = (inputs.fMenuStart() && (inputs.fMenuStart() != oldInputs.fMenuStart()));
        const bool bMenuOk = (inputs.fMenuOk() && (!oldInputs.fMenuOk()));
        const bool bMenuUp = inputs.fMenuUp();
        const bool bMenuDown = inputs.fMenuDown();
        const bool bMenuLeft = inputs.fMenuLeft();
        const bool bMenuRight = inputs.fMenuRight();
        const bool bMenuMove = (bMenuUp || bMenuDown || bMenuLeft || bMenuRight);
        const bool bMenuAnyInput = (bMenuMove || bMenuOk || bMenuStart || inputs.fMenuBack());
    #else
        const padbuttons_t ticButtons = gTicButtons[0];

        const bool bMenuStart = ((ticButtons != gOldTicButtons[0]) && (ticButtons & PAD_START));
        const bool bMenuOk = ((ticButtons != gOldTicButtons[0]) && (ticButtons & PAD_ACTION_BTNS));
        const bool bMenuUp = (ticButtons & PAD_UP);
        const bool bMenuDown = (ticButtons & PAD_DOWN);
        const bool bMenuLeft = (ticButtons & PAD_LEFT);
        const bool bMenuRight = (ticButtons & PAD_RIGHT);
        const bool bMenuMove = (ticButtons & PAD_DIRECTION_BTNS);
        const bool bMenuAnyInput = (ticButtons != 0);
    #endif

    if (bMenuAnyInput) {
        gMenuTimeoutStartTicCon = gTicCon;
    }

    // Exit the menu if timed out
    if (gTicCon - gMenuTimeoutStartTicCon >= 1800)
        return ga_timeout;

    // Animate the skull cursor
    if ((gGameTic > gPrevGameTic) && ((gGameTic & 3) == 0)) {
        gCursorFrame ^= 1;
    }

    // If start is pressed then begin a game, no matter what menu option is picked
    if (bMenuStart)
        return ga_exit;

    // If an 'ok' button is pressed then we can either start a map or open the options
    if (bMenuOk && (gCursorPos[0] >= gamemode)) {
        if (gCursorPos[0] < options)
            return ga_exit;

        if (gCursorPos[0] == options) {
            // Options entry pressed: run the options menu.
            // Note that if a level password is entered correctly there, we exit with 'ga_warped' as the action.
            if (MiniLoop(O_Init, O_Shutdown, O_Control, O_Drawer) == ga_warped)
                return ga_warped;
        }

        // PsyDoom: quit the game if that option is chosen
        #if PSYDOOM_MODS
            if (gCursorPos[0] == menu_quit)
                return ga_quitapp;
        #endif
    }

    // Check for movement with the DPAD direction buttons.
    // If there is none then we can just stop here:
    if (!bMenuMove) {
        gVBlanksUntilMenuMove[0] = 0;
        return ga_nothing;
    }

    // Movement repeat rate limiting for when movement buttons are held down
    gVBlanksUntilMenuMove[0] -= gPlayersElapsedVBlanks[0];

    if (gVBlanksUntilMenuMove[0] > 0)
        return ga_nothing;

    gVBlanksUntilMenuMove[0] = MENU_MOVE_VBLANK_DELAY;

    // Do menu up/down movements
    if (bMenuDown) {
        gCursorPos[0]++;

        if (gCursorPos[0] == NUMMENUITEMS) {
            gCursorPos[0] = (menu_t) 0;
        }

        S_StartSound(nullptr, sfx_pstop);
    }
    else if (bMenuUp) {
        gCursorPos[0]--;

        if (gCursorPos[0] == -1) {
            gCursorPos[0] = NUMMENUITEMS - 1;
        }

        S_StartSound(nullptr, sfx_pstop);
    }

    if (gCursorPos[0] == gamemode) {
        // Menu left/right movements: game mode.
        // PsyDoom: multiplayer can now be disabled by mods that don't want to support it or which can't support it. It's also disabled for the demo.
        #if PSYDOOM_MODS
            const bool bAllowMultiplayer = ((!MapInfo::getGameInfo().bDisableMultiplayer) && (!Game::gbIsDemoVersion));
        #else
            const bool bAllowMultiplayer = true;
        #endif

        if (bAllowMultiplayer) {
            if (bMenuRight) {
                if (gStartGameType < gt_deathmatch) {
                    gStartGameType = (gametype_t)((uint32_t) gStartGameType + 1);

                    if (gStartGameType == gt_coop) {
                        gStartMapOrEpisode = 1;
                    }

                    S_StartSound(nullptr, sfx_swtchx);
                }
            }
            else if (bMenuLeft) {
                if (gStartGameType != gt_single) {
                    gStartGameType = (gametype_t)((uint32_t) gStartGameType -1);

                    if (gStartGameType == gt_single) {
                        gStartMapOrEpisode = 1;
                    }

                    S_StartSound(nullptr, sfx_swtchx);
                }
            }
        } else {
            #if PSYDOOM_MODS
                if (bMenuRight || bMenuLeft) {
                    // PsyDoom: play a sound when trying to switch to multiplayer when it is not allowed.
                    // Don't play the sound for the demo version of the game however, to replicate the original demo behavior.
                    if (!Game::gbIsDemoVersion) {
                        S_StartSound(nullptr, sfx_itemup);
                    }
                }
            #endif
        }

        if (gStartGameType == gt_single) {
            gMaxStartEpisodeOrMap = Game::getNumEpisodes();
        } else {
            #if PSYDOOM_MODS
                // PsyDoom: allow selection of all maps for co-op/dm, including secret ones:
                gMaxStartEpisodeOrMap = Game::getNumMaps();
            #else
                gMaxStartEpisodeOrMap = Game::getNumRegularMaps();
            #endif
        }

        if (gStartMapOrEpisode > gMaxStartEpisodeOrMap) {
            gStartMapOrEpisode = 1;
        }

        // PsyDoom: make sure the start episode is valid in single player and skip past hidden ones
        #if PSYDOOM_MODS
            M_FixupStartEpisodeNumber(true);
        #endif

        return ga_nothing;
    }
    else if (gCursorPos[0] == level) {
        // Menu left/right movements: level/episode select
        if (bMenuRight) {
            gStartMapOrEpisode += 1;

            if (gStartMapOrEpisode <= gMaxStartEpisodeOrMap) {
                S_StartSound(nullptr, sfx_swtchx);
            } else {
                #if PSYDOOM_MODS
                    // PsyDoom: loop around to the beginning
                    S_StartSound(nullptr, sfx_swtchx);
                    gStartMapOrEpisode = 1;
                #else
                    gStartMapOrEpisode = gMaxStartEpisodeOrMap;
                #endif
            }

            // PsyDoom: make sure the start episode is valid in single player and skip past hidden ones
            #if PSYDOOM_MODS
                M_FixupStartEpisodeNumber(true);
            #endif
        }
        else if (bMenuLeft) {
            gStartMapOrEpisode -= 1;

            if (gStartMapOrEpisode > 0) {
                S_StartSound(nullptr, sfx_swtchx);
            } else {
                #if PSYDOOM_MODS
                    // PsyDoom: loop around to the end
                    S_StartSound(nullptr, sfx_swtchx);
                    gStartMapOrEpisode = gMaxStartEpisodeOrMap;
                #else
                    gStartMapOrEpisode = 1;
                #endif
            }

            // PsyDoom: make sure the start episode is valid in single player and skip past hidden ones
            #if PSYDOOM_MODS
                M_FixupStartEpisodeNumber(false);
            #endif
        }

        return ga_nothing;
    }
    else if (gCursorPos[0] == difficulty) {
        // Menu left/right movements: difficulty select
        #if PSYDOOM_MODS
            const bool bAllowDifficultySelect = (!Game::gbIsDemoVersion);   // Difficulty select disabled for the Doom demo
        #else
            const bool bAllowDifficultySelect = true;
        #endif

        if (bAllowDifficultySelect) {
            if (bMenuRight) {
                // PsyDoom: allow the previously hidden 'Nightmare' skill to be selected
                #if PSYDOOM_MODS
                    constexpr skill_t MAX_ALLOWED_SKILL = sk_nightmare;
                #else
                    constexpr skill_t MAX_ALLOWED_SKILL = sk_hard;
                #endif

                if (gStartSkill < MAX_ALLOWED_SKILL) {
                    gStartSkill = (skill_t)((uint32_t) gStartSkill + 1);
                    S_StartSound(nullptr, sfx_swtchx);
                }
            }
            else if (bMenuLeft) {
                if (gStartSkill != sk_baby) {
                    gStartSkill = (skill_t)((uint32_t) gStartSkill - 1);
                    S_StartSound(nullptr, sfx_swtchx);
                }
            }
        }
    }

    return ga_nothing;
}

//------------------------------------------------------------------------------------------------------------------------------------------
// Renders the main menu
//------------------------------------------------------------------------------------------------------------------------------------------
void M_Drawer() noexcept {
    // Draw the menu background and increment frame count for the texture cache.
    I_IncDrawnFrameCount();

    #if PSYDOOM_MODS
        Utils::onBeginUIDrawing();  // PsyDoom: UI drawing setup for the new Vulkan renderer
    #endif

    I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_BACK());
    M_DrawDoomLogo();

    // Figure out the y positions for everything
    #if PSYDOOM_MODS
        int32_t skullCursorY = {};
        
        switch (gCursorPos[0]) {
            case gamemode:      skullCursorY = gMenuElems[MenuElem_GameMode_Line1].yPos;    break;
            case level:         skullCursorY = gMenuElems[MenuElem_Level].yPos;             break;
            case difficulty:    skullCursorY = gMenuElems[MenuElem_Difficulty_Line1].yPos;  break;
            case options:       skullCursorY = gMenuElems[MenuElem_Options].yPos;           break;
            case menu_quit:     skullCursorY = gMenuElems[MenuElem_Quit].yPos;              break;

            default:
                ASSERT_FAIL("Bad menu option!");
                skullCursorY = 2;
                break;
        }

        skullCursorY -= 2;

        const int32_t gameModeLine1Y = gMenuElems[MenuElem_GameMode_Line1].yPos;
        const int32_t gameModeLine2Y = gMenuElems[MenuElem_GameMode_Line2].yPos;
        const int32_t levelY = gMenuElems[MenuElem_Level].yPos;
        const int32_t difficultyLine1Y = gMenuElems[MenuElem_Difficulty_Line1].yPos;
        const int32_t difficultyLine2Y = gMenuElems[MenuElem_Difficulty_Line2].yPos;
        const int32_t optionsY = gMenuElems[MenuElem_Options].yPos;
        const int32_t quitY = gMenuElems[MenuElem_Quit].yPos;
    #else
        const int32_t skullCursorY = gMenuYPos[gCursorPos[0]] - 2;
        const int32_t gameModeLine1Y = gMenuYPos[gamemode];
        const int32_t gameModeLine2Y = gMenuYPos[gamemode] + 20;
        const int32_t levelY = gMenuYPos[level];
        const int32_t difficultyLine1Y = gMenuYPos[difficulty];
        const int32_t difficultyLine2Y = gMenuYPos[difficulty] + 20;
        const int32_t optionsY = gMenuYPos[options];
    #endif

    // Draw the skull cursor
    I_DrawSprite(
        gTex_STATUS.texPageId,
        Game::getTexClut_STATUS(),
        50,
        (int16_t) skullCursorY,
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

    // Draw the text for the various menu entries
    I_DrawString(74, gameModeLine1Y, "Game Mode");
    I_DrawString(90, gameModeLine2Y, gGameTypeNames[gStartGameType]);

    if (gStartGameType == gt_single) {
        I_DrawString(74, levelY, Game::getEpisodeName(gStartMapOrEpisode).c_str().data());
    } else {
        // Coop or deathmatch game, draw the level number rather than episode.
        // PsyDoom: use a more flexible drawing method that can accomodate any number of digits.
        #if PSYDOOM_MODS
            char levelNumStr[64];
            std::snprintf(levelNumStr, C_ARRAY_SIZE(levelNumStr), "Level %d", gStartMapOrEpisode);
            I_DrawString(74, levelY, levelNumStr);
        #else
            I_DrawString(74, levelY, "Level");
            const int32_t xpos = (gStartMapOrEpisode >= 10) ? 148 : 136;
            I_DrawNumber(xpos, levelY, gStartMapOrEpisode);
        #endif
    }

    I_DrawString(74, difficultyLine1Y, "Difficulty");
    I_DrawString(90, difficultyLine2Y, gSkillNames[gStartSkill]);
    I_DrawString(74, optionsY, "Options");

    // PsyDoom: adding a 'Quit' option
    #if PSYDOOM_MODS
        I_DrawString(74, quitY, "Quit");
    #endif

    // PsyDoom: draw any enabled performance counters
    #if PSYDOOM_MODS
        I_DrawEnabledPerfCounters();
    #endif

    // Finish up the frame
    I_SubmitGpuCmds();
    I_DrawPresent();
}

#if PSYDOOM_MODS
//------------------------------------------------------------------------------------------------------------------------------------------
// Draws the display when the game is waiting for a network connection.
// Used by the Network module to re-draw the display during network connection for the Vulkan renderer.
// This is required if the user resizes the view while we are connecting, when we need to refresh the display.
//------------------------------------------------------------------------------------------------------------------------------------------
void M_DrawNetworkConnectDisplay() noexcept {
    I_IncDrawnFrameCount();
    Utils::onBeginUIDrawing();

    I_CacheAndDrawBackgroundSprite(gTex_BACK, Game::getTexClut_BACK());
    M_DrawDoomLogo();
    I_CacheAndDrawSprite(gTex_CONNECT, 54, 103, Game::getTexClut_CONNECT());

    I_SubmitGpuCmds();
    I_DrawPresent();
}
#endif  // #if PSYDOOM_MODS
