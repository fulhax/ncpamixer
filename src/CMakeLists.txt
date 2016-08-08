cmake_minimum_required(VERSION 2.8)

project(pulsemixer)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

set(COMMON_SRC
    "pulsemixer.cpp"
    "pa.cpp"
    "view.cpp"
)

include_directories(
    "src/"
)

add_executable(${CMAKE_PROJECT_NAME} ${COMMON_SRC} ${UI_RESOURCES})


target_link_libraries(${CMAKE_PROJECT_NAME}
    ncursesw
    pulse
)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)
# target_link_libraries(pulsemixer Threads::Threads)
# SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
