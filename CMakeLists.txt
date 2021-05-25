cmake_minimum_required(VERSION 3.20.2)
project(libgameaudio LANGUAGES C)
set(PROJECT_VERSION 0.1.0-rc1)
add_library(libgameaudio STATIC
    src/libgameaudio.c
    src/track.c)
add_executable(play_bgm
    test/play_bgm.c)
target_link_libraries(play_bgm openal opusfile libgameaudio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include /usr/include/opus)
set_target_properties(libgameaudio PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libgameaudio PROPERTIES PUBLIC_HEADER include/libgameaudio.h)
target_include_directories(libgameaudio PRIVATE .)
install(TARGETS libgameaudio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
