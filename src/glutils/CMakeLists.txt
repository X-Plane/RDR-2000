set(SRC gl.c renderer.c)
set(HDR
    glutils/gl.h
    glutils/renderer.h
    glutils/stb_image.h
    glutils_impl.h)
set(ALL_SRC ${SRC} ${HDR})

add_library(glutils STATIC ${ALL_SRC})
target_compile_options(glutils PRIVATE -Wall -Wextra  -Werror)
target_link_libraries(glutils PUBLIC helpers m ${OPENGL_LIBRARIES} cglm GLEW::glew_s)
target_include_directories(glutils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    target_compile_definitions(glutils PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
elseif(WIN32)
    target_compile_definitions(glutils PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
else()
    target_compile_definitions(glutils PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
endif()

set_target_properties(glutils
PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
    LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fno-stack-protector"
)


