set(SOURCE_FILES
    "AudioUtils.cpp"
    "AudioUtils.h"
    "Lcd.cpp"
    "Lcd.h"
    "MidiConvert.cpp"
    "MidiConvert.h"
    "MidiTypes.cpp"
    "MidiTypes.h"
    "MidiUtils.cpp"
    "MidiUtils.h"
    "Module.cpp"
    "Module.h"
    "ModuleFileUtils.cpp"
    "ModuleFileUtils.h"
    "PsxAdsrEnvelope.h"
    "PsxPatch.cpp"
    "PsxPatch.h"
    "PsxPatchGroup.cpp"
    "PsxPatchGroup.h"
    "PsxPatchSample.cpp"
    "PsxPatchSample.h"
    "PsxPatchVoice.cpp"
    "PsxPatchVoice.h"
    "Sequence.cpp"
    "Sequence.h"
    "Track.cpp"
    "Track.h"
    "TrackCmd.cpp"
    "TrackCmd.h"
    "VagUtils.cpp"
    "VagUtils.h"
    "WavUtils.cpp"
    "WavUtils.h"
    "WmdFileTypes.cpp"
    "WmdFileTypes.h"
)

set(OTHER_FILES
)

set(INCLUDE_PATHS
    "."
)

add_library(${AUDIO_TOOLS_COMMON_TGT_NAME} ${SOURCE_FILES} ${OTHER_FILES})
setup_source_groups("${SOURCE_FILES}" "${OTHER_FILES}")

add_psydoom_common_target_compile_options(${AUDIO_TOOLS_COMMON_TGT_NAME})

target_link_libraries(${AUDIO_TOOLS_COMMON_TGT_NAME}
    ${BASELIB_TGT_NAME}
)

target_include_directories(${AUDIO_TOOLS_COMMON_TGT_NAME} PUBLIC INTERFACE ${INCLUDE_PATHS})
