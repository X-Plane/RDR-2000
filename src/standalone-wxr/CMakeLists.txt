set(SRC rds-81.c xplane.c)
set(HDR rds-81_impl.h rds-81.h xplane.h)
set(ALL_SRC ${SRC} ${HDR})


add_xplane_plugin(${CMAKE_PROJECT_NAME} 411 ${ALL_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glutils helpers xpwidgets xplm)