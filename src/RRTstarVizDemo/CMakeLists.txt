set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(rrtstar-viz-demo
               configobjdialog.h
               configobjdialog.cpp
               mainwindow.h
               mainwindow.cpp
               rrtstar_viz_demo.cpp
               )

include_directories(
                   )

target_link_libraries(rrtstar-viz-demo
                      rrtstar
                      rrtstar-viz
                      ${QT_LIBRARIES}
                     )
