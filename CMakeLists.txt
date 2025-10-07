cmake_minimum_required(VERSION 3.12)
project(rdr2000 VERSION 1.1.2 LANGUAGES C)
include(cmake/xplm.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
add_definitions("-DGL_SILENCE_DEPRECATION=1")

if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version" FORCE)
endif()

use_static_libc()
find_xplane_sdk("${PROJECT_SOURCE_DIR}/SDK")

add_subdirectory(lib)
add_subdirectory(src/glutils)
add_subdirectory(src/helpers)
add_subdirectory(src/rdr2000)
