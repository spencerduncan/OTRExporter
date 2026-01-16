set(PROJECT_NAME OTRExporter)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AnimationExporter.h"
    "ArrayExporter.h"
    "AudioExporter.h"
    "BackgroundExporter.h"
    "BlobExporter.h"
	"CKeyFrameExporter.h"
    "CollisionExporter.h"
    "command_macros_base.h"
    "CutsceneExporter.h"
    "DisplayListExporter.h"
    "Exporter.h"
    "ExporterArchive.h"
    "ExporterArchiveO2R.h"
    "ExporterArchiveOTR.h"
    "Main.h"
    "MtxExporter.h"
    "PathExporter.h"
    "PlayerAnimationExporter.h"
    "RoomExporter.h"
    "SkeletonExporter.h"
    "SkeletonLimbExporter.h"
    "TextExporter.h"
    "TextMMExporter.h"
    "TextureExporter.h"
    "TextureAnimationExporter.h"
    "VersionInfo.h"
    "VtxExporter.h"
    "z64cutscene.h"
    "z64cutscene_commands.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AnimationExporter.cpp"
    "ArrayExporter.cpp"
    "AudioExporter.cpp"
    "BackgroundExporter.cpp"
    "BlobExporter.cpp"
    "CollisionExporter.cpp"
	"CKeyFrameExporter.cpp"
    "CutsceneExporter.cpp"
    "DisplayListExporter.cpp"
    "Exporter.cpp"
    "ExporterArchive.cpp"
    "ExporterArchiveO2R.cpp"
    "ExporterArchiveOTR.cpp"
    "Main.cpp"
    "MtxExporter.cpp"
    "PathExporter.cpp"
    "PlayerAnimationExporter.cpp"
    "RoomExporter.cpp"
    "SkeletonExporter.cpp"
    "SkeletonLimbExporter.cpp"
    "TextExporter.cpp"
    "TextMMExporter.cpp"
    "TextureExporter.cpp"
    "TextureAnimationExporter.cpp"
    "VersionInfo.cpp"
    "VtxExporter.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target - Build both OoT and MM variants
################################################################################

# Build both game variants unless OTREXPORTER_SINGLE_GAME is set
if(OTREXPORTER_SINGLE_GAME)
    set(GAME_VARIANTS ${OTREXPORTER_SINGLE_GAME})
else()
    set(GAME_VARIANTS "OoT" "MM")
endif()

foreach(GAME_VARIANT ${GAME_VARIANTS})
    set(TARGET_NAME "OTRExporter_${GAME_VARIANT}")

    add_library(${TARGET_NAME} STATIC ${ALL_FILES})

    # Set game-specific compile definition
    if(GAME_VARIANT STREQUAL "MM")
        target_compile_definitions(${TARGET_NAME} PRIVATE GAME_MM)
    else()
        target_compile_definitions(${TARGET_NAME} PRIVATE GAME_OOT)
    endif()

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        use_props(${TARGET_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
    endif()
endforeach()

# Create alias for backward compatibility (points to OoT by default)
if(NOT OTREXPORTER_SINGLE_GAME)
    add_library(OTRExporter ALIAS OTRExporter_OoT)
endif()

set(ROOT_NAMESPACE OTRExporter)

# Fetch StormLib once (shared by both targets)
FetchContent_Declare(
    StormLib
    GIT_REPOSITORY https://github.com/ladislav-zezula/StormLib.git
    GIT_TAG v9.25
)
FetchContent_MakeAvailable(StormLib)

find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# Apply settings to each game variant
foreach(GAME_VARIANT ${GAME_VARIANTS})
    set(TARGET_NAME "OTRExporter_${GAME_VARIANT}")

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set_target_properties(${TARGET_NAME} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
        )
    endif()

    ################################################################################
    # MSVC runtime library
    ################################################################################
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
        string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
            $<$<CONFIG:Debug>:
                MultiThreadedDebug
            >
            $<$<CONFIG:Release>:
                MultiThreaded
            >
            $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
        )
        set_target_properties(${TARGET_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
    endif()

    ################################################################################
    # Compile definitions
    ################################################################################
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            "$<$<CONFIG:Debug>:"
                "_DEBUG"
            ">"
            "$<$<CONFIG:Release>:"
                "NDEBUG"
            ">"
            "_CONSOLE;"
            "_CRT_SECURE_NO_WARNINGS;"
            STORMLIB_NO_AUTO_LINK
        )
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
        target_compile_definitions(${TARGET_NAME} PRIVATE
            "$<$<CONFIG:Debug>:"
                "_DEBUG"
            ">"
            "$<$<CONFIG:Release>:"
                "NDEBUG"
            ">"
            "_CONSOLE;"
            "_CRT_SECURE_NO_WARNINGS;"
        )
    endif()

    ################################################################################
    # Compile and link options
    ################################################################################
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../ZAPDTR/ZAPD/
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship/extern
        ${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship/src/resource
        ${CMAKE_CURRENT_SOURCE_DIR}/../../games/mm/2s2h
        ${stormlib_SOURCE_DIR}/src
        .
    )

    target_link_libraries(${TARGET_NAME} PUBLIC nlohmann_json::nlohmann_json)
    target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /Oi-
            >
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
                /O2
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
        target_link_options(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
                /INCREMENTAL:NO
            >
            /SUBSYSTEM:CONSOLE
        )
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
        )
    endif()

    ################################################################################
    # Dependencies
    ################################################################################
    add_dependencies(${TARGET_NAME}
        libultraship
    )

    target_link_libraries(${TARGET_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
endforeach()

