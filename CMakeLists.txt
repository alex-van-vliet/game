cmake_minimum_required(VERSION 3.24)
project(game CXX)

set(CMAKE_CXX_STANDARD 23)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.1
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

add_subdirectory(libsdl)
add_subdirectory(app)
