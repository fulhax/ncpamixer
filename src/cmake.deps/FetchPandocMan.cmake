FetchContent_Declare(
  cmake_modules
  GIT_REPOSITORY https://github.com/rnpgp/cmake-modules.git
  GIT_TAG        main
)

FetchContent_MakeAvailable(cmake_modules)
FetchContent_GetProperties(cmake_modules)
list(APPEND CMAKE_MODULE_PATH "${cmake_modules_SOURCE_DIR}")

