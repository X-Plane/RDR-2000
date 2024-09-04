cmake_minimum_required(VERSION 3.12)
project(standalone-wxr VERSION 1.0.0 LANGUAGES C)

include(cmake/xplm.cmake)
find_package(OpenGL REQUIRED)

use_static_libc()
find_xplane_sdk("${PROJECT_SOURCE_DIR}/SDK")

add_subdirectory(lib)
add_subdirectory(src/glutils)
add_subdirectory(src/helpers)
add_subdirectory(src/standalone-wxr)
