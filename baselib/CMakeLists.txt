set(SOURCE_FILES
    "Asserts.h"
    "ByteInputStream.h"
    "ByteVecOutputStream.h"
    "Endian.h"
    "FatalErrors.cpp"
    "FatalErrors.h"
    "FileInputStream.h"
    "FileOutputStream.h"
    "FileUtils.cpp"
    "FileUtils.h"
    "Finally.h"
    "Flags.h"
    "IniUtils.cpp"
    "IniUtils.h"
    "InputStream.h"
    "JsonUtils.h"
    "Macros.h"
    "Matrix4.h"
    "OutputStream.h"
    "SmallString.h"
)

set(OTHER_FILES
)

set(INCLUDE_PATHS
    "."
)

add_library(${BASELIB_TGT_NAME} ${SOURCE_FILES} ${OTHER_FILES})
setup_source_groups("${SOURCE_FILES}" "${OTHER_FILES}")

add_psydoom_common_target_compile_options(${BASELIB_TGT_NAME})
target_link_libraries(${BASELIB_TGT_NAME} ${RAPID_JSON_TGT_NAME})
target_include_directories(${BASELIB_TGT_NAME} PUBLIC ${INCLUDE_PATHS})

if (COMPILER_MSVC)
    target_compile_definitions(${BASELIB_TGT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)   # Don't complain about using regular 'std::fopen()' etc.
endif()

if (COMPILER_GCC)
    target_link_libraries(${BASELIB_TGT_NAME} stdc++fs)     # Fix linker errors about missing 'std::filesystem'
endif()
