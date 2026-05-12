cmake_minimum_required(VERSION 3.16)
project(NovaDinoRun VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Usar raylib local si existe, si no intentar find_package
if (EXISTS "${CMAKE_SOURCE_DIR}/raylib-5.0/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(raylib-5.0)
else()
    find_package(raylib 4.0 QUIET)
    if (NOT raylib_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
            GIT_SHALLOW TRUE
        )
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Obstacle.cpp
    src/Coin.cpp
    src/PowerUp.cpp
    src/UI.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_include_directories(${PROJECT_NAME} PRIVATE src)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
    # Linking estático: el .exe no necesita DLLs externas
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static-libgcc)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static winpthread)
endif()

# Copiar assets al directorio de build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
