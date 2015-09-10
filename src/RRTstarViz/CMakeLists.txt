
set(LIB rrtstar-viz)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${LIB} SHARED
            path_planning_info.h
            path_planning_info.cpp
            rrtstar_viz.h
            rrtstar_viz.cpp
           )

include_directories(
                   )

target_link_libraries(${LIB} 
                      rrtstar
                      ${LIBXML2_LIBRARIES}
                      ${QT_LIBRARIES}
                     )
