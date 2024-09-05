set(SRC helpers.c)
set(HDR helpers/helpers.h)
set(ALL_SRC ${SRC} ${HDR})

add_library(helpers STATIC ${ALL_SRC})
target_compile_options(helpers PRIVATE -Wall -Wextra  -Werror)
target_link_libraries(helpers PUBLIC m)
target_include_directories(helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    target_compile_definitions(helpers PUBLIC -DAPL=1 -DIBM=0 -DLIN=0)
elseif(WIN32)
    target_compile_definitions(helpers PUBLIC -DAPL=0 -DIBM=1 -DLIN=0)
else()
    target_compile_definitions(helpers PUBLIC -DAPL=0 -DIBM=0 -DLIN=1)
endif()

set_target_properties(helpers
PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
    LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fno-stack-protector"
)


