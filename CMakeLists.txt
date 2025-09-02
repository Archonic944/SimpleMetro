cmake_minimum_required(VERSION 3.15)
project(SimpleMetro)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    add_executable(metro main.cc MetronomeWin.cc)
    target_link_libraries(metro winmm)

elseif(APPLE)
    add_executable(metro main.cc MetronomeMac.mm)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    target_link_libraries(metro
        ${COCOA_FRAMEWORK}
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(AO REQUIRED ao)
    pkg_check_modules(SNDFILE REQUIRED sndfile)

    add_executable(metro main.cc MetronomeLinux.cc)

    target_include_directories(metro PRIVATE
        ${AO_INCLUDE_DIRS}
        ${SNDFILE_INCLUDE_DIRS}
    )

    target_link_libraries(metro
        ${AO_LIBRARIES}
        ${SNDFILE_LIBRARIES}
    )

    target_compile_options(metro PRIVATE
        ${AO_CFLAGS_OTHER}
        ${SNDFILE_CFLAGS_OTHER}
    )
endif()
