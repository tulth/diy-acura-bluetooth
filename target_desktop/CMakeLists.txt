cmake_minimum_required (VERSION 2.8)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project (mbus)

include_directories("../common" "../common/mbus")

file(GLOB SRC "../common/*.cpp" "../common/*.c" "../common/mbus/*.cpp" "../common/mbus/*.c")
add_library(Mbus SHARED ${SRC})
