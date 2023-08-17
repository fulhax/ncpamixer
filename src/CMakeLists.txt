cmake_minimum_required(VERSION 3.1)

project(ncpamixer)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(USE_WIDE)
    set(CURSES_NEED_WIDE TRUE)
endif()

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
# See https://gitlab.kitware.com/cmake/cmake/issues/18517
find_library(MENU_LIBRARY menu)

find_package(PulseAudio REQUIRED)
find_package(Threads REQUIRED)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "empty")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

execute_process(COMMAND
    "git" "describe" "--tags" "--dirty"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_VERSION
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
    )

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
    )

message("-- Got version ${GIT_VERSION}")

set(COMMON_SRC
    "pulsemixer.cpp"
    "pa_object.cpp"
    "pa_card.cpp"
    "pa_sink.cpp"
    "pa_input.cpp"
    "pa_source.cpp"
    "pa_source_output.cpp"
    "pa.cpp"
    "config.cpp"
    "ui/ui.cpp"
    "ui/tab.cpp"
    "ui/tabs/playback.cpp"
    "ui/tabs/recording.cpp"
    "ui/tabs/output.cpp"
    "ui/tabs/input.cpp"
    "ui/tabs/configuration.cpp"
    "ui/tabs/fallback.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
    )

include_directories(
    "src/"
    ${CURSES_INCLUDE_DIR}
    ${PULSEAUDIO_INCLUDE_DIR}
    )

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC} ${UI_RESOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${CURSES_LIBRARIES}
    ${MENU_LIBRARY}
    ${PULSEAUDIO_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    )

target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Werror")
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Wall")
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-Wpedantic")

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
