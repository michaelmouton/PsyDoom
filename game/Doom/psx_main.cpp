#include "psx_main.h"

#include "Base/i_main.h"
#include "cdmaptbl.h"
#include "FatalErrors.h"
#include "PsyDoom/Cheats.h"
#include "PsyDoom/Config/Config.h"
#include "PsyDoom/Controls.h"
#include "PsyDoom/Game.h"
#include "PsyDoom/Input.h"
#include "PsyDoom/IntroLogos.h"
#include "PsyDoom/ModMgr.h"
#include "PsyDoom/PlayerPrefs.h"
#include "PsyDoom/ProgArgs.h"
#include "PsyDoom/PsxVm.h"
#include "PsyDoom/Utils.h"
#include "PsyDoom/Video.h"

#if PSYDOOM_MODS
    // PsyDoom: a flag set to 'true' if the result of demo playback is unexpected/wrong (when checking demo results).
    // This is used to set the exit code for the application accordingly ('1' if the demo result checks fail, '0' otherwise).
    bool gbCheckDemoResultFailed = false;
#endif

//------------------------------------------------------------------------------------------------------------------------------------------
// This was the old reverse engineered entrypoint for PSXDOOM.EXE, which executed before 'main()' was called.
//
// The code that was previously here was generated by the compiler and/or toolchain and did stuff which the user application could not do
// such as deciding on the value of the 'gp' register, zero initializing the BSS section global variables and so on. Basically all of the
// bootstrap stuff before the C program can run.
//
// There used to be other actual PSX code in here, but as layers of emulation were removed this function had less and less purpose.
// Now I'm using it as a place to initialize all of PsyDoom's systems before launching the original 'main()' of PSXDOOM.EXE.
//
// Code which was removed includes:
//  (1) Save and restore the current return address register (ra) for returning control to operating system after the program is done.
//  (2) Setting the value of the 'global pointer' (gp) register which is used as a base to reference many global variables.
//      For the 'Greatest Hits' edition of PlayStation Doom (US) 'gp' was set to: 0x800775E0
//  (3) Setting the value of the 'frame pointer' (fp) register.
//  (4) Setting the value of the 'stack pointer' (sp) register.
//  (5) Zero initializing the BSS section globals. These are globals with a defaulted (0) value.
//  (6) Calling 'LIBAPI_InitHeap' to initialize the PsyQ SDK Heap.
//      Note that since Doom used it's own zone memory management system, that call was effectively useless.
//      It never once asked for memory from the PsyQ SDK, nor did any of the PsyQ functions used.
//      Doom's own heap would have been corrupted if LIBAPI's heap was in conflicting use of the same memory.
//------------------------------------------------------------------------------------------------------------------------------------------
int psx_main(const int argc, const char* const* const argv) noexcept {
    // PsyDoom: setup logic for the new game host environment
    #if PSYDOOM_MODS
        // Parse command line arguments and configuration and initialize input systems
        Utils::installFatalErrorHandler();
        ProgArgs::init(argc, argv);

        if (!Controls::didInit()) {
            Controls::init();
        }

        if (!Config::didInit()) {
            Config::init();
        }

        Input::init();
        PlayerPrefs::load();

        // Initialize the emulated PSX components using the PSX Doom disc (supplied as a .cue file).
        // This must be provided in order for the game to run.
        const char* const cueFilePath = (ProgArgs::gCueFileOverride) ? ProgArgs::gCueFileOverride : Config::gCueFilePath.c_str();

        if ((!cueFilePath) || (!cueFilePath[0])) {
            FatalErrors::raise(
                "The path to the .cue file for the game disc is not specified; PsyDoom needs this in order to run!\n"
                "\n"
                "You can set the path to the default .cue file by changing the 'CueFilePath' setting in 'game_cfg.ini'\n"
                "or via the 'Game' settings tab in the launcher.\n"
                "\n"
                "Alternatively, you can use the '-cue <CUE_FILE_PATH>' program argument to specify the .cue file."
            );
        }

        if (!PsxVm::init(cueFilePath))
            return 1;

        // Determine the game type and variant and initialize the table of files on the CD from the file system
        Game::determineGameTypeAndVariant();
        CdMapTbl_Init();

        // Initialize the display, modding manager, cheats and intro logos
        Video::initVideo();
        ModMgr::init();
        Cheats::init();
        IntroLogos::init();
    #endif

    // Call the original PSX Doom 'main()' function
    I_Main();

    // PsyDoom: cleanup logic after Doom itself is done and save player prefs (unless headless mode)
    #if PSYDOOM_MODS
        const bool bIsCheckingADemoResult = (ProgArgs::gCheckDemoResultFilePath[0] != 0);

        if (!ProgArgs::gbHeadlessMode) {
            PlayerPrefs::save();
        }

        IntroLogos::shutdown();
        Video::shutdownVideo();
        PsxVm::shutdown();
        Cheats::shutdown();
        ModMgr::shutdown();
        Input::shutdown();
        Config::shutdown();
        Controls::shutdown();
        ProgArgs::shutdown();
        Utils::uninstallFatalErrorHandler();
    #endif

    // PsyDoom: if we were checking the result of a demo and it produced an unexpected outcome then return error code '1' to indicate that.
    // Otherwise return code '0' to indicate normal execution without any issues:
    #if PSYDOOM_MODS
        return (bIsCheckingADemoResult && gbCheckDemoResultFailed) ? 1 : 0;
    #else
        return 0;
    #endif
}
