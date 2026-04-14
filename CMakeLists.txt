cmake_minimum_required(VERSION 3.22)
project(vivify VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── QPM / NDK setup ─────────────────────────────────────────────────────────
include(extern/CMakeIncludes/extern.cmake OPTIONAL RESULT_VARIABLE HAS_QPM)
if (NOT HAS_QPM)
    message(FATAL_ERROR "QPM extern.cmake not found – run: qpm restore")
endif()

# ── Mod metadata (mirrors qpm.json) ─────────────────────────────────────────
set(MOD_ID   "vivify")
set(MOD_NAME "Vivify")
set(MOD_VER  "0.1.0")

COMPILE_GAME_ASSEMBLIES()

# ── All source files ─────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
)

# ── Shared library target ────────────────────────────────────────────────────
add_library(${MOD_ID} SHARED ${SOURCES})

target_include_directories(${MOD_ID} PRIVATE
    include
    extern/includes
    extern/includes/beatsaber-hook/shared
    extern/includes/custom-types/shared
    extern/includes/bs-cordl/include
    extern/includes/songcore/shared
    extern/includes/bsml/shared
    extern/includes/lapiz/shared
    extern/includes/metacore/shared
    extern/includes/config-utils/shared
    extern/includes/paper2_scotland2/shared
)

target_compile_definitions(${MOD_ID} PRIVATE
    MOD_ID="${MOD_ID}"
    MOD_VERSION="${MOD_VER}"
    BS_QUEST          # Used to guard Quest-specific code
    UNITY_ANDROID     # Asset bundle target platform
)

target_compile_options(${MOD_ID} PRIVATE
    -frtti
    -fexceptions
    -O2
    -Wno-invalid-offsetof
)

# Link QPM-managed extern libs
target_link_libraries(${MOD_ID} PRIVATE
    ${EXTERN_LIB_FILES}
    log
    android
    EGL
    GLESv3
)

# ── qmod packaging target ────────────────────────────────────────────────────
set(QMOD_OUTPUT "${CMAKE_SOURCE_DIR}/${MOD_NAME}.qmod")
set(BINARYDIR   "${CMAKE_BINARY_DIR}")

add_custom_target(package
    COMMAND qpm qmod build
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${MOD_ID}
    COMMENT "Building ${MOD_NAME}.qmod"
)
