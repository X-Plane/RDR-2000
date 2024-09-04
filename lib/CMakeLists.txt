set(HDR
    nanovg/src/fontstash.h
    nanovg/src/nanovg_gl_utils.h
    nanovg/src/nanovg_gl.h
    nanovg/src/nanovg.h
    nanovg/src/stb_image.h
    nanovg/src/stb_truetype.h)
set(SRC nanovg/src/nanovg.c)

add_library(NanoVG STATIC nanovg/src/nanovg.c)
target_include_directories(NanoVG PUBLIC nanovg/src PRIVATE nanovg/src)
target_link_libraries(NanoVG PUBLIC glad glfw)
