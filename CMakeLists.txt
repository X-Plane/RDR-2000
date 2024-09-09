cmake_minimum_required(VERSION 3.12)
project(rdr2000 VERSION 1.0.0 LANGUAGES C)

include(cmake/xplm.cmake)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})
add_definitions("-DGL_SILENCE_DEPRECATION=1")

use_static_libc()
find_xplane_sdk("${PROJECT_SOURCE_DIR}/SDK")

add_subdirectory(lib)
add_subdirectory(src/glutils)
add_subdirectory(src/helpers)
add_subdirectory(src/rdr2000)
