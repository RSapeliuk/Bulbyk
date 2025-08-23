cmake_minimum_required(VERSION 4.0)
project(Bulbyk)

set(CMAKE_CXX_STANDARD 26)

include_directories("C:/msys64/mingw64/include")
link_directories("C:/msys64/mingw64/lib")
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES src/*.cpp)


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} raylib)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 winmm)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/msys64/mingw64/bin/libraylib.dll"
        "C:/msys64/mingw64/bin/libwinpthread-1.dll"
        "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll"
        "C:/msys64/mingw64/bin/libstdc++-6.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
