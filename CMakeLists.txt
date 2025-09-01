cmake_minimum_required(VERSION 3.15)
project(SimpleMetro)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_executable(metronome_win main.cc MetronomeWin.cc)
    target_link_libraries(metronome_win winmm)
elseif(APPLE)
    add_executable(metronome_mac main.cc MetronomeMac.mm)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    target_link_libraries(metronome_mac
        ${COCOA_FRAMEWORK}
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
else()
    add_executable(metronome main.cc)
endif()
