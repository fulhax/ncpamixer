cmake_minimum_required(VERSION 2.8)

project(ncpamixer)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++14")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(CursesW REQUIRED)
find_package(PulseAudio REQUIRED)

set(COMMON_SRC
    "pulsemixer.cpp"
    "pa_object.cpp"
    "pa_sink.cpp"
    "pa_input.cpp"
    "pa_source.cpp"
    "pa.cpp"
    "ui.cpp"
    "tab.cpp"
    "tabs/playback.cpp"
    "tabs/recording.cpp"
    "tabs/output.cpp"
    "tabs/input.cpp"
    "tabs/configuration.cpp"
)

include_directories(
    "src/"
    ${CURSES_INCLUDE_DIR}
    ${PULSEAUDIO_INCLUDE_DIR}
)

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC} ${UI_RESOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${CURSES_LIBRARIES}
    ${PULSEAUDIO_LIBRARY}
)
