option(CGLM_SHARED "Shared build" OFF)
option(CGLM_STATIC "Static build" ON)
option(CGLM_USE_C99 "" OFF) # C11 
option(CGLM_USE_TEST "Enable Tests" OFF) # for make check - make test
add_subdirectory(cglm)


set(HDR
    nanovg/src/fontstash.h
    nanovg/src/nanovg_gl_utils.h
    nanovg/src/nanovg_gl.h
    nanovg/src/nanovg.h
    nanovg/src/stb_image.h
    nanovg/src/stb_truetype.h)
set(SRC nanovg/src/nanovg.c)

add_library(nanovg STATIC nanovg/src/nanovg.c)
target_include_directories(nanovg PUBLIC nanovg/src PRIVATE nanovg/src)
set_target_properties(nanovg PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)

add_subdirectory(glew)
