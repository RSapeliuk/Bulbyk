cmake_minimum_required(VERSION 4.00)
project(Bulbyk LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===========================================
# PLATFORM DETECTION
# ===========================================
if(WIN32)
    message(STATUS "Building for Windows")
elseif(APPLE)
    message(STATUS "Building for macOS")
elseif(UNIX)
    message(STATUS "Building for Linux")
endif()

# ===========================================
# RAYLIB SETUP (IMPROVED)
# ===========================================
# Option to force using FetchContent even if raylib is installed
option(FORCE_FETCH_RAYLIB "Force downloading raylib even if installed" OFF)

set(RAYLIB_TARGET "")

if(NOT FORCE_FETCH_RAYLIB)
    # Try to find installed raylib
    find_package(raylib QUIET)

    if(raylib_FOUND)
        message(STATUS "✅ Found installed raylib")

        # Determine the correct target name
        if(TARGET raylib::raylib)
            set(RAYLIB_TARGET raylib::raylib)
            message(STATUS "Using target: raylib::raylib")
        elseif(TARGET raylib)
            set(RAYLIB_TARGET raylib)
            message(STATUS "Using target: raylib")
        else()
            message(WARNING "Raylib found but no usable target, falling back to FetchContent")
            set(raylib_FOUND FALSE)
        endif()
    endif()
endif()

# If raylib not found or forced to fetch, download it
if(NOT raylib_FOUND OR FORCE_FETCH_RAYLIB)
    message(STATUS "📥 Downloading raylib from GitHub...")

    include(FetchContent)

    # Clear any cached raylib from previous runs
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5  # Stable version instead of 5.5
            OVERRIDE_FIND_PACKAGE  # This prevents conflicts
    )

    # Configure raylib options before making it available
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

    # Prevent raylib from installing itself
    set(RAYLIB_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(raylib)

    # After FetchContent, raylib target should be available
    set(RAYLIB_TARGET raylib)
    message(STATUS "Using downloaded raylib target: ${RAYLIB_TARGET}")
endif()

# Verify we have a raylib target
if(NOT RAYLIB_TARGET)
    message(FATAL_ERROR "❌ No raylib target available!")
endif()


# ===========================================
# ECS CORE LIBRARY
# ===========================================
add_library(BulbykECS STATIC
        src/core/Entity.cpp
        src/core/EntityManager.cpp
        src/systems/TransformSystem.cpp
        src/systems/RenderSystem.cpp
        src/systems/CollisionSystem.cpp
)

target_include_directories(BulbykECS PUBLIC
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(BulbykECS PUBLIC ${RAYLIB_TARGET})

# ===========================================
# ECS TEST EXECUTABLE
# ===========================================
add_executable(BulbykECSTest
        src/testECS.cpp
)

target_link_libraries(BulbykECSTest PRIVATE BulbykECS ${RAYLIB_TARGET})

# ===========================================
# COLLISION TEST
# ===========================================
add_executable(BulbykCollisionTest
        src/test_collision.cpp
)

target_link_libraries(BulbykCollisionTest PRIVATE BulbykECS ${RAYLIB_TARGET})


# ===========================================
# SOURCES COLLECTION
# ===========================================
# Collect all .cpp files from src/
file(GLOB_RECURSE SOURCES
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Remove any test files if they exist
list(FILTER SOURCES EXCLUDE REGEX ".*test.*")

message(STATUS "📁 Found source files:")
foreach(SOURCE ${SOURCES})
    message(STATUS "  ${SOURCE}")
endforeach()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# ===========================================
# INCLUDE DIRECTORIES
# ===========================================
target_include_directories(${PROJECT_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/src"
)

# ===========================================
# LINKING
# ===========================================
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_TARGET})

message(STATUS "🔗 Linking with: ${RAYLIB_TARGET}")

# ===========================================
# PLATFORM-SPECIFIC SETTINGS
# ===========================================

if(WIN32)
    message(STATUS "🪟 Configuring for Windows...")

    # Windows specific compile options
    if(NOT MSVC)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
            target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra -Wno-missing-braces)
        else()
            target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
        endif()
    endif()

    # Handle DLL copying only for shared libraries
    get_target_property(RAYLIB_TYPE ${RAYLIB_TARGET} TYPE)
    if(RAYLIB_TYPE STREQUAL "SHARED_LIBRARY")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${RAYLIB_TARGET}>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "Copying raylib DLL to output directory"
        )
    endif()

endif()

if(APPLE)
    message(STATUS "🍎 Configuring for macOS...")

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra -Wno-missing-braces)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()

    # Link macOS frameworks
    target_link_libraries(${PROJECT_NAME} PRIVATE
            "-framework CoreVideo"
            "-framework IOKit"
            "-framework Cocoa"
            "-framework GLUT"
            "-framework OpenGL"
    )

    # Set bundle properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.bulbyk"
            MACOSX_BUNDLE_BUNDLE_NAME "Bulbyk"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
endif()

if(UNIX AND NOT APPLE)
    message(STATUS "🐧 Configuring for Linux...")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${PROJECT_NAME} PRIVATE _DEBUG)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()

    # Linux system libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
            GL
            m
            pthread
            dl
            rt
            X11
    )
endif()

# ===========================================
# COMPILER-SPECIFIC SETTINGS
# ===========================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wno-missing-field-initializers
            -Wno-unused-parameter
    )
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /permissive-
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            NOMINMAX
    )
endif()

# ===========================================
# BUILD CONFIGURATION
# ===========================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    message(STATUS "No build type specified, defaulting to Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "🔧 Building in Debug mode")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "🚀 Building in Release mode")
endif()

# ===========================================
# SUMMARY
# ===========================================
message(STATUS "")
message(STATUS "🎮 === BUILD CONFIGURATION SUMMARY ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Raylib Target: ${RAYLIB_TARGET}")
message(STATUS "========================================")
message(STATUS "")
