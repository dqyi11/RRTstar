set(CMAKE_BUILD_WITH_INSTALL_PATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++0x)

find_package(LibXml2)
if( NOT LIBXML2_FOUND )
  message( FATAL_ERROR "libxml2 not found\n" )
  return()
endif()
include_directories(${LIBXML2_INCLUDE_DIR})


find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
if( NOT Qt4_FOUND )
  message( FATAL_ERROR "Qt4 not found\n" )
  return()
endif()
include(${QT_USE_FILE})
include_directories(${QT_INCLUDE_DIR})

# add the include directories to the build
include_directories(${PROJECT_SOURCE_DIR}/src/RRTstar
                    ${PROJECT_SOURCE_DIR}/src/RRTstarViz
                    ${PROJECT_SOURCE_DIR}/src/RRTstarVizDemo
                   )

add_definitions(-Wall -g -O3)

add_subdirectory(RRTstar)
add_subdirectory(RRTstarViz)
add_subdirectory(RRTstarVizDemo)

