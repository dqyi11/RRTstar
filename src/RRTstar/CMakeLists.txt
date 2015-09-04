
set(LIB rrtstar)

add_library(${LIB} SHARED
            kdtree++/allocator.hpp
            kdtree++/function.hpp
            kdtree++/iterator.hpp
            kdtree++/kdtree.hpp
            kdtree++/node.hpp
            kdtree++/region.hpp
            rrtstar.h
            rrtstar.cpp
           )

target_link_libraries(${LIB})
